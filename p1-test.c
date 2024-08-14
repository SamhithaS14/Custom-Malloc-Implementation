#include <stdio.h>
#include <stdlib.h>

#include "mymalloc.h"


#ifndef MEMLEN
#define MEMLEN 4096
#endif

#ifndef HEADERLEN
#define HEADERLEN 8
#endif

#define WHOLE (MEMLEN-HEADERLEN)
#define QUARTER (MEMLEN/4 - HEADERLEN)

#define BGCOUNT 32
#define BGLEN (MEMLEN/BGCOUNT - HEADERLEN)

#define SMCOUNT 64
#define SMLEN (MEMLEN/SMCOUNT - HEADERLEN)

// Utilities

void fail(char *message) {
    fprintf(stderr, "Autograder: %s\n", message);
    exit(EXIT_FAILURE);
}

void validate(char *pointer, size_t memory_size) {
    // Do a quick read/write, hoping that the program will
    // segfault if the pointer is invalid
    // Also ensures that students don't rely on contents
    // of allocated memory
    if (pointer == NULL) {
        fail("Failed to allocate");
    }

    for (size_t i = 0; i < memory_size; i++) {
        pointer[i] = 'a';
        if (pointer[i] != 'a') {
            fail("Failed to read/write");
        }
    }
}

// Tests which should SUCCEED

// Allocate 10 bytes and deallocate
void test_0() {
    char *p = malloc(10);
    validate(p, 10);
    free(p);
}

// Allocate all of memory, deallocate, allocate all of memory again
void test_1() {
    void *p = malloc(WHOLE);
    validate(p, WHOLE);
    free(p);
    p = malloc(WHOLE);
    validate(p, WHOLE);
}

// Allocate four large chunks, free the 1st, 3rd, 2nd, and 4th, allocate all of memory
void test_2() {
    void *p[4];

    for (size_t i = 0; i < 4; i++) {
        p[i] = malloc(QUARTER);
        validate(p[i], QUARTER);
    }

    free(p[0]);
    free(p[2]);
    free(p[1]);
    free(p[3]);

    void *q = malloc(WHOLE);
    validate(q, WHOLE);
}

// Allocate 64 64-byte chunks, free all, allocate 32 128-byte chunks, free all
void test_3() {
    void *p[SMCOUNT];

    for (size_t i = 0; i < SMCOUNT; i++) {
        p[i] = malloc(SMLEN);
        validate(p[i], SMLEN);
    }

    for (size_t i = 0; i < SMCOUNT; i++) {
        free(p[i]);
    }

    void *q[BGCOUNT];

    for (size_t i = 0; i < BGCOUNT; i++) {
        q[i] = malloc(BGLEN);
        validate(q[i], BGLEN);
    }

    for (size_t i = 0; i < BGCOUNT; i++) {
        free(q[i]);
    }
}

// Test overlap
void test_4(void)
{
	int i, j;
	char *p[SMCOUNT];
	// allocate and fill blocks
	for (i = 0; i < SMCOUNT; i++) {
		p[i] = malloc(SMLEN);
		for (j = 0; j < SMLEN; j++) {
			p[i][j] = i;
		}
	}
	
	// validate block contents
	for (i = 0; i < SMCOUNT; i++) {
		for (j = 0; j < SMLEN; j++) {
			if (p[i][j] != i) {
				fprintf(stderr, "Autograder: Unexpected value in object %d byte %d\n", i, j);
				exit(EXIT_FAILURE);
			}
		}
	}
}

// Tests which should REPORT ERRORS

// Allocate 5000 bytes
void test_5() {
    puts("Should fail: too large");
    void *p = malloc(5000);
    validate(p, 5000);
}

// Allocate 1000 bytes, double free
void test_6() {
    void *p = malloc(1000);
    validate(p, 1000);

    free(p);
    puts("Should fail: double free");
    free(p);
}

// Allocate 1000 bytes, free pointer + 100 bytes
void test_7() {
    void *p = malloc(1000);
    validate(p, 1000);

    puts("Should fail: non-object pointer");
    free(p + 100);
}

// Allocate 1000 bytes, free address of a local variable
void test_8() {
    void *p = malloc(1000);
    validate(p, 1000);

    puts("Should fail: non-heap pointer");
    free(&p);
}

// Allocate four 1000-byte chunks, free 1st and 4th, allocate 1500 bytes
void test_9() {
    void *p[4];

    for (size_t i = 0; i < 4; i++) {
        p[i] = malloc(QUARTER);
        validate(p[i], QUARTER);
    }

    free(p[0]);
    free(p[3]);

    puts("Should fail: memory fragmentation");
    void *q = malloc(QUARTER+500);
    validate(q, QUARTER+500);
}

// allocate 64 chunks, free them all, then try to allocate too much memory
// (catch incorrect math when coalescing)
void test_10() {
	void *p[SMCOUNT];
	
	for (int i = 0; i < SMCOUNT; i++) {
		p[i] = malloc(SMLEN);
		validate(p[i], SMLEN);
	}

	free(p[1]);
	free(p[0]);
	for (int i = 3; i < SMCOUNT; i += 2) {
		free(p[i]);
	}
	for (int i = 2; i < SMCOUNT; i += 2) {
		free(p[i]);
	}
	
	puts("should fail: too large");
	p[0] = malloc(WHOLE+1);
	validate(p[0], WHOLE+1);
}



typedef void (*Test)();

#define TESTCOUNT 11

Test tests[TESTCOUNT] = {
    test_0,
    test_1,
    test_2,
    test_3,
    test_4,
    test_5,
    test_6,
    test_7,
    test_8,
    test_9,
    test_10
};

int main(int argc, char **argv) {
    if (argc != 2) {
        fprintf(stderr, "Usage: %s [TEST]\n", argv[0]);
        return EXIT_FAILURE;
    }

    int test_index = atoi(argv[1]);
    if (test_index < 0 || test_index >= TESTCOUNT) {
        fprintf(stderr, "Invalid test index\n");
        return EXIT_FAILURE;
    }

    tests[test_index]();

    return EXIT_SUCCESS;
}
