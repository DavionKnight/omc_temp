/*******************************************************************************
********************************************************************************
* 文件名称:  dru_fpga_ram.c
* 功能描述: fpga中ram读写函数
* 使用说明: 所有fpga 中ram的操作 在此文件中定义
* 文件作者:	H4
* 编写日期: （2012/08/03）
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
#include "../../driver/omap_fpga.h"
#include "../../driver/dru_reg.h"
#include "../../driver/dru_fpga_ram.h"
/*******************************************************************************
* 函数名称: dru_fpga_DDC_ram_write
* 功    能:像fpga的ddcram中写入数据
* 参    数:
* 参数名称         类型                描述
* 无
* 返回值:
* 无
* 说   明:
* 日   期     版本    作者   修改人      DEBUG
* -----------------------------------------------------------------
* 2012/08/03  V1.0     H4     无       ：
*******************************************************************************/
int dru_fpga_ddc_ram_write(unsigned short addr,unsigned short data,unsigned short channel)
{
	if(addr>7)
	{
		return -1;
	}
	if(channel>1)
	{
		return -1;
	}
	if(channel==RAM_F8_SELECT)
	{
		*DRU_REGADDR(fpga_base_addr,DDC_DDSdata_f8_out_ADDR)= 0;
	}else{
		*DRU_REGADDR(fpga_base_addr,DDC_DDSdata_m8_out_ADDR)=0;
	}
	*DRU_REGADDR(fpga_base_addr,DDC_DDSchansel_ADDR)=addr;
	*DRU_REGADDR(fpga_base_addr,DDC_DDSdata_ADDR)=data;
	if(channel==RAM_F8_SELECT){
		*DRU_REGADDR(fpga_base_addr,DDC_DDSdata_f8_out_ADDR)=0;
		*DRU_REGADDR(fpga_base_addr,DDC_DDSdata_f8_out_ADDR)=1;
		*DRU_REGADDR(fpga_base_addr,DDC_DDSdata_f8_out_ADDR)=0;
	}else{
		*DRU_REGADDR(fpga_base_addr,DDC_DDSdata_m8_out_ADDR)=0;
		*DRU_REGADDR(fpga_base_addr,DDC_DDSdata_m8_out_ADDR)=1;
		*DRU_REGADDR(fpga_base_addr,DDC_DDSdata_m8_out_ADDR)=0;
	}
	return 0;
}
/*******************************************************************************
* 函数名称: dru_fpga_DDC_ram_read
* 功    能:像fpga的ddcram中读出数据
* 参    数:
* 参数名称         类型                描述
* 无
* 返回值:
* 无
* 说   明:
* 日   期     版本    作者   修改人      DEBUG
* -----------------------------------------------------------------
* 2012/08/03  V1.0     H4     无       ：
*******************************************************************************/
int dru_fpga_ddc_ram_read(unsigned short addr,unsigned short *data,unsigned short channel)
{
	if(addr>7)
		return -1;
	if(channel>1)
		return -1;
	*DRU_REGADDR(fpga_base_addr,DDC_DDSchansel_ADDR)=addr;
	//*DRU_REGADDR(fpga_base_addr, DDC_DDSdata_ADDR)=data;
	if(channel==RAM_F8_SELECT){
		*data=*DRU_REGADDR(fpga_base_addr,DDC_DDSdata_f8_out_ADDR);
	}else{
		*data=*DRU_REGADDR(fpga_base_addr,DDC_DDSdata_m8_out_ADDR);
	}
	return 0;
}
/*******************************************************************************
* 函数名称: dru_fpga_DDC_ram_read
* 功    能:像fpga的ddcram中读出数据
* 参    数:
* 参数名称         类型                描述
* 无
* 返回值:
* 无
* 说   明:
* 日   期     版本    作者   修改人      DEBUG
* -----------------------------------------------------------------
* 2012/08/03  V1.0     H4     无       ：
*******************************************************************************/
int dru_fpga_ddc_ram_init(void)
{
	unsigned short data;
	dru_fpga_ddc_ram_write(1,6,0);
	dru_fpga_ddc_ram_write(3,8,1);
		data=0;
	dru_fpga_ddc_ram_read(1,&data,0);
	 printf("1addr %d\n",data); 
	 	data=0;
	 dru_fpga_ddc_ram_read(3,&data,1);
	 printf("3addr %d\n",data); 
}
