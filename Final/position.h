#ifndef POSITION__h__
#define POSITION__h__

#include <xc.h> // processor SFR definitions
#include <sys/attribs.h> // __ISR macro
#include "encoder.h"
#include "utilities.h"
#include "pwm.h"
#include <stdio.h>

float position_controller(float ref_angle, float samp_angle); // Position PID controller
void set_ref_angle(float ref_angle); // sets the reference angle, helper for the interrupt i guess
float ret_current(void); // returns the current back up to the main level, also a helper i guess

void get_num_samples(int qty); // gets the number of samples and stores it in the traj_count
void get_angle_set(int index, float angle); // gets the angle and stores it in the array specified by the index
void send_results(void); // sends results back to matlab

void initialize_POSTimer(void); // initializes the POSTION CONTROLLER timer
void set_position_gains(float portional, float integral, float derive); // sets the position gains
float get_positionP_gains(void); // gets the proportional current gain
float get_positionI_gains(void); // gets the integral current gain
float get_positionD_gains(void); // gets the integral current gain

#endif

