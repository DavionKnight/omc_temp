/*******************************************************************************
********************************************************************************
* 文件名称:  omap_epld.h
* 功能描述:  将epld作为omap的一个外设挂载在物理地址0x62000000.包括寄存器定义fpga读写宏定义
* 使用说明: 	寄存器可以根据地址顺序增加在一下结构。数据总线为16位宽，寄存器定义为unsigned short
*						epld中寄存器为32位宽。所以每定义一个寄存器必须定义一个16位宽的保留字以填补位宽
* 文件作者:	H4
* 编写日期: （2012/06/15）
* 修改历史:
* 修改日期    修改人       修改内容
*-------------------------------------------------------------------------------

*******************************************************************************/
#ifndef _OMAP_EPLD_H
#define _OMAP_EPLD_H
#define EPLD_PHY_ADDR_BASE 0x62000000

#define EPLD_REGOFFSET_VERSION 0X00
#define EPLD_REGOFFSET_TEST 0X01
#define EPLD_REGOFFSET_SPI_CLK 0X02
#define EPLD_REGOFFSET_SPI_LE 0X03
#define EPLD_REGOFFSET_SPI_MOSI 0X04
#define EPLD_REGOFFSET_SPI_MISO 0X05
#define EPLD_REGOFFSET_SPI_SELECT 0X06
#define EPLD_REGOFFSET_SPI_ADC_CLK 0X07
#define EPLD_REGOFFSET_SPI_ADC_LE 0X08
#define EPLD_REGOFFSET_SPI_ADC_MOSI 0X09
#define EPLD_REGOFFSET_DOWNLOAD_FPGA_DATA 0X0A
#define EPLD_REGOFFSET_DOWNLOAD_FPGA_CS 0X0B
#define EPLD_REGOFFSET_DOWNLOAD_FPGA_PROG 0X0C
#define EPLD_REGOFFSET_DOWNLOAD_FPGA_INIT 0X0D
#define EPLD_REGOFFSET_DOWNLOAD_FPGA_DONE 0X0E
#define EPLD_REGOFFSET_DOWNLOAD_FPGA_CLK 0X0F
unsigned short dru_epld_write(unsigned int addr,unsigned short data);
unsigned short dru_epld_read(unsigned int addr,unsigned short *data);
int dru_epld_write_change(int argc, char * argv[]);
int dru_epld_read_change(int argc, char * argv[]);
extern unsigned int *epld_base_addr;
/*typedef struct {
	
	volatile unsigned short version;//0
	unsigned short rev0;
	volatile unsigned short test_rw;//04
	unsigned short rev1;
	volatile unsigned short spi_clk;//08
	unsigned short rev2;
	volatile unsigned short spi_le;//0c
	unsigned short rev3;
	volatile unsigned short spi_mosi;//10
	unsigned short rev4;
	volatile unsigned short spi_miso;//14
	unsigned short rev5;
	volatile unsigned short spi_select;//18
	unsigned short rev6;
	volatile unsigned short adc_spi_clk;// 1c
	unsigned short rev7;
	volatile unsigned short adc_spi_le;// 20
	unsigned short rev8;
	volatile unsigned short adc_spi_mosi;//24
	unsigned short rev9;
	volatile unsigned short fpga_data;
	unsigned short rev16;
	volatile unsigned short fpga_cs;
	unsigned short rev11;
	volatile unsigned short fpga_prog;
	unsigned short rev12;
	volatile unsigned short fpga_init;
	unsigned short rev13;
	volatile unsigned short fpga_done;
	unsigned short rev14;
	volatile unsigned short fpga_clk;
	unsigned short rev15;
	volatile unsigned short test_rw1;//28
	unsigned short rev10;
}epld_reg;
*/
//#define READ_EPLD(reg,value) ((value)=(epld_t->reg))
//#define WRITE_EPLD(reg,value) ((epld_t->reg)=(value))

//extern epld_reg *epld_t;

#endif
