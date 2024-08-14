Names and NetIDs:   Krithish Ayyappan          ka759
                    Samhitha Sangaraju         ss4043


------------------------------------------- File Name: mymalloc.c -------------------------------------------
CHANGES:
 - In malloc(): Checks size if given size is larger than memory
 - In free():
    - Checks if given pointer is already freed (double-free)
    - Checks if pointer is non-object or invalid pointer given (added else condition)
    - Before coalescing, it will check if the next pointer exists in the first place
        - Added if statement to check if exists, then it will check the allocation

Our Code Design

void *mymalloc(size_t size, char *file, int line): 
- checks if size requested is negative, as negative bites cannot be allocated, and prints an error in that case
- otherwise, requested size is rounded up to 8 bytes for 8-byte alignment
- while the end of the heap has not been reached, using the requested payload size, we accounted for 3 situations
    1. when the chunkSize is 0 and nothing is allocated (then update metadata with requested payload size, and set next metadata after the allocated data)
    2. when the chunkSize is greater than/equal to the size (the heap already has some allocated memory, but we need to account for 
        allocating the requested size and updating next metadata)
    3. when there is not enough space to allocate the desired size or we hit an allocated space (iterate to next metadata)
- if none of these conditions are met, an error "Insufficient memory space requested" message is printed


void myfree(void *ptr, char *file, int line): 
- check is pointer given is null or out of bounds, and prints an error in that case
- if pointer valid begins freeing process by taking the chunksize of the start
- while the end of the heap has not been reached, we account for 3 situations:
    1. when the current block (head) is free and the pointer we want to free is next
        - then, we merge these blocks, we also check if the next one is also free to coalesce these as well!
        - then, we mark as free
    2. when the current block (head) reaches the target pointer (input by user)
        - then, we merge these blocks and also check if the next one is also free to merge
        - then, we mark as free 
    3. when we are not at the desired pointer or the adjacent is one is not free/the pointer input
        - then, we iterate our current block to the next chunk
- if none of these conditions are met, an error "Pointer: (input) not in heap!"



int memClean():
- returns an integer (0 for false and 1 for true)
- checks if the memory is unintialized or the memory is fully free
    - will return 1 if true, and print "fully cleared" message, along with the alloc of head, which should be 0
    - if the chunkSize of the start = MEMLENGTH -> fully free!
- if memory is not freed
    - return 0 or false meaning that there is still memory allocated!



Helper Methods:
- getChunkSize()/setChunkSize()/getAlloc()/setAlloc():  allow us to edit and retrieve the values of the chunkSize and 
                                                        getAlloc to see the size/status of the allocation

- isNextFree(): checks if 2 adjacent blocks are both free by checking allocation status and location in the heap
                prints the results regarding the ptr arguments

- merge(): works to merge two  free chunks by taking in their ptrs as arguments
           sets the chunkSize in the first chunk's metadata to the size of both chunks + 8, since we are merging into
           one larger free chunk

*** Note: isNextFree() and merge() work together to coalesce 2 adjacent free blocks

------------------------------------------- Error Reporting -------------------------------------------------

Within mymalloc():
- checking if size is valid to account for negative sizes requested -> printf("Error: Can't allocate 0 bytes");
- function takes in ptrs to head and end, and goes through while loop checking chunkSize and allocation
    -> if size requested isn't avaliable in the heap, 
        fprintf(stderr, "Insufficient memory space requested (%ld bytes) in File: '%s', Function: 'malloc()', on Line: '%d'\n", size, file, line);
        where the file name, function name, and line are printed along with the insufficient space requested

Within myfree():
- checks if ptr argument is within bounds, in order to be able to free it, if not 
    -> fprintf(stderr, "Pointer %p given out of bound in File: '%s', Function: 'myfree()', on Line: '%d'\n", ptr, file, line);
       where the file name, function name, and line are printed along with the out of bounds ptr
- if the ptr user wants to free is not in the heap, that also results in an error, as there exists nothing to free
    -> fprintf(stderr, "Pointer: (%p) not in heap! File: '%s', Function: 'myfree()', on Line: '%d'\n", ptr, file, line);
       where the file name, function name, and line are printed along with the ptr that cannot be found in the heap

Within memClean():
- testing for leaks in memory
- can be used as an error test whenever we want to check if all memory has been allocated or freed
    - used this to check allocation and free in our stress tests
- if memory is fully clear this print statement should run -> printf("Memory is fully cleared! Allocation of head: %d\n", alloc);
- if memory is not cleared and still allocated, this print statement should run -> printf("Memory is not cleared! %d\n", alloc);

------------------------------------------- File Name: memgrind.c -------------------------------------------
Testing Design: 
- we wrote our different tests in separate methods
- in our main function, we run each test 50 times
- we use the getTimeOfDay() function to get the start and end of the run time
    - we calculated the difference in seconds and microseconds (dividing by 50 to take the average)
- we call memClean() at the end of the main function to make sure all allocations are free

- if we run with DEBUG 
    - each function has a debug call where we check if memClean() at the end of every test
    - this checks if our memory is fully free or not
- the main function changes from running each test case 50 times and showing the average time to running each test once

- test1, test2, test3 all given in write-up

Our Extra Stress Tests (besides the tests given):

test4()  
Design: 
- start with a char* ptrArray  of 120
- while the entire ptrArray is not yet allocated
    - with helper method getRandomEvenNumber -> return a randomized even number to be sizeToAlloc
    - if sizeToAlloc = 0, continue and get a new random even number
    - if there is enough space to allocate the desired size, allocate as many spaces as sizeToAlloc calls for
        - ex: if sizeToAlloc = 2, malloc twice into ptrArray
    - update the amount of space left and the number of allocated space to keep track of array
- free the entire array

Purpose: 
- To test the diversity of malloc-ing different chunk sizes and also check bound errors when allocating

test5()  
Design:
- helper method shuffle: takes in an int array and n size -> randomizes n numbers, and places them into an array
- start with a char* ptrArray of size 120 and an int numCheck array of same size (which will be used to place the shuffled random numbers)
- populate the ptrArray and numCheck array
- shuffle the numCheck array to have numbers 0 - 119 randomly placed (these will act as indices in our ptrArray)
- iterate through numCheck indices (which has randomized numbers) -> access the number stored in that index
- make this number the index to free in ptrArray
    - this will cause ptrArray's indicies to be freed randomly

Purpose: 
- To test free-ing random indexes and stress free's search capacity and coalesce ability

test6()  
Design: 
- start with a char* ptrArray of size 120
- malloc 1 into each index of the ptrArray 
- free the ptrArray starting at the center index, free-ing down and free-ing up 
    - (free-ing in opposite directions)

Purpose:
- Checks free's ability of error-bounding and not free-ing in a linear method


------------------------------------------- What We Learned :p -------------------------------------------
1. How coalescing works with multipe free blocks and checking for adjacent blocks
2. How to create boundary checks with malloc and free calls
3. Checking for valid pointers in free
4. Returning error if freeing twice or freeing something that has not been allocated
5. Creating a function check if all memory has been freed in our "heap"
6. Using pointer casting/iteration to traverse through heap
7. Understanding the metadata and how to access the allocation and chunkSize
8. Traversing safely to get the next metadata without breaking bounds