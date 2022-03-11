## some notes for this lab
### 24.1.2) 
- R = 6.8K Ohm? This might be wrong

### 24.2.1)
You will use Timer3 as the timer base for OC1. You want a 20 kHz
PWM on OC1. Timer3 takes the PBCLK as input and uses a prescaler of 1. What should
PR3 be?
- Period = (PR3 + 1) * N * 12.5ns
    - 1/20k = (PR3 + 1) * 1 * 12.5ns
    PR3 = 3999
### 24.2.2)