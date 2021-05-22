#!/bin/bash
#申请pb1(gpio44)
echo 44 > "/sys/class/gpio/export" 
#设置输入模式
echo in > "/sys/class/gpio/gpio44/direction"
#循环n次，读取开关状态
for var in {1..10..1}
do 
	#读取开关量
	cat /sys/class/gpio/gpio44/value
	sleep 1
done
#设置输出模式
echo out > "/sys/class/gpio/gpio44/direction"
#取消申请
echo 44 > "/sys/class/gpio/unexport"
