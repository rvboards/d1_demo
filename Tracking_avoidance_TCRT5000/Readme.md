# Tracking obstacle avoidance module TCRT5000
## Introduction

TCRT5000 will output the state of "1" when it detects an obstacle in a close distance, which is "0" by default.
In this example, the GPIO drive control of the system is adopted to read the state value through GPIO

test script : tcrt5000_gpio44.sh

### test process

1. increase the execution authority : chmod +x tcrt5000_gpio44.sh
2. Execute the script program : ./tcrt5000_gpio44.sh 
3. Simulate the obstacle by hand, approach the module, and observe the output state
