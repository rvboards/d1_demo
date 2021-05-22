# Passive buzzer
## Passive Buzzer Features
1. passive internal without shock source, so if the DC signal can not make it sing.It must be driven by a 2K ~ 5K square wave (PWM)
2. the sound frequency is controllable, can make the effect of the sound spectrum
3. In some special cases, a control port can be reused with LED
## Test the demo
In this example, the PWM drive system is used to control the sound of the buzzer
Test script pwm_buzzer.sh
### test process
1. increase the execution authority : chmod +x pwm_buzzer.sh
2. Execute the script program : ./pwm_buzzer.sh 7 300000 150000
  1. The first parameter (7) : Refers to the 7th PWM channel
  2. Second parameter (300,000) : Specifies the frequency of the PWM
    1. 1000 	  ==>   1Mhz
    2. 10000    ==>   100Khz
    3. 100000   ==>   10Khz
    4. 1000000  ==>   1Khz
  3. Third parameter (150000) : Specifies the duty cycle of the PWM







