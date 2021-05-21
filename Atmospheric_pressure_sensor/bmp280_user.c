#include <stdio.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <sys/ioctl.h>
#include <fcntl.h>
#include <stdlib.h>
#include <string.h>
#include <poll.h>
#include <sys/select.h>
#include <sys/time.h>
#include <signal.h>
#include "bmp280.h"

/**************************传感器值转定点值*************************************/
int t_fine;			//用于计算补偿
bmp280_dig_t bmp280_dig;
/*
 = {
    .T1 = 0x6be2,
    .T2 = 0x634e,
    .T3 = 0x0032,
    .P1 = 0x939d,
    .P2 = 0xd666,
    .P3 = 0x0bd0,
    .P4 = 0x1c10,
    .P5 = 0xff8e,
    .P6 = 0xfff9,
    .P7 = 0x1aa3,
    .P8 = 0xedfa,
    .P9 = 0xfc5c,
};
*/
double bmp280_compensate_T_double(int adc_T)
{
	double var1, var2, T;
	var1 = (((double)adc_T)/16384.0f - ((double)bmp280_dig.T1)/1024.0f) * ((double)bmp280_dig.T2);
	var2 = ((((double)adc_T)/131072.0f - ((double)bmp280_dig.T1)/8192.0f) *
	(((double)adc_T)/131072.0f - ((double) bmp280_dig.T1)/8192.0f)) * ((double)bmp280_dig.T3);
	t_fine = (int)(var1 + var2);
	T = (var1 + var2) / 5120.0f;
	return T;
}
double bmp280_compensate_P_double(int adc_p)
{
    double var1,var2,p;
    var1 = ((double)t_fine/2.0f) - 64000.0f;
	var2 = var1 * var1 * ((double)bmp280_dig.P6) / 32768.0f;
	var2 = var2 + var1 * ((double)bmp280_dig.P5) * 2.0;
	var2 = (var2/4.0f)+(((double)bmp280_dig.P4) * 65536.0f);
	var1 = (((double)bmp280_dig.P3) * var1 * var1 / 524288.0f + ((double)bmp280_dig.P2) * var1) / 524288.0f;
	var1 = (1.0f + var1 / 32768.0f)*((double)bmp280_dig.P1);
	if (var1 == 0.0f)
	{
	return 0; // avoid exception caused by division by zero
	}
	p = 1048576.0f - (double)adc_p;
	p = (p - (var2 / 4096.0f)) * 6250.0f / var1;
	var1 = ((double)bmp280_dig.P9) * p * p / 2147483648.0f;
	var2 = p * ((double)bmp280_dig.P8) / 32768.0f;
	p = p + (var1 + var2 + ((double)bmp280_dig.P7)) / 16.0f;
	return p;
}

int main(int argc, char *argv[])
{
    int fd,read_cnt;
    int ret;
    char *filename;
    int databuf[40] = {0};
    int adc_press,adc_temp;
    double press,temp;
    unsigned char data[14];

    if(argc != 2){
        printf("Error Usage %d\r\n",__LINE__);
        return -1;
    }
    filename = argv[1];
    fd = open(filename, O_RDWR);
    if(fd < 0){
        printf("can't open file %s %d\r\n",filename,__LINE__);
        return -1;
    }
    read_cnt = 0;
    while(read_cnt < 100){
        //读取bmp280  adc 原始数据
        ret = read(fd, databuf, sizeof(databuf)+sizeof(bmp280_dig_t));
        if(ret == 0){
           adc_press = databuf[0];
           adc_temp = databuf[1]; 
           //获取校准系数
           memcpy(&bmp280_dig,&databuf[2],sizeof(bmp280_dig_t));
           //大气压强、温度  转换浮点值
           press = bmp280_compensate_P_double(adc_press);
           temp = bmp280_compensate_T_double(adc_temp);
           printf("read bmp280 pressure %10f temperature %10f\r\n",press,temp);
        }
        read_cnt++;
        sleep(1);
    }
    close(fd);
    return 0;
}