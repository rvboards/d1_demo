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

/**************************传感器值转定点值*************************************/
int t_fine;			//用于计算补偿

int main(int argc, char *argv[])
{
    int fd,read_cnt;
    int ret;
    char *filename;
    unsigned char databuf[4] = {0};
    int grv_data;
    float grv_d;
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
        //读取bh1750fvi   adc 原始数据
        ret = read(fd, databuf, 2);
        if(ret == 0){
           grv_data = (int)databuf[0];
           grv_data = (int)grv_data * 256 + (int)databuf[1]; 
           // 转换浮点值
           grv_d = (float)grv_data /1.2f;
           printf("read bh1750fvi illumination intensity %10f lx \r\n",grv_d );
        }
        sleep(1);
        read_cnt++;
    }
    close(fd);
    return 0;
}