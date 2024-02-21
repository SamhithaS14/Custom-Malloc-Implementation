CC =gcc
CFLAGS =-g -Wall -Wvla -fsanitize=address,undefined -std=c99 -O2

all: memgrind memtest

memgrind: mymalloc.c memgrind.c
	$(CC) $(CFLAGS) memgrind.c mymalloc.c -o memgrind

memtest: memtest.c mymalloc.c
	$(CC) $(CFLAGS) memtest.c mymalloc.c -o memtest

realgrind: memgrind.c mymalloc.c
	$(CC) $(CFLAGS) -DREALMALLOC memgrind.c mymalloc.c -o realgrind
