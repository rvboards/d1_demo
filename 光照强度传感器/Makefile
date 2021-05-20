CROSS_COMPLE :=  $(PWD)/../../brandy-2.0/tools/toolchain/riscv64-linux-x86_64-20200528/bin/riscv64-unknown-linux-gnu-
CC:=$(CROSS_COMPLE)gcc
LD:=$(CROSS_COMPLE)ld
ARCH := riscv
KERNELDIR := $(PWD)/../../linux-5.4/
CURRENT_PATH := $(shell pwd)
obj-m := bh1750fvi_drv.o
module-objs = bh1750fvi_drv.o
default:
	$(MAKE) -C $(KERNELDIR) M=$(PWD) modules ARCH=riscv
clean:
	$(MAKE) -C $(KERNELDIR) M=$(CURRENT_PATH) clean
