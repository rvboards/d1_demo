#include <linux/types.h>
#include <linux/kernel.h>
#include <linux/delay.h>
#include <linux/ide.h>
#include <linux/init.h>
#include <linux/module.h>
#include <linux/errno.h>
#include <linux/gpio.h>
#include <linux/cdev.h>
#include <linux/device.h>
#include <linux/of_gpio.h>
#include <linux/semaphore.h>
#include <linux/timer.h>
#include <linux/i2c.h>
//#include <asm/mach/map.h>
#include <asm/uaccess.h>
#include <asm/io.h>

/* bh1750fvi reg  */
#define BH1750FVI_REG_POWEROFF          0x0
#define BH1750FVI_REG_POWERON           0x1
#define BH1750FVI_REG_RESET             0x7
#define BH1750FVI_REG_H_MODE            0x10
#define BH1750FVI_REG_H_MODE2           0x11
#define BH1750FVI_REG_L_MODE            0x13
#define BH1750FVI_REG_ONCE_H_MODE       0x20
#define BH1750FVI_REG_ONCE_H_MODE2      0x21
#define BH1750FVI_REG_ONCE_L_MODE       0x23
#define BH1750FVI_REG_SET_M_TIME_H      0x40
#define BH1750FVI_REG_SET_M_TIME_L      0x60
/*采样模式*/
#define BH1750FVI_H_MODE            0x0
#define BH1750FVI_H_MODE2           0x1
#define BH1750FVI_L_MODE            0x2
#define BH1750FVI_ONCE_H_MODE       0x3
#define BH1750FVI_ONCE_H_MODE2      0x4
#define BH1750FVI_ONCE_L_MODE       0x5




#define BH1750FVI_CNT       1
#define BH1750FVI_NAME      "bh1750fvi"

struct bh1750fvi_dev{
    dev_t devid;
    struct cdev cdev;
    struct class *class;
    struct device *device;
    struct device_node *nd;
    int major;
    void *private_data;
    int alt_addr_gpio;
    int resolution_mode;
}bh1750fvi_dev;

static int bh1750fvi_read_regs(struct bh1750fvi_dev *dev, void *val, int len)
{
    int ret;
    struct i2c_msg msg;
    struct i2c_client *client = (struct i2c_client *)dev->private_data;
    /* msg 为读取数据 */
    msg.addr = client->addr;
    msg.flags = I2C_M_RD;
    msg.buf = val;
    msg.len = len;
    ret = i2c_transfer(client->adapter, &msg, 1);
    if(ret == 1){
        ret = 0;
    }else{
        printk("i2c rd failed=%d len=%d \n",ret, len);
        ret = -EREMOTEIO;
    }
    return ret;
}

static unsigned int bh1750fvi_write_regs(struct bh1750fvi_dev *dev, unsigned char reg, unsigned char*buf, unsigned char len)
{
    unsigned char *b;
    unsigned char i;
    struct i2c_msg msg;
    struct i2c_client *client = (struct i2c_client *)dev->private_data;
    b = kzalloc(len+1,GFP_KERNEL);
    b[0] = reg;
    //memcpy(&b[1], buf, len);
//    for(i = 0; i < 128; i++){
    msg.addr = client->addr;//i;
    msg.flags = 0;
    msg.buf = b;
    msg.len = len+1;
  //  i2c_transfer(client->adapter, &msg, 1);
 //   }

    return i2c_transfer(client->adapter, &msg, 1);
}


static int bh1750fvi_open(struct inode *inode, struct file *filp)
{
    filp->private_data = &bh1750fvi_dev;
    return 0;
}
static ssize_t bh1750fvi_read(struct file *filp, char __user *buf, size_t cnt, loff_t *off)
{
    int resolution_mode,ret;
    struct bh1750fvi_dev* dev = (struct bh1750fvi_dev*)filp->private_data;
    unsigned char data_buf[2] = {0};
    resolution_mode = dev->resolution_mode;

    switch(resolution_mode){
        case BH1750FVI_H_MODE:
        case BH1750FVI_H_MODE2:
        case BH1750FVI_L_MODE:
            bh1750fvi_read_regs(dev, data_buf,2);
        break;
        case BH1750FVI_ONCE_H_MODE:
            /* set power on */
            bh1750fvi_write_regs(&bh1750fvi_dev,BH1750FVI_REG_POWERON,NULL,0);
            /* 设置采样模式 */
            bh1750fvi_write_regs(&bh1750fvi_dev,BH1750FVI_REG_ONCE_H_MODE,NULL,0);
            mdelay(30);
            bh1750fvi_read_regs(dev, data_buf,2);
        break;
        case BH1750FVI_ONCE_H_MODE2:
            /* set power on */
            bh1750fvi_write_regs(&bh1750fvi_dev,BH1750FVI_REG_POWERON,NULL,0);
            /* 设置采样模式 */
            bh1750fvi_write_regs(&bh1750fvi_dev,BH1750FVI_REG_ONCE_H_MODE2,NULL,0);
            mdelay(30);
            bh1750fvi_read_regs(dev, data_buf,2);
        break;
        case BH1750FVI_ONCE_L_MODE:
            /* set power on */
            bh1750fvi_write_regs(&bh1750fvi_dev,BH1750FVI_REG_POWERON,NULL,0);
            /* 设置采样模式 */
            bh1750fvi_write_regs(&bh1750fvi_dev,BH1750FVI_REG_ONCE_L_MODE,NULL,0);
            mdelay(30);
            bh1750fvi_read_regs(dev, data_buf,2);
        break;
        default:
        break;
    }
    ret = copy_to_user(buf,data_buf,2);
    //printk("debug %s 0x%2x 0x%2x\r\n",__FUNCTION__,data_buf[0],data_buf[1]);
    return ret;
}
static int bh1750fvi_release(struct inode *inode, struct file *filp)
{
    return 0;
}

static const struct file_operations bh1750fvi_ops = {
    .owner = THIS_MODULE,
    .open = bh1750fvi_open,
    .read = bh1750fvi_read,
    .release = bh1750fvi_release,
};



static int bh1750fvi_probe(struct i2c_client *client, const struct i2c_device_id *id)
{
    int ret;
    if(bh1750fvi_dev.major){
        bh1750fvi_dev.devid = MKDEV(bh1750fvi_dev.major, 0);
        register_chrdev_region(bh1750fvi_dev.devid, BH1750FVI_CNT, BH1750FVI_NAME );
    }else {
        alloc_chrdev_region(&bh1750fvi_dev.devid, 0, BH1750FVI_CNT, BH1750FVI_NAME);
        bh1750fvi_dev.major = MAJOR(bh1750fvi_dev.devid);
    }

    cdev_init(&bh1750fvi_dev.cdev, &bh1750fvi_ops);
    cdev_add(&bh1750fvi_dev.cdev, bh1750fvi_dev.devid, BH1750FVI_CNT);

    bh1750fvi_dev.class = class_create(THIS_MODULE,BH1750FVI_NAME);
    if(IS_ERR(bh1750fvi_dev.class)){
        return PTR_ERR(bh1750fvi_dev.class);
    }

    bh1750fvi_dev.device = device_create(bh1750fvi_dev.class, NULL, bh1750fvi_dev.devid, NULL, BH1750FVI_NAME);
    if(IS_ERR(bh1750fvi_dev.device)){
        return PTR_ERR(bh1750fvi_dev.device);
    }

    bh1750fvi_dev.private_data = client;
    #if 0
    /* 获取设备树中 alt_addr_gpio 信号 */
    bh1750fvi_dev.nd = of_find_node_by_path("/soc@3000000/twi@2502800/bh1750fvi@46");
    if(bh1750fvi_dev.nd == NULL){
        printk("bh1750fvi node not find %s %d \r\n",__FUNCTION__,__LINE__);
        return -EINVAL;
    }
    bh1750fvi_dev.alt_addr_gpio = of_get_named_gpio(bh1750fvi_dev.nd, "alt_addr_gpio", 0);
    if(bh1750fvi_dev.alt_addr_gpio < 0){
        printk("can't get alt_addr_gpio %s %d\r\n",__FUNCTION__,__LINE__);
        return -EINVAL;
    }
    ret = gpio_direction_output(bh1750fvi_dev.alt_addr_gpio, 1);
    if(ret < 0){
        printk("can't set alt_addr_gpio %s %d \r\n",__FUNCTION__,__LINE__);
    }
    /*ALT  ADDRESS引脚接地时地址为0x46，接电源时地址为0xB8*/
    gpio_set_value(bh1750fvi_dev.alt_addr_gpio,0);
#endif
    /* 初始化 bh1750fvi */
    bh1750fvi_write_regs(&bh1750fvi_dev,BH1750FVI_REG_POWERON,NULL,0);
    /* 设置采样模式 */
    bh1750fvi_write_regs(&bh1750fvi_dev,BH1750FVI_REG_H_MODE,NULL,0);
    
    bh1750fvi_dev.resolution_mode = BH1750FVI_H_MODE;
    mdelay(180);

    return 0;
}
static int bh1750fvi_remove(struct i2c_client * client)
{
    cdev_del(&bh1750fvi_dev.cdev);
    unregister_chrdev_region(bh1750fvi_dev.devid, BH1750FVI_CNT);

    device_destroy(bh1750fvi_dev.class, bh1750fvi_dev.devid);
    class_destroy(bh1750fvi_dev.class);
    return 0;
}

static const struct i2c_device_id bh1750fvi_id[] = {
    {"tina-d1,rvboards-bh1750fvi",0},
    {},
};

static const struct of_device_id bh1750fvi_of_match[] = {
    {.compatible = "tina-d1,rvboards-bh1750fvi"},
    {},
};

static struct i2c_driver bh1750fvi_driver = {
    .probe = bh1750fvi_probe,
    .remove = bh1750fvi_remove,
    .driver = {
        .owner = THIS_MODULE,
        .name = "bh1750fvi",
        .of_match_table = bh1750fvi_of_match,
    },
    .id_table = bh1750fvi_id,
};

static int __init bh1750fvi_init(void)
{
    int ret = i2c_add_driver(&bh1750fvi_driver);
    return ret;
}

static void __exit bh1750fvi_exit(void)
{
    i2c_del_driver(&bh1750fvi_driver);
}

module_init(bh1750fvi_init);
module_exit(bh1750fvi_exit);
MODULE_LICENSE("GPL");
MODULE_AUTHOR("chshwei");