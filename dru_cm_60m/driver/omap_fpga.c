/*******************************************************************************
********************************************************************************
* 文件名称:  omap_fpga.c
* 功能描述:  fpga 读写操作
*						
* 使用说明: 
* 文件作者:	H4
* 编写日期: （2012/10/16）
* 修改历史:
* 修改日期    修改人       修改内容
*-------------------------------------------------------------------------------

*******************************************************************************/
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include "../../driver/omap_fpga.h"
#include "../../driver/dru_reg.h"
#include "common.h"

/*******************************************************************************
* 函数名称: dru_fpga_write
* 功    能: fpga 写操作
* 参    数:
* 参数名称         类型                描述
* addr      unsigned int      fpga偏移地址
*	data			unsigned short    要写入的数据
* 返回值:
*           unsigned short    所写入的数据
* 说   明:
* 日   期     版本    作者   修改人      DEBUG
* -----------------------------------------------------------------
* 2012/10/17  V1.0     H4     无       ：
*******************************************************************************/
unsigned short dru_fpga_write(unsigned int addr,unsigned short data)
{
		*DRU_REGADDR(fpga_base_addr,addr)=data;
		return data;
}
int dru_fpga_write_change(int argc, char * argv[])
{
	unsigned int para1;
	unsigned short para2;

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
	dru_fpga_write((unsigned int)para1, (unsigned short)para2);
	sprintf(msg_tmp.mtext, "dru_fpga_write reg:0x%x=0x%x.\r\n", para1, para2);
	msg_send(TASK2_MODULE, (char *)&msg_tmp, strlen(msg_tmp.mtext));
	return 0;
}
/*******************************************************************************
* 函数名称: dru_fpga_read
* 功    能: 读出fpga 寄存器的值
* 参    数:
* 参数名称         类型                描述
* addr      unsigned int      fpga中的偏移地址
* data			unsigned short *  读出数据放置地址
* 返回值:
* 					unsigned short  	读出的数据值
* 说   明:
* 日   期     版本    作者   修改人      DEBUG
* -----------------------------------------------------------------
* 2012/10/17  V1.0     H4     无       ：
*******************************************************************************/
unsigned short dru_fpga_read(unsigned int addr,unsigned short *data)
{
		*data=*DRU_REGADDR(fpga_base_addr,addr);
		return *DRU_REGADDR(fpga_base_addr,addr);
}
int dru_fpga_read_change(int argc, char * argv[])
{
	unsigned int para1;
	unsigned short para2;

	msg_tmp.mtype = MSG_FUN_RESULT;	
	if(argc != 2){
		printf("input para cnt is not 2.\r\n");
		sprintf(msg_tmp.mtext, "input para cnt is not 2.\r\n");
		msg_send(TASK2_MODULE, (char *)&msg_tmp, strlen(msg_tmp.mtext));
		return 1;
	}
	para1 = strtol(argv[1], NULL, 16);
	printf("para1=%x. \r\n", para1);
	dru_fpga_read((unsigned int)para1, (unsigned short *)&para2);
	sprintf(msg_tmp.mtext, "dru_fpga_read reg:0x%x=0x%x.\r\n", para1, para2);
	msg_send(TASK2_MODULE, (char *)&msg_tmp, strlen(msg_tmp.mtext));
	return 0;
}
