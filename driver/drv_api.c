#include "omap_epld.h"
#include "common.h"

extern unsigned short dru_fpga_write(unsigned int addr,unsigned short data);
extern unsigned short dru_fpga_read(unsigned int addr,unsigned short *data);
/*
** 函数功能：读FPGA接口函数
** 输入参数: addr=FPGA地址
** 输出参数：pdata=从FPGA读到的数据
** 返回值：0=成功 其他=失败
** 备注：
*/
int drv_read_fpga(unsigned int addr, unsigned short * pdata)
{
	unsigned short tmp;
	int ret;

	lock_sem(SEM_DRV);
	tmp = dru_fpga_read(addr, pdata);
	if(tmp == *pdata){
		ret = 0;
	}else{
		ret = -1;
	}
	unlock_sem(SEM_DRV);

	return ret;
}
/*
** 函数功能：写FPGA接口函数
** 输入参数: addr=FPGA地址 data=数据
** 输出参数：无
** 返回值：0=成功 其他=失败
** 备注：
*/
int drv_write_fpga(unsigned int addr, unsigned short data)
{
	unsigned short tmp;
	int ret;

	lock_sem(SEM_DRV);
	tmp = dru_fpga_write(addr, data);
	if(tmp == data){
		ret = 0;
	}else{
		ret = -1;
	}
	unlock_sem(SEM_DRV);

	return ret;
}
/*
** 函数功能：读EPLD接口函数
** 输入参数: addr=EPLD地址
** 输出参数：pdata=从EPLD读到的数据
** 返回值：0=成功 其他=失败
** 备注：
*/
int drv_read_epld(unsigned int addr, unsigned short * pdata)
{
	unsigned short tmp;
	int ret;

	lock_sem(SEM_DRV);
	tmp = dru_epld_read(addr, pdata);
	if(tmp == *pdata){
		ret = 0;
	}else{
		ret = -1;
	}
	unlock_sem(SEM_DRV);

	return ret;
}
/*
** 函数功能：写EPLD接口函数
** 输入参数: addr=EPLD地址 data=数据
** 输出参数：无
** 返回值：0=成功 其他=失败
** 备注：
*/
int drv_write_epld(unsigned int addr, unsigned short data)
{
	unsigned short tmp;
	int ret;

	lock_sem(SEM_DRV);
	tmp = dru_epld_write(addr, data);
	if(tmp == data){
		ret = 0;
	}else{
		ret = -1;
	}
	unlock_sem(SEM_DRV);

	return ret;
}
/*
** 函数功能：写DDC配置接口函数
** 输入参数: addr=信道号 data=数据
** 输出参数：无
** 返回值：0=成功 其他=失败
** 备注：
*/
int drv_write_ddc(unsigned char addr,unsigned short data)
{
	lock_sem(SEM_DRV);
	if (addr < 8)
	{
		emif_fpga_ddc_carrier_f8_write(addr,data);
	}
	else
	{
		emif_fpga_ddc_carrier_m8_write(addr-8,data);
	}
	unlock_sem(SEM_DRV);
	return 0;
}
/*
** 函数功能：写DUC配置接口函数
** 输入参数: addr=信道号 data=数据
** 输出参数：无
** 返回值：0=成功 其他=失败
** 备注：
*/
int drv_write_duc(unsigned char addr,unsigned short data)
{
	lock_sem(SEM_DRV);
	if (addr < 8)
	{
		emif_fpga_duc_carrier_f8_write(addr,data);
	}
	else
	{
		emif_fpga_duc_carrier_m8_write(addr-8,data);
	}
	unlock_sem(SEM_DRV);
	return 0;
}