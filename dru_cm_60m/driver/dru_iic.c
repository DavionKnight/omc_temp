
/*******************************************************************************
********************************************************************************
* 文件名称:  dru_iic.c
* 功能描述:  模拟iic部分的实现。
*						
* 使用说明: 
* 文件作者:	H4
* 编写日期: （2012/07/03）
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
#include "../../driver/dru_iic.h"
#include "common.h"

/*******************************************************************************
* 函数名称: iic_delay
* 功    能: 用于spi工作延时
* 参    数:
* 参数名称         类型                描述
* delayTime      unsigned char      延时时长
* 返回值:
* 无
* 说   明:
* 日   期     版本    作者   修改人      DEBUG
* -----------------------------------------------------------------
* 2012/07/03  V1.0     H4     无       ：
*******************************************************************************/
void iic_delay(unsigned char delayTime)                                                   
{                                                                                        
    unsigned int index = 0;                                                                    
    for (index = 0; index < delayTime * 80; index++)   
    {                                                                                    
        asm(" nop");                                                                     
    }                                        
	//	usleep(delayTime);
    return;                                                                              
}          
/*******************************************************************************
* 函数名称: dru_iic_8873_init
* 功    能: 初始化8873 的iic
* 参    数:
* 参数名称         类型                描述
*
* 返回值:
* 无
* 说   明:
* 日   期     版本    作者   修改人      DEBUG
* -----------------------------------------------------------------
* 2012/07/04  V1.0     H4     无       ：
*******************************************************************************/
void dru_iic_8873_init(void)
{
	DRU_IIC_8873_SAVE_DIRREG();
	DRU_ICC_8873_SCL_DIR_OUT();
	DRU_ICC_8873_SDA_DIR_OUT();
	//printf("gpio->dir01=%x\n",gpio_t->dir01);
	DRU_ICC_8873_SCL_OUT_LOW();
	DRU_ICC_8873_SDA_OUT_LOW();
}

/*******************************************************************************
* 函数名称: dru_iic_8873_start
* 功    能: 8873 的iic start
* 参    数:
* 参数名称         类型                描述
*
* 返回值:
* 无
* 说   明:
* 日   期     版本    作者   修改人      DEBUG
* -----------------------------------------------------------------
* 2012/07/04  V1.0     H4     无       ：
*******************************************************************************/
void dru_iic_8873_start(void)
{
	DRU_ICC_8873_SCL_OUT_LOW();
	iic_delay(IIC_DELAY_TIME);
	DRU_ICC_8873_SDA_OUT_HIGH();
	iic_delay(IIC_DELAY_TIME);
	DRU_ICC_8873_SCL_OUT_HIGH();
	iic_delay(IIC_DELAY_TIME);
	DRU_ICC_8873_SDA_OUT_LOW();
	iic_delay(IIC_DELAY_TIME);
	DRU_ICC_8873_SCL_OUT_LOW();
	iic_delay(IIC_DELAY_TIME);
}
/*******************************************************************************
* 函数名称: dru_iic_8873_stop
* 功    能: 8873 的iic stop
* 参    数:
* 参数名称         类型                描述
*
* 返回值:
* 无
* 说   明:
* 日   期     版本    作者   修改人      DEBUG
* -----------------------------------------------------------------
* 2012/07/04  V1.0     H4     无       ：
*******************************************************************************/
void dru_iic_8873_stop(void)
{
	DRU_ICC_8873_SDA_OUT_LOW();
	iic_delay(IIC_DELAY_TIME);
	DRU_ICC_8873_SCL_OUT_HIGH();
	iic_delay(IIC_DELAY_TIME);
	DRU_ICC_8873_SDA_OUT_HIGH();
	iic_delay(IIC_DELAY_TIME);
	DRU_ICC_8873_SCL_OUT_LOW();
	iic_delay(IIC_DELAY_TIME);
}
/*******************************************************************************
* 函数名称: dru_iic_8873_ack
* 功    能: 8873 的iic ack
* 参    数:
* 参数名称         类型                描述
*
* 返回值:
* 无
* 说   明:
* 日   期     版本    作者   修改人      DEBUG
* -----------------------------------------------------------------
* 2012/07/04  V1.0     H4     无       ：
*******************************************************************************/
int dru_iic_8873_ack(void)
{
	unsigned int data;
	DRU_ICC_8873_SDA_DIR_IN();
	iic_delay(IIC_DELAY_TIME);
	DRU_ICC_8873_SCL_OUT_HIGH();
	iic_delay(IIC_DELAY_TIME);
	DRU_ICC_8873_SDA_IN(data);
	iic_delay(IIC_DELAY_TIME);
	DRU_ICC_8873_SCL_OUT_LOW();
	iic_delay(2*IIC_DELAY_TIME);
	DRU_ICC_8873_SDA_DIR_OUT();
	iic_delay(2*IIC_DELAY_TIME);
	if(data!=0){
		return 1;
	}
	return 0;
}
/*******************************************************************************
* 函数名称: dru_iic_8873_noack
* 功    能: 8873 的
* 参    数:
* 参数名称         类型                描述
*
* 返回值:
* 无
* 说   明:
* 日   期     版本    作者   修改人      DEBUG
* -----------------------------------------------------------------
* 2012/07/04  V1.0     H4     无       ：
*******************************************************************************/
void dru_iic_8873_noack(void)
{
	DRU_ICC_8873_SDA_OUT_HIGH();
	iic_delay(IIC_DELAY_TIME);
	DRU_ICC_8873_SCL_OUT_HIGH();
	iic_delay(IIC_DELAY_TIME);
	DRU_ICC_8873_SCL_OUT_LOW();
	iic_delay(IIC_DELAY_TIME);
}
/*******************************************************************************
* 函数名称: dru_iic_8873_read_byte
* 功    能: 8873 的iic 读单个字节
* 参    数:
* 参数名称         类型                描述
*data 			unsigned char *		存储8873 iic所读数据的地址
* 返回值:
* 无
* 说   明:
* 日   期     版本    作者   修改人      DEBUG
* -----------------------------------------------------------------
* 2012/07/04  V1.0     H4     无       ：
*******************************************************************************/
void dru_iic_8873_read_byte(unsigned char *data)
{
	unsigned char index=0;
	unsigned char tmp_data=0;
	unsigned int level=0;
	DRU_ICC_8873_SDA_DIR_IN();
	iic_delay(IIC_DELAY_TIME);
	for(index=0;index<8;index++)
	{
		DRU_ICC_8873_SCL_OUT_HIGH();
		iic_delay(IIC_DELAY_TIME);
		DRU_ICC_8873_SDA_IN(level);
		iic_delay(IIC_DELAY_TIME);
		DRU_ICC_8873_SCL_OUT_LOW();
		iic_delay(2*IIC_DELAY_TIME);
		//printf("level=%x\n",level);
		if(level){
			tmp_data|=(unsigned char)(1<<(7-index));
		}
	}
	*data=tmp_data;
	//printf("read iic=%x\n",tmp_data);
	DRU_ICC_8873_SDA_DIR_OUT();
	iic_delay(2*IIC_DELAY_TIME);
	return;
}
/*******************************************************************************
* 函数名称: dru_iic_8873_write_byte
* 功    能: 8873 的iic 写单个字节
* 参    数:
* 参数名称         类型                描述
*data 			unsigned char 	   8873 iic需要写的数据
* 返回值:
* 无
* 说   明:
* 日   期     版本    作者   修改人      DEBUG
* -----------------------------------------------------------------
* 2012/07/04  V1.0     H4     无       ：
*******************************************************************************/
void dru_iic_8873_write_byte(unsigned char data)
{
	unsigned char index=0,data_tmp;
	data_tmp=data;
	for(index=0;index<8;index++){
		if(((data_tmp<<index)&0x80)!=0){
			DRU_ICC_8873_SDA_OUT_HIGH();
			iic_delay(IIC_DELAY_TIME);
			DRU_ICC_8873_SCL_OUT_HIGH();
			//printf("gpio->set_data01=%x\n",gpio_t->set_data01);
			iic_delay(IIC_DELAY_TIME);
			DRU_ICC_8873_SCL_OUT_LOW();
			//printf("gpio->clr_data01=%x\n",gpio_t->clr_data01);
			iic_delay(IIC_DELAY_TIME);
		}else{
			DRU_ICC_8873_SDA_OUT_LOW();
			iic_delay(IIC_DELAY_TIME);
			DRU_ICC_8873_SCL_OUT_HIGH();
			//printf("gpio->set_data01=%x\n",gpio_t->set_data01);
			iic_delay(IIC_DELAY_TIME);			
			DRU_ICC_8873_SCL_OUT_LOW();
			iic_delay(IIC_DELAY_TIME);
			//printf("gpio->clr_data01=%x\n",gpio_t->clr_data01);
		}
		//printf("write index=%x\n",index);
	}
	return;
}
/*******************************************************************************
* 函数名称: dru_8873_write
* 功    能: 8873 的寄存器中写入数据
* 参    数:
* 参数名称         类型                描述
*addr 			unsigned char 	   8873寄存器地址
*data 			unsigned char 	   8873 寄存器需要写的数据
* 返回值:
* 无
* 说   明:
* 日   期     版本    作者   修改人      DEBUG
* -----------------------------------------------------------------
* 2012/07/04  V1.0     H4     无       ：
*******************************************************************************/
int dru_8873_write(unsigned char addr,unsigned char data)
{
	dru_iic_8873_init();
	dru_iic_8873_start();
	dru_iic_8873_write_byte(IIC_8873_WRITE_ADDR);
	if(dru_iic_8873_ack()){
		dru_iic_8873_stop();
		return 1;
	}
	dru_iic_8873_write_byte(addr);
	if(dru_iic_8873_ack()){
		dru_iic_8873_stop();
		return 1;
	}
	dru_iic_8873_write_byte(data);
	if(dru_iic_8873_ack()){
		dru_iic_8873_stop();
		return 1;
	}
	dru_iic_8873_stop();
	return 0;
}
int dru_8873_write_change(int argc, char * argv[])
{
	unsigned char para1;
	unsigned char para2;

	msg_tmp.mtype = MSG_FUN_RESULT;	
	if(argc != 3){
		printf("input para cnt is not 3.\r\n");
		sprintf(msg_tmp.mtext, "input para cnt is not 3.\r\n");
		msg_send(TASK2_MODULE, (char *)&msg_tmp, strlen(msg_tmp.mtext));
		return 1;
	}
	para1 = strtol(argv[1], NULL, 16);
	para2 = strtol(argv[2], NULL, 16);
	printf("para1=%x. para2=%x.\r\n", para1, para2);
	dru_8873_write((unsigned char)para1, (unsigned char)para2);
	sprintf(msg_tmp.mtext, "dru_8873_write reg:0x%x=0x%x.\r\n", para1, para2);
	msg_send(TASK2_MODULE, (char *)&msg_tmp, strlen(msg_tmp.mtext));
	return 0;
}
/*******************************************************************************
* 函数名称: dru_8873_read
* 功    能: 从8873 的寄存器中读数据
* 参    数:
* 参数名称         类型                描述
*addr 			unsigned char 	   8873寄存器地址
*data 			unsigned char *	   存储8873 所读数据的地址
* 返回值:
* 无
* 说   明:
* 日   期     版本    作者   修改人      DEBUG
* -----------------------------------------------------------------
* 2012/07/04  V1.0     H4     无       ：
*******************************************************************************/
int dru_8873_read(unsigned char addr,unsigned char *data)
{
	unsigned char tmp_data=0;
	dru_iic_8873_init();
			//	iic_delay(10*IIC_DELAY_TIME);
	//usleep(1000);
	dru_iic_8873_start();
	//iic_delay(10*IIC_DELAY_TIME);
	dru_iic_8873_write_byte(IIC_8873_WRITE_ADDR);
	if(dru_iic_8873_ack()){
		dru_iic_8873_stop();
		printf("ack err write first\n");
		return 1;
	}
	dru_iic_8873_write_byte(addr);
	if(dru_iic_8873_ack()){
		dru_iic_8873_stop();
		printf("ack err write addr\n");
		return 1;
	}
	//iic_delay(10*IIC_DELAY_TIME);
	dru_iic_8873_start();
	//iic_delay(10*IIC_DELAY_TIME);
	dru_iic_8873_write_byte(IIC_8873_READ_ADDR);
	if(dru_iic_8873_ack()){
		dru_iic_8873_stop();
		printf("ack err write read\n");
		return 1;
	}
	//iic_delay(10*IIC_DELAY_TIME);
	dru_iic_8873_read_byte(&tmp_data);
	//iic_delay(10*IIC_DELAY_TIME);
	*data=tmp_data;
	dru_iic_8873_noack();
	//iic_delay(10*IIC_DELAY_TIME);
	dru_iic_8873_stop();
	//printf("ack ok\n");

	return 0;
}
int dru_8873_read_change(int argc, char * argv[])
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
	dru_8873_read((unsigned char)para1, (unsigned char *)&tmp);
	sprintf(msg_tmp.mtext, "dru_8873_read reg:0x%x=0x%x.\r\n", para1, tmp);
	msg_send(TASK2_MODULE, (char *)&msg_tmp, strlen(msg_tmp.mtext));
	return 0;
}
