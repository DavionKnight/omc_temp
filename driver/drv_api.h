#ifndef _DRV_API_H_
#define _DRV_API_H_

/*
** 函数功能：读FPGA接口函数
** 输入参数: addr=FPGA地址
** 输出参数：pdata=从FPGA读到的数据
** 返回值：0=成功 其他=失败
** 备注：
*/
extern int drv_read_fpga(unsigned int addr, unsigned short * pdata);

/*
** 函数功能：写FPGA接口函数
** 输入参数: addr=FPGA地址 data=数据
** 输出参数：无
** 返回值：0=成功 其他=失败
** 备注：
*/
extern int drv_write_fpga(unsigned int addr, unsigned short data);

/*
** 函数功能：读EPLD接口函数
** 输入参数: addr=EPLD地址
** 输出参数：pdata=从EPLD读到的数据
** 返回值：0=成功 其他=失败
** 备注：
*/
extern int drv_read_epld(unsigned int addr, unsigned short * pdata);

/*
** 函数功能：写EPLD接口函数
** 输入参数: addr=EPLD地址 data=数据
** 输出参数：无
** 返回值：0=成功 其他=失败
** 备注：
*/
extern int drv_write_epld(unsigned int addr, unsigned short data);

extern int drv_write_ddc(unsigned char addr,unsigned short data);
extern int drv_write_duc(unsigned char addr,unsigned short data);
extern void set_bandwidth_model(int model);
#endif

