#include "utilities.h"
#define SAMPS 15

enum mode_t mode;

enum mode_t get_mode() {
    return mode;
}

void set_mode(enum mode_t mood){
    mode = mood;
}

void set_PWM_cycle(int duty){
    if (duty <= 0) {
        LATDbits.LATD1 = 1;
    } else {
        LATDbits.LATD1 = 0;
    }
           OC1RS = (abs(duty) * 4000)/100;
}

float read_current(){
    float samples[SAMPS]; // stores our samples
    float result = 0;

    for (int ii = 0; ii < SAMPS; ii++){ // sample SAMPS times
        samples[ii] = INA219_read_current();
    }

    for (int jj = 0; jj < SAMPS; jj++){ // add all SAMPS samples
        result = result + samples[jj];
    }

    // divide result by 5 and return;
    result = result/SAMPS;
    return result;
}

float read_degrees(){ // reads degrees
    WriteUART2("a");
    while(!get_encoder_flag()){}; // wait for the encoder flag to be ready
    set_encoder_flag(0); // reset encoder flag
    float degrees = 360*(((float) get_encoder_count())/1356);
    return degrees;
}