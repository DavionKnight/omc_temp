/*******************************************************************************
********************************************************************************
* 文件名称:  dru_spi.h
* 功能描述:  spi头文件。对epld和fpga各个寄存器和spi管脚做映射
* 使用说明: 	所有有关spi的宏定义都在这个文件中。无论是gpio模拟还是寄存器模拟
*						也不管是给哪个器件做配置
* 文件作者:	H4
* 编写日期: （2012/06/18）
* 修改历史:
* 修改日期    修改人       修改内容
*-------------------------------------------------------------------------------

*******************************************************************************/
#ifndef _DRU_SPI_H
#define _DRU_SPI_H

#define SPI_TIME_DELAY 1
#define SPI_H 1
#define SPI_L 0

#define EMFI_CS_CONFIG 0X0422221D





void omap_pinmux_init(void);
void emif_epld_spi_channel(unsigned short channel);
int emif_epld_spi_write(unsigned int data,unsigned char length);
int emif_epld_adc_spi_write(unsigned int data,unsigned char length);
int emif_fpga_spi_write(unsigned int data,unsigned char length,unsigned char choice);
int emif_fpga_spi_read(unsigned int *data,unsigned char length,unsigned char choice);
void omap_emif_init(void);
void emif_fpga_ddc_carrier_f8_write(unsigned char addr,unsigned short data);
void emif_fpga_ddc_carrier_m8_write(unsigned char addr,unsigned short data);
void emif_fpga_duc_carrier_f8_write(unsigned char addr,unsigned short data);
void emif_fpga_duc_carrier_m8_write(unsigned char addr,unsigned short data);
unsigned short emif_fpga_carrier_read(unsigned char addr);
#endif
