CC = gcc
CFLAGS = -std=c11 -pedantic -Wall -Werror -D_GNU_SOURCE

.PHONY: all clean

all: test

clean:
	rm -f test.o test halde.o halde

test: test.o halde.o
	$(CC) $(CFLAGS) -o test.o halde.o

#test-ref: test.o halde-ref.o
#	$(CC) $(CFLAGS) -o test.o halde-ref.o

test.o: test.c halde.h
	$(CC) $(CFLAGS) -c test.c

halde.o: halde.c halde.h
	$(CC) $(CFLAGS) -c halde.c