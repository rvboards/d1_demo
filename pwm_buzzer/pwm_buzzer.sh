#!/bin/bash
#pwm 节点
pwmnode=pwm$1
#输出提示pwm$1 通道
echo  /sys/class/pwm/pwmchip0/${pwmnode}
#申请pwm$1 通道
echo $1 > /sys/class/pwm/pwmchip0/export
#指定pwm$1 通道的频率
echo $2 > /sys/class/pwm/pwmchip0/${pwmnode}/period
#指定pwm$1 通道的占空比
echo $3 > /sys/class/pwm/pwmchip0/${pwmnode}/duty_cycle
#使能pwm$1 通道 
echo 1 > /sys/class/pwm/pwmchip0/${pwmnode}/enable
#使能pwm$1 通道 工作时间10s
sleep 10
#失能pwm$1 通道 
echo 0 > /sys/class/pwm/pwmchip0/${pwmnode}/enable
#等待pwm$1 通道 关闭
sleep 1
#注销pwm$1 通道
echo $1 > /sys/class/pwm/pwmchip0/unexport
