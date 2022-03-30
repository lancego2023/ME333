#ifndef PWM__h__
#define PWM__h__
#include "NU32.h"
#include "utilities.h"
#include "i2c_master_noint.h"
#include "ina219.h"
#include <stdio.h>

void initialize_Timer3(void); // using timer3 for our PWM (20kHz)
void initialize_ISRTimer(void); // initializes timer2, which will be used for our ISR (5kHz)
void set_current_gains(float portional, float integral); // sets the current gains

float get_currentP_gains(void); // gets the proportional current gain
float get_currentI_gains(void); // gets the integral current gain

int current_controller(float ref, float samp); // takes a current, returns an INT PWM duty cycle

void current_test(void); // ITEST
void send_matlab(void);  // prints results to matlab, helper for ITEST

#endif
