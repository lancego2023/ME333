//bubble sort
// lance go

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
// max length
#define MAX_LENGTH 100

// HELPER PROTOTYPES
void getString(char *str);
void printResult(char *str);
int greaterThan(char ch1, char ch2);
int swap(char *str, int index1, int index2); // i changed this to return an int, not sure if that's allowed or not

int main(){

    int len;   // length of entered string
    char str[MAX_LENGTH]; // input
    int sorted = 0; // keeps track if the entire string is sorted or not, reset at the end of each loop

    getString(str);
    len = strlen(str);
    // loop stuff:
    // while(!sorted){ // iterates over and over until none are out of order
        
    while(!sorted){
        sorted = 1; // initialize sorted as 1 for every sort
        for(int ii = 0; ii < len; ii++){ //go through entire string and test for swaps
            if(ii+1 < len){ // dont swap on the last one (memory)
                if(!swap(str, ii, ii+1)){
                    sorted = 0; // set sorted to 0 if the sort does happen
                }
            }
        }
    }
    
    printResult(str);
    return(0);

}

// gets string from user input
void getString(char *str){
    printf("\n");
    // prompt for the string:
    printf("Enter your string now: ");
    // have the user enter the string:
    scanf("%100s", str);
    // formatting:
    printf("\n \n");
    // ur done
}

// prints result
void printResult(char *str){
    // this is it
    printf("Here is the sorted string: %s\n \n \n", str);
}

// checks if one element is greater than the other. Returns 1 if char1>char2, 0 otherwise (includes =)
int greaterThan(char ch1, char ch2){
    int result; // holds the result (this is technically not necessary but i want it for debug)
    if(ch1 > ch2){
        result = 1;
    } else {
        result = 0;
    }
    // debug:
    // printf("%d and %d, result: %d \n", ch1, ch2, result);
    // done
    return result;
}

// swaps two elements. If the swap does take place, then it returns 0. If no swap, then 1
int swap(char *str, int index1, int index2){
    char temp1 = str[index1]; // store char at index1 in temp1
    char temp2 = str[index2]; // same for temp2

    if(greaterThan(temp1, temp2)){ // if char at index1 is less than char at index2, than swap
        str[index1] = temp2;
        str[index2] = temp1;
        return 0;
    } else { // dont swap, return true
        return 1;
    }
}
