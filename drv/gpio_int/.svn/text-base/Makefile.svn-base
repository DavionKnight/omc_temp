ifneq ($(KERNELRELEASE),)
# call from kernel build system
obj-m	:= gpio_int.o
gpio_int-objs := main.o 

else
#KERNEL_RELEASE=$(shell uname -r)
#KERNELDIR ?= /usr/src/kernels/$(KERNEL_RELEASE) 
KERNELDIR ?= ~/linux-3.2.18 

PWD       := $(shell pwd)

default:
	$(MAKE) -C $(KERNELDIR) M=$(PWD) modules

endif

clean:
	rm -rf *.o *.ko *.ko.* *.mod.* modules.* Module.* .*.ko.cmd .*.mod.o.cmd .*.o.cmd
