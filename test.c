#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

#include "halde.h"

int main(int argc, char *argv[]) {
	printList();

	char *m1 = malloc(200*1024);
	printList();

	free(m1);
	printList();

	exit(EXIT_SUCCESS);
}
