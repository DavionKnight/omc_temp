/*******************************************************************************
********************************************************************************
* 文件名称:  omap_fpga.h
* 功能描述:  将fpga作为omap的一个外设挂载在物理地址0x66000000.包括寄存器定义fpga读写宏定义
* 使用说明: 	寄存器可以根据地址顺序增加在一下结构。数据总线为16位宽，寄存器定义为unsigned short
*						fpga中寄存器为32位宽。所以每定义一个寄存器必须定义一个16位宽的保留字以填补位宽
* 文件作者:	H4
* 编写日期: （2012/06/15）
* 修改历史:
* 修改日期    修改人       修改内容
*-------------------------------------------------------------------------------

*******************************************************************************/
#ifndef _OMAP_FPGA_H
#define _OMAP_FPGA_H
#define FPGA_PHY_ADDR_BASE 0x66000000
//fpga 相连的芯片配置所用模拟spi
#define FPGA_REGOFFSET_SPI_CLK1 0X01
#define FPGA_REGOFFSET_SPI_CLK2 0X02
#define FPGA_REGOFFSET_SPI_LE1 0X03
#define FPGA_REGOFFSET_SPI_LE2 0X04
#define FPGA_REGOFFSET_SPI_MOSI 0X05
#define FPGA_REGOFFSET_SPI_MISO 0X06

//----------------- DDC DUC registers ------------------

#define DDC_f8_DDSwe_ADDR                        0x0030
#define DDC_m8_DDSwe_ADDR                        0x0031
#define DDC_DDSdata_ADDR                         0x0032
#define DDC_DDSchansel_ADDR                      0x0033
#define DDC_DDSdata_f8_out_ADDR                  0x0034
#define DDC_DDSdata_m8_out_ADDR                  0x0035

#define DUC_f8_DDSwe_ADDR                        0x0036
#define DUC_m8_DDSwe_ADDR                        0x0037
#define DUC_DDSdata_ADDR                         0x0038
#define DUC_DDSchansel_ADDR                      0x0039
#define DUC_DDSdata_f8_out_ADDR                  0x003A
#define DUC_DDSdata_m8_out_ADDR                  0x003B
//------------------------
#define DUC_RST  																 0x003D 
#define MASK_CARRIER															0x50
#define ADC_SOURCE_FREQENCY 										 0x003C
#define ADC_SOURCE_SEL 													 0x003E
#define LOOP_BACK_SEL 													 0x003F
#define DAC_SYNC  															 0x0044

#define LTE1_GAIN 															 0x0040
#define LTE2_GAIN  															 0x0041
#define TD_GAIN    															 0x0042
#define GSM_GAIN  															 0x0043


#define LTE1_ATT  															 0x0046
#define LTE2_ATT  															 0x0047
#define WCDMA_ATT 															 0x0048
#define GSM_ATT   															 0x0049
#define AGC_STEP    								 						 0x004e
#define CPU_AGC_EN  														 0x004f


extern unsigned int *fpga_base_addr;


/*typedef struct{
	unsigned short rev0;
	unsigned short rev1;
	volatile unsigned short spi_clk1;
	unsigned short rev2;
	volatile unsigned short spi_clk2;
	unsigned short rev3;
	volatile unsigned short spi_le1;
	unsigned short rev4;
	volatile unsigned short spi_le2;
	unsigned short rev5;
	volatile unsigned short spi_mosi;
	unsigned short rev6;
	volatile unsigned short spi_miso;
	unsigned short rev7;
}fpga_reg;
*/
//#define READ_FPGA(reg,value) (value=fpga_t->reg)
//#define WRITE_FPGA(reg,value) (fpga_t->reg=value)
//extern fpga_reg *fpga_t;
extern int dru_fpga_read_change(int argc, char * argv[]);
extern int dru_fpga_write_change(int argc, char * argv[]);
#endif
