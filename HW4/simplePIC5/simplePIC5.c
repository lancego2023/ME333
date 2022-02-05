#include <xc.h>          // Load the proper header for the processor

// Defines //
#define MAXCYCLES 1000000
#define DELTACYCLES 100000
////////

void delay(int cycles);

int main(void) {
  TRISF = 0xFFFC;        // Pins 0 and 1 of Port F are LED1 and LED2.  Clear
                         // bits 0 and 1 to zero, for output.  Others are inputs.
  LATFbits.LATF0 = 0;    
  LATFbits.LATF1 = 1;

  int cycle_amount = MAXCYCLES; // declare int cycle_amount, to be passed to delay()

  while(1) {
    delay(cycle_amount);
    LATFINV = 0x0003;    // toggle LED1 and LED2; same as LATFINV = 0x3;

    if(cycle_amount>0){ 
      cycle_amount = cycle_amount - DELTACYCLES; // if cycles>0, then cycle_amount is decremented by delta
    } else {
      cycle_amount = MAXCYCLES; // otherwise reset back to max
    }
  }
  return 0;
}

void delay(int cycles) {
  int j;
  for (j = 0; j < cycles; j++) {
    while(!PORTDbits.RD7) {
        ;   // Pin D7 is the USER switch, low (FALSE) if pressed.
    }
  }
}
