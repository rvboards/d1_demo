#include <linux/init.h>
#include <linux/module.h>
#include <linux/spi/spi.h>
#include <linux/delay.h>
#include <linux/gpio.h>
#include <linux/spi/spi.h>

#include <linux/ctype.h>
#include <linux/delay.h>
#include <linux/export.h>
#include <linux/gpio.h>
#include <linux/gpio/driver.h>
#include <linux/init.h>
#include <linux/of_gpio.h>
#include <linux/of.h>
#include <linux/pinctrl/consumer.h>
#include <linux/of.h>
#include <linux/of_address.h>
#include <linux/types.h>
#include <linux/kernel.h>
#include <linux/delay.h>
#include <linux/cdev.h>
#include <asm/io.h>

#include <linux/uaccess.h>

#include "bmp280.h"


#define BMP280_CNT      1
#define BMP280_NAME     "bmp280"
struct bmp280_dev{
    dev_t devid;
    struct cdev cdev;
    struct class *class;
    struct device *device;
    struct device_node *nd;
    int major;
    void *private_data;
    int cs_gpio;
    int	mosi_gpio;
    int	miso_gpio;
    int clk_gpio;
    //bmp280较准数据
    bmp280_dig_t bmp280_dig;
}bmp280_dev;

static unsigned char spi_gpio_mode(unsigned char spi_mode,unsigned char tx)
{
    int i = 0;
    unsigned char rx = 0;
    switch(spi_mode){
        case 0:
            for(i = 0; i < 8;i++){
                gpio_set_value(bmp280_dev.clk_gpio,0);
                mdelay(1);
                gpio_set_value(bmp280_dev.clk_gpio,1);
                if((tx << i) & 0x80){
                    gpio_set_value(bmp280_dev.mosi_gpio,1);
                }else{
                    gpio_set_value(bmp280_dev.mosi_gpio, 0);
                }
                rx = rx << 1;
                rx |= gpio_get_value(bmp280_dev.miso_gpio);
                mdelay(1);
            }
            gpio_set_value(bmp280_dev.clk_gpio, 0);
            break;
        case 1:
            for(i = 0; i < 8;i++){
                gpio_set_value(bmp280_dev.clk_gpio,1);
                mdelay(1);
                gpio_set_value(bmp280_dev.clk_gpio,0);
                if((tx << i) & 0x80){
                    gpio_set_value(bmp280_dev.mosi_gpio,1);
                }else{
                    gpio_set_value(bmp280_dev.mosi_gpio, 0);
                }
                rx = rx << 1;
                rx |= gpio_get_value(bmp280_dev.miso_gpio);
                mdelay(1);
            }
            gpio_set_value(bmp280_dev.clk_gpio, 1);
            break;
        default:
        break;
    }
    return rx;
}


static int bmp280_write_regs(struct bmp280_dev *dev, unsigned char reg, void *buf, int len)
{
    int ret,cnt;
    unsigned char *txdata;
    struct spi_message m;
    struct spi_transfer *t;
    struct spi_device *spi = (struct spi_device *)dev->private_data;
    txdata = kzalloc(len+1,GFP_KERNEL);
    //写入时，最高位置0，读时最高位置1
    txdata[0] = reg & 0x7f;
    memcpy(&txdata[1],buf,len);
    t = kzalloc(sizeof(struct spi_transfer), GFP_KERNEL);
    gpio_set_value(dev->cs_gpio,0);
    #if 0
    t->tx_buf = &reg;//txdata;
    t->len = 1;
    spi_message_init(&m);
    spi_message_add_tail(t, &m);
    ret = spi_sync(spi, &m);

    t->tx_buf = buf;
    t->len = len;
    spi_message_init(&m);
    spi_message_add_tail(t, &m);
    ret = spi_sync(spi, &m);
    #else
    for(cnt = 0; cnt < len+1;cnt++){
        spi_gpio_mode(1, txdata[cnt]);
    }

    #endif

    gpio_set_value(dev->cs_gpio, 1);
    kfree(txdata);
    kfree(t);
    return ret;
}
static int bmp280_read_regs(struct bmp280_dev *dev, unsigned char reg, void *buf, int len)
{
    int ret,cnt ;
    unsigned char *txdata;
    unsigned char *rxdata = (unsigned char *)buf;
    struct spi_message m;
    struct spi_transfer *t;
    struct spi_device *spi = (struct spi_device *)dev->private_data;

    txdata = kzalloc(len,GFP_KERNEL);
    t = kzalloc(sizeof(struct spi_transfer), GFP_KERNEL);
    gpio_set_value(dev->cs_gpio, 0);
    //写入时，最高位置0，读时最高位置1
    #if 0
    txdata[0] = reg;
    t->tx_buf = &reg ;//txdata;
    t->len = 1;

    spi_message_init(&m);
    spi_message_add_tail(t,&m);
    ret = spi_sync(spi,&m);

    txdata[0] = 0x0F;
    t->rx_buf = buf;
    t->len = len;

    spi_message_init(&m);
    spi_message_add_tail(t,&m);
    ret = spi_sync(spi,&m);
    #else
        spi_gpio_mode(1, reg);
    for(cnt = 0; cnt < len;cnt++){
        rxdata[cnt] = spi_gpio_mode(1, 0xff);
    }
    #endif
    gpio_set_value(dev->cs_gpio, 1);

    kfree(txdata);
    kfree(t);

    return ret;
}
static void bmp280_set_tempoversamp(BMP_OVERSAMPLE_MODE *oversample_mode)
{
    unsigned char regtmp;
    regtmp = (oversample_mode->T_Osample << 5) | (oversample_mode->P_Osample << 2)|
                (oversample_mode->WORKMODE);
    bmp280_write_regs(&bmp280_dev,BMP280_CTRLMEAS_REG, &regtmp, 1);
}
static void bmp280set_standby_filter(BMP_CONFIG *bmp_config)
{
    unsigned char regtmp;
    regtmp = (bmp_config->T_SB << 5) | (bmp_config->FILTER_COEFFICIENT << 2) |
                (bmp_config->SPI_EN);
    bmp280_write_regs(&bmp280_dev, BMP280_CONFIG_REG, &regtmp,1);
}
//读取id，判断是否是bmp280
//读取较准数据
static int bmp280_reginit(void)
{
    int ret = 0;
    unsigned char Lsb,Msb;
    BMP_OVERSAMPLE_MODE bmp280_oversample_mode;
    BMP_CONFIG bmp280_config;
    //读取id，判断是否是bmp280
    printk("bmp280 id is \r\n");
    bmp280_read_regs(&bmp280_dev, BMP280_CHIPID_REG, &Lsb, 1);
    printk("bmp280 id is 0x%x\r\n", Lsb);

    /********************接下来读出矫正参数*********************/
	//温度传感器的矫正值
    bmp280_read_regs(&bmp280_dev,BMP280_DIG_T1_LSB_REG,&Lsb,1);
    bmp280_read_regs(&bmp280_dev,BMP280_DIG_T1_MSB_REG,&Msb,1);
    bmp280_dev.bmp280_dig.T1 = ((unsigned short)Msb << 8) + Lsb;
    bmp280_read_regs(&bmp280_dev,BMP280_DIG_T2_LSB_REG,&Lsb,1);
    bmp280_read_regs(&bmp280_dev,BMP280_DIG_T2_MSB_REG,&Msb,1);
    bmp280_dev.bmp280_dig.T2 = ((unsigned short)Msb << 8) + Lsb;
    bmp280_read_regs(&bmp280_dev,BMP280_DIG_T3_LSB_REG,&Lsb,1);
    bmp280_read_regs(&bmp280_dev,BMP280_DIG_T3_MSB_REG,&Msb,1);
    bmp280_dev.bmp280_dig.T3 = ((unsigned short)Msb << 8) + Lsb;
    //大气压传感器的矫正值
    bmp280_read_regs(&bmp280_dev,BMP280_DIG_P1_LSB_REG,&Lsb,1);
    bmp280_read_regs(&bmp280_dev,BMP280_DIG_P1_MSB_REG,&Msb,1);
    bmp280_dev.bmp280_dig.P1 = ((unsigned short)Msb << 8) + Lsb;
    bmp280_read_regs(&bmp280_dev,BMP280_DIG_P2_LSB_REG,&Lsb,1);
    bmp280_read_regs(&bmp280_dev,BMP280_DIG_P2_MSB_REG,&Msb,1);
    bmp280_dev.bmp280_dig.P2 = ((unsigned short)Msb << 8) + Lsb;
    bmp280_read_regs(&bmp280_dev,BMP280_DIG_P3_LSB_REG,&Lsb,1);
    bmp280_read_regs(&bmp280_dev,BMP280_DIG_P3_MSB_REG,&Msb,1);
    bmp280_dev.bmp280_dig.P3 = ((unsigned short)Msb << 8) + Lsb;
    bmp280_read_regs(&bmp280_dev,BMP280_DIG_P4_LSB_REG,&Lsb,1);
    bmp280_read_regs(&bmp280_dev,BMP280_DIG_P4_MSB_REG,&Msb,1);
    bmp280_dev.bmp280_dig.P4 = ((unsigned short)Msb << 8) + Lsb;
    bmp280_read_regs(&bmp280_dev,BMP280_DIG_P5_LSB_REG,&Lsb,1);
    bmp280_read_regs(&bmp280_dev,BMP280_DIG_P5_MSB_REG,&Msb,1);
    bmp280_dev.bmp280_dig.P5 = ((unsigned short)Msb << 8) + Lsb;
    bmp280_read_regs(&bmp280_dev,BMP280_DIG_P6_LSB_REG,&Lsb,1);
    bmp280_read_regs(&bmp280_dev,BMP280_DIG_P6_MSB_REG,&Msb,1);
    bmp280_dev.bmp280_dig.P6 = ((unsigned short)Msb << 8) + Lsb;
    bmp280_read_regs(&bmp280_dev,BMP280_DIG_P7_LSB_REG,&Lsb,1);
    bmp280_read_regs(&bmp280_dev,BMP280_DIG_P7_MSB_REG,&Msb,1);
    bmp280_dev.bmp280_dig.P7 = ((unsigned short)Msb << 8) + Lsb;
    bmp280_read_regs(&bmp280_dev,BMP280_DIG_P8_LSB_REG,&Lsb,1);
    bmp280_read_regs(&bmp280_dev,BMP280_DIG_P8_MSB_REG,&Msb,1);
    bmp280_dev.bmp280_dig.P8 = ((unsigned short)Msb << 8) + Lsb;
    bmp280_read_regs(&bmp280_dev,BMP280_DIG_P9_LSB_REG,&Lsb,1);
    bmp280_read_regs(&bmp280_dev,BMP280_DIG_P9_MSB_REG,&Msb,1);
    bmp280_dev.bmp280_dig.P9 = ((unsigned short)Msb << 8) + Lsb;
    //要先读再复位，因为复位后，他的寄存器读不出来。
    Lsb = BMP280_RESET_VALUE;
    bmp280_write_regs(&bmp280_dev,BMP280_RESET_REG, &Lsb,1);

    bmp280_oversample_mode.P_Osample = BMP280_P_MODE_3;
    bmp280_oversample_mode.T_Osample = BMP280_T_MODE_1;
    bmp280_oversample_mode.WORKMODE = BMP280_NORMAL_MODE;
    bmp280_set_tempoversamp(&bmp280_oversample_mode);

    bmp280_config.T_SB = BMP280_T_SB5;
    bmp280_config.FILTER_COEFFICIENT = BMP280_FILTER_MODE_4;
    bmp280_config.SPI_EN = 0;
    bmp280set_standby_filter(&bmp280_config);

    return ret;
}

//获取BMP当前状态
//status_flag = BMP280_MEASURING ||
//			 	BMP280_IM_UPDATE
static unsigned char bmp280_getstatus(unsigned char status_flag)
{
	unsigned char flag;
	bmp280_read_regs(&bmp280_dev,BMP280_STATUS_REG,&flag,1);
	if(flag&status_flag)	return 1;
	else return 0;
}


static int bmp280_open(struct inode *inode, struct file *filp)
{
    filp->private_data = &bmp280_dev;
    return 0;
}
/*
    读取bmp280原始温度和压力值
    没有转换，需要用户层转换
返回值： 等于0，读取成功
        小于0 ，重新读取，bmp280状态不对
        大于0，重新读取，系统错误，copy_to_user 问题
*/
static ssize_t bmp280_read(struct file *filp, char __user *buf, size_t cnt, loff_t *off)
{
    unsigned char XLsb,Lsb,Msb;
    int src_bmp280data[2] = {0};
    int err = 0;
    struct bmp280_dev *dev = (struct bmp280_dev *)filp->private_data;

/*
    gpio_set_value(bmp280_dev.cs_gpio,1);
    mdelay(10);
    gpio_set_value(bmp280_dev.cs_gpio,0);
    mdelay(10);
    gpio_set_value(bmp280_dev.cs_gpio,1);
    return 0;
    */
    //判断状态,不
    if(bmp280_getstatus(BMP280_MEASURING) == 1)
        return -1;
    if(bmp280_getstatus(BMP280_IM_UPDATE) == 1)
        return -1;
    //读取原始数据，浮点数据转换在用户层
    bmp280_read_regs(dev,BMP280_PRESSURE_XLSB_REG,&XLsb,1);
    bmp280_read_regs(dev,BMP280_PRESSURE_LSB_REG,&Lsb,1);
    bmp280_read_regs(dev,BMP280_PRESSURE_MSB_REG,&Msb,1);
    src_bmp280data[0]= (int)((unsigned int)Msb<<12 | (unsigned int)Lsb<<4 | (unsigned int)XLsb>>4);

    bmp280_read_regs(dev,BMP280_TEMPERATURE_XLSB_REG,&XLsb,1);
    bmp280_read_regs(dev,BMP280_TEMPERATURE_LSB_REG,&Lsb,1);
    bmp280_read_regs(dev,BMP280_TEMPERATURE_MSB_REG,&Msb,1);
    src_bmp280data[1]= (int)((unsigned int)Msb<<12 | (unsigned int)Lsb<<4 | (unsigned int)XLsb>>4);
    //printk("kernel debug src_press 0x%x src_temp 0x%x \r\n",src_bmp280data[0],src_bmp280data[1]);

    err = copy_to_user(buf,src_bmp280data,sizeof(src_bmp280data));
    err = copy_to_user(buf + sizeof(src_bmp280data), &bmp280_dev.bmp280_dig, sizeof(bmp280_dig_t));
    return err;
}
static int bmp280_release(struct inode *inode, struct file *filp)
{
    return 0;
}

static const struct file_operations bmp280_ops = {
    .owner = THIS_MODULE,
    .open = bmp280_open,
    .read = bmp280_read,
    .release = bmp280_release,
};

static int bmp280_probe(struct spi_device *spi)
{
    int ret = 0;
    if(bmp280_dev.major){
        bmp280_dev.devid = MKDEV(bmp280_dev.major,0);
        register_chrdev_region(bmp280_dev.devid, BMP280_CNT,BMP280_NAME);
    }else{
        alloc_chrdev_region(&bmp280_dev.devid,0,BMP280_CNT,BMP280_NAME);
        bmp280_dev.major = MAJOR(bmp280_dev.devid);
    }

    cdev_init(&bmp280_dev.cdev,&bmp280_ops);
    cdev_add(&bmp280_dev.cdev,bmp280_dev.devid,BMP280_CNT);

    bmp280_dev.class = class_create(THIS_MODULE,BMP280_NAME);
    if(IS_ERR(bmp280_dev.class)){
        return PTR_ERR(bmp280_dev.class);
    }

    bmp280_dev.device = device_create(bmp280_dev.class, NULL, bmp280_dev.devid, NULL, BMP280_NAME);
    if(IS_ERR(bmp280_dev.device)){
        return PTR_ERR(bmp280_dev.device);
    }

    bmp280_dev.nd = of_find_node_by_path("/soc@3000000/spi@4026000/bmp280@0");
    if(bmp280_dev.nd == NULL){
        printk("bmp280 node not find %d\r\n",__LINE__);
        return -EINVAL;
    }
    bmp280_dev.cs_gpio = of_get_named_gpio(bmp280_dev.nd, "cs_gpio",0);
    if(bmp280_dev.cs_gpio < 0){
        printk("can't get cs-gpio %d \r\n",__LINE__);
        return -EINVAL;
    }

    ret = gpio_direction_output(bmp280_dev.cs_gpio, 1);
    if(ret < 0){
        printk("can't set cs gpio %d\r\n",__LINE__);
    }

    bmp280_dev.mosi_gpio = of_get_named_gpio(bmp280_dev.nd, "mosi_gpio",0);
    if(bmp280_dev.mosi_gpio < 0){
        printk("can't get mosi-gpio %d \r\n",__LINE__);
        return -EINVAL;
    }

    ret = gpio_direction_output(bmp280_dev.mosi_gpio, 1);
    if(ret < 0){
        printk("can't set mosi gpio %d\r\n",__LINE__);
    }

    bmp280_dev.miso_gpio = of_get_named_gpio(bmp280_dev.nd, "miso_gpio",0);
    if(bmp280_dev.miso_gpio < 0){
        printk("can't get miso-gpio %d \r\n",__LINE__);
        return -EINVAL;
    }

    ret = gpio_direction_input(bmp280_dev.miso_gpio);
    if(ret < 0){
        printk("can't set miso gpio %d\r\n",__LINE__);
    }

    bmp280_dev.clk_gpio = of_get_named_gpio(bmp280_dev.nd, "clk_gpio",0);
    if(bmp280_dev.clk_gpio < 0){
        printk("can't get clk-gpio %d \r\n",__LINE__);
        return -EINVAL;
    }

    ret = gpio_direction_output(bmp280_dev.clk_gpio, 1);
    if(ret < 0){
        printk("can't set clk gpio %d\r\n",__LINE__);
    }

    //cs 下拉一次，锁定spi接口
    //for(ret = 0; ret < 10000; ret++){
    gpio_set_value(bmp280_dev.cs_gpio, 1);
    mdelay(1);
    gpio_set_value(bmp280_dev.cs_gpio, 0);
    mdelay(1);
    gpio_set_value(bmp280_dev.cs_gpio, 1);
    //}
    //spi->mode = SPI_MODE_0;
    //spi_setup(spi);
    //bmp280_dev.private_data = spi;

    //初始化bmp280
    bmp280_reginit();

    return 0;
}
static int bmp280_remove(struct spi_device *spi){
    cdev_del(&bmp280_dev.cdev);
    unregister_chrdev_region(bmp280_dev.devid, BMP280_CNT);

    device_destroy(bmp280_dev.class, bmp280_dev.devid);
    class_destroy(bmp280_dev.class);
    return 0;
}
static const struct spi_device_id bmp280_id[] = {
    {"tina-d1,rvboards-bmp280",0},
    {}
};

static const struct of_device_id bmp280_of_match[] = {
    {.compatible = "tina-d1,rvboards-bmp280"},
    {}
};

static struct spi_driver bmp280_driver = {
    .probe = bmp280_probe,
    .remove = bmp280_remove,
    .driver = {
        .owner = THIS_MODULE,
        .name = "bmp280",
        .of_match_table = bmp280_of_match,
    },
    .id_table = bmp280_id,
};

static int __init bmp280_init(void)
{
    return spi_register_driver(&bmp280_driver);
}

static void __exit bmp280_exit(void)
{
    spi_unregister_driver(&bmp280_driver);
}

module_init(bmp280_init);
module_exit(bmp280_exit);
MODULE_LICENSE("GPL");
MODULE_AUTHOR("chshwei");


