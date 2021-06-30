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


int main(int argc, char *argv[])
{
    int fd,read_cnt;
    int ret,i;
    char *filename;
    unsigned char databuf[0x1000] = {0};
    float temp,hum;
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
        //读取dht11  温湿度数据
        ret = read(fd, databuf, 4);
        if(ret == 0){
           //temp  databuf[2]  databuf[3]
           // hum  databuf[0] 
           // 转换浮点值
           printf("read %3d dht11 temp %3d.%d hum %4d \r\n",read_cnt,databuf[2],databuf[3],databuf[0]);
        }
        usleep(1500000);
        read_cnt++;
    }
    close(fd);
    return 0;
}
