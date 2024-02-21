#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <sys/time.h>
// Compile with -DREALMALLOC to use the real malloc() instead of mymalloc()
#ifndef REALMALLOC
#include "mymalloc.h"
#endif
#include <time.h>

#ifndef DEBUG
#define DEBUG 0
#endif

void test1() {
    if(DEBUG)   printf("Starting test 1!\n");
    for(int i = 0; i < 120; i++) {
        char *ptr = malloc(1);  // Allocate 1 byte of memory
        free(ptr);  // Release the memory
    }
    if(DEBUG)   memClean(); 

}

void test2() {
    if(DEBUG)   printf("Starting test 2!\n");
    char *ptrArray[120];  // Array to store 120 pointers

    for(int i = 0; i < 120; i++) {
        ptrArray[i] = malloc(1);  // Allocate 1 byte of memory and store the address
    }

    for(int i = 0; i < 120; i++) {
        free(ptrArray[i]);  // Release the memory
        //if (i == 1) break; 
    }

    if(DEBUG)   memClean(); 
}


void test3() {
    if(DEBUG)   printf("Starting test 3!\n");
    char* ptrArray[120]; //Array to store 120 pointers
    int numAlloc = 0; //track num of alloc
    
    while (numAlloc != 120) { //run till we allocated 120 times
        int randNum = (rand() % 2);  //random number
        
        if (randNum == 0) { 
            ptrArray[numAlloc] = malloc(1); //allocate
            numAlloc++; 
        
        } else {
            
            if (numAlloc > 0) { //free
                free(ptrArray[numAlloc - 1]); 
                numAlloc--;
            }
        }
    }
   
    for (int i=0; i < 120; i++) { //clean up array
        free(ptrArray[i]);
    }

    if(DEBUG)   memClean(); 
}

int getRandomEvenNumber() {
    int num = (rand() % 5) * 2; // Generate a random number between 0 and 4, then double it
    return num;
}

void test4(){
    if (DEBUG)  printf("Starting test 4!\n");
    char* ptrArray[120]; // Array to store 120 pointers
    int numAlloc = 0; //number of allocations to track
    int numLeft = 120; //Left side index
    int currentIndex = 0; //track current

    while(numAlloc != 120){ //run until we allocated 120 times
        int sizeToAlloc = getRandomEvenNumber(); //get random num btwn 0 and 4 & double

        if (sizeToAlloc == 0) { //0 byte case
            if(DEBUG)   printf("Attempt to malloc 0 bytes.\n");
            continue; // Skip the rest of the loop iteration
        }

        if(numLeft >= sizeToAlloc){ //checking if there is enough space to allocate desired size (rand generated)
            for(int i = 0; i < sizeToAlloc; i++){      //allocate as many indicies as sizeToAlloc
                ptrArray[currentIndex] = malloc(1);
                currentIndex++;
            } 
        
        numLeft -= sizeToAlloc; //keep track of how much space is left in array
        numAlloc += sizeToAlloc; //increase the number of spaces allocated

        } else {
            if(DEBUG)   printf("Requested allocation size not avaliable in heap.\n");
            continue;
        }
   }

   for (int i=0; i < 120; i++) { //free array and clean
        free(ptrArray[i]);
    }

    if(DEBUG)   memClean(); 
}

void shuffle(int arr[], int n) { //randomize indexes and place into an int array
    srand(time(NULL));
    for (int i = n - 1; i > 0; i--) {
        int j = rand() % (i + 1);
        int temp = arr[i];
        arr[i] = arr[j];
        arr[j] = temp;
    }
}

void test5() {
    if(DEBUG)   printf("Starting test 5!\n");
    char* ptrArray[120]; //ptr array of 120
    int numAlloc = 0;
    int numCheck[120]; //array to keep track of populated indices
    
    for(int i=0; i<120; i++) { //allocates indices of ptrArray and initialized numCheck array with different index values
        ptrArray[i] = malloc(1);
        numCheck[i] = i;
        numAlloc++;
    }

    shuffle(numCheck, 120); //shuffles values in numCheck array to be randomized

    for(int i=0; i<120; i++) {
        int indexFree = numCheck[i]; //gets an index value from numCheck and makes that the index to be freed
        free(ptrArray[indexFree]); //frees that index
    }

   if(DEBUG)    memClean(); 
}

void test6(){
    if(DEBUG)   printf("Starting test 6!\n");
    char* ptrArray[120]; //array to store 120 pointers
    
    for(int i = 0; i < 120; i++){ //allocate each index in the array with malloc
        ptrArray[i] = malloc(1);
    }

    for(int i = (120/2) - 1; i >= 0; i--){ //free the ptr array starting from the center going down (index 59 -> 0)
        free(ptrArray[i]);
    }
    
    for(int i = (120/2); i < 120; i++){ //free the ptr array starting from the center going up (index 60 -> 120)
        free(ptrArray[i]);
    }

    if(DEBUG)   memClean(); //- error
}

int main(int argc, char **argv) {
    if(!DEBUG) {
        struct timeval start; //structs for getTimeOfDay()
        struct timeval end;
        
        gettimeofday(&start, NULL); //get the start timing
        for(int i=0; i<50; i++) { //run each test 50 times
            test1();
        }
        gettimeofday(&end, NULL); //get end time
        printf("Average run time for test1: %ld seconds %ld microseconds\n", (end.tv_sec-start.tv_sec)/50, (end.tv_usec-start.tv_usec)/50);
    //print the avg run time for test using structs start and end

        gettimeofday(&start, NULL); 
        for(int i=0; i<50; i++) {
            test2();
        }
        gettimeofday(&end, NULL);
        printf("Average run time for test2: %ld seconds %ld microseconds\n", (end.tv_sec-start.tv_sec)/50, (end.tv_usec-start.tv_usec)/50);

        gettimeofday(&start, NULL);
        for(int i=0; i<50; i++) {
            test3();
        }  
        gettimeofday(&end, NULL);
        printf("Average run time for test3: %ld seconds %ld microseconds\n", (end.tv_sec-start.tv_sec)/50, (end.tv_usec-start.tv_usec)/50);

        gettimeofday(&start, NULL);
        for(int i=0; i<50; i++) {
            test4();
        }
        gettimeofday(&end, NULL);
        printf("Average run time for test4: %ld seconds %ld microseconds\n", (end.tv_sec-start.tv_sec)/50, (end.tv_usec-start.tv_usec)/50);

        gettimeofday(&start, NULL);
        for(int i=0; i<50; i++) {
            test5();
        }
        gettimeofday(&end, NULL);
        printf("Average run time for test5: %ld seconds %ld microseconds\n", (end.tv_sec-start.tv_sec)/50, (end.tv_usec-start.tv_usec)/50);

        gettimeofday(&start, NULL);
        for(int i=0; i<50; i++) {
            test6();
        }
        gettimeofday(&end, NULL);
        printf("Average run time for test6: %ld seconds %ld microseconds\n", (end.tv_sec-start.tv_sec)/50, (end.tv_usec-start.tv_usec)/50);
        
        memClean();
    }
    else {
        test1();
        test2();
        test3();
        test4();
        test5();
        test6();
    }
}
