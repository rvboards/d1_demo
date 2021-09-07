#!/bin/bash
#rvboards led 使用d1扩展gpio控制
#本例使用user 模式控制 rvboards led
#gpio  系统编号  rvboards-rgbled
#PP0    2020    LED1_R
#PP1    2021    LED1_G
#...    ...     no_led
#PP7    2027    LED1_B

#申请gpio
echo 2020 > /sys/class/gpio/export
echo 2021 > /sys/class/gpio/export
echo 2027 > /sys/class/gpio/export
#使能gpio输出功能
echo out > /sys/class/gpio/gpio2020/direction
echo out > /sys/class/gpio/gpio2021/direction
echo out > /sys/class/gpio/gpio2027/direction
#使能gpio高电平有效
echo 1 > /sys/class/gpio/gpio2020/active_low
echo 1 > /sys/class/gpio/gpio2021/active_low
echo 1 > /sys/class/gpio/gpio2027/active_low

#控制gpio 输出 高低电平，控制led亮灭闪烁
    echo 0 > /sys/class/gpio/gpio2020/value
    echo 0 > /sys/class/gpio/gpio2021/value
    echo 0 > /sys/class/gpio/gpio2027/value
for var in {1..10..1}
do
    echo 1 > /sys/class/gpio/gpio2020/value
    sleep 1
    echo 1 > /sys/class/gpio/gpio2021/value
    sleep 1
    echo 1 > /sys/class/gpio/gpio2027/value
    sleep 1
    echo 0 > /sys/class/gpio/gpio2020/value
    sleep 1
    echo 0 > /sys/class/gpio/gpio2021/value
    sleep 1
    echo 0 > /sys/class/gpio/gpio2027/value
    sleep 1
done
#注销gpio
echo 2020 > /sys/class/gpio/unexport
echo 2021 > /sys/class/gpio/unexport
echo 2027 > /sys/class/gpio/unexport