#ifndef ENCODER_H
#define ENCODER_H

#include <xc.h>

// functions
static int encoder_command(int read);
int encoder_counts(void);
void encoder_init(void);

#endif