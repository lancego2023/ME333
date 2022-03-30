#include "NU32.h"
#include "encoder.h"
#include "pwm.h"
#include "position.h"
#include "ina219.h"
#include "utilities.h"
#include "i2c_master_noint.h"
#include <stdio.h>
#include <stdlib.h>

// Lance's Notes //
// Current Gain that works well:
// Kp = 0.7
// Ki = 1.1 (roughly)
// Position gains that work well:
// Kp = 10
// Kd = 1100
// Ki = 0.005

// DEFINES //
#define BUF_SIZE 200
#define CURRENT_TEST_SIZE 200

// GLOBALS //
enum mode_t curr_mode;
static volatile int PWM_setting;
static volatile float position_angle;
static volatile int traj_count;

// INTERRUPT //
void __ISR(_TIMER_2_VECTOR, IPL5SOFT) master_control(void) {
    TRISDbits.TRISD1 = 0; // defines pin D1 as digial out
    enum mode_t curr_mode = get_mode();
   switch (curr_mode) {
       case IDLE:
       {
           OC1RS = 0;
           LATDbits.LATD1 = 0;
           break;
       }
       case PWM: // PWM function
       {
           set_PWM_cycle(PWM_setting);
           break;
       }
       case ITEST: // current test
       {
           current_test();
           break;
       }
       case HOLD: // Sends the bar to an angle. Takes our calculated current from position.c
       {
            set_PWM_cycle(current_controller(ret_current(),     // get our ref_current from the PID
                                            read_current()));   // compare it against the read current, do PWM
            break;    
                                                                
       }
       case TRACK:
       {
           set_PWM_cycle(current_controller(ret_current(),     // get our ref_current from the PID
                                            read_current()));   // compare it against the read current, do PWM
           break;
       }
       default:
       {
           break;
       }
   }
    IFS0bits.T2IF = 0;// clear interrupt flag
}

int main(){
    char buffer[BUF_SIZE];
    NU32_Startup(); // cache on, min flash wait, interrupts on, LED/button init, UART init
    INA219_Startup();
    UART2_Startup();
    NU32_LED1 = 1; // turn off the LEDs
    NU32_LED2 = 1;
    
    __builtin_disable_interrupts();
    // in future, initialize modules or peripherals here
    set_mode(IDLE); // Initialize the system as IDLE mode
    initialize_Timer3(); // Intialize the output-compare for PWM (Timer3 OC)
    initialize_ISRTimer(); // Initializes the timer for the ISR (Timer2)
    initialize_POSTimer(); // Initializes the position controller interrupt timer (Timer45)
    __builtin_enable_interrupts();

    while(1) {
        NU32_ReadUART3(buffer,BUF_SIZE); // we expect the next character to be a menu command
        NU32_LED2 = 1; // clear the error LED
        
        switch (buffer[0]) {
            case 'a': // current sense (ADC counts)
            {
                float milamps = INA219_read_current();
                int counts = (int) (milamps/3200) * 1024;
                // do some conversion here, not done yet
                sprintf(buffer, "%f\r\n", milamps);
                NU32_WriteUART3(buffer);
                break;
            }
            case 'b': // current sense (mA)
            {
                float curr = read_current();
                sprintf(buffer, "%f\r\n", curr);
                NU32_WriteUART3(buffer);
                break;
            }
            case 'c': // get the encoder reading but its in counts
            {
                WriteUART2("a");
                while(!get_encoder_flag()){}; // wait for the encoder flag to be ready
                set_encoder_flag(0); // reset encoder flag
                sprintf(buffer, "%d\r\n", get_encoder_count());
                NU32_WriteUART3(buffer);
                break;
            }
            case 'd': // encoder reading but its in degrees
            {
                sprintf(buffer, "%.2f\r\n", read_degrees());
                NU32_WriteUART3(buffer);
                break;
            }
            case 'e': // reset encoder
            {
                WriteUART2("b");
                break;
            }
            case 'f': // PWM mode
            {
                set_mode(PWM);
                int n = 0;
                NU32_ReadUART3(buffer, BUF_SIZE);
                sscanf(buffer, "%d", &n);
                PWM_setting = n;
                break;
            }
            case 'h': // get current gains
            {
                float Pcurr = get_currentP_gains();
                float Icurr = get_currentI_gains();
                sprintf(buffer, "Proportional Gain: %f Integral Gain: %f\r\n", Pcurr, Icurr);
                NU32_WriteUART3(buffer);
                break;
            }
            case 'g': // set current gains
            {
                float newP;
                float newI;
                NU32_ReadUART3(buffer, BUF_SIZE);
                sscanf(buffer, "%f", &newP);
                NU32_ReadUART3(buffer, BUF_SIZE);
                sscanf(buffer, "%f", &newI);
                set_current_gains(newP, newI);
                break;
            }
            case 'i': // set position gains
            {
                float newKp;
                float newKi;
                float newKd;
                NU32_ReadUART3(buffer, BUF_SIZE);
                sscanf(buffer, "%f", &newKp);
                NU32_ReadUART3(buffer, BUF_SIZE);
                sscanf(buffer, "%f", &newKi);
                NU32_ReadUART3(buffer, BUF_SIZE);
                sscanf(buffer, "%f", &newKd);
                set_position_gains(newKp, newKi, newKd);
                break;
            }
            case 'j': // read out position gains
            {
                float Ppos = get_positionP_gains();
                float Ipos = get_positionI_gains();
                float Dpos = get_positionD_gains();
                sprintf(buffer, "Proportional Gain: %f Integral Gain: %f Derivative Gain: %f \r\n", Ppos, Ipos, Dpos);
                NU32_WriteUART3(buffer);
                break;
            }
            case 'k': // test current gains
            {
                set_mode(ITEST); // first run the test, and then send results to MATLAB
                break;
            }
            case 'l': // HOLD mode, sets a position for the motor to adjust to
            {
                float temp_angle;
                NU32_ReadUART3(buffer, BUF_SIZE); // get user input to 
                sscanf(buffer, "%f", &temp_angle);
                position_angle = temp_angle;
                set_ref_angle(position_angle);
                set_mode(HOLD);
                break;
            }
            case 'm': // Step trajectory
            {
                static float temp_step;
                NU32_ReadUART3(buffer, BUF_SIZE); // get the size of the array first
                sscanf(buffer, "%d", &traj_count);
                get_num_samples(traj_count); // send to position.c
                for(int ii; ii < traj_count; ii++){ // get all angles, store in the traj_angles array
                    NU32_ReadUART3(buffer, BUF_SIZE);
                    sscanf(buffer, "%f", &temp_step);
                    get_angle_set(ii, temp_step); // send to position.c
                }
                break;
            }
            case 'n': // Cubic trajectory (literally the same thing as the step trajectory)
            {
                static float temp_cube;
                NU32_ReadUART3(buffer, BUF_SIZE); // get the size of the array first
                sscanf(buffer, "%d", &traj_count);
                get_num_samples(traj_count); // send to position.c
                for(int jj; jj < traj_count; jj++){ // get all angles, store in the traj_angles array
                    NU32_ReadUART3(buffer, BUF_SIZE);
                    sscanf(buffer, "%f", &temp_cube);
                    get_angle_set(jj, temp_cube); // send to position.c
                }
                break;
            }
            case 'o': // execute trajectory
            {
                set_mode(TRACK);
                break;
            }
            case 'q':
            {
                // handle q for quit.
                set_mode(IDLE);
                break;
            }
            case 'r': // gets the mode and prints it. Does not set the mode
            {
                curr_mode = get_mode();
                if (curr_mode == IDLE){
                    sprintf(buffer, "Mode: IDLE\r\n");
                } else if(curr_mode == PWM){
                    sprintf(buffer, "Mode: PWM\r\n");
                } else if(curr_mode == ITEST){
                    sprintf(buffer, "Mode: ITEST\r\n");
                } else if(curr_mode == HOLD){
                    sprintf(buffer, "Mode: HOLD\r\n");
                } else if(curr_mode == TRACK){
                    sprintf(buffer, "Mode: TRACK\r\n");
                }
                NU32_WriteUART3(buffer);
                break;
            }
            case 'p': // powers off the motor by setting to idle
            {
                set_mode(IDLE);
                break;
            }
            default:
            {
                NU32_LED2 = 0; // turn on LED2 to indicate an error
                break;
            }
        }
    }
    return 0;
}