/* Defines functions for sorting arrays in parallel.
 * CSC 357, Assignment 7
 * Given code, Winter '24 */

#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <sys/wait.h>

/* fsort: Sorts an array using a parallelized merge sort.
 * TODO: Implement this function. It should sort the "n" elements of "base",
 *       each of "width" bytes, creating a child process to sort the latter
 *       half of the array if and only if "n > min", and returning 1 if the
 *       requisite child processes could not be created and 0 otherwise. */

void merge(void *base, size_t leftEnd, size_t middle, size_t rightEnd, size_t width, int (*cmp)(const void *, const void *)) {
    size_t leftSize = middle - leftEnd + 1; //calculate the left side from the smallest to the mid value (including the mid value)
    size_t rightSize = rightEnd - middle; //then the right size which is the right - mid 
    
    void *leftTempArray = malloc(leftSize * width); //make space for left array
    void *rightTempArray = malloc(rightSize * width); //make space for right array
    
    memcpy(leftTempArray, (char *)base + leftEnd * width, leftSize * width); //copy stuff from left to the left temporary array
    //destination, source, size
    //destination is the left temp array, the source is the base array at the left index at that width, and the stuff to copy is all the leftSize times the width.
    memcpy(rightTempArray, (char *)base + (middle + 1) * width, rightSize * width); //copy stuff from the right to the right temporary array 
    //mid + 1 because the left side is indluding the middle value and the right side is not
    
    size_t leftIdx = 0; 
    size_t rightIdx = 0;
    size_t baseIdx = leftEnd; //base is set to the left end because this is recursion and the left end will change every time only in the first case is it the start of the base array 
    
    while (leftIdx < leftSize && rightIdx < rightSize) { //while I am within the bounds of both halves of the arrays 
        char* leftValue = (char *)leftTempArray + leftIdx * width; //start from the pointer to that index, then add whatever index you are at times how wide the data is. 
        char* rightValue = (char *)rightTempArray + rightIdx * width; //this math was explained in the spec sheet
        if (cmp(leftValue, rightValue) <=0){  //cmp returns 0 or -1 if the first value is less than or equal to the second one 
            memcpy((char *)base + baseIdx * width, (char *)leftValue, width); //then copy the smaller value into the base array 
            //destination is the base index, the source is the left value, and the size is the width of the data
            leftIdx++; //since you copied the left one, and that one was smaller, I go to the next left value 
        } else { //the right one was smaller
            memcpy((char *)base + baseIdx * width, (char *)rightTempArray + rightIdx * width, width);  //copy the right value into the base 
            rightIdx++; //then memcpy the right one into the base array 
        }
        baseIdx++; //once a comparison has happened, increment forward in the base array
    }
    
    while (leftIdx < leftSize) { //if there is still stuff left in my left side copy that over
        char* leftValue = (char *)leftTempArray + leftIdx * width;  
        memcpy((char *)base + baseIdx * width, (char *)leftValue, width);
        leftIdx++;
        baseIdx++;
    }
    
    while (rightIdx < rightSize) { //if there is still stuff in my right side copy that over 
        char* rightValue = (char *)rightTempArray + rightIdx * width;
        memcpy((char *)base + baseIdx * width, (char *)rightValue, width);
        rightIdx++;
        baseIdx++;
    }
    
    free(leftTempArray); //free the stuff I used 
    free(rightTempArray);
}


int fsort(void *base, size_t n, size_t width, size_t min, int (*cmp)(const void *, const void *)) {
    if(n <= min){
        qsort(base, n, width, cmp); // pass it the rigth stuff 
        return EXIT_SUCCESS;
    }

        size_t mid = n / 2;

        int OGdata[2]; //pipe that will hold the original unsorted data
        int sortedData[2]; //second pipe that holds the sorted data

        if (pipe(OGdata) == -1 || pipe(sortedData) == -1){
            return EXIT_FAILURE;
        } //if the pipe fails then it fails


        pid_t child = fork(); // make child

        if (child == -1){ //if the fork fails then fail the whole thing
            close(OGdata[0]);
            close(OGdata[1]);
            close(sortedData[0]);
            close(sortedData[1]);
            return EXIT_FAILURE;
        }

        if (child == 0){ //if the fork didn't fail now go within child
            close(OGdata[1]); //child closes the write of the og data since it only needs to read from this
            close(sortedData[0]); //child closes the read end from here since it wants to write to it
            char * startOfRight = (char *)base + mid * width; 
            int sizeToSort = n - mid;
            size_t widthOfRightSortingThing = sizeToSort * width;

            read(OGdata[0], startOfRight, widthOfRightSortingThing); // read the right unsorted data out
            close(OGdata[0]); //close since I am done reading

            fsort(startOfRight, sizeToSort, width, min, cmp); //within the child sort the right side including the mid index so the larger half) 
            write(sortedData[1], startOfRight, widthOfRightSortingThing); //write the new sorted data to here
            close(sortedData[1]); //close the writing end after writing to it
            exit(EXIT_SUCCESS);
        }

        else {
    
            //in the parent again now
            close(OGdata[0]); //parent closes the read since it is only writing to it
            close(sortedData[1]); //parent closes the write end of the sorted since its only reading from here
            

            //right variables
            char * startOfRight = (char *)base + mid * width; 
            int sizeToSort = n - mid;
            size_t widthOfRightSortingThing = sizeToSort * width;

            write(OGdata[1], startOfRight, widthOfRightSortingThing); // write the unsorted stuff to the child so it can sort it
            close(OGdata[1]); //can close it after I wrote to it 

            fsort(base, mid, width, min, cmp); // if in the parent, sort the left side (not including the mid index)

            //once child is done sorting, I can now read the sorted parts back out and merge
            read(sortedData[0], startOfRight, widthOfRightSortingThing);
            close(sortedData[0]); //can close the reading since I am done reading

            wait(NULL); // wait for the child to finish 

            merge(base, 0, mid - 1, n - 1, width, cmp);
      }
        return EXIT_SUCCESS;
    }


    //pseudocode for writing this:
    // if n > min, then do the following:
    //     split the given array into 2 halves
    //     calculate the midpoint
    //     create a pipe from the parent to child  - need 2 pipes, first one is to transfer the unsorted data
    //     create a child by forking - parent can clsoe the read end
    //         if that fork fails, return an error 
    //     success, go within the child process
    //         immediately close the write end of the pipe since child only needs to read
    //         sort the half of the array 
    //         write the sorted part to the pipe  - different pipe, one to transfer just the sorted data
    //         child can close read end since it will be writing to this pipe
    //     in parent
    //         can close the write end of the pipe since its only reading
    //         sort the first half of the array 
    //         wait for the child process to finish 
    //         read the second half of the array from the pipe and store it somewhere
    //         call merge on the two halves 
