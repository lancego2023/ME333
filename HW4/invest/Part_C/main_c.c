
#include "helperc.h"
#include "NU32.h"


int main(void) {

  NU32_Startup();
  Investment inv; 
            
  while(getUserInput(&inv)) {    
    inv.invarray[0] = inv.inv0;  
    calculateGrowth(&inv);       
    sendOutput(inv.invarray,     
               inv.years);       
  }

  return 0;                      
} 