/*******************************************************************************
********************************************************************************
* 文件名称:  dru_spi.c
* 功能描述:  模拟spi部分的实现。包括epld寄存器扩展spi 。
*						gpio模拟spi等等。支持上升沿读写 下降沿读写模式，以及支持各个芯片时序
* 使用说明: 
* 文件作者:	H4
* 编写日期: （2012/06/15）
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
#include "omap_pinmux.h"
#include "omap_epld.h"
#include "omap_fpga.h"
#include "omap_emif.h"
#include "dru_spi.h"
#include "dru_reg.h"
#include "dru_ads58c48.h"
#include "common.h"
#include "dru_psw.h"

//测试用这个函数应该出现在初始化文件中，或是有系统设置好从而删除这个函数
void omap_pinmux_init(void)
{
	MUX(EMIFA_CLK_PINMUXREG,EMIFA_CLK_PINMUX,PINMUX_CONFIG_EMIFA);
	MUX( KSZ8873_MDC_PINMUXREG,KSZ8873_MDC_PINMUX,PINMUX_CONFIG_MD_GPIO);
	MUX(KSZ8873_MDIO_PINMUXREG,KSZ8873_MDIO_PINMUX,PINMUX_CONFIG_MD_GPIO);
	MUX( TMP_SCL_REG,TMP_SCL_PIN,PINMUX_CONFIG_GPIO);
	MUX(TMP_SDA_REG,TMP_SDA_PIN,PINMUX_CONFIG_GPIO);
	MUX(1,1,8);
	MUX(1,7,8);
	MUX(1,3,8);
	MUX(1,4,8);
	MUX(1,5,8);
}
void omap_emif_init(void)
{
	WRITE_EMIF(ce3cfg,EMFI_CS_CONFIG);
	WRITE_EMIF(ce5cfg,EMFI_CS_CONFIG);
}
/*******************************************************************************
* 函数名称: spi_delay
* 功    能: 用于spi工作延时
* 参    数:
* 参数名称         类型                描述
* delayTime      unsigned char      延时时长
* 返回值:
* 无
* 说   明:
* 日   期     版本    作者   修改人      DEBUG
* -----------------------------------------------------------------
* 2012/06/15  V1.0     H4     无       ：
*******************************************************************************/
void spi_delay(unsigned char delayTime)                                                   
{                                                                                        
    unsigned int index = 0;                                                                    
    for (index = 0; index < delayTime * 10; index++)   
    {                                                                                    
        asm(" nop");                                                                     
    }                                                                                    
    return;                                                                              
}          
/*******************************************************************************
* 函数名称: emif_spi_channel
* 功    能: 并口寄存器spi通道选择。
* 参    数:
* 参数名称         类型                描述
* channel      unsigned short      用于控制spi通道
* 返回值:
* 无
* 说   明:
* 日   期     版本    作者   修改人      DEBUG
* -----------------------------------------------------------------
* 2012/06/15  V1.0     H4     无       ：
*******************************************************************************/
void emif_epld_spi_channel(unsigned short channel)
{

	//  WRITE_EPLD(EMIF_SPI_SELECT_REG,channel);  
	*DRU_REGADDR(epld_base_addr, EPLD_REGOFFSET_SPI_SELECT)=channel;
	//  epld_t->spi_select=0x0005;
	//  printf("the epld_t is %x\n",READ_EPLD(EMIF_SPI_SELECT_REG,channel)); 
}
/*******************************************************************************
* 函数名称: emif_epld_spi_clk
* 功    能: 并口寄存器spi通道clk寄存器控制。
* 参    数:
* 参数名称         类型                描述
* level      unsigned char         为spi的clk脚输出电平
* 返回值:
* 无
* 说   明:
* 日   期     版本    作者   修改人      DEBUG
* -----------------------------------------------------------------
* 2012/06/15  V1.0     H4     无       ：
*******************************************************************************/
void emif_epld_spi_clk(unsigned char level)                                              
{                                                                             
    if (level == 1)                                                         
    {                                                                         
    	//WRITE_EPLD(EMIF_SPI_CLK_REG,SPI_H);
    	*DRU_REGADDR(epld_base_addr, EPLD_REGOFFSET_SPI_CLK)=SPI_H;
    }                                                                         
    else                                                                      
    {                                                                         
		//WRITE_EPLD(EMIF_SPI_CLK_REG,SPI_L);
		*DRU_REGADDR(epld_base_addr, EPLD_REGOFFSET_SPI_CLK)=SPI_L;
    }                                                                                                             
    return;                                                                   
}
/*******************************************************************************
* 函数名称: emif_epld_spi_le
* 功    能: 并口寄存器spi通道le寄存器控制。
* 参    数:
* 参数名称         类型                描述
* level      unsigned char         为spi的le脚输出电平
* 返回值:
* 无
* 说   明:
* 日   期     版本    作者   修改人      DEBUG
* -----------------------------------------------------------------
* 2012/06/15  V1.0     H4     无       ：
*******************************************************************************/
void emif_epld_spi_le(unsigned char level)                                              
{                                                                             
    if (level == 1)                                                         
    {                                                                         
    	//WRITE_EPLD(EMIF_SPI_LE_REG,SPI_H);
    	*DRU_REGADDR(epld_base_addr, EPLD_REGOFFSET_SPI_LE)=SPI_H;
    }                                                                         
    else                                                                      
    {                                                                         
			//WRITE_EPLD(EMIF_SPI_LE_REG,SPI_L);
	*DRU_REGADDR(epld_base_addr, EPLD_REGOFFSET_SPI_LE)=SPI_L;
    }                                                                                                             
    return;                                                                   
}
/*******************************************************************************
* 函数名称: emif_epld_spi_out
* 功    能: 并口寄存器spi通道MOSI寄存器控制。
* 参    数:
* 参数名称         类型                描述
* level      unsigned char         为spi的MOSI脚
* 返回值:
* 无
* 说   明:
* 日   期     版本    作者   修改人      DEBUG
* -----------------------------------------------------------------
* 2012/06/15  V1.0     H4     无       ：
*******************************************************************************/
void emif_epld_spi_out(unsigned char level)                                              
{                                                                             
    if (level == 1)                                                         
    {                                                                         
    	//WRITE_EPLD(EMIF_SPI_MOSI_REG,SPI_H);
    	*DRU_REGADDR(epld_base_addr, EPLD_REGOFFSET_SPI_MOSI)=SPI_H;
    }                                                                         
    else                                                                      
    {                                                                         
	//		WRITE_EPLD(EMIF_SPI_MOSI_REG,SPI_L);
	*DRU_REGADDR(epld_base_addr, EPLD_REGOFFSET_SPI_MOSI)=SPI_L;
    }                                                                                                             
    return;                                                                   
}
/*******************************************************************************
* 函数名称: emif_epld_spi_in
* 功    能: 并口寄存器spi通道MISO寄存器控制。
* 参    数:
* 参数名称         类型                描述
* level      unsigned short         为spi的MISO脚输入电平
* 返回值:
* 无
* 说   明:
* 日   期     版本    作者   修改人      DEBUG
* -----------------------------------------------------------------
* 2012/06/15  V1.0     H4     无       ：
*******************************************************************************/
void emif_epld_spi_in(unsigned short *level)                                              
{                                                                             
    //READ_EPLD(EMIF_SPI_MOSI_REG,*level);      
	*level=*DRU_REGADDR(epld_base_addr, EPLD_REGOFFSET_SPI_MISO);
    return;                                                                   
}
/*******************************************************************************
* 函数名称: emif_epld_adc_spi_clk
* 功    能: 并口寄存器adc spi通道clk寄存器控制。
* 参    数:
* 参数名称         类型                描述
* level      unsigned char         为adc spi的clk脚输出电平
* 返回值:
* 无
* 说   明:
* 日   期     版本    作者   修改人      DEBUG
* -----------------------------------------------------------------
* 2012/06/15  V1.0     H4     无       ：
*******************************************************************************/
void emif_epld_adc_spi_clk(unsigned char level)                                              
{                                                                             
    if (level == 1)                                                         
    {                                                                         
    	//WRITE_EPLD(EMIF_ADC_SPI_CLK_REG,SPI_H);
    	*DRU_REGADDR(epld_base_addr, EPLD_REGOFFSET_SPI_ADC_CLK)=SPI_H;
    }                                                                         
    else                                                                      
    {                                                                         
	//		WRITE_EPLD(EMIF_ADC_SPI_CLK_REG,SPI_L);
	*DRU_REGADDR(epld_base_addr, EPLD_REGOFFSET_SPI_ADC_CLK)=SPI_L;
    }                                                                                                             
    return;                                                                   
}
/*******************************************************************************
* 函数名称: emif_epld_adc_spi_le
* 功    能: 并口寄存器adc spi通道le寄存器控制。
* 参    数:
* 参数名称         类型                描述
* level      unsigned char         为adc spi的le脚输出电平
* 返回值:
* 无
* 说   明:
* 日   期     版本    作者   修改人      DEBUG
* -----------------------------------------------------------------
* 2012/06/15  V1.0     H4     无       ：
*******************************************************************************/
void emif_epld_adc_spi_le(unsigned char level)                                              
{                                                                             
    if (level == 1)                                                         
    {                                                                         
    	//WRITE_EPLD(EMIF_ADC_SPI_LE_REG,SPI_H);
    	*DRU_REGADDR(epld_base_addr, EPLD_REGOFFSET_SPI_ADC_LE)=SPI_H;
    }                                                                         
    else                                                                      
    {                                                                         
	//		WRITE_EPLD(EMIF_ADC_SPI_LE_REG,SPI_L);
	*DRU_REGADDR(epld_base_addr, EPLD_REGOFFSET_SPI_ADC_LE)=SPI_L;
    }                                                                                                             
    return;                                                                   
}
/*******************************************************************************
* 函数名称: emif_epld_adc_spi_out
* 功    能: 并口寄存器adc spi通道MOSI寄存器控制。
* 参    数:
* 参数名称         类型                描述
* level      unsigned char         为adc spi的MOSI脚输出电平
* 返回值:
* 无
* 说   明:
* 日   期     版本    作者   修改人      DEBUG
* -----------------------------------------------------------------
* 2012/06/15  V1.0     H4     无       ：
*******************************************************************************/
void emif_epld_adc_spi_out(unsigned char level)                                              
{                                                                             
    if (level == 1)                                                         
    {                                                                         
    //	WRITE_EPLD(EMIF_ADC_SPI_MOSI_REG,SPI_H);
    *DRU_REGADDR(epld_base_addr, EPLD_REGOFFSET_SPI_ADC_MOSI)=SPI_H;
    }                                                                         
    else                                                                      
    {                                                                         
	//			WRITE_EPLD(EMIF_ADC_SPI_MOSI_REG,SPI_L);
	*DRU_REGADDR(epld_base_addr, EPLD_REGOFFSET_SPI_ADC_MOSI)=SPI_L;
    }                                                                                                             
    return;                                                                   
}
/*******************************************************************************
* 函数名称: emif_epld_spi_in
* 功    能: 并口寄存器spi通道MISO寄存器控制。
* 参    数:
* 参数名称         类型                描述
* level      unsigned short         为spi的MISO脚输入电平
* 返回值:
* 无
* 说   明:
* 日   期     版本    作者   修改人      DEBUG
* -----------------------------------------------------------------
* 2012/06/15  V1.0     H4     无       ：
*******************************************************************************/
void emif_epld_adc_spi_in(unsigned short *level)                                              
{                                                                             
  //  READ_EPLD(EMIF_SPI_MOSI_REG,*level);                                                                    
    return;                                                                   
}
/*******************************************************************************
* 函数名称: emif_fpga_spi_clk
* 功    能: 并口寄存器fpga spi通道clk寄存器控制。
* 参    数:
* 参数名称         类型                描述
* level      unsigned char         为fpga spi的clk脚输出电平
* clkChoice  unsigned char				clk寄存器选择
* 返回值:
* 无
* 说   明:
* 日   期     版本    作者   修改人      DEBUG
* -----------------------------------------------------------------
* 2012/06/15  V1.0     H4     无       ：
*******************************************************************************/
void emif_fpga_spi_clk(unsigned char level,unsigned char clkChoice)                                              
{                                                                             
  if(clkChoice==1){
  	  if (level == 1)                                                         
  	  { 
  	  	//WRITE_FPGA(FPGA_SPI_CLK1_REG,SPI_H);           
  	  	*DRU_REGADDR(fpga_base_addr, FPGA_REGOFFSET_SPI_CLK1)=SPI_H;
  	  }                                                                         
  	  else                                                                      
  	  {    
  	  	//WRITE_FPGA(FPGA_SPI_CLK1_REG,SPI_L);       
		*DRU_REGADDR(fpga_base_addr, FPGA_REGOFFSET_SPI_CLK1)=SPI_L;
  	  }
  }else {                                                       
  	  if (level == 1)                                                         
  	  { 
  	  	//WRITE_FPGA(FPGA_SPI_CLK2_REG,SPI_H);       
  	  	*DRU_REGADDR(fpga_base_addr, FPGA_REGOFFSET_SPI_CLK2)=SPI_H;
  	  }                                                                         
  	  else                                                                      
  	  {    
  	  	//WRITE_FPGA(FPGA_SPI_CLK2_REG,SPI_L);     
  	  	*DRU_REGADDR(fpga_base_addr, FPGA_REGOFFSET_SPI_CLK2)=SPI_L;
  	  }
  }                     
    return;                                                                   
}                                                                             
/*******************************************************************************
* 函数名称: emif_fpga_spi_le
* 功    能: 并口寄存器fpga spi通道le寄存器控制。
* 参    数:
* 参数名称         类型                描述
* level      unsigned char         为fpga spi的le脚输出电平
* leChoice   unsigned char				le寄存器选择
* 返回值:
* 无
* 说   明:
* 日   期     版本    作者   修改人      DEBUG
* -----------------------------------------------------------------
* 2012/06/15  V1.0     H4     无       ：
*******************************************************************************/
void emif_fpga_spi_le(unsigned char level,unsigned char leChoice)                                              
{                                                                             
  if(leChoice==1){
  	  if (level == 1)                                                         
  	  { 
  	  	//WRITE_FPGA(FPGA_SPI_LE1_REG,SPI_H);       
  	  	*DRU_REGADDR(fpga_base_addr, FPGA_REGOFFSET_SPI_LE1)=SPI_H;
  	  }                                                                         
  	  else                                                                      
  	  {    
  	  	//WRITE_FPGA(FPGA_SPI_LE1_REG,SPI_L);      
  	  	*DRU_REGADDR(fpga_base_addr, FPGA_REGOFFSET_SPI_LE1)=SPI_L;
  	  }
  }else {                                                       
  	  if (level == 1)                                                         
  	  { 
  	  	//WRITE_FPGA(FPGA_SPI_LE2_REG,SPI_H);       
  	  	*DRU_REGADDR(fpga_base_addr, FPGA_REGOFFSET_SPI_LE2)=SPI_H;
  	  }                                                                         
  	  else                                                                      
  	  {    
  	  	//WRITE_FPGA(FPGA_SPI_LE2_REG,SPI_L);         
  	  	*DRU_REGADDR(fpga_base_addr, FPGA_REGOFFSET_SPI_LE2)=SPI_L;
  	  }
  }                     
    return;                                                                   
}           
/*******************************************************************************
* 函数名称: emif_fpga_spi_out
* 功    能: 并口寄存器fpga spi通道MOSI寄存器控制。
* 参    数:
* 参数名称         类型                描述
* level      unsigned char         为fpga spi的MOSI脚输出电平
* 返回值:
* 无
* 说   明:
* 日   期     版本    作者   修改人      DEBUG
* -----------------------------------------------------------------
* 2012/06/15  V1.0     H4     无       ：
*******************************************************************************/
void emif_fpga_spi_out(unsigned char level)                                              
{                                                                             
 	 if (level == 1)                                                         
 	 { 
 	 	//WRITE_FPGA(FPGA_SPI_MOSI_REG,SPI_H);
 	 	*DRU_REGADDR(fpga_base_addr, FPGA_REGOFFSET_SPI_MOSI)=SPI_H;
 	 }                                                                         
 	 else                                                                      
 	 {    
 	 	//WRITE_FPGA(FPGA_SPI_MOSI_REG,SPI_L);
 	 	*DRU_REGADDR(fpga_base_addr, FPGA_REGOFFSET_SPI_MOSI)=SPI_L;
 	 }     
   return;                                                                   
}           
/*******************************************************************************
* 函数名称: emif_epld_spi_in
* 功    能: 并口寄存器fpga spi通道MISO寄存器控制。
* 参    数:
* 参数名称         类型                描述
* level      unsigned short         为fpga spi的MISO脚输入电平
* 返回值:
* 无
* 说   明:
* 日   期     版本    作者   修改人      DEBUG
* -----------------------------------------------------------------
* 2012/06/15  V1.0     H4     无       ：
*******************************************************************************/
void emif_fpga_spi_in(unsigned short *level)                                              
{                                                                             
    //READ_FPGA(FPGA_SPI_MISO_REG,*level);
    *level=*DRU_REGADDR(fpga_base_addr, FPGA_REGOFFSET_SPI_MISO);
    return;                                                                   
}
/*******************************************************************************
* 函数名称: emif_epld_spi_write
* 功    能: 并口寄存器epld spi通道写数据。先写高位 上升沿写数
* 参    数:
* 参数名称         类型                描述
* data      unsigned int        所要写入的数据
* length    unsigned char				数据位宽
* 返回值:
* 无
* 说   明:
* 日   期     版本    作者   修改人      DEBUG
* -----------------------------------------------------------------
* 2012/06/15  V1.0     H4     无       ：
*******************************************************************************/
int emif_epld_spi_write(unsigned int data,unsigned char length)
{
	unsigned char i;
	if(length>32){
		return -1;	
	}
	emif_epld_spi_le(SPI_H);
	spi_delay(SPI_TIME_DELAY);
	emif_epld_spi_clk(SPI_L);
	emif_epld_spi_le(SPI_L);
	for(i=0;i<length;i++){
		if((data&(1<<(length-1-i)))!=0)
		emif_epld_spi_out(SPI_H);
		else
		emif_epld_spi_out(SPI_L);
		emif_epld_spi_clk(SPI_L);
		spi_delay(SPI_TIME_DELAY);
		emif_epld_spi_clk(SPI_H);
		spi_delay(SPI_TIME_DELAY);
		emif_epld_spi_clk(SPI_L);
	}
	emif_epld_spi_le(SPI_H);
	spi_delay(SPI_TIME_DELAY);
	return 0;
}
/*******************************************************************************
* 函数名称: emif_epld_adc_spi_write
* 功    能: 并口寄存器epld adc spi通道写数据。先写高位 下降沿写数
* 参    数:
* 参数名称         类型                描述
* data      unsigned int        所要写入的数据
* length    unsigned char				数据位宽
* 返回值:
* 无
* 说   明:
* 日   期     版本    作者   修改人      DEBUG
* -----------------------------------------------------------------
* 2012/06/15  V1.0     H4     无       ：
*******************************************************************************/
int emif_epld_adc_spi_write(unsigned int data,unsigned char length)
{
	unsigned char i;
	if(length>32){
		return -1;	
	}
	emif_epld_adc_spi_le(SPI_H);
	spi_delay(SPI_TIME_DELAY);
	emif_epld_adc_spi_clk(SPI_H);
	emif_epld_adc_spi_le(SPI_L);
	for(i=0;i<length;i++){
		if((data&(1<<(length-1-i)))!=0)
		emif_epld_adc_spi_out(SPI_H);
		else
		emif_epld_adc_spi_out(SPI_L);
		emif_epld_adc_spi_clk(SPI_H);
		spi_delay(SPI_TIME_DELAY);
		emif_epld_adc_spi_clk(SPI_L);
		spi_delay(SPI_TIME_DELAY);
		emif_epld_adc_spi_clk(SPI_H);
	}
	emif_epld_adc_spi_le(SPI_H);
	spi_delay(SPI_TIME_DELAY);
	return 0;
}
int emif_epld_adc_spi_write_change(int argc, char * argv[])
{
	unsigned int para1;

	msg_tmp.mtype = MSG_FUN_RESULT;	
	if(argc != 2){
		printf("input para cnt is not 2.\r\n");
		sprintf(msg_tmp.mtext, "input para cnt is not 2.\r\n");
		msg_send(TASK2_MODULE, (char *)&msg_tmp, strlen(msg_tmp.mtext));
		return 1;
	}
	para1 = strtol(argv[1], NULL, 16);
	emif_epld_adc_spi_write((unsigned int)para1, ADS58C48_REG_LENGTH);
	sprintf(msg_tmp.mtext, "emif_cpld_adc_spi_write data=0x%x.\r\n", para1);
	msg_send(TASK2_MODULE, (char *)&msg_tmp, strlen(msg_tmp.mtext));
	return 0;
}
/*******************************************************************************
* 函数名称: emif_fpga_spi_write
* 功    能: 并口寄存器fpga spi通道写数据。先写高位 上升沿写数
* 参    数:
* 参数名称         类型                描述
* data      unsigned int        所要写入的数据
* length    unsigned char				数据位宽
* choice    unsigned char				clk及le选择
* 返回值:
* 无
* 说   明:
* 日   期     版本    作者   修改人      DEBUG
* -----------------------------------------------------------------
* 2012/06/15  V1.0     H4     无       ：
*******************************************************************************/
int emif_fpga_spi_write(unsigned int data,unsigned char length,unsigned char choice)
{
	unsigned char i;
	if(length>32){
		return -1;	
	}
	emif_fpga_spi_le(SPI_H,choice);
	spi_delay(SPI_TIME_DELAY);
	emif_fpga_spi_clk(SPI_H,choice);
	spi_delay(SPI_TIME_DELAY);
	emif_fpga_spi_clk(SPI_L,choice);
	spi_delay(SPI_TIME_DELAY);
	emif_fpga_spi_clk(SPI_H,choice);
	spi_delay(SPI_TIME_DELAY);
	emif_fpga_spi_clk(SPI_L,choice);
	spi_delay(SPI_TIME_DELAY);
	emif_fpga_spi_le(SPI_L,choice);
	for(i=0;i<length;i++){
		if((data&(1<<(length-1-i)))!=0)
		emif_fpga_spi_out(SPI_H);
		else
		emif_fpga_spi_out(SPI_L);
		spi_delay(SPI_TIME_DELAY);
		emif_fpga_spi_clk(SPI_L,choice);
		
		spi_delay(SPI_TIME_DELAY);
		emif_fpga_spi_clk(SPI_H,choice);
	}
	spi_delay(SPI_TIME_DELAY);
	emif_fpga_spi_clk(SPI_H,choice);
	spi_delay(SPI_TIME_DELAY);
	emif_fpga_spi_clk(SPI_L,choice);
	spi_delay(SPI_TIME_DELAY);
	emif_fpga_spi_clk(SPI_H,choice);
	spi_delay(SPI_TIME_DELAY);
	emif_fpga_spi_clk(SPI_L,choice);
	spi_delay(SPI_TIME_DELAY);
	return 0;
}
/*******************************************************************************
* 函数名称: emif_fpga_spi_read
* 功    能: 并口寄存器fpga spi通道写数据。先写高位 上升沿写数
* 参    数:
* 参数名称         类型                描述
* data      unsigned int        所要写入的数据
* length    unsigned char				数据位宽
* choice    unsigned char				clk及le选择
* 返回值:
* 无
* 说   明:
* 日   期     版本    作者   修改人      DEBUG
* -----------------------------------------------------------------
* 2012/06/15  V1.0     H4     无       ：
*******************************************************************************/
int emif_fpga_spi_read(unsigned int *data,unsigned char length,unsigned char choice)
{
	unsigned char i;
	unsigned short data_r=0;
	unsigned int tmp=0,data_tmp;
	data_tmp=*data;
	if(length>32){
		return -1;	
	}
	emif_fpga_spi_le(SPI_H,choice);
	spi_delay(SPI_TIME_DELAY);
	emif_fpga_spi_clk(SPI_H,choice);
	spi_delay(SPI_TIME_DELAY);
	emif_fpga_spi_clk(SPI_L,choice);
	spi_delay(SPI_TIME_DELAY);
	emif_fpga_spi_clk(SPI_H,choice);
	spi_delay(SPI_TIME_DELAY);
	emif_fpga_spi_clk(SPI_L,choice);
	spi_delay(SPI_TIME_DELAY);
	emif_fpga_spi_le(SPI_L,choice);
	for(i=0;i<length;i++){
		if(i<8){
			if((data_tmp&(1<<(length-1-i)))!=0)
			emif_fpga_spi_out(SPI_H);
			else
			emif_fpga_spi_out(SPI_L);
			emif_fpga_spi_clk(SPI_L,choice);
			spi_delay(SPI_TIME_DELAY);
			emif_fpga_spi_clk(SPI_H,choice);
			spi_delay(SPI_TIME_DELAY);
			emif_fpga_spi_clk(SPI_L,choice);
			spi_delay(SPI_TIME_DELAY);
		}else{
			emif_fpga_spi_in(&data_r);
			if(data_r)
				tmp|=(1<<(length-1-i));
			emif_fpga_spi_clk(SPI_H,choice);
			spi_delay(SPI_TIME_DELAY);
			emif_fpga_spi_clk(SPI_L,choice);
			spi_delay(SPI_TIME_DELAY);

		}
	}
	*data=0x00ff&tmp;
	spi_delay(SPI_TIME_DELAY);
	emif_fpga_spi_clk(SPI_H,choice);
	spi_delay(SPI_TIME_DELAY);
	emif_fpga_spi_clk(SPI_L,choice);
	spi_delay(SPI_TIME_DELAY);
	emif_fpga_spi_clk(SPI_H,choice);
	spi_delay(SPI_TIME_DELAY);
	emif_fpga_spi_clk(SPI_L,choice);
	spi_delay(SPI_TIME_DELAY);
	return 0;
}

void emif_fpga_ddc_carrier_f8_write(unsigned char addr,unsigned short data)
{
	
*DRU_REGADDR(fpga_base_addr, 0x33)= addr;
*DRU_REGADDR(fpga_base_addr, 0x32) = data;

*DRU_REGADDR(fpga_base_addr, 0x30) = 0;	
*DRU_REGADDR(fpga_base_addr, 0x30) = 1;	
*DRU_REGADDR(fpga_base_addr, 0x30) = 0;	

}
int emif_fpga_ddc_carrier_f8_write_change(int argc, char * argv[])
{
	unsigned int para1;
	unsigned int para2;

	msg_tmp.mtype = MSG_FUN_RESULT;	
	if(argc != 3){
		printf("input para cnt is not 3.\r\n");
		sprintf(msg_tmp.mtext, "input para cnt is not 3.\r\n");
		msg_send(TASK2_MODULE, (char *)&msg_tmp, strlen(msg_tmp.mtext));
		return 1;
	}
	para1 = strtol(argv[1], NULL, 10);
	para2 = strtol(argv[2], NULL, 10);
	emif_fpga_ddc_carrier_f8_write((unsigned char)para1, (unsigned short)para2);
	sprintf(msg_tmp.mtext, "emif_fpga_ddc_carrier_f8_write(%d, %d).\r\n", para1, para2);
	msg_send(TASK2_MODULE, (char *)&msg_tmp, strlen(msg_tmp.mtext));
	return 0;
}
void emif_fpga_ddc_carrier_m8_write(unsigned char addr,unsigned short data)
{
	
*DRU_REGADDR(fpga_base_addr, 0x33) = addr;
*DRU_REGADDR(fpga_base_addr, 0x32) = data;

*DRU_REGADDR(fpga_base_addr, 0x31) = 0;	
*DRU_REGADDR(fpga_base_addr, 0x31) = 1;	
*DRU_REGADDR(fpga_base_addr, 0x31) = 0;	

}
int emif_fpga_ddc_carrier_m8_write_change(int argc, char * argv[])
{
	unsigned int para1;
	unsigned int para2;

	msg_tmp.mtype = MSG_FUN_RESULT;	
	if(argc != 3){
		printf("input para cnt is not 3.\r\n");
		sprintf(msg_tmp.mtext, "input para cnt is not 3.\r\n");
		msg_send(TASK2_MODULE, (char *)&msg_tmp, strlen(msg_tmp.mtext));
		return 1;
	}
	para1 = strtol(argv[1], NULL, 10);
	para2 = strtol(argv[2], NULL, 10);
	emif_fpga_ddc_carrier_m8_write((unsigned char)para1, (unsigned short)para2);
	sprintf(msg_tmp.mtext, "emif_fpga_ddc_carrier_m8_write(%d, %d).\r\n", para1, para2);
	msg_send(TASK2_MODULE, (char *)&msg_tmp, strlen(msg_tmp.mtext));
	return 0;
}
void emif_fpga_duc_carrier_f8_write(unsigned char addr,unsigned short data)
{
	
*DRU_REGADDR(fpga_base_addr, 0x39) = addr;
*DRU_REGADDR(fpga_base_addr, 0x38) = data;

*DRU_REGADDR(fpga_base_addr, 0x36) = 0;	
*DRU_REGADDR(fpga_base_addr, 0x36) = 1;	
*DRU_REGADDR(fpga_base_addr, 0x36) = 0;	

}
int emif_fpga_duc_carrier_f8_write_change(int argc, char * argv[])
{
	unsigned int para1;
	unsigned int para2;

	msg_tmp.mtype = MSG_FUN_RESULT;	
	if(argc != 3){
		printf("input para cnt is not 3.\r\n");
		sprintf(msg_tmp.mtext, "input para cnt is not 3.\r\n");
		msg_send(TASK2_MODULE, (char *)&msg_tmp, strlen(msg_tmp.mtext));
		return 1;
	}
	para1 = strtol(argv[1], NULL, 10);
	para2 = strtol(argv[2], NULL, 10);
	emif_fpga_duc_carrier_f8_write((unsigned char)para1, (unsigned short)para2);
	sprintf(msg_tmp.mtext, "emif_fpga_duc_carrier_f8_write(%d, %d).\r\n", para1, para2);
	msg_send(TASK2_MODULE, (char *)&msg_tmp, strlen(msg_tmp.mtext));
	return 0;
}
void emif_fpga_duc_carrier_m8_write(unsigned char addr,unsigned short data)
{
	
*DRU_REGADDR(fpga_base_addr, 0x39) = addr;
*DRU_REGADDR(fpga_base_addr, 0x38) = data;

*DRU_REGADDR(fpga_base_addr, 0x37) = 0;	
*DRU_REGADDR(fpga_base_addr, 0x37) = 1;	
*DRU_REGADDR(fpga_base_addr, 0x37) = 0;	

}
int emif_fpga_duc_carrier_m8_write_change(int argc, char * argv[])
{
	unsigned int para1;
	unsigned int para2;

	msg_tmp.mtype = MSG_FUN_RESULT;	
	if(argc != 3){
		printf("input para cnt is not 3.\r\n");
		sprintf(msg_tmp.mtext, "input para cnt is not 3.\r\n");
		msg_send(TASK2_MODULE, (char *)&msg_tmp, strlen(msg_tmp.mtext));
		return 1;
	}
	para1 = strtol(argv[1], NULL, 10);
	para2 = strtol(argv[2], NULL, 10);
	emif_fpga_duc_carrier_m8_write((unsigned char)para1, (unsigned short)para2);
	sprintf(msg_tmp.mtext, "emif_fpga_duc_carrier_m8_write(%d, %d).\r\n", para1, para2);
	msg_send(TASK2_MODULE, (char *)&msg_tmp, strlen(msg_tmp.mtext));
	return 0;
}

unsigned short emif_fpga_carrier_read(unsigned char addr)
{
	unsigned short temp;
	temp = *DRU_REGADDR(fpga_base_addr, 0x34); 
	
	
	return temp;
	
}
