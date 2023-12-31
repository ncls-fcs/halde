#include "halde.h"
#include <errno.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/mman.h>

/// Magic value for occupied memory chunks.
#define MAGIC ((void*)0xbaadf00d)

/// Size of the heap (in bytes).
#define SIZE (1024*1024*1)

/// Memory-chunk structure.
struct mblock {
	struct mblock *next;
	size_t size;
	char memory[];
};

/// Heap-memory area.
static char *memory;

/// Pointer to the first element of the free-memory list.
static struct mblock *head;

/// Helper function to visualise the current state of the free-memory list.
void printList(void) {
	struct mblock *lauf = head;

	// Empty list
	if (head == NULL) {
		char empty[] = "(empty)\n";
		write(STDERR_FILENO, empty, sizeof(empty));
		return;
	}

	// Print each element in the list
	const char fmt_init[] = "(off: %7zu, size:: %7zu)";
	const char fmt_next[] = " --> (off: %7zu, size:: %7zu)";
	const char * fmt = fmt_init;
	char buffer[sizeof(fmt_next) + 2 * 7];

	while (lauf) {
		size_t n = snprintf(buffer, sizeof(buffer), fmt
			, (uintptr_t) lauf - (uintptr_t)memory, lauf->size);
		if (n) {
			write(STDERR_FILENO, buffer, n);
		}

		lauf = lauf->next;
		fmt = fmt_next;
	}
	write(STDERR_FILENO, "\n", 1);
}

void *malloc (size_t size) {
	// TODO: implement me!
	//malloc with first-fit implementation

	//nochmal anschauen @memory
	if(head == NULL) {
		//Init heap and memory-control structure
		memory = mmap(NULL, SIZE, PROT_READ | PROT_WRITE, MAP_PRIVATE | MAP_ANONYMOUS, -1, 0); 	//map memory of size $SIZE with permissions to read and write granted and a private and anonymous map type. 
		if(memory == MAP_FAILED) {
			return NULL;
		}

		//init control-structure
		head = (struct mblock *)memory;
		head->next = NULL;
		head->size = SIZE - sizeof(struct mblock);
	}

	struct mblock *currentBlock = head;

	//check if first block in list is sufficient
	if(currentBlock->size > (size + sizeof(struct mblock))) {
		//create and init new free block with remaining memory as size parameter
		struct mblock *newBlock = (struct mblock *) &(currentBlock->memory[size]);
		newBlock->next = currentBlock->next;
		newBlock->size = currentBlock->size - size - sizeof(struct mblock);		//set remaining size to difference between size of previous block and allocated size plus size of one mblock
		
		//setting new free block as head
		head = newBlock;

		//returning start of memory of current block after marking it as occupied and updating its size
		currentBlock->size = size;
		currentBlock->next = MAGIC;
		return currentBlock->memory;
	}else if (currentBlock->size == size){
		head = currentBlock->next;	//remove current block from list
		currentBlock->next = MAGIC;
		return currentBlock->memory;
	}
	
	//if size was not sufficient, walk through list an search for big enough block
	struct mblock *previousBlock = NULL;

	while(currentBlock->next != NULL) {
		previousBlock = currentBlock;
		currentBlock = currentBlock->next;

		if(currentBlock->size > (size + sizeof(struct mblock))) {
			//create and init new free block with remaining memory as size parameter
			struct mblock *newBlock = (struct mblock *) &(currentBlock->memory[size]);
			newBlock->next = currentBlock->next;
			newBlock->size = currentBlock->size - size - sizeof(struct mblock);		//set remaining size to difference between size of previous block and allocated size plus size of one mblock

			//setting new free block as head
			previousBlock->next = newBlock;

			//returning start of memory of current block after marking it as occupied and updating its size
			currentBlock->size = size;
			currentBlock->next = MAGIC;
			return currentBlock->memory;
		}else if (currentBlock->size == size){
			previousBlock->next = currentBlock->next;	//remove current block from list
			currentBlock->next = MAGIC;
			return currentBlock->memory;
		}
	}
	//if function still has not returned here, no block with sufficient space was found -> return NULL pointer
	errno = ENOMEM;
	return NULL;
}

void free (void *ptr) {
	if(ptr == NULL) {
		//NULL pointer was given
		return;
	}

	struct mblock *mbp = ((struct mblock *) ptr) - 1;	//"1" represents one mblock
	
	//check if block is really allocated
	if(mbp->next != MAGIC) {
		//error handling, block was not allocated
		abort();
	}

	//reintegrate now free block into list
	mbp->next = head;
	head = mbp;
}

void *realloc (void *ptr, size_t size) {
	// TODO: implement me!
	if(ptr == NULL) {			//edge case if ptr is NULL
		return malloc(size);
	}
	if(size == 0 && ptr != NULL) {	//edge case creates minimum sized block and frees original memory
		free(ptr);
		return malloc(0);
	}

	char *newptr = malloc(size);

	struct mblock *ptr_mblock = (struct mblock *) ptr;		//cast ptr to struct to read its size
	if(size < ptr_mblock->size){				//check if new location is bigger or smaller than old one
		memcpy(newptr, ptr, size);				//copy at most new size bytes from old memory location to new if new location is smaller than old one
	}else{
		memcpy(newptr, ptr, ptr_mblock->size);	//copy every byte in original memory area
	}
	free(ptr);		//free old ptr
	
	return newptr;
}

void *calloc (size_t nmemb, size_t size) {
	if((nmemb * size) / size != nmemb) {				//check if integer overflow would occur:
		return NULL;
	}
	char *ptr = malloc(nmemb * size);					//allocate memory
	for(int i = 0; i < (nmemb * size)/8; i+=8) {		//zero allocated memory bytewise
		ptr[i] = 0x0;
	}
	return ptr;
}


/*
newBlock->size = (SIZE + &head) - (&newBlock + sizeof(struct mblock));	//sets remaining size to the difference between the address of the start of the structure (address of head) with an offset of $SIZE (representing the complete address space mapped by mmap) and the address of the new mblock with an offset of its size. The resulting integer is a representation of the total remaining space in the mapped memory area

*/
