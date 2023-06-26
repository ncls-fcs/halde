#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

#include "halde.h"

int main(int argc, char *argv[]) {
    printList();

    char *m1 = malloc(2*1024);
    printList();

    char *m2 = malloc(100*1024);
    printList();

    char *m3 = malloc(12*1024);
    printList();

    char *m4 = malloc(3*16);
    printList();

    free(m1);
    printList();

    free(m2);
    printList();

    free(m3);
    printList();

    free(m4);
    printList();

    char *m5 = calloc(20, sizeof(int));
    printList();

    char *m6 = malloc(256);
    printList();

    free(m5);
    printList();

    char *m7 = malloc(400*1024);
    printList();

    char *m8 = malloc(30*1024);
    printList();

    m6 = realloc(m6, 10*2014);
    printList();
    
    free(m7);
    printList();
    
    free(m8);
    printList();

    free(m6);
    printList();

    char *m9 = malloc(512);
    printList();

    free(NULL); //SPECIAL CASE: THIS SHOULD DO NOTHING
    printList();
    
    char *m10 = realloc(NULL, 40*1024); //SPECIAL CASE: THIS SHOULD WORK LIKE MALLOC
    printList();

    free(m9);
    printList();

    free(m10);
    printList(); 

    exit(EXIT_SUCCESS);
}
