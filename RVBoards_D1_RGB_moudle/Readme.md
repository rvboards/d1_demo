# Rvboards led module
This paper introduces the method of controlling Rvboards-LED module with TINA-D1 extended IO. The module of extended IO is PCF8574. The device communicates with SOC through I2C.
See 《D1 Tina Linux扩展IO开发指南.pdf》 section 4 for example.
Rvboards-led example see rvboards_led_test.sh

## test run
1. Add the permission to run rvboards_led_test.sh
   1. chmod +x rvboards_led_test.sh
2. Run the rvboards_led_test.sh command to observe the LED indicator changes
   1. ./rvboards_led_test.sh
