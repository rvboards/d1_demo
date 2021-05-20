# 光照强度传感器

## 模块介绍

1. ic控制芯片：bh1750fvi
2. 传输总线：i2c

## 源码介绍

### 驱动源码

1. bh1750fvi_drv.c
2. Makefile

编译

1. 在“tina-d1/lichee/”新建“drivers/bh1750fvi”文件夹
2. 把bh1750fvi_drv.c和Makefile 放到"bh1750fvi"文件夹内
3. 执行make，编译生成bh1750fvi_drv.ko
4. 把bh1750fvi_drv.ko复制到开发板上，执行insmod bh1750fvi_drv.ko ,加载驱动模块



### 测试源码

1. bh1750fvi_user.c

编译

1. 把bh1750fvi_user.c放到开发板上
2. 执行gcc -o bh1750fvi_user bh1750fvi_user.c，编译
3. 测试命令：./bh1750fvi_user /dev/bh1750fvi





