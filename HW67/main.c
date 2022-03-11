#include <stdio.h>
#include<xc.h> // need? Maybe not
#include "NU32.h"
#include "LCD.h"


// DEFINES //
#define NUMSAMPS 1000 // number of points in waveform
#define PER3 3999 // Period setting
#define PLOTPTS 200 // number of data points to plot
#define DECIMATION 10 // plot every 10th point


// PROTOTYPES //
void makeWaveform(void); // Generates the waveform that will be used for reference
void printGainsToLCD(void); // Prints the Kp and Ki to the LCD when sent via MATLAB
unsigned int PIControl(int target ,int actual); // PI control calculation
int ADCsample(void); // samples pin 14 of the PIC32


// GLOBALS //
static volatile int Waveform[NUMSAMPS]; // waveform
static volatile int ADCarray[PLOTPTS]; // measured values to plot
static volatile int REFarray[PLOTPTS]; // reference values to plot
static volatile int StoringData = 0; // if this flag = 1, currently storing plot data
static volatile float Kp = 0, Ki = 0; // control gains
static volatile float eint = 0.0; //integral error


// INTERRUPTS //
void __ISR(_TIMER_2_VECTOR, IPL5SOFT) Controller(void) { // _TIMER_2_VECTOR = 8
    static int counter = 0; // initialize counter once
    static int plotind = 0; // index for data arrays; counts up to PLOTPTS
    static int decctr = 0; // counts to store data one every DECIMATION
    static int adcval = 0; //
    static int elapsed = 0;
    static int finish = 0;

    adcval = ADCsample(); // read sample, store in adcval

    OC1RS = PIControl(Waveform[counter], adcval);
    //OC1RS = Waveform[counter]; // sets 0C1RS to be the waveform generated

    if (StoringData) {
        decctr++;
        if (decctr == DECIMATION) { // after DECIMATION control loops,
            decctr = 0; // reset decimation counter
            ADCarray[plotind] = adcval; // store data in global arrays
            REFarray[plotind] = Waveform[counter];
            plotind++; // increment plot data index
        }
        if (plotind == PLOTPTS) { // if max number of plot points plot is reached,
            plotind = 0; // reset the plot index
            StoringData = 0; // tell main data is ready to be sent to MATLAB
        }
    }

    counter++;
    if (counter == NUMSAMPS){
        counter = 0;
    }
    IFS0bits.T2IF = 0;// clear interrupt flag
} // run this interrupt at 1kHz


// MAIN //
int main(void) {
    NU32_Startup(); // Initialize everything
    LCD_Setup(); // Setup the LCD screen 
    makeWaveform(); // generate the waveform
    //_CP0_SET_COUNT(0);

    AD1PCFGbits.PCFG14 = 0; // AN0 is an ADC pin
    AD1CON1bits.ADON = 1; // turn on A/D converter
    AD1CON3bits.ADCS = 2; 


    // MATLAB stuff
    char message[100]; // message to and from MATLAB
    float kptemp = 0, kitemp = 0; // temporary local gains
    int i = 0; // plot data loop counter
    int test = 0;

    // Timer3 Setup //
    T3CONbits.TCKPS = 0; // Timer3 prescaler N=0 (1:1)
    PR3 = PER3; 
    TMR3 = 0; 
    OC1CONbits.OCTSEL = 1; // select timer3
    OC1CONbits.OCM = 0b110; 
    OC1RS = 3000;
    OC1R = 3000; 
    T3CONbits.ON = 1; 
    OC1CONbits.ON = 1;

    // Timer2/Interrupt setup //
    __builtin_disable_interrupts();     // disable interrupt at CPU
    T2CONbits.TCKPS = 3;                // Set up the peripheral. Prescaler at 3, or a 1:1 ratio
    PR2 = 9999;                          // Prescaler (Timer must be at 1 kHz)
    TMR2 = 0;                           // Initialize Timer at 0
    T2CONbits.ON = 1;                   // Turn Timer2 on
    IPC2bits.T2IP = 5;                  // Set priority
    IPC2bits.T2IS = 0;                  // subpriority
    IFS0bits.T2IF = 0;
    IEC0bits.T2IE = 1;                  // enable interrupt
    __builtin_enable_interrupts();      // renable interrupt at CPU

    _CP0_SET_COUNT(0); 

     while(1) { // in charge of writing to matlab
        NU32_ReadUART3(message, sizeof(message)); // wait for a message from MATLAB
        sscanf(message, "%f %f" , &kptemp, &kitemp);
        __builtin_disable_interrupts(); // keep ISR disabled as briefly as possible
        Kp = kptemp; // copy local variables to globals used by ISR
        Ki = kitemp;
        __builtin_enable_interrupts(); // only 2 simple C commands while ISRs disabled
        eint = 0;
        StoringData = 1; // message to ISR to start storing data
        
         printGainsToLCD(); // Writes Kp and Ki to the screen (PUT IT HERE)

        while (StoringData) { // wait until ISR says data storing is done
            ; // do nothing 
        }
        for (i=0; i<PLOTPTS; i++) { // send plot data to MATLAB
            // when first number sent = 1, MATLAB knows we’re done
            sprintf(message, "%d %d %d\r\n", PLOTPTS-i, ADCarray[i], REFarray[i]);
            NU32_WriteUART3(message);
        }
    }; // infinite loop 
return 0;
}

// HELPERS
void makeWaveform() {
    int j = 0, center = 500, A = 300; // square wave, fill in center value and amplitude

    for (j = 0; j < NUMSAMPS; ++j) {
        if ( j < NUMSAMPS/2) {
            Waveform[j] = center + A;
        } else { 
            Waveform[j] = center - A;
        }
    }
}

void printGainsToLCD(void) {
    char Kp_display[20];
    char Ki_display[20];
    sprintf(Kp_display, "Kp: %.3f", Kp); // format Kp string for print
    sprintf(Ki_display, "Ki: %.3f", Ki); // format Ki string for print

    LCD_Move(0, 0);
    LCD_WriteString(Kp_display);
    LCD_Move(1, 0);
    LCD_WriteString(Ki_display);
}

unsigned int PIControl(int target, int actual){
    float error;
    float integral;
    float u;
    float unew;
    unsigned int result;

    error = (float) (target - actual);
    eint = eint + error;

    u = ((Kp * error + Ki * eint)/1024) * 100;
    unew = u + 50.0;
    // correction:
    if (unew > 100.0) {
        unew = 100.0;
    } else if (unew < 0.0) { 
        unew = 0.0;
    }

    // final output:
    result = (unsigned int) ((unew/100.0) * PER3);
    return result;
}

int ADCsample(){

    int start_time = _CP0_GET_COUNT();
    int finish_time = start_time + 10; // sample time = 10 ticks, 250 ns

    AD1CHSbits.CH0SA = 14; // select pin
    AD1CON1bits.SAMP = 1; // Sample
    while (_CP0_GET_COUNT() < finish_time) { // sample until the finish time
        ;
    }

    AD1CON1bits.SAMP = 0; // Stop sampling
    while (!AD1CON1bits.DONE) {
        ; // wait for conversion
    }
    
    return ADC1BUF0; // read sample
    
}


// Notes to myself:
// Error = desired - actual
// integral = (error - error_prior)/iteration time
// output  = KP*error + KI*integral
// integral_prior = integral