#KERNELDIR = /home/h4/linuxsrc/linux-3.2.4/
#TFTPBOOT = /home/h4/tftpboot/
#CROSS_COMPILE=arm-linux-
CROSS = /usr/armtool/arm-2009q1/bin/arm-none-linux-gnueabi-
OBJ = dru_object_
CC = $(CROSS_COMPILE)gcc
LD = $(CROSS_COMPILE)ld
#FLAG=  -static -I $(KERNELDIR)

objects_main =dru_spi.c dru_object.c dru_ad9523_main.c dru_lmx2531.c dru_ads58c48.c dru_dac3484.c dru_mmap.c dru_fpga_download.c dru_ksz8873.c dru_iic.c omap_gpio.c dru_lmh6517.c dru_fpga_ram.c omap_epld.c
objects_rau =dru_spi.c dru_object.c dru_ad9523_rau.c dru_lmx2531.c dru_ads58c48.c dru_dac3484.c dru_mmap.c dru_fpga_download.c dru_ksz8873.c dru_iic.c omap_gpio.c dru_lmh6517.c dru_fpga_ram.c omap_epld.c
#all:clean main rau install
all:clean main rau 
main:$(objects_main)
	$(CC)  $(FLAG) $(objects_main) -o $(OBJ)$@
rau:$(objects_rau)
	$(CC)  $(FLAG) $(objects_rau) -o $(OBJ)$@
install:
	cp dru_object_* $(TFTPBOOT)

.PHONY:clean
clean :
	-rm dru_object_*  

