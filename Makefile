CC =gcc
CFLAGS =-g -Wall -Wvla -fsanitize=address,undefined -std=c99 -O2

all: memgrind p1-test memtest realmalloc

memgrind: mymalloc.c memgrind.c
	$(CC) $(CFLAGS) memgrind.c mymalloc.c -o memgrind

p1-test: mymalloc.c p1-test.c
	$(CC) $(CFLAGS) p1-test.c mymalloc.c -o p1-test

memtest: memtest.c mymalloc.c
	$(CC) $(CFLAGS) memtest.c mymalloc.c -o memtest

realmalloc: memgrind.c mymalloc.c
	$(CC) $(CFLAGS) -DREALMALLOC memgrind.c mymalloc.c -o realgrind

clean: 
	rm -f memgrind p1-test memtest realgrind