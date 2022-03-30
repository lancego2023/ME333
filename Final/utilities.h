#ifndef UTILITIES__H__
#define UTILITIES__H__

#include <xc.h> // processor SFR definitions
#include <sys/attribs.h> // __ISR macro
#include "i2c_master_noint.h"
#include "ina219.h"
#include "encoder.h"
#include <stdio.h>

enum mode_t{IDLE, PWM, ITEST, HOLD, TRACK};

enum mode_t get_mode(void); // returns the mode of the system
void    set_mode(enum mode_t mood); // sets the mode of the system
void    set_PWM_cycle(int duty); // sets the PWM cycle by setting OC1RS
float   read_current(void); // reads the current. Samples the current 5 times and then averages the current.
float   read_degrees(void); // converts encoder counts to degrees

#endif // INA219__H__