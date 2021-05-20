#!/bin/bash

for var in {1..1..1}
do
	insmod bh1750fvi_drv.ko && ./bh1750fvi_user /dev/bh1750fvi && rmmod bh1750fvi_drv.ko 
done	
