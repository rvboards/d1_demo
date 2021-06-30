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
#include <linux/of.h>
#include <linux/of_address.h>
#include <linux/of_gpio.h>
#include <asm/uaccess.h>
#include <asm/io.h>

//	rvboards_dht11:rvboards_dht11@0{
//		#address-cells = <1>;
//		#size-cells = <0>;
//		pinctrl-names = "default", "sleep";
//		compatible = "rvboards-d1,dht11";
//		dht11_gpio = <&pio PC 1 GPIO_ACTIVE_HIGH>;
//		status = "disabled";
//	};

#define DHT11_CNT       1
#define DHT11_NAME      "dht11"

union dht11_src_data{
    unsigned char bt_t[5];
    struct{
        unsigned char data[4];
        unsigned char chechsum;
    }dt;
};
struct dht11_dev{
    dev_t devid;
    struct cdev cdev;
    struct class *class;
    struct device *device;
    int major;
    int minor;
    struct device_node *nd;
    int dht11_gpio;

}dht11_dev;

static int dht11_open(struct inode *inode, struct file *filp)
{
    filp->private_data = &dht11_dev;
    return 0;
}
struct char_bt{
    unsigned char a0:1;
    unsigned char a1:1;
    unsigned char a2:1;
    unsigned char a3:1;
    unsigned char a4:1;
    unsigned char a5:1;
    unsigned char a6:1;
    unsigned char a7:1;
};
union union_bt {
    unsigned char dt;
    struct char_bt bt;
};
//读取dht11 数据 ,计录5ms内数据
//80us 低  80us 高，40 *（50us + (01)? 30 : 70us）
static int dht11_bus_data(struct dht11_dev *dev, union dht11_src_data *src_data)
{
    unsigned char dht11_cnt[1000];
    struct char_bt dht11_bt;
    union union_bt dht11_dt;
    unsigned char i = 0;
    int rd_cnt,j;
    int status,status0;
    status=status0 = 0;
    rd_cnt = j = 0;
    while(j< 600){
        status = gpio_get_value(dev->dht11_gpio);
        if(status0 != status){
            dht11_cnt[rd_cnt++] = i;
            i = 0;
        }
        if(i > 100){
            udelay(5000);
            break;
        }
        status0 = status;
        j++;
        i++;
        udelay(9);
    }
    if(dht11_cnt[0] >= 20)
        return 1;
    rd_cnt = 0;
    for(j = 3,i=0; j < 82;j+=16,i++){
        if(dht11_cnt[j+14] > 5)     dht11_bt.a0 = 1 ;
        else                        dht11_bt.a0 = 0;
        if(dht11_cnt[j+12] > 5)     dht11_bt.a1 = 1;
        else                        dht11_bt.a1 = 0;
        if(dht11_cnt[j+10] > 5)     dht11_bt.a2 = 1;
        else                        dht11_bt.a2 = 0;
        if(dht11_cnt[j+8] > 5)      dht11_bt.a3 = 1;
        else                        dht11_bt.a3 = 0;
        if(dht11_cnt[j+6] > 5)      dht11_bt.a4 = 1;
        else                        dht11_bt.a4 = 0;
        if(dht11_cnt[j+4] > 5)      dht11_bt.a5 = 1;
        else                        dht11_bt.a5 = 0;
        if(dht11_cnt[j+2] > 5)      dht11_bt.a6 = 1;
        else                        dht11_bt.a6 = 0;
        if(dht11_cnt[j] > 5)        dht11_bt.a7 = 1;
        else                        dht11_bt.a7 = 0;
        dht11_dt.bt = dht11_bt;
        src_data->bt_t[i] = dht11_dt.dt;
    }
    
    return 0;
}

static ssize_t dht11_read(struct file *filp, char __user *buf, size_t cnt, loff_t *offt)
{
    union dht11_src_data src_data;
    unsigned char tmp_data = 0;
    int ret,rd_cnt;
    struct dht11_dev *dev = (struct dht11_dev*)filp->private_data;
    //主机拉抵20ms，向dht11 发送开始信号
    gpio_set_value(dev->dht11_gpio, 0);
    mdelay(20);
    //主机拉高
    gpio_set_value(dev->dht11_gpio, 1);
    udelay(50);
    //主机拉低
    gpio_set_value(dev->dht11_gpio,0);
    //主机切换输入模式，等待dht11 响应信号
    ret = gpio_direction_input(dev->dht11_gpio);
    if(ret < 0){
        printk("can't set dht11 gpio\r\n");
        goto ERR_STATUS;
    }
    //读取dht11 数据 ,计录5ms内数据
    //80us 低  80us 高，40 *（50us + (01)? 30 : 70us）
    ret = dht11_bus_data(dev,&src_data);
    if(ret != 0){
        goto ERR_STATUS;
    }
    //校验dht11 数据
    for(rd_cnt = 0; rd_cnt < 4; rd_cnt++){
        tmp_data += src_data.dt.data[rd_cnt];
    }
    if(tmp_data != src_data.dt.chechsum){
        goto ERR_CHECHSUM;
    }
    //主机拉高，总线
    ret = gpio_direction_output(dev->dht11_gpio, 1);
    if(ret < 0){
        printk("can't set dht11 gpio\r\n");
        return -EINVAL;
    }
    ret = copy_to_user(buf, src_data.dt.data, 4);

    return 0;
ERR_CHECHSUM: 
ERR_STATUS:
    //主机拉高，总线
    ret = gpio_direction_output(dev->dht11_gpio, 1);
    if(ret < 0){
        printk("can't set dht11 gpio\r\n");
        return -EINVAL;
    }
    return 1;
}

static ssize_t dht11_write(struct file *filp, const char __user *buf, size_t cnt, loff_t *offt)
{
    return 0;
}

static int dht11_release(struct inode *inode, struct file *filp)
{
    return 0;
}

static struct file_operations dht11_fops = {
    .owner = THIS_MODULE,
    .open = dht11_open,
    .read = dht11_read,
    .write = dht11_write,
    .release = dht11_release,
};



static int __init dht11_init(void)
{
    int ret = 0;
    dht11_dev.nd = of_find_node_by_path("/rvboards_dht11@0");
    if(dht11_dev.nd == NULL){
        printk("dht11 node can't found %s %d\r\n",__FUNCTION__,__LINE__);
        return -EINVAL;
    }else {
        printk("dht11 node has been found %s %d \r\n",__FUNCTION__,__LINE__);
    }

    dht11_dev.dht11_gpio = of_get_named_gpio(dht11_dev.nd, "dht11_gpio",0);
    if(dht11_dev.dht11_gpio < 0){
        printk("can't get dth11_gpio %s %d \r\n",__FUNCTION__,__LINE__);
        return -EINVAL;
    }
    printk("dht11_gpio num = %d \r\n",dht11_dev.dht11_gpio);
    ret = gpio_direction_output(dht11_dev.dht11_gpio, 1);
    if(ret < 0){
        printk("can't set dht11 gpio\r\n");
    }

    if(dht11_dev.major){
        dht11_dev.devid = MKDEV(dht11_dev.major, 0);
        register_chrdev_region(dht11_dev.devid, DHT11_CNT,DHT11_NAME);
    }else{
        alloc_chrdev_region(&dht11_dev.devid, 0, DHT11_CNT, DHT11_NAME);
        dht11_dev.major = MAJOR(dht11_dev.devid);
        dht11_dev.minor = MINOR(dht11_dev.devid);
    }
    printk("dht11 major=%d, minor=%d\r\n",dht11_dev.major,dht11_dev.minor);

    dht11_dev.cdev.owner = THIS_MODULE;
    cdev_init(&dht11_dev.cdev, &dht11_fops);

    cdev_add(&dht11_dev.cdev, dht11_dev.devid, DHT11_CNT);

    dht11_dev.class = class_create(THIS_MODULE,DHT11_NAME);
    if(IS_ERR(dht11_dev.class)){
        return PTR_ERR(dht11_dev.class);
    }

    dht11_dev.device = device_create(dht11_dev.class, NULL, dht11_dev.devid, NULL, DHT11_NAME);
    if(IS_ERR(dht11_dev.device)){
        return PTR_ERR(dht11_dev.device);
    }

    return 0;
}

static void __exit dht11_exit(void)
{
    cdev_del(&dht11_dev.cdev);
    unregister_chrdev_region(dht11_dev.devid, DHT11_CNT);

    device_destroy(dht11_dev.class, dht11_dev.devid);
    class_destroy(dht11_dev.class);
}

module_init(dht11_init);
module_exit(dht11_exit);
MODULE_LICENSE("GPL");
MODULE_AUTHOR("chshwei");