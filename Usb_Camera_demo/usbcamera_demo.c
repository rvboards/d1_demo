#include <stdio.h>
#include <string.h>
#include <errno.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <time.h>
#include <sys/mman.h>
#include <assert.h>
#include <linux/videodev2.h>
#include <linux/fb.h>
 
//照相机延时等待
#define TimeOut 5 
//拍照 个数
#define CapNum 10
//设置照片宽度 高度
#define CapWidth 320
#define CapHeight 240
//申请Buf个数
#define ReqButNum 4
//使用前置或者后置Camera 前置设0，后置设1
#define IsRearCamera 0
//设置帧率
#define  FPS 10
//设置格式
#define PIXELFMT V4L2_PIX_FMT_YUYV
 
#define CapDelay 0*500*1000
 
 
#define CLEAR(x)    memset(&(x), 0, sizeof(x))
typedef unsigned char BYTE;
typedef unsigned short WORD;
typedef unsigned int DWORD;
typedef long LONG;
 
typedef struct
{
	void *start;
	int length;
}BUFTYPE;
 
 
BUFTYPE *user_buf;
static int n_buffer = 0;
 
 
static struct fb_var_screeninfo vinfo;
static struct fb_fix_screeninfo finfo;
static int lcd_buf_size;
static char *fb_buf = NULL;
 
//打开摄像头设备
int open_camer_device()
{
	int fd;
 
//非阻塞方式打开，如果打开错误，会立即返回
	if((fd = open("/dev/video0",O_RDWR | O_NONBLOCK)) < 0)
	{
		perror("Fail to open");
		exit(EXIT_FAILURE);
	} 
 
	printf("open cam success %d\n",fd);
	return fd;
}
 
//打开摄像头设备
int open_lcd_device()
{
	int fd;
//非阻塞方式打开，如果打开错误，会立即返回
	if((fd = open("/dev/fb0",O_RDWR | O_NONBLOCK)) < 0)
	{
		perror("Fail to open");
		exit(EXIT_FAILURE);
	} 
	printf("open lcd success %d\n",fd);
	return fd;
}
 
//申请Camera Buf，并映射到用户空间，利用全局变量user_buf保存映射信息
int init_mmap(int lcd_fd, int cam_fd)
{
	int i = 0;
	int err;
	int ret;
	struct v4l2_control ctrl;
	struct v4l2_requestbuffers reqbuf;
 
 
    //mmap framebuffer    
    fb_buf = (char *)mmap(
	    NULL,
	    lcd_buf_size,
	    PROT_READ | PROT_WRITE,MAP_SHARED ,
	    lcd_fd, 
	    0);    
	if(NULL == fb_buf)
	{
		perror("Fail to mmap fb_buf");
		exit(EXIT_FAILURE);
	}
 
	bzero(&reqbuf,sizeof(reqbuf));
	reqbuf.count = ReqButNum;
	reqbuf.type = V4L2_BUF_TYPE_VIDEO_CAPTURE;
	reqbuf.memory = V4L2_MEMORY_MMAP;
	printf("start VIDIOC_REQBUFS\n");
	//申请视频缓冲区(这个缓冲区位于内核空间，需要通过mmap映射)
	//这一步操作可能会修改reqbuf.count的值，修改为实际成功申请缓冲区个数
	if(-1 == ioctl(cam_fd,VIDIOC_REQBUFS,&reqbuf))
	{
		perror("Fail to ioctl 'VIDIOC_REQBUFS'");
		exit(EXIT_FAILURE);
	}
 
	n_buffer = reqbuf.count;
 
	user_buf = calloc(reqbuf.count,sizeof(*user_buf));
	if(user_buf == NULL){
		fprintf(stderr,"Out of memory\n");
		exit(EXIT_FAILURE);
	}
 
	//将内核缓冲区映射到用户进程空间
	for(i = 0; i < reqbuf.count; i ++)
	{
		struct v4l2_buffer buf;
		
		bzero(&buf,sizeof(buf));
		buf.type = V4L2_BUF_TYPE_VIDEO_CAPTURE;
		buf.memory = V4L2_MEMORY_MMAP;
		buf.index = i;
		//查询申请到内核缓冲区的信息
		if(-1 == ioctl(cam_fd,VIDIOC_QUERYBUF,&buf))
		{
			perror("Fail to ioctl : VIDIOC_QUERYBUF");
			exit(EXIT_FAILURE);
		}
 
		user_buf[i].length = buf.length;
		user_buf[i].start = 
			mmap(
					NULL,/*start anywhere*/
					buf.length,
					PROT_READ | PROT_WRITE,
					MAP_SHARED,
					cam_fd,buf.m.offset
				);
		
		if(MAP_FAILED == user_buf[i].start)
		{
			perror("Fail to mmap\n");
			printf("%d\n",i);
			exit(EXIT_FAILURE);
		}
	//	printf("start:08%lx\n",user_buf[i].start);
	}	
 
	return 0;
}
 
 
//初始化视频设备
int init_device(int lcd_fd, int cam_fd)
{
	struct v4l2_fmtdesc fmt;
	struct v4l2_capability cap;
	struct v4l2_format stream_fmt;
	struct v4l2_input input;
	struct v4l2_control ctrl;
	struct v4l2_streamparm stream;
	int err;
	int ret;
 
 
 
 
	if(-1 == ioctl(lcd_fd,FBIOGET_FSCREENINFO,&finfo))
	{
		perror("Fail to ioctl:FBIOGET_FSCREENINFO\n");
		exit(EXIT_FAILURE);
	}
	if (-1==ioctl(lcd_fd, FBIOGET_VSCREENINFO, &vinfo)) 
	{
		perror("Fail to ioctl:FBIOGET_VSCREENINFO\n");
		exit(EXIT_FAILURE);
	}
    lcd_buf_size = vinfo.xres * vinfo.yres * vinfo.bits_per_pixel / 8;	
	printf("vinfo.xres:%d, vinfo.yres:%d, vinfo.bits_per_pixel:%d, lcd_buf_size:%d, finfo.line_length:%d\n",vinfo.xres, vinfo.yres, vinfo.bits_per_pixel, lcd_buf_size, finfo.line_length); 
 
 
 
 
 
	
	memset(&fmt,0,sizeof(fmt));
	fmt.index = 0;
	fmt.type = V4L2_BUF_TYPE_VIDEO_CAPTURE;
	
//枚举视频设置支持的格式
	while((ret = ioctl(cam_fd,VIDIOC_ENUM_FMT,&fmt)) == 0)
	{
		fmt.index ++ ;
		printf("{pixelformat = %c%c%c%c},description = '%s'\n",
				fmt.pixelformat & 0xff,(fmt.pixelformat >> 8)&0xff,
				(fmt.pixelformat >> 16) & 0xff,(fmt.pixelformat >> 24)&0xff,
				fmt.description);
	}
//查询视频设备支持的功能
	ret = ioctl(cam_fd,VIDIOC_QUERYCAP,&cap);
	if(ret < 0){
		perror("FAIL to ioctl VIDIOC_QUERYCAP");
		exit(EXIT_FAILURE);
	}
 
 
	if(!(cap.capabilities & V4L2_CAP_VIDEO_CAPTURE))
	{
		printf("The Current device is not a video capture device\n");
		exit(EXIT_FAILURE);
	
	}
 
	if(!(cap.capabilities & V4L2_CAP_STREAMING))
	{
		printf("The Current device does not support streaming i/o\n");
		exit(EXIT_FAILURE);
	}
 
	CLEAR(stream_fmt);
//设置摄像头采集数据格式，如设置采集数据的
//长,宽，图像格式(JPEG,YUYV,MJPEG等格式)
	stream_fmt.type = V4L2_BUF_TYPE_VIDEO_CAPTURE;
	stream_fmt.fmt.pix.width = CapWidth;
	stream_fmt.fmt.pix.height = CapHeight;
	stream_fmt.fmt.pix.pixelformat = PIXELFMT;
	stream_fmt.fmt.pix.field = V4L2_FIELD_INTERLACED;
 
	if(-1 == ioctl(cam_fd,VIDIOC_S_FMT,&stream_fmt))
	{
		printf("Can't set the fmt\n");
		perror("Fail to ioctl\n");
		exit(EXIT_FAILURE);
	}
	printf("VIDIOC_S_FMT successfully\n");
 
	init_mmap(lcd_fd, cam_fd);
	
//通过S_PARM来设置FPS		
	/* fimc_v4l2_s_parm */
 
  CLEAR(stream);
    stream.type = V4L2_BUF_TYPE_VIDEO_CAPTURE;
    stream.parm.capture.capturemode = 0;
    stream.parm.capture.timeperframe.numerator = 1;
    stream.parm.capture.timeperframe.denominator = FPS;
 
    err = ioctl(cam_fd, VIDIOC_S_PARM, &stream);
	if(err < 0)
    printf("FimcV4l2 start: error %d, VIDIOC_S_PARM", err);
 
	return 0;
}
 
int start_capturing(int cam_fd)
{
	unsigned int i;
	enum v4l2_buf_type type;
	//将申请的内核缓冲区放入一个队列中
	for(i = 0;i < n_buffer;i ++)
	{
		struct v4l2_buffer buf;
 
 
		bzero(&buf,sizeof(buf));
		buf.type = V4L2_BUF_TYPE_VIDEO_CAPTURE;
		buf.memory = V4L2_MEMORY_MMAP;
		buf.index = i;
		
		if(-1 == ioctl(cam_fd,VIDIOC_QBUF,&buf))
		{
			perror("Fail to ioctl 'VIDIOC_QBUF'");
			exit(EXIT_FAILURE);
		}
	}
 
 
	//开始采集数据
	type = V4L2_BUF_TYPE_VIDEO_CAPTURE;
	if(-1 == ioctl(cam_fd,VIDIOC_STREAMON,&type))
	{
		printf("i = %d.\n",i);
		perror("Fail to ioctl 'VIDIOC_STREAMON'");
		exit(EXIT_FAILURE);
	}
	return 0;
}
 
int clip(int value, int min, int max) 
{    
	return (value > max ? max : value < min ? min : value); 
}
//将采集好的数据放到文件中
int process_image(void *addr,int length)
{
    unsigned char* in=(char*)addr;    
	int width=CapWidth;    
	int height=CapHeight;    
	int istride=CapWidth *2;    
	int x,y,j;   
	int y0,u,y1,v,r,g,b;    
	long location=0; 
	//printf("vinfo.xoffset:%d,vinfo.yoffset:%d\n",vinfo.xoffset,vinfo.yoffset);   
	for ( y = 0; y < height; ++y)	
	{        
		for (j = 0, x=0; j < width * 2 ; j += 4,x +=2)		
		{         
			location = (x+vinfo.xoffset) * (vinfo.bits_per_pixel/8) + (y+vinfo.yoffset) * finfo.line_length; 
			y0 = in[j];         
			u = in[j + 1] - 128;
			y1 = in[j + 2];                  
			v = in[j + 3] - 128; 
			r = (298 * y0 + 409 * v + 128) >> 8;
			g = (298 * y0 - 100 * u - 208 * v + 128) >> 8;
			b = (298 * y0 + 516 * u + 128) >> 8;  
			fb_buf[ location + 0] = clip(b, 0, 255);
			fb_buf[ location + 1] = clip(g, 0, 255);
			fb_buf[ location + 2] = clip(r, 0, 255);
			fb_buf[ location + 3] = 255; 
			r = (298 * y1 + 409 * v + 128) >> 8; 
			g = (298 * y1 - 100 * u - 208 * v + 128) >> 8;
			b = (298 * y1 + 516 * u + 128) >> 8; 
			fb_buf[ location + 4] = clip(b, 0, 255);
			fb_buf[ location + 5] = clip(g, 0, 255);  
			fb_buf[ location + 6] = clip(r, 0, 255);
			fb_buf[ location + 7] = 255;              
		}        in +=istride;      }
	
//	usleep(500);
	return 0;
}
 
 
int read_frame(int cam_fd)
{
	struct v4l2_buffer buf;
	unsigned int i;
	
	bzero(&buf,sizeof(buf));
	buf.type = V4L2_BUF_TYPE_VIDEO_CAPTURE;
	buf.memory = V4L2_MEMORY_MMAP;
//从Camera buf中把数据拿出来
	if(-1 == ioctl(cam_fd,VIDIOC_DQBUF,&buf))
	{
		perror("Fail to ioctl 'VIDIOC_DQBUF'");
		exit(EXIT_FAILURE);
	}
 
    assert(buf.index < n_buffer);
 
	process_image(user_buf[buf.index].start,user_buf[buf.index].length);
//把处理过的Buf 重新入队	
	if(-1 == ioctl(cam_fd,VIDIOC_QBUF,&buf))
	{
		perror("Fail to ioctl 'VIDIOC_QBUF'");
		exit(EXIT_FAILURE);
	}
	return 1;
}
 
//利用select 进行超时处理
int mainloop(int cam_fd)
{ 
	int count = 1;//CapNum;
	clock_t startTime, finishTime;
	double selectTime, frameTime;
	
	while(count++  > 0)
	{
		for(;;)
		{
			fd_set fds;
			struct timeval tv;
			int r;
		//	startTime = clock();
 
			
			FD_ZERO(&fds);
			FD_SET(cam_fd,&fds);
 
 
			/*Timeout*/
			tv.tv_sec = TimeOut;
			tv.tv_usec = 0;
		
			r = select(cam_fd + 1,&fds,NULL,NULL,&tv);
 
 
			if(-1 == r)
			{
				if(EINTR == errno)
					continue;
				
				perror("Fail to select");
				exit(EXIT_FAILURE);
			}
 
 
			if(0 == r)
			{
				fprintf(stderr,"select Timeout\n");
				exit(EXIT_FAILURE);
			}
			startTime = clock();
			if(read_frame(cam_fd))
			{
				finishTime = clock();
		//		printf("delta:%dms\n", (finishTime - startTime)/1000);
				break;
			}
		}
	//		usleep(CapDelay);
	}
 
 
	return 0;
}
 
 
void stop_capturing(int cam_fd)
{
	enum v4l2_buf_type type;
	
	type = V4L2_BUF_TYPE_VIDEO_CAPTURE;
	if(-1 == ioctl(cam_fd,VIDIOC_STREAMOFF,&type))
	{
		perror("Fail to ioctl 'VIDIOC_STREAMOFF'");
		exit(EXIT_FAILURE);
	}
	
	return;
}
 
 
void uninit_camer_device()
{
	unsigned int i;
 
 
	for(i = 0;i < n_buffer;i ++)
	{
		if(-1 == munmap(user_buf[i].start, user_buf[i].length))
		{
			exit(EXIT_FAILURE);
		}
	}
	if (-1 == munmap(fb_buf, lcd_buf_size)) 
	{          
		perror(" Error: framebuffer device munmap() failed.\n");          
		exit (EXIT_FAILURE) ;       
	}   
	free(user_buf);
 
 
	return;
}
 
 
void close_camer_device(int lcd_fd, int cam_fd)
{
	if(-1 == close(lcd_fd))
	{
		perror("Fail to close lcd_fd");
		exit(EXIT_FAILURE);
	}
	if(-1 == close(cam_fd))
	{
		perror("Fail to close cam_fd");
		exit(EXIT_FAILURE);
	}
 
	return;
}
 
 
int main()
{
 
	int lcd_fd;
	int cam_fd;
 
	lcd_fd = open_lcd_device();
	cam_fd = open_camer_device();
	
	init_device(lcd_fd, cam_fd);
 
	
	start_capturing(cam_fd);
	
	mainloop(cam_fd);
	
	stop_capturing(cam_fd);
 
	uninit_camer_device();
 
	close_camer_device(lcd_fd, cam_fd);
 
 
	return 0;
}
