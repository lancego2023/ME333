#ifndef HELPER_H
#define HELPER_H

#include <stdio.h>

// defines //
#define MAX_YEARS 100   

/////////////


// creates the investment struct
typedef struct {
  double inv0;                    
  double growth;                  
  int years;                      
  double invarray[MAX_YEARS+1];   
} Investment; 


// Functions //
int getUserInput(Investment *invp); // gets user input
void calculateGrowth(Investment *invp); // calculates the growth
void sendOutput(double *arr, int years); // sends to the output
///////////////

#endif // HELPER_H 