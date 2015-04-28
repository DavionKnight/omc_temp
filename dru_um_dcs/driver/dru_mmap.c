/*******************************************************************************
********************************************************************************
* 文件名称:  dru_mmap.c
* 功能描述:  将物理地址映射到用户空间
* 使用说明: 
* 文件作者:	H4
* 编写日期: （2012/06/18）
* 修改历史:
* 修改日期    修改人       修改内容
*-------------------------------------------------------------------------------

*******************************************************************************/
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <sys/mman.h>
#include <string.h>
#include <time.h>
#include "../../driver/omap_gpio.h"
#include "../../driver/omap_fpga.h"
#include "../../driver/omap_epld.h"
#include "../../driver/omap_pinmux.h"
#include "../../driver/omap_emif.h"
#include "common.h"
	int fd;
gpio_reg *gpio_t;
unsigned int *fpga_base_addr;
unsigned int *epld_base_addr;
pinmux_reg *pinmux_t;
void *pinmux_tmp;
emif_reg *emif_t;

/*******************************************************************************
* 函数名称: dru_get_val
* 功    能: 将物理地址映射到用户空间
* 参    数:
* 参数名称         类型                描述
* 无
* 返回值:
* 无
* 说   明:
* 日   期     版本    作者   修改人      DEBUG
* -----------------------------------------------------------------
* 2012/06/15  V1.0     H4     无       ：
*******************************************************************************/
int dru_get_val(void)
{

	unsigned int pagesize;

	char dev_name[] = "/dev/mem"; 
	fd  =  open(dev_name,O_RDWR); 
       if(fd<0){ 
               printf("open %s is error\n",dev_name); 
               return -1 ; 
        } 
	pagesize = sysconf(_SC_PAGESIZE); 

	//gpio_t = (gpio_reg *)mmap(NULL,pagesize,PROT_READ|PROT_WRITE, MAP_SHARED,fd,GPIO_PHY_ADDR_BASE);
	pinmux_tmp=mmap(NULL,pagesize,PROT_READ|PROT_WRITE, MAP_SHARED,fd,PINMUX_PHY_ADDR_BASE);
	pinmux_t=(pinmux_reg *)(pinmux_tmp+PINMUX_ADDR_OFFSET);
	//epld_t=(epld_reg *)mmap(pinmux_t+pagesize-PINMUX_ADDR_OFFSET,pagesize,PROT_READ|PROT_WRITE, MAP_SHARED,fd,EPLD_PHY_ADDR_BASE);
	//fpga_t=(fpga_reg *)mmap(epld_t+pagesize,pagesize*4,PROT_READ|PROT_WRITE, MAP_SHARED,fd,FPGA_PHY_ADDR_BASE);
	emif_t=(emif_reg *)mmap(NULL,pagesize*8,PROT_READ|PROT_WRITE, MAP_SHARED,fd,EMIF_PHY_ADDR_BASE);	
	if((pinmux_t == NULL)||(emif_t == NULL)){ 
                printf("gpio base mmap is error\n"); 
                close(fd); 
                return -1; 
        } 
	omap_pinmux_init();
	omap_emif_init();
 	sleep(1);
	close(fd); 
	   
	   sleep(1);
	  fd  =  open(dev_name,O_RDWR|O_SYNC); 
       if(fd<0){ 
               printf("open %s is error\n",dev_name); 
               return -1 ; 
        } 
	gpio_t = (gpio_reg *)mmap(NULL,pagesize,PROT_READ|PROT_WRITE, MAP_SHARED,fd,GPIO_PHY_ADDR_BASE);
	epld_base_addr=(unsigned int *)mmap(NULL,pagesize,PROT_READ|PROT_WRITE, MAP_SHARED,fd,EPLD_PHY_ADDR_BASE);
	printf("epld_base_addr=0x%x.\r\n", epld_base_addr);
	fpga_base_addr=(unsigned int *)mmap(NULL,pagesize*4,PROT_READ|PROT_WRITE, MAP_SHARED,fd,FPGA_PHY_ADDR_BASE);
	printf("fpga_base_addr=0x%x.\r\n", fpga_base_addr);
	//close(fd);
	return 1;
}
/*******************************************************************************
* 函数名称: dru_rm_val
* 功    能: 取消地址映射
* 参    数:
* 参数名称         类型                描述
* 无
* 返回值:
* 无
* 说   明:
* 日   期     版本    作者   修改人      DEBUG
* -----------------------------------------------------------------
* 2012/06/15  V1.0     H4     无       ：
*******************************************************************************/
int dru_rm_val(void)
{
	unsigned int pagesize;
	pagesize = sysconf(_SC_PAGESIZE); 
	munmap(gpio_t,pagesize);
	munmap(epld_base_addr,pagesize);
	munmap(epld_base_addr,pagesize*4);
	munmap(pinmux_tmp,pagesize);
	munmap(emif_t,pagesize*8);
	close(fd);
	return 1;
}

// gpio pinmux
#define PINMUX4 0x01C14130
#define pinmux4_reg *(volatile int *)(PINMUX4)
int pinmux_init(void)
{
/*
	tmp = pinmux4_reg;
	tmp &= ~(0xf<<20);
	tmp |= (8<<20);
	pinmux4_reg = tmp;
	*/
	MUX(4,5,8);
	return 0;
}
#define DIR01 0x01E26010
#define dir01_reg *(volatile int *)(DIR01)
int set_dir(void)
{
	int tmp = 0;
/*
	tmp = dir01_reg;
	tmp &= ~(1<<18);
	dir01_reg = tmp;
	*/
	tmp = gpio_t->dir01;
	tmp &= ~(1<<18);
	gpio_t->dir01 = tmp;
	return 0;
}
#define OUT_DATA01 0x01E26014
#define out_data01_reg *(volatile int *)(OUT_DATA01)
int set_data(int value)
{
	int tmp = 0;
	
	if(value == 1){
		gpio_t->set_data01 |= 1<<18;
		printf("set_data01=0x%x.\r\n", gpio_t->set_data01);
	}else{
		gpio_t->clr_data01 |= 1<<18;
		printf("clr_data01=0x%x.\r\n", gpio_t->clr_data01);
	}
	printf("out_data01=0x%x\r\n", gpio_t->out_data01);

	return 0;
}
int set_pin_tx_change(int argc, char * argv[])
{
	unsigned char para1;
	unsigned char para2;
	unsigned char tmp = 0;

	msg_tmp.mtype = MSG_FUN_RESULT;	
	if(argc != 2){
		printf("input para cnt is not 2.\r\n");
		sprintf(msg_tmp.mtext, "input para cnt is not 2.\r\n");
		msg_send(TASK2_MODULE, (char *)&msg_tmp, strlen(msg_tmp.mtext));
		return 1;
	}
	para1 = strtol(argv[1], NULL, 16);
	set_data((unsigned char)para1);
	sprintf(msg_tmp.mtext, "set_pin_tx=%d.\n", para1);
	msg_send(TASK2_MODULE, (char *)&msg_tmp, strlen(msg_tmp.mtext));
	return 0;
}


