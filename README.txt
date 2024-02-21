Names and NetIDs:   Krithish Ayyappan          ka759
                    Samhitha Sangaraju         ss4043


------------------------------------------- File Name: mymalloc.c -------------------------------------------
Our Code Design

void *mymalloc(size_t size, char *file, int line): 
-   checks if size requested is negative, as negative bites cannot be allocated, and prints an error in that case
-   otherwise, requested size is rounded up to 8 bytes for 8-byte alignment
-   while the end of the heap has not been reached, using the requested payload size, we accounted for 3 situations
    1. when the chunkSize is 0 and nothing is allocated (then update metadata with requested payload size, and set next metadata after the allocated data)
    2. when the chunkSize is greater than/equal to the size (the heap already has some allocated memory, but we need to account for 
        allocating the requested size and updating next metadata)
    3. when there is not enough space to allocate the desired size or we hit an allocated space (iterate to next metadata)
- if none of these conditions are met, an error "Insufficient memory space requested" message is printed


void myfree(void *ptr, char *file, int line): 



int memClean():



Helper Methods:
- getChunkSize()/setChunkSize()/getAlloc()/setAlloc():  allow us to edit and retrieve the values of the chunkSize and 
                                                        getAlloc to see the size/status of the allocation

- isNextFree(): checks if 2 adjacent blocks are both free by checking allocation status and location in the heap
                prints the results regarding the ptr arguments

- merge(): works to merge two  free chunks by taking in their ptrs as arguments
           sets the chunkSize in the first chunk's metadata to the size of both chunks + 8, since we are merging into
           one larger free chunk

*** Note: isNextFree() and merge() work together to coalesce 2 adjacent free blocks

------------------------------------------- Error Reporting Design ------------------------------------------

Within mymalloc():
- checking if size is valid to account for negative sizes requested -> printf("Error: Can't allocate 0 bytes");
- function takes in ptrs to head and end, and goes through while loop checking chunkSize and allocation
    -> if size requested isn't avaliable in the heap, 
        fprintf(stderr, "Insufficient memory space requested (%ld bytes) in File: '%s', Function: 'malloc()', on Line: '%d'\n", size, file, line);
        where the file name, function name, and line are printed along with the error

Within myfree():
- checks if ptr argument is within bounds, in order to be able to free

------------------------------------------- File Name: memgrind.c -------------------------------------------
Our Extra Stress Tests (besides the tests given):

test4()  
Design:
Purpose:

test5()  
Design:
Purpose:

test6()  
Design:
Purpose: