#include "position.h"

static volatile float Kp;
static volatile float Ki;
static volatile float Kd;
static volatile float eint = 0.0;
static volatile float edot = 0.0;
static volatile float eprev = 0.0;
static volatile float reference = 0.0;
float control_current = 0.0;

static volatile int traj_count;
static volatile float traj_angles[1000];
static volatile float samp_angles[1000];



void __ISR(_TIMER_5_VECTOR, IPL4SOFT) pos_control(void) { // USES timer45 to do a 200 Hz interrupt
    enum mode_t mood = get_mode();
    static float samp;
    if(mood == HOLD){                                           // Check if we are in fact in the HOLD mode (for go to angle)
        samp = read_degrees();                                  // sample our encoder
        control_current = position_controller(reference, samp); // calculate the current needed for the control
    } else if (mood == TRACK){                                  // Check for track mode
        static int counter = 0;
        samp_angles[counter] = read_degrees();                                  
        control_current = position_controller(traj_angles[counter], samp_angles[counter]);
        counter++;                                              // increment counter
        
        if (counter >= traj_count){                             // finish condition, reset the counter back to 0
            set_mode(IDLE);
            counter = 0;
            send_results();                                     // send results to matlab
            reference = traj_angles[traj_count - 1];
            set_mode(HOLD);                        
        }
    }
    IFS0bits.T5IF = 0;                                          // clear interrupt flag for Timer45
}

void initialize_POSTimer(){             // 200Hz timer to run our ISR
    T4CONbits.T32 = 1;                  // for T45: enable 32 bit mode Timer45
    PR4 = 399999;                       // set PR so timer rolls over at 10 Hz
    TMR4 = 0;                           // initialize count to 0
    T4CONbits.TON = 1;                  // turn Timer45 on 
    IPC5bits.T5IP = 4;                  // INT step 4: priority for Timer5 (int for Timer45)
    IFS0bits.T5IF = 0;                  // INT step 5: clear interrupt flag for Timer45
    IEC0bits.T5IE = 1;                  // INT step 6: enable interrupt for Timer45
}

float position_controller(float ref_angle, float samp_angle){ // Position PID controller, takes a ref angle and sampled angle and calculates a current for it
    static float error;
    static float u;
    static float current;

    // bounds correction, if sampled angle is below -360 or above 360.
    if(samp_angle > 360){
        while(samp_angle > 360){
            samp_angle = samp_angle - 360;
        }
    } else if(samp_angle < -360){
        while(samp_angle < -360){
            samp_angle = samp_angle + 360;
        }
    }

    // PID controller part
    error = ref_angle - samp_angle;
    eint = eint + error;
    edot = error - eprev;
    eprev = error;

    u = (Kp*error + Ki*eint + Kd*edot) / 360; // calculates the control as a fraction of a full rotation
    if (u > 1){
        u = 0.9;
    } else if(u < -1){
        u = -0.9;
    }

    current = u * -700; // 700 is the stall current, so now we calculate the current
    return current; // return the current to be the ~reference~ current for the PI current controller
}

// HELPERS for track mode
void get_num_samples(int qty){ // gets the number of samples and stores it in the traj_count
    traj_count = qty;
}

void get_angle_set(int index, float angle){ // gets the angle and stores it in the array specified by the index
    traj_angles[index] = angle;
}

void send_results(){ // sends results back to matlab
    char buffer[50];
    // first send how many samples (traj_count)
    sprintf(buffer, "%d\r\n", traj_count);
    NU32_WriteUART3(buffer);

    // then for every sample, send as a pair with ref
    for ( int jj = 0; jj < traj_count; jj++) {
        sprintf(buffer, "%f %f\r\n", traj_angles[jj], samp_angles[jj]);
        NU32_WriteUART3(buffer);
    }

    return;
}

// don't fuck with stuff here
void set_ref_angle(float ref_angle){
    reference = ref_angle;
}

float ret_current(){
    return control_current;
}

void set_position_gains(float portional, float integral, float derive) {
    Kp = portional;
    Ki = integral;
    Kd = derive;
}

float get_positionP_gains(){
    return Kp;
}
float get_positionI_gains(){
    return Ki;
}
float get_positionD_gains(){
    return Kd;
}