#include "helper.h"

// calculates growth
void calculateGrowth(Investment *invp) {

  int i;


  for (i = 1; i <= invp->years; i= i + 1) {   
    invp->invarray[i] = invp->growth * invp->invarray[i-1]; 
  }
} 


// gets the user input
int getUserInput(Investment *invp) {

  int valid;
  char in_msg[100] = {};
  char out_msg[100] = {};

  sprintf(out_msg, "Enter investment, growth rate, number of yrs (up to %d): ",MAX_YEARS);
  NU32_WriteUART3(out_msg);

  NU32_ReadUART3(in_msg, 100);
  sscanf(in_msg, "%lf %lf %d", &(invp->inv0), &(invp->growth), &(invp->years));

  valid = (invp->inv0 > 0) && (invp->growth > 0) && (invp->years > 0) && (invp->years <= MAX_YEARS);

  sprintf(out_msg, "Valid input?  %d\r\n",valid);
  NU32_WriteUART3(out_msg);

  if (!valid) { 
    sprintf(out_msg, "Invalid input; exiting.\r\n");
    NU32_WriteUART3(out_msg);
  }
  return(valid);
} 


// sends to the output
void sendOutput(double *arr, int yrs) {

  int i;
  char outstring[100];
  char msg[100];      

  sprintf(msg, "\r\nRESULTS:\r\n\r\n");
  NU32_WriteUART3(msg);

  for (i=0; i<=yrs; i++) {  
    sprintf(outstring,"Year %3d:  %10.2f\r\n",i,arr[i]); 
    NU32_WriteUART3(outstring);
  }
  sprintf(msg, "\r\n");
  NU32_WriteUART3(msg);

} 