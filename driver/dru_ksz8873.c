/*******************************************************************************
********************************************************************************
* 文件名称:  dru_ksz8873.c
* 功能描述:  对ksz8873的初始化配置和寄存器读写在此文件中
*						
* 使用说明: 
* 文件作者:	H4
* 编写日期: （2012/07/04）
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
#include "omap_gpio.h"
#include "dru_iic.h"
#include "dru_ksz8873.h"
#include "../common/status.h"
/*******************************************************************************
* 函数名称: dru_ksz8873_init
* 功    能: 对ksz8873的配置
* 参    数:
* 参数名称         类型                描述
*
* 返回值:
* 无
* 说   明:
* 日   期     版本    作者   修改人      DEBUG
* -----------------------------------------------------------------
* 2012/07/04 V1.0     H4     无       ：
*******************************************************************************/
void dru_ksz8873_init(void)
{
	unsigned char data=0;
	unsigned char addr;
	int ret;
	addr=0x00;
	ret=dru_8873_read(addr,&data);
	
	printf("8873 addr=%x data=%x ret=%d\n",addr,data,ret);
	addr=0x03;
	data=0;
	if(dru_8873_read(addr,&data))
	{
		printf("8873 read addr=%x err\n",addr);
		return;
	}
	data&=0xcf;
	if(dru_8873_write(addr,data))
	{
		printf("8873 write addr=%x err\n",addr);
		return;
	}
	addr=0x0e;
	data=0;
	if(dru_8873_read(addr,&data))
	{
		printf("8873 read addr=%x err\n",addr);
		return;
	}
	data&=0xbf;
	if(dru_8873_write(addr,data))
	{
		printf("8873 write addr=%x err\n",addr);
		return;
	}
	if(get_device_type()==DEVICE_TYPE_MAIN){
		addr=0x11;
		data=0x05;
		if(dru_8873_write(addr,data))
		{
			printf("8873 write addr=%x err\n",addr);
			return;
		}
		addr=0x21;
		data=0x06;
		if(dru_8873_write(addr,data))
		{
			printf("8873 write addr=%x err\n",addr);
			return;
		}
		addr=0x31;
		data=0x07;
		if(dru_8873_write(addr,data))
		{
			printf("8873 write addr=%x err\n",addr);
			return;
		}
	}else{//for expend and rau have no Vlan;
		addr=0x11;
		data=0x07;
		if(dru_8873_write(addr,data))
		{
			printf("8873 write addr=%x err\n",addr);
			return;
		}
		addr=0x21;
		data=0x07;
		if(dru_8873_write(addr,data))
		{
			printf("8873 write addr=%x err\n",addr);
			return;
		}
		addr=0x31;
		data=0x07;
		if(dru_8873_write(addr,data))
		{
			printf("8873 write addr=%x err\n",addr);
			return;
		}
	}
	return;
}
