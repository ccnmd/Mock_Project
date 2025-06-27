obj-m := led_driver.o btn_driver.o

#duong dan den thu muc kernel source hoac headers da build boi buildroot cho rpi4
KDIR := /home/duong/buildroot/output/build/linux-custom


#duong dan den thu muc bin cua toolchain buildroot
BUILDROOT_TOOLCHAIN_BIN := /home/duong/buildroot/output/host/bin

#toolchain cross-compiler cho ARM32
CROSS_COMPILE := $(BUILDROOT_TOOLCHAIN_BIN)/arm-buildroot-linux-gnueabihf-
ARCH := arm

#thu muc hien tai cua driver
PWD := $(shell pwd)

all:
	$(MAKE) -C $(KDIR) ARCH=$(ARCH) CROSS_COMPILE=$(CROSS_COMPILE) M=$(PWD) modules

clean:
	$(MAKE) -C $(KDIR) ARCH=$(ARCH) CROSS_COMPILE=$(CROSS_COMPILE) M=$(PWD) clean

.PHONY: all clean
