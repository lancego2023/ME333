#include "NU32.h"          // constants, funcs for startup and UART
#include <stdio.h>

int state = 0;
int baseline = 0;
int start = 0;
int curr_time = 0;
char out_msg[50];


void __ISR(_EXTERNAL_0_VECTOR, IPL6SRS) Ext0ISR(void) { // step 1: the ISR
    baseline = _CP0_GET_COUNT();

    while(curr_time - baseline < 400000) { // debounce script. Always waits 10ms for valid button push
        curr_time = _CP0_GET_COUNT();
    }

    if(PORTDbits.RD7 == 0) { //  If user is still pushed, then you know its a real bounce
        if(state == 0) {
            start = _CP0_GET_COUNT();
            NU32_WriteUART3("Time Started. Press USER again to stop the timer \r\n");
            state = 1;
            //
        } else if(state == 1){
            curr_time = _CP0_GET_COUNT();
            sprintf(out_msg, "Final time is: %.3f seconds \r\n", (float) (curr_time - start)/40000000);
            NU32_WriteUART3(out_msg);
            state = 2;
        } else {
            NU32_WriteUART3(out_msg);
        }
    }      
    baseline = _CP0_GET_COUNT();
    IFS0bits.INT0IF = 0;   // clear interrupt flag IFS0<3>        
}

int main(void) {
  NU32_Startup(); 
  __builtin_disable_interrupts(); // step 2: disable interrupts
  INTCONbits.INT0EP = 0;          // step 3: INT0 triggers on falling edge
  IPC0bits.INT0IP = 6;            // step 4: interrupt priority 2
  IPC0bits.INT0IS = 1;            // step 4: interrupt priority 1
  IFS0bits.INT0IF = 0;            // step 5: clear the int flag
  IEC0bits.INT0IE = 1;            // step 6: enable INT0 by setting IEC0<3>
  __builtin_enable_interrupts();  // step 7: enable interrupts
                                  // Connect RD7 (USER button) to INT0 (RD0)

    NU32_WriteUART3("Press the USER button to start the timer. \r\n");
    _CP0_SET_COUNT(0);
  while(1) {
      ; // do nothing, loop forever
  }

  return 0;
}
