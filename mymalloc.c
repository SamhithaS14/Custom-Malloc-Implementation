#include <stddef.h>
#include <stdio.h>
#include "mymalloc.h"
#define MEMLENGTH 512

#ifndef DEBUG
#define DEBUG 0
#endif

static double memory[MEMLENGTH];

int getChunkSize(char* ptr) {
    if(DEBUG) printf("Chunk size at %p: %d\n", ptr, *((int*)ptr-2));
    return *((int*)ptr-2);
}

void setChunkSize(char* ptr, int size) {
    if(DEBUG) printf("Setting Chunk at %p size: %d to %d\n", ptr, *((int*)ptr-2), size);
    *((int*)ptr-2) = size;
}

int getAlloc(char* ptr) {
    if(DEBUG) printf("Getting Allocation at %p: %d\n", ptr, *((int*)ptr-1));
    return *((int*)ptr-1);
}

void setAlloc(char* ptr, int taken) {
    if(DEBUG) printf("Setting Allocation at %p: %d to %d\n", ptr, *((int*)ptr-1), taken);
    *((int*)ptr-1) = taken;
}

int isNextFree(char* ptr, char* ptr2) {
    //checks if the next adjacent block is also free
    int chunkSize = getChunkSize(ptr);
    if(getAlloc(ptr)==0 && ptr2==ptr+chunkSize+8) {
        if(DEBUG) printf("Adjacent blocks %p and %p are both free!\n", ptr, ptr2);
        return 1;
    } else {
        if(DEBUG) printf("Adjacent blocks %p and %p are not next to each other or free!\n", ptr, ptr2);
        return 0;
    }
}

void merge(char* ptr, char* ptr2) {
    //merge free block together
    int chunkSize = getChunkSize(ptr);
    int chunkSize2 = getChunkSize(ptr2);
    if(DEBUG) printf("Merge offset for %p to %p is %d\n", ptr, ptr2, chunkSize+chunkSize2+8);
    setChunkSize(ptr, chunkSize+chunkSize2+8);
}


void *mymalloc(size_t size, char *file, int line){

    if(size <= 0) {
        printf("Error: Can't allocate 0 bytes");
        return NULL;
    }
    //Round size to 8 bytes
    size = (size+7) & ~7;

    char* ret = NULL;
    char* head = ((char*)memory)+8;
    char* end = ((char*)memory)+MEMLENGTH*8;

    while(head < end) {
        int chunkSize = getChunkSize(head);
        int isAlloc =  getAlloc(head);
        if(chunkSize==0 && isAlloc==0) {
            //set the chunk size & alloc
            setChunkSize(head, size);
            setAlloc(head, 1);
            ret = head;
            //setting next chunksize after
            if(MEMLENGTH>size) {
                setChunkSize(head+size+8, MEMLENGTH*8-size-16);
                setAlloc(head+size+8, 0);
            }
            return ret;

        } else if(chunkSize >= size && isAlloc==0) {
            //set the chunk size 
            setChunkSize(head, size);
            setAlloc(head, 1);
            ret = head;
            //need to update next
            if(chunkSize>size) {
                setChunkSize(head+size+8, chunkSize-8-size);
                setAlloc(head+size+8, 0);
            }
            return ret;
            
        } else if(chunkSize<size || isAlloc==1) {
            //iterate to next metadata
            head+=chunkSize+8;
        }
    }


    fprintf(stderr, "Insufficient memory space requested (%ld bytes) in File: '%s', Function: 'mymalloc()', on Line: '%d'\n", size, file, line);
    return 0;
}
void myfree(void *ptr, char *file, int line){

    char* head = (char*)memory+8;
    char* end = (char*)(memory+MEMLENGTH);

    //check if pointer is valid to start
    if(ptr==NULL || (char*)ptr<head || (char*)ptr>end) {
        fprintf(stderr, "Pointer %p given out of bound in File: '%s', Function: 'myfree()', on Line: '%d'\n", ptr, file, line);
        return;
    }

    while(head < end) {
        //check for merging
        int headChunk = getChunkSize(head);
        if (DEBUG)  printf("head is size: %d, status %d, ptr is size: %d, status: %d\n", getChunkSize(head), getAlloc(head), getChunkSize(ptr), getAlloc(ptr));
        if(isNextFree(head, ptr)) {
            //merge
            //int ptrChunk = getChunkSize(ptr);
            merge(head, ptr);
            //get the next block of pointer and compare
            headChunk = getChunkSize(head);
            if(getAlloc(head+headChunk+8)==0) {
                merge(head, head+headChunk+8);
            }
            if(DEBUG)   printf("Freeing at head %p\n", head);
            //u need to make the pointer invalid
            setAlloc(ptr, 0);
            return;

        } else if(head == ptr) {
            //we found the target pointer
            if(DEBUG)   printf("Head: %p equal to Ptr: %p\n", head, ptr);
            int ptrChunk = getChunkSize(ptr);
            if(getAlloc(ptr+ptrChunk+8)==0) {
                //merge if next one free
                merge(ptr, ptr+ptrChunk+8);
            }
            //mark free
            setAlloc(ptr, 0);
            if(DEBUG)   printf("Freeing at%p\n", ptr);
            return;

        } else if(head+headChunk+8 < end) {
            //iterate to find
            if(DEBUG)   printf("Iterating\n");
            head+=headChunk+8;
        }
    }

    fprintf(stderr, "Pointer: (%p) not in heap! File: '%s', Function: 'myfree()', on Line: '%d'\n", ptr, file, line);
    return;
}

//Testing free function for leaky memory (if memory is cleared): 
int memClean() {
    char *head = (char*)memory+8;
    int chunkSize = getChunkSize(head);
    int alloc = getAlloc(head);

    if(DEBUG)   printf("The size of block is %d\n", chunkSize);

    // Case: Memory pool is uninitialized or fully cleared
    if ((chunkSize == 0 && alloc == 0) || ((chunkSize == (8*MEMLENGTH-8)) && alloc == 0)) {
        printf("Memory is fully cleared! Allocation of head: %d\n", alloc);
        return 1;
    } 
    
    printf("Memory is not cleared! %d\n", alloc);
    
    return 0;
}
