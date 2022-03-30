## ME333 Final Project
Everything for the final ME333 project needed is found here. Ignore the folders for HW8/9/10 (just for submissions)

### Contents of each file:
#### main.c
- includes the main operation loop
- includes the ISR in charge of the modes
- Has the initialization of all timers and peripherals
- Responsible for these modes: IDLE, PWM

#### pwm.c
- Slightly confusingly named
- Has the code for the current controller, not actually for PWM (set/see current gains, ITEST)
- Stores the current gains here

#### position.c
- Has the position controller
- Includes the 200Hz ISR for the position PID (HOLD, TRACK)
- Stores the position gains here (set/see position gains)

#### utilities.c
- Any extra helpers here
- Some initialization here as well

#### client.m
- Menu system
- relies on read_plot_matrix.m and genRef.m