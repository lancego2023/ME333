#include "pwm.h"

#define SAMPLES 100
static volatile float currentP;
static volatile float currentI;
static volatile float eint = 0.0;   
static volatile float samp_value[100]; // sample value
static volatile float ref_value[100]; // build the reference signal, bounces between 200 and -200 mA


void initialize_ISRTimer(){ // 5kHz timer to run our ISR
    T2CONbits.TCKPS = 0;                // Set up the peripheral. Prescaler at 1:1
    PR2 = 15999;                         
    TMR2 = 0;                           // Initialize Timer at 0
    T2CONbits.ON = 1;                   // Turn Timer2 on
    IPC2bits.T2IP = 5;                  // Set priority
    IPC2bits.T2IS = 0;                  // subpriority
    IFS0bits.T2IF = 0;                  // clear flag
    IEC0bits.T2IE = 1;                  // enable interrupt
}

void initialize_Timer3() { // Our 20kHz timer that starts the PWM signal
    T3CONbits.TCKPS = 0; // Timer3 prescaler N=0 (1:1)
    PR3 = 3999; // set at 5kHZ
    TMR3 = 0; // initialize Timer3 at 0
    OC1CONbits.OCTSEL = 1; // select timer3
    OC1CONbits.OCM = 0b110; 
    OC1RS = 1000;
    OC1R = 1000; 
    T3CONbits.ON = 1; 
    OC1CONbits.ON = 1;
}

void set_current_gains(float portional, float integral){ //sets the current gains
    currentP = portional;
    currentI = integral;
} 

float get_currentP_gains(){ 
    return currentP;
}

float get_currentI_gains(){ 
    return currentI;
}

// PI controller, to also be used by the positional controller
int current_controller(float ref, float samp){
    float error;
    float u;
    int duty_cycle;
    
    error = ref - samp;
    eint = eint + error;
    u = (currentP*error + currentI*eint) / 700; // stall current = 720 mA (this should be calibrated somewhat frequently)

    duty_cycle = (int) (u * 100); // calculate the duty cycle (out of 100 or -100)
    return duty_cycle;
}


// CURRENT TEST FUNCTION BELOW //
// only for ITEST!!!!! //

void current_test(){
    static int time_ref = 0;

    // generate the reference signal (square wave from 200 to -200), store in our reference array
    if(time_ref < 25 || (time_ref >= 50 && time_ref < 75)){
        ref_value[time_ref] = 200;
    } else if(time_ref > 99) { // return condition
        set_mode(IDLE);
        send_matlab();
        time_ref = 0;
        eint = 0.0;
        return;
    } else {
        ref_value[time_ref] = -200;
    }

    samp_value[time_ref] = read_current(); // sample our current, store in our sample array
       
    set_PWM_cycle(current_controller(ref_value[time_ref],
                                    samp_value[time_ref])); 
    time_ref++;
}

void send_matlab() { // run the PI, and then send the results over
    char buffer[50];
    // first send how many samples (100)
    sprintf(buffer, "%d\r\n", 100);
    NU32_WriteUART3(buffer);

    // then for every sample, send as a pair with ref
    for ( int ii = 0; ii < 100; ii++) {
        sprintf(buffer, "%f %f\r\n", ref_value[ii], samp_value[ii]);
        NU32_WriteUART3(buffer);
    }

    return;
}
