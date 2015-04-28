/*******************************************************************************
********************************************************************************
* 文件名称:  dru_dac3484.c
* 功能描述:  dac3484的配置操作函数
* 使用说明:  
* 文件作者:	H4
* 编写日期: （2012/06/18）
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
#include "../../driver/dru_spi.h"
#include "../../driver/dru_reg.h"
#include "../../driver/omap_epld.h"
#include "../../driver/dru_dac3484.h"
#include "common.h"
#include "status.h"
/*******************************************************************************
* 函数名称: dru_dac3484_init
* 功    能: dac3484初始化配置
* 参    数:
* 参数名称         类型                描述
* 无
* 返回值:
* 无
* 说   明:
* 日   期     版本    作者   修改人      DEBUG
* -----------------------------------------------------------------
* 2012/06/18  V1.0     H4     无       ：
*******************************************************************************/


void dac1_reset_n(unsigned char level)
{
	*DRU_REGADDR(epld_base_addr, 0x13) = level;
	
}

void dac1_txen(unsigned char level)
{
	*DRU_REGADDR(epld_base_addr, 0x14) = level;
	
}

void dac2_reset_n(unsigned char level)
{
	*DRU_REGADDR(epld_base_addr, 0x15) = level;
	
}

void dac2_txen(unsigned char level)
{
	*DRU_REGADDR(epld_base_addr, 0x16) = level;
	
}
int dru_dac3484_init(void)
{
	if(get_device_type()==DEVICE_TYPE_MAIN){
		dru_dac3484_init_m();
	}else{
		dru_dac3484_init_s();
	}
}

int dru_dac3484_init_s(void)
//#if DEV_TYPE == 1
{
	emif_epld_spi_channel(SELECT_DAC3481_1);
	dac1_txen(0);
	dac1_reset_n(0);
	usleep(1000);
	dac1_reset_n(1);
emif_epld_spi_write(0X00f89f,DAC3484_REG_LENGTH);         //0xf89f qmc en=1,interp=16x//0XF49f qmc en=1,interp=8x
emif_epld_spi_write(0X01050E,DAC3484_REG_LENGTH);
emif_epld_spi_write(0X02F0F2,DAC3484_REG_LENGTH);         //word-wide data interface; 2's complement;4 spi;mixer en=1;mixer gain en=1;nco en=1;
emif_epld_spi_write(0X03a000,DAC3484_REG_LENGTH);         //dac 电流强度 Ifs
emif_epld_spi_write(0X04FFFF,DAC3484_REG_LENGTH);
emif_epld_spi_write(0X050000,DAC3484_REG_LENGTH);         // write clear alarm registers
emif_epld_spi_write(0X063300,DAC3484_REG_LENGTH);         // temerature read only
emif_epld_spi_write(0X07FFFF,DAC3484_REG_LENGTH);         // mask of alarm
emif_epld_spi_write(0X080000,DAC3484_REG_LENGTH);         // auto sync
emif_epld_spi_write(0X098000,DAC3484_REG_LENGTH);         // fifo_offset when sync
emif_epld_spi_write(0X0A0000,DAC3484_REG_LENGTH);
emif_epld_spi_write(0X0B0000,DAC3484_REG_LENGTH);
emif_epld_spi_write(0X0C07ff,DAC3484_REG_LENGTH);         /* 通道0的I路增益 */
emif_epld_spi_write(0X0D07ff,DAC3484_REG_LENGTH);         /* 通道0的Q路增益 */ 
emif_epld_spi_write(0X0E07ff,DAC3484_REG_LENGTH);         /* 通道1的I路增益 */ 
emif_epld_spi_write(0X0F07ff,DAC3484_REG_LENGTH);         /* 通道1的Q路增益 */ 
emif_epld_spi_write(0X100000,DAC3484_REG_LENGTH);         //qmc phase 
emif_epld_spi_write(0X110000,DAC3484_REG_LENGTH);         //qmc phase
emif_epld_spi_write(0X120000,DAC3484_REG_LENGTH);
emif_epld_spi_write(0X130000,DAC3484_REG_LENGTH);

emif_epld_spi_write(0X151800,DAC3484_REG_LENGTH);         //nco frequence word [15:0]   187.16*2^32/983.04=18000000   92.16*2^32/491.52 =30 000000
emif_epld_spi_write(0X140000,DAC3484_REG_LENGTH);         //nco frequence word [31:16]  AB   76.8M
emif_epld_spi_write(0X171800,DAC3484_REG_LENGTH);         //nco frequence word [15:0]     
emif_epld_spi_write(0X160000,DAC3484_REG_LENGTH);         //nco frequence word [31:16]  CD

emif_epld_spi_write(0X18280F,DAC3484_REG_LENGTH);         //external pll
emif_epld_spi_write(0X190440,DAC3484_REG_LENGTH);         // pll
emif_epld_spi_write(0X1A0020,DAC3484_REG_LENGTH);         // pll sleep=1; DACa,b,c,d sleep =0



//emif_epld_spi_write(0X182ce0,24);         // internal pll
//emif_epld_spi_write(0X191004,24);         // pll
//emif_epld_spi_write(0X1AEc00,24);         // pll sleep=1; DACa,b,c,d sleep =0



emif_epld_spi_write(0X1B0000,DAC3484_REG_LENGTH);        //defaut
emif_epld_spi_write(0X1C0000,DAC3484_REG_LENGTH);          //defaut
emif_epld_spi_write(0X1D0000,DAC3484_REG_LENGTH);           //defaut
emif_epld_spi_write(0X1E1111,DAC3484_REG_LENGTH);           //qmc sync source sel 
emif_epld_spi_write(0X1F8884,DAC3484_REG_LENGTH);           // mixer,nco sync source sel ,sif_sync
emif_epld_spi_write(0X201101,DAC3484_REG_LENGTH);           //fifo sync source sel =  sync
emif_epld_spi_write(0X221B1B,DAC3484_REG_LENGTH);
emif_epld_spi_write(0X23FFFF,DAC3484_REG_LENGTH);
emif_epld_spi_write(0X240C00,DAC3484_REG_LENGTH);           // dataclk delay  3bit 011= 3*50 ps=150 ps 000 000 
emif_epld_spi_write(0X257A7A,DAC3484_REG_LENGTH);
emif_epld_spi_write(0X26B6B6,DAC3484_REG_LENGTH);
emif_epld_spi_write(0X27EAEA,DAC3484_REG_LENGTH);
emif_epld_spi_write(0X284545,DAC3484_REG_LENGTH);
emif_epld_spi_write(0X291A1A,DAC3484_REG_LENGTH);
emif_epld_spi_write(0X2A1616,DAC3484_REG_LENGTH);
emif_epld_spi_write(0X2BAAAA,DAC3484_REG_LENGTH);
emif_epld_spi_write(0X2CC6C6,DAC3484_REG_LENGTH);
emif_epld_spi_write(0X2D0002,DAC3484_REG_LENGTH);
emif_epld_spi_write(0X2E0000,DAC3484_REG_LENGTH);
emif_epld_spi_write(0X2F0000,DAC3484_REG_LENGTH);
emif_epld_spi_write(0X300E5C,DAC3484_REG_LENGTH);
emif_epld_spi_write(0X7F0000,DAC3484_REG_LENGTH);      //version read only
emif_epld_spi_write(0X1F8884,DAC3484_REG_LENGTH);      // soft sync
emif_epld_spi_write(0X00f89b,DAC3484_REG_LENGTH);      // disable clock_divider_sync
emif_epld_spi_write(0X1F8886,DAC3484_REG_LENGTH);      //clear soft sync
dac1_txen(1);
emif_epld_spi_channel(SELECT_DAC3481_2);
dac2_txen(0);
dac2_reset_n(0);
	usleep(1000);
dac2_reset_n(1);
emif_epld_spi_write(0X00f89f,DAC3484_REG_LENGTH);         //0XF29B qmc en=1,interp=8x
emif_epld_spi_write(0X01050E,DAC3484_REG_LENGTH);
emif_epld_spi_write(0X02F0F2,DAC3484_REG_LENGTH);         //word-wide data interface; 2's complement;4 spi;mixer en=1;mixer gain en=1;nco en=1;
emif_epld_spi_write(0X03a000,DAC3484_REG_LENGTH);         //dac 电流强度 Ifs
emif_epld_spi_write(0X04FFFF,DAC3484_REG_LENGTH);
emif_epld_spi_write(0X050000,DAC3484_REG_LENGTH);         // write clear alarm registers
emif_epld_spi_write(0X063300,DAC3484_REG_LENGTH);         // temerature read only
emif_epld_spi_write(0X07FFFF,DAC3484_REG_LENGTH);         // mask of alarm
emif_epld_spi_write(0X080000,DAC3484_REG_LENGTH);         // auto sync
emif_epld_spi_write(0X098000,DAC3484_REG_LENGTH);         // fifo_offset when sync
emif_epld_spi_write(0X0A0000,DAC3484_REG_LENGTH);
emif_epld_spi_write(0X0B0000,DAC3484_REG_LENGTH);
emif_epld_spi_write(0X0C07ff,DAC3484_REG_LENGTH);         /* 通道0的I路增益 */
emif_epld_spi_write(0X0D07ff,DAC3484_REG_LENGTH);         /* 通道0的Q路增益 */ 
emif_epld_spi_write(0X0E07ff,DAC3484_REG_LENGTH);         /* 通道1的I路增益 */ 
emif_epld_spi_write(0X0F07ff,DAC3484_REG_LENGTH);         /* 通道1的Q路增益 */ 
emif_epld_spi_write(0X100000,DAC3484_REG_LENGTH);         //qmc phase 
emif_epld_spi_write(0X110000,DAC3484_REG_LENGTH);         //qmc phase
emif_epld_spi_write(0X120000,DAC3484_REG_LENGTH);
emif_epld_spi_write(0X130000,DAC3484_REG_LENGTH);

emif_epld_spi_write(0X160000,DAC3484_REG_LENGTH);         //nco frequence word [15:0]     140.16*2^32/983.04 = 0x24800000
emif_epld_spi_write(0X1723b8,DAC3484_REG_LENGTH);         //nco frequence word [31:16]  AB   76.8M
emif_epld_spi_write(0X151800,DAC3484_REG_LENGTH);         //nco frequence word [15:0]     282.16*2^32/983.04 = 0x497aaaab
emif_epld_spi_write(0X140000,DAC3484_REG_LENGTH);         //nco frequence word [31:16]  CD

emif_epld_spi_write(0X18280F,DAC3484_REG_LENGTH);         //pll
emif_epld_spi_write(0X190440,DAC3484_REG_LENGTH);         // pll
emif_epld_spi_write(0X1A0020,DAC3484_REG_LENGTH);         // pll sleep=1; DACa,b,c,d sleep =0


//emif_epld_spi_write(0X182ce0,24);         //pll
//emif_epld_spi_write(0X191004,24);         // pll
//emif_epld_spi_write(0X1AEc00,24);         // pll sleep=1; DACa,b,c,d sleep =0




emif_epld_spi_write(0X1B0000,DAC3484_REG_LENGTH);        //defaut
emif_epld_spi_write(0X1C0000,DAC3484_REG_LENGTH);          //defaut
emif_epld_spi_write(0X1D0000,DAC3484_REG_LENGTH);           //defaut
emif_epld_spi_write(0X1E1111,DAC3484_REG_LENGTH);           //qmc sync source sel 
emif_epld_spi_write(0X1F8884,DAC3484_REG_LENGTH);           // mixer,nco sync source sel ,sif_sync
emif_epld_spi_write(0X201101,DAC3484_REG_LENGTH);           //fifo sync source sel =  sync
emif_epld_spi_write(0X221B1B,DAC3484_REG_LENGTH);
emif_epld_spi_write(0X23FFFF,DAC3484_REG_LENGTH);
emif_epld_spi_write(0X240C00,DAC3484_REG_LENGTH);           // dataclk delay  3bit 011= 3*50 ps=150 ps 000 000 
emif_epld_spi_write(0X257A7A,DAC3484_REG_LENGTH);
emif_epld_spi_write(0X26B6B6,DAC3484_REG_LENGTH);
emif_epld_spi_write(0X27EAEA,DAC3484_REG_LENGTH);
emif_epld_spi_write(0X284545,DAC3484_REG_LENGTH);
emif_epld_spi_write(0X291A1A,DAC3484_REG_LENGTH);
emif_epld_spi_write(0X2A1616,DAC3484_REG_LENGTH);
emif_epld_spi_write(0X2BAAAA,DAC3484_REG_LENGTH);
emif_epld_spi_write(0X2CC6C6,DAC3484_REG_LENGTH);
emif_epld_spi_write(0X2D0002,DAC3484_REG_LENGTH);
emif_epld_spi_write(0X2E0000,DAC3484_REG_LENGTH);
emif_epld_spi_write(0X2F0000,DAC3484_REG_LENGTH);
emif_epld_spi_write(0X300E5C,DAC3484_REG_LENGTH);
emif_epld_spi_write(0X7F0000,DAC3484_REG_LENGTH);      //version read only
emif_epld_spi_write(0X1F8884,DAC3484_REG_LENGTH);      // soft sync
emif_epld_spi_write(0X00f89b,DAC3484_REG_LENGTH);      // disable clock_divider_sync
emif_epld_spi_write(0X1F8886,DAC3484_REG_LENGTH);      //clear soft sync
dac2_txen(1);
return 0;
}
//#endif

// 主单元调用
int dru_dac3484_init_m(void)
//#if DEV_TYPE == 0
{
	emif_epld_spi_channel(SELECT_DAC3481_1);
	
	dac1_txen(0);
	dac1_reset_n(0);
spi_delay(1000);
	dac1_reset_n(1);
emif_epld_spi_write(0X00f89f,DAC3484_REG_LENGTH);         //0xf89f qmc en=1,interp=16x//0XF49f qmc en=1,interp=8x
emif_epld_spi_write(0X01050E,DAC3484_REG_LENGTH);
emif_epld_spi_write(0X02F0F2,DAC3484_REG_LENGTH);         //word-wide data interface; 2's complement;4 spi;mixer en=1;mixer gain en=1;nco en=1;
emif_epld_spi_write(0X03a000,DAC3484_REG_LENGTH);         //dac 电流强度 Ifs
emif_epld_spi_write(0X04FFFF,DAC3484_REG_LENGTH);
emif_epld_spi_write(0X050000,DAC3484_REG_LENGTH);         // write clear alarm registers
emif_epld_spi_write(0X063300,DAC3484_REG_LENGTH);         // temerature read only
emif_epld_spi_write(0X07FFFF,DAC3484_REG_LENGTH);         // mask of alarm
emif_epld_spi_write(0X080000,DAC3484_REG_LENGTH);         // auto sync
emif_epld_spi_write(0X098000,DAC3484_REG_LENGTH);         // fifo_offset when sync
emif_epld_spi_write(0X0A0000,DAC3484_REG_LENGTH);
emif_epld_spi_write(0X0B0000,DAC3484_REG_LENGTH);

emif_epld_spi_write(0X0C07ff,DAC3484_REG_LENGTH);         /* 通道0的I路增益 */
emif_epld_spi_write(0X0D07ff,DAC3484_REG_LENGTH);         /* 通道0的Q路增益 */ 
emif_epld_spi_write(0X0E07ff,DAC3484_REG_LENGTH);         /* 通道1的I路增益 */ 
emif_epld_spi_write(0X0F07ff,DAC3484_REG_LENGTH);         /* 通道1的Q路增益 */ 
emif_epld_spi_write(0X100000,DAC3484_REG_LENGTH);         //qmc phase 
emif_epld_spi_write(0X110000,DAC3484_REG_LENGTH);         //qmc phase
emif_epld_spi_write(0X120000,DAC3484_REG_LENGTH);
emif_epld_spi_write(0X130000,DAC3484_REG_LENGTH);

emif_epld_spi_write(0X151800,DAC3484_REG_LENGTH);         //nco frequence word [15:0]   58.6*2^32/983.04=0x0f42aaab  92.16*2^32/491.52 =30 000000
emif_epld_spi_write(0X140000,DAC3484_REG_LENGTH);         //nco frequence word [31:16]  AB   76.8M
emif_epld_spi_write(0X160000,DAC3484_REG_LENGTH);         //nco frequence word [15:0]     
emif_epld_spi_write(0X171800,DAC3484_REG_LENGTH);         //nco frequence word [31:16]  CD

emif_epld_spi_write(0X18280F,DAC3484_REG_LENGTH);         //external clk
emif_epld_spi_write(0X190440,DAC3484_REG_LENGTH);         // pll
emif_epld_spi_write(0X1A0020,DAC3484_REG_LENGTH);         // pll sleep=1; DACa,b,c,d sleep =0



//emif_epld_spi_write(0X182ce0,24);         // internal pll
//emif_epld_spi_write(0X191004,24);         // pll
//emif_epld_spi_write(0X1AEc00,24);         // pll sleep=1; DACa,b,c,d sleep =0



emif_epld_spi_write(0X1B0000,DAC3484_REG_LENGTH);        //defaut
emif_epld_spi_write(0X1C0000,DAC3484_REG_LENGTH);          //defaut
emif_epld_spi_write(0X1D0000,DAC3484_REG_LENGTH);           //defaut
emif_epld_spi_write(0X1E1111,DAC3484_REG_LENGTH);           //qmc sync source sel 
emif_epld_spi_write(0X1F8884,DAC3484_REG_LENGTH);           // mixer,nco sync source sel ,sif_sync
emif_epld_spi_write(0X201101,DAC3484_REG_LENGTH);           //fifo sync source sel =  sync
emif_epld_spi_write(0X221B1B,DAC3484_REG_LENGTH);
emif_epld_spi_write(0X23FFFF,DAC3484_REG_LENGTH);
emif_epld_spi_write(0X240C00,DAC3484_REG_LENGTH);           // dataclk delay  3bit 011= 3*50 ps=150 ps 000 000 
emif_epld_spi_write(0X257A7A,DAC3484_REG_LENGTH);
emif_epld_spi_write(0X26B6B6,DAC3484_REG_LENGTH);
emif_epld_spi_write(0X27EAEA,DAC3484_REG_LENGTH);
emif_epld_spi_write(0X284545,DAC3484_REG_LENGTH);
emif_epld_spi_write(0X291A1A,DAC3484_REG_LENGTH);
emif_epld_spi_write(0X2A1616,DAC3484_REG_LENGTH);
emif_epld_spi_write(0X2BAAAA,DAC3484_REG_LENGTH);
emif_epld_spi_write(0X2CC6C6,DAC3484_REG_LENGTH);
emif_epld_spi_write(0X2D0002,DAC3484_REG_LENGTH);
emif_epld_spi_write(0X2E0000,DAC3484_REG_LENGTH);
emif_epld_spi_write(0X2F0000,DAC3484_REG_LENGTH);
emif_epld_spi_write(0X300E5C,DAC3484_REG_LENGTH);
emif_epld_spi_write(0X7F0000,DAC3484_REG_LENGTH);      //version read only
emif_epld_spi_write(0X1F8884,DAC3484_REG_LENGTH);      // soft sync
emif_epld_spi_write(0X00f89b,DAC3484_REG_LENGTH);      // disable clock_divider_sync
emif_epld_spi_write(0X1F8886,DAC3484_REG_LENGTH);      //clear soft sync
dac1_txen(1);
emif_epld_spi_channel(SELECT_DAC3481_2);
dac2_txen(0);
dac2_reset_n(0);
	spi_delay(1000);
dac2_reset_n(1);
emif_epld_spi_write(0X00f89f,DAC3484_REG_LENGTH);         //0XF29B qmc en=1,interp=8x
emif_epld_spi_write(0X01050E,DAC3484_REG_LENGTH);
emif_epld_spi_write(0X02F0F2,DAC3484_REG_LENGTH);         //word-wide data interface; 2's complement;4 spi;mixer en=1;mixer gain en=1;nco en=1;
emif_epld_spi_write(0X03a000,DAC3484_REG_LENGTH);         //dac 电流强度 Ifs
emif_epld_spi_write(0X04FFFF,DAC3484_REG_LENGTH);
emif_epld_spi_write(0X050000,DAC3484_REG_LENGTH);         // write clear alarm registers
emif_epld_spi_write(0X063300,DAC3484_REG_LENGTH);         // temerature read only
emif_epld_spi_write(0X07FFFF,DAC3484_REG_LENGTH);         // mask of alarm
emif_epld_spi_write(0X080000,DAC3484_REG_LENGTH);         // auto sync
emif_epld_spi_write(0X098000,DAC3484_REG_LENGTH);         // fifo_offset when sync
emif_epld_spi_write(0X0A0000,DAC3484_REG_LENGTH);
emif_epld_spi_write(0X0B0000,DAC3484_REG_LENGTH);
emif_epld_spi_write(0X0C07ff,DAC3484_REG_LENGTH);         /* 通道0的I路增益 */
emif_epld_spi_write(0X0D07ff,DAC3484_REG_LENGTH);         /* 通道0的Q路增益 */ 
emif_epld_spi_write(0X0E07ff,DAC3484_REG_LENGTH);         /* 通道1的I路增益 */ 
emif_epld_spi_write(0X0F07ff,DAC3484_REG_LENGTH);         /* 通道1的Q路增益 */ 
emif_epld_spi_write(0X100000,DAC3484_REG_LENGTH);         //qmc phase 
emif_epld_spi_write(0X110000,DAC3484_REG_LENGTH);         //qmc phase
emif_epld_spi_write(0X120000,DAC3484_REG_LENGTH);
emif_epld_spi_write(0X130000,DAC3484_REG_LENGTH);

emif_epld_spi_write(0X160000,DAC3484_REG_LENGTH);         //nco frequence word [15:0]     108.6*2^32/983.04 = 0x1c480000  gsm
emif_epld_spi_write(0X171c48,DAC3484_REG_LENGTH);         //nco frequence word [31:16]  AB   76.8M
emif_epld_spi_write(0X140000,DAC3484_REG_LENGTH);         //nco frequence word [15:0]     wcdma  main 946.64*2^32/983.04 = 0xf6855555
emif_epld_spi_write(0X151800,DAC3484_REG_LENGTH);         //nco frequence word [31:16]  CD

emif_epld_spi_write(0X18280F,DAC3484_REG_LENGTH);         //external clk
emif_epld_spi_write(0X190440,DAC3484_REG_LENGTH);         // pll
emif_epld_spi_write(0X1A0020,DAC3484_REG_LENGTH);         // pll sleep=1; DACa,b,c,d sleep =0


//emif_epld_spi_write(0X182ce0,24);         //internal pll
//emif_epld_spi_write(0X191004,24);         // pll
//emif_epld_spi_write(0X1AEc00,24);         // pll sleep=1; DACa,b,c,d sleep =0




emif_epld_spi_write(0X1B0000,DAC3484_REG_LENGTH);        //defaut
emif_epld_spi_write(0X1C0000,DAC3484_REG_LENGTH);          //defaut
emif_epld_spi_write(0X1D0000,DAC3484_REG_LENGTH);           //defaut
emif_epld_spi_write(0X1E1111,DAC3484_REG_LENGTH);           //qmc sync source sel 
emif_epld_spi_write(0X1F8884,DAC3484_REG_LENGTH);           // mixer,nco sync source sel ,sif_sync
emif_epld_spi_write(0X201101,DAC3484_REG_LENGTH);           //fifo sync source sel =  sync
emif_epld_spi_write(0X221B1B,DAC3484_REG_LENGTH);
emif_epld_spi_write(0X23FFFF,DAC3484_REG_LENGTH);
emif_epld_spi_write(0X240C00,DAC3484_REG_LENGTH);           // dataclk delay  3bit 011= 3*50 ps=150 ps 000 000 
emif_epld_spi_write(0X257A7A,DAC3484_REG_LENGTH);
emif_epld_spi_write(0X26B6B6,DAC3484_REG_LENGTH);
emif_epld_spi_write(0X27EAEA,DAC3484_REG_LENGTH);
emif_epld_spi_write(0X284545,DAC3484_REG_LENGTH);
emif_epld_spi_write(0X291A1A,DAC3484_REG_LENGTH);
emif_epld_spi_write(0X2A1616,DAC3484_REG_LENGTH);
emif_epld_spi_write(0X2BAAAA,DAC3484_REG_LENGTH);
emif_epld_spi_write(0X2CC6C6,DAC3484_REG_LENGTH);
emif_epld_spi_write(0X2D0002,DAC3484_REG_LENGTH);
emif_epld_spi_write(0X2E0000,DAC3484_REG_LENGTH);
emif_epld_spi_write(0X2F0000,DAC3484_REG_LENGTH);
emif_epld_spi_write(0X300E5C,DAC3484_REG_LENGTH);
emif_epld_spi_write(0X7F0000,DAC3484_REG_LENGTH);      //version read only
emif_epld_spi_write(0X1F8884,DAC3484_REG_LENGTH);      // soft sync
emif_epld_spi_write(0X00f89b,DAC3484_REG_LENGTH);      // disable clock_divider_sync
emif_epld_spi_write(0X1F8886,DAC3484_REG_LENGTH);      //clear soft sync
dac2_txen(1);
return 0;



}
//#endif

void dac3484_lte1_qmc_offset_a(unsigned short offset)
{
emif_epld_spi_channel(SELECT_DAC3481_1);
	spi_delay(100);	
emif_epld_spi_write((0X080000|offset),DAC3484_REG_LENGTH); 	
	
}

void dac3484_lte1_qmc_offset_b(unsigned short offset)
{
emif_epld_spi_channel(SELECT_DAC3481_1);
	spi_delay(100);	
emif_epld_spi_write((0X098000|offset),DAC3484_REG_LENGTH); 	
	
}


void dac3484_lte2_qmc_offset_c(unsigned short offset)
{
emif_epld_spi_channel(SELECT_DAC3481_1);
	spi_delay(100);	
emif_epld_spi_write((0X0a0000|offset),DAC3484_REG_LENGTH); 	
	
}

void dac3484_lte2_qmc_offset_d(unsigned short offset)
{
emif_epld_spi_channel(SELECT_DAC3481_1);
	spi_delay(100);	
emif_epld_spi_write((0X0b0000|offset),DAC3484_REG_LENGTH); 	
	
}

void dac3484_gsm_qmc_offset_a(unsigned short offset)
{
emif_epld_spi_channel(SELECT_DAC3481_2);
	spi_delay(100);	
emif_epld_spi_write((0X080000|offset),DAC3484_REG_LENGTH); 	
	
}
int dac3484_gsm_qmc_offset_a_change(int argc, char * argv[])
{
	unsigned int para1;

	msg_tmp.mtype = MSG_FUN_RESULT;	
	if(argc != 2){
		printf("input para cnt is not 2.\r\n");
		sprintf(msg_tmp.mtext, "input para cnt is not 2.\r\n");
		msg_send(TASK2_MODULE, (char *)&msg_tmp, strlen(msg_tmp.mtext));
		return 1;
	}
	para1 = strtol(argv[1], NULL, 10);
	printf("para1=%d. \r\n", para1);
	dac3484_gsm_qmc_offset_a((unsigned short)para1);
	sprintf(msg_tmp.mtext, "dac3484_gsm_qmc_offset_a=%d\r\n", para1);
	msg_send(TASK2_MODULE, (char *)&msg_tmp, strlen(msg_tmp.mtext));
	return 0;
}

void dac3484_gsm_qmc_offset_b(unsigned short offset)
{
emif_epld_spi_channel(SELECT_DAC3481_2);
	spi_delay(100);	
emif_epld_spi_write((0X098000|offset),DAC3484_REG_LENGTH); 	
	
}
int dac3484_gsm_qmc_offset_b_change(int argc, char * argv[])
{
	unsigned int para1;

	msg_tmp.mtype = MSG_FUN_RESULT;	
	if(argc != 2){
		printf("input para cnt is not 2.\r\n");
		sprintf(msg_tmp.mtext, "input para cnt is not 2.\r\n");
		msg_send(TASK2_MODULE, (char *)&msg_tmp, strlen(msg_tmp.mtext));
		return 1;
	}
	para1 = strtol(argv[1], NULL, 10);
	printf("para1=%d. \r\n", para1);
	dac3484_gsm_qmc_offset_b((unsigned short)para1);
	sprintf(msg_tmp.mtext, "dac3484_gsm_qmc_offset_b=%d\r\n", para1);
	msg_send(TASK2_MODULE, (char *)&msg_tmp, strlen(msg_tmp.mtext));
	return 0;
}


void dac3484_td_qmc_offset_c(unsigned short offset)
{
emif_epld_spi_channel(SELECT_DAC3481_2);
	spi_delay(100);	
emif_epld_spi_write((0X0a0000|offset),DAC3484_REG_LENGTH); 	
	
}

void dac3484_td_qmc_offset_d(unsigned short offset) //12bit 4096
{
emif_epld_spi_channel(SELECT_DAC3481_2);
	spi_delay(100);	
emif_epld_spi_write((0X0b0000|offset),DAC3484_REG_LENGTH); 	
	
}

void dac3484_lte1_qmc_gain_a(unsigned short offset) //11bit 2048
{
emif_epld_spi_channel(SELECT_DAC3481_1);
	spi_delay(100);	
emif_epld_spi_write((0X0c0000|offset),DAC3484_REG_LENGTH); 	
	
}
void dac3484_lte1_qmc_gain_b(unsigned short offset) //11bit 2048
{
emif_epld_spi_channel(SELECT_DAC3481_1);
	spi_delay(100);	
emif_epld_spi_write((0X0d0000|offset),DAC3484_REG_LENGTH); 	
	
}

void dac3484_lte2_qmc_gain_c(unsigned short offset) //11bit 2048
{
emif_epld_spi_channel(SELECT_DAC3481_1);
	spi_delay(100);	
emif_epld_spi_write((0X0e0000|offset),DAC3484_REG_LENGTH); 	
	
}
void dac3484_lte2_qmc_gain_d(unsigned short offset) //11bit 2048
{
emif_epld_spi_channel(SELECT_DAC3481_1);
	spi_delay(100);	
emif_epld_spi_write((0X0f0000|offset),DAC3484_REG_LENGTH); 	
	
}

void dac3484_gsm_qmc_gain_a(unsigned short offset) //11bit 2048
{
emif_epld_spi_channel(SELECT_DAC3481_2);
	spi_delay(100);	
emif_epld_spi_write((0X0c0000|offset),DAC3484_REG_LENGTH); 	
	
}
void dac3484_gsm_qmc_gain_b(unsigned short offset) //11bit 2048
{
emif_epld_spi_channel(SELECT_DAC3481_2);
	spi_delay(100);	
emif_epld_spi_write((0X0d0000|offset),DAC3484_REG_LENGTH); 	
	
}
void dac3484_td_qmc_gain_c(unsigned short offset) //11bit 2048
{
emif_epld_spi_channel(SELECT_DAC3481_2);
	spi_delay(100);	
emif_epld_spi_write((0X0e0000|offset),DAC3484_REG_LENGTH); 	
	
}
void dac3484_td_qmc_gain_d(unsigned short offset) //11bit 2048
{
emif_epld_spi_channel(SELECT_DAC3481_2);
	spi_delay(100);	
emif_epld_spi_write((0X0f0000|offset),DAC3484_REG_LENGTH); 	
	
}

void dac3484_lte1_qmc_phase_ab(signed short offset) //12bit 4096
{
emif_epld_spi_channel(SELECT_DAC3481_1);
	spi_delay(100);	
emif_epld_spi_write((0X100000|offset),DAC3484_REG_LENGTH); 	
	
}

void dac3484_lte2_qmc_phase_cd(signed short offset) //12bit 4096
{
emif_epld_spi_channel(SELECT_DAC3481_1);
	spi_delay(100);	
emif_epld_spi_write((0X110000|offset),DAC3484_REG_LENGTH); 	
	
}

void dac3484_gsm_qmc_phase_ab(signed short offset) //12bit 4096
{
emif_epld_spi_channel(SELECT_DAC3481_2);
	spi_delay(100);	
emif_epld_spi_write((0X100000|offset),DAC3484_REG_LENGTH); 	
	
}

void dac3484_td_qmc_phase_cd(signed short offset) //12bit 4096
{
emif_epld_spi_channel(SELECT_DAC3481_2);
	spi_delay(100);	
emif_epld_spi_write((0X110000|offset),DAC3484_REG_LENGTH); 	
	
}

int dru_dac3484_init_m_gz(void)
{
	emif_epld_spi_channel(SELECT_DAC3481_1);

	dac1_txen(0);
	dac1_reset_n(0);
	spi_delay(1000);
	dac1_reset_n(1);
	emif_epld_spi_write(0X00f89f,DAC3484_REG_LENGTH);         //0xf89f qmc en=1,interp=16x//0XF49f qmc en=1,interp=8x
	emif_epld_spi_write(0X01050E,DAC3484_REG_LENGTH);
	emif_epld_spi_write(0X02F0F2,DAC3484_REG_LENGTH);         //word-wide data interface; 2's complement;4 spi;mixer en=1;mixer gain en=1;nco en=1;
	emif_epld_spi_write(0X03a000,DAC3484_REG_LENGTH);         //dac 电流强度 Ifs
	emif_epld_spi_write(0X04FFFF,DAC3484_REG_LENGTH);
	emif_epld_spi_write(0X050000,DAC3484_REG_LENGTH);         // write clear alarm registers
	emif_epld_spi_write(0X063300,DAC3484_REG_LENGTH);         // temerature read only
	emif_epld_spi_write(0X07FFFF,DAC3484_REG_LENGTH);         // mask of alarm
	emif_epld_spi_write(0X080000,DAC3484_REG_LENGTH);         // auto sync
	emif_epld_spi_write(0X098000,DAC3484_REG_LENGTH);         // fifo_offset when sync
	emif_epld_spi_write(0X0A0000,DAC3484_REG_LENGTH);
	emif_epld_spi_write(0X0B0000,DAC3484_REG_LENGTH);

	emif_epld_spi_write(0X0C07ff,DAC3484_REG_LENGTH);         /* 通道0的I路增益 */
	emif_epld_spi_write(0X0D07ff,DAC3484_REG_LENGTH);         /* 通道0的Q路增益 */ 
	emif_epld_spi_write(0X0E07ff,DAC3484_REG_LENGTH);         /* 通道1的I路增益 */ 
	emif_epld_spi_write(0X0F07ff,DAC3484_REG_LENGTH);         /* 通道1的Q路增益 */ 
	emif_epld_spi_write(0X100000,DAC3484_REG_LENGTH);         //qmc phase 
	emif_epld_spi_write(0X110000,DAC3484_REG_LENGTH);         //qmc phase
	emif_epld_spi_write(0X120000,DAC3484_REG_LENGTH);
	emif_epld_spi_write(0X130000,DAC3484_REG_LENGTH);

	emif_epld_spi_write(0X151800,DAC3484_REG_LENGTH);         //nco frequence word [15:0]   58.6*2^32/983.04=0x0f42aaab  92.16*2^32/491.52 =30 000000
	emif_epld_spi_write(0X140000,DAC3484_REG_LENGTH);         //nco frequence word [31:16]  AB   76.8M
	emif_epld_spi_write(0X160000,DAC3484_REG_LENGTH);         //nco frequence word [15:0]     
	emif_epld_spi_write(0X171800,DAC3484_REG_LENGTH);         //nco frequence word [31:16]  CD

	emif_epld_spi_write(0X18280F,DAC3484_REG_LENGTH);         //external clk
	emif_epld_spi_write(0X190440,DAC3484_REG_LENGTH);         // pll
	emif_epld_spi_write(0X1A0020,DAC3484_REG_LENGTH);         // pll sleep=1; DACa,b,c,d sleep =0

	//emif_epld_spi_write(0X182ce0,24);         // internal pll
	//emif_epld_spi_write(0X191004,24);         // pll
	//emif_epld_spi_write(0X1AEc00,24);         // pll sleep=1; DACa,b,c,d sleep =0

	emif_epld_spi_write(0X1B0000,DAC3484_REG_LENGTH);        //defaut
	emif_epld_spi_write(0X1C0000,DAC3484_REG_LENGTH);          //defaut
	emif_epld_spi_write(0X1D0000,DAC3484_REG_LENGTH);           //defaut
	emif_epld_spi_write(0X1E1111,DAC3484_REG_LENGTH);           //qmc sync source sel 
	emif_epld_spi_write(0X1F8884,DAC3484_REG_LENGTH);           // mixer,nco sync source sel ,sif_sync
	emif_epld_spi_write(0X201101,DAC3484_REG_LENGTH);           //fifo sync source sel =  sync
	emif_epld_spi_write(0X221B1B,DAC3484_REG_LENGTH);
	emif_epld_spi_write(0X23FFFF,DAC3484_REG_LENGTH);
	emif_epld_spi_write(0X240C00,DAC3484_REG_LENGTH);           // dataclk delay  3bit 011= 3*50 ps=150 ps 000 000 
	emif_epld_spi_write(0X257A7A,DAC3484_REG_LENGTH);
	emif_epld_spi_write(0X26B6B6,DAC3484_REG_LENGTH);
	emif_epld_spi_write(0X27EAEA,DAC3484_REG_LENGTH);
	emif_epld_spi_write(0X284545,DAC3484_REG_LENGTH);
	emif_epld_spi_write(0X291A1A,DAC3484_REG_LENGTH);
	emif_epld_spi_write(0X2A1616,DAC3484_REG_LENGTH);
	emif_epld_spi_write(0X2BAAAA,DAC3484_REG_LENGTH);
	emif_epld_spi_write(0X2CC6C6,DAC3484_REG_LENGTH);
	emif_epld_spi_write(0X2D0002,DAC3484_REG_LENGTH);
	emif_epld_spi_write(0X2E0000,DAC3484_REG_LENGTH);
	emif_epld_spi_write(0X2F0000,DAC3484_REG_LENGTH);
	emif_epld_spi_write(0X300E5C,DAC3484_REG_LENGTH);
	emif_epld_spi_write(0X7F0000,DAC3484_REG_LENGTH);      //version read only
	emif_epld_spi_write(0X1F8884,DAC3484_REG_LENGTH);      // soft sync
	emif_epld_spi_write(0X00f89b,DAC3484_REG_LENGTH);      // disable clock_divider_sync
	emif_epld_spi_write(0X1F8886,DAC3484_REG_LENGTH);      //clear soft sync
	dac1_txen(1);
	emif_epld_spi_channel(SELECT_DAC3481_2);
	dac2_txen(0);
	dac2_reset_n(0);
	spi_delay(1000);
	dac2_reset_n(1);
	emif_epld_spi_write(0X00f89f,DAC3484_REG_LENGTH);         //0XF29B qmc en=1,interp=8x
	emif_epld_spi_write(0X01050E,DAC3484_REG_LENGTH);
	emif_epld_spi_write(0X02F0F2,DAC3484_REG_LENGTH);         //word-wide data interface; 2's complement;4 spi;mixer en=1;mixer gain en=1;nco en=1;
	emif_epld_spi_write(0X03a000,DAC3484_REG_LENGTH);         //dac 电流强度 Ifs
	emif_epld_spi_write(0X04FFFF,DAC3484_REG_LENGTH);
	emif_epld_spi_write(0X050000,DAC3484_REG_LENGTH);         // write clear alarm registers
	emif_epld_spi_write(0X063300,DAC3484_REG_LENGTH);         // temerature read only
	emif_epld_spi_write(0X07FFFF,DAC3484_REG_LENGTH);         // mask of alarm
	emif_epld_spi_write(0X080000,DAC3484_REG_LENGTH);         // auto sync
	emif_epld_spi_write(0X098000,DAC3484_REG_LENGTH);         // fifo_offset when sync
	emif_epld_spi_write(0X0A0000,DAC3484_REG_LENGTH);
	emif_epld_spi_write(0X0B0000,DAC3484_REG_LENGTH);
	emif_epld_spi_write(0X0C07ff,DAC3484_REG_LENGTH);         /* 通道0的I路增益 */
	emif_epld_spi_write(0X0D07ff,DAC3484_REG_LENGTH);         /* 通道0的Q路增益 */ 
	emif_epld_spi_write(0X0E07ff,DAC3484_REG_LENGTH);         /* 通道1的I路增益 */ 
	emif_epld_spi_write(0X0F07ff,DAC3484_REG_LENGTH);         /* 通道1的Q路增益 */ 
	emif_epld_spi_write(0X100000,DAC3484_REG_LENGTH);         //qmc phase 
	emif_epld_spi_write(0X110000,DAC3484_REG_LENGTH);         //qmc phase
	emif_epld_spi_write(0X120000,DAC3484_REG_LENGTH);
	emif_epld_spi_write(0X130000,DAC3484_REG_LENGTH);

	emif_epld_spi_write(0X16aaaa,DAC3484_REG_LENGTH);         //nco frequence word [15:0]  58.6M*2^32/983.04M=0x0f42aaaa   
	emif_epld_spi_write(0X170f42,DAC3484_REG_LENGTH);         //nco frequence word [31:16]  广州移动测试
	emif_epld_spi_write(0X140000,DAC3484_REG_LENGTH);         //nco frequence word [15:0]  
	emif_epld_spi_write(0X151800,DAC3484_REG_LENGTH);         //nco frequence word [31:16]

	emif_epld_spi_write(0X18280F,DAC3484_REG_LENGTH);         //external clk
	emif_epld_spi_write(0X190440,DAC3484_REG_LENGTH);         // pll
	emif_epld_spi_write(0X1A0020,DAC3484_REG_LENGTH);         // pll sleep=1; DACa,b,c,d sleep =0

	//emif_epld_spi_write(0X182ce0,24);         //internal pll
	//emif_epld_spi_write(0X191004,24);         // pll
	//emif_epld_spi_write(0X1AEc00,24);         // pll sleep=1; DACa,b,c,d sleep =0

	emif_epld_spi_write(0X1B0000,DAC3484_REG_LENGTH);        //defaut
	emif_epld_spi_write(0X1C0000,DAC3484_REG_LENGTH);          //defaut
	emif_epld_spi_write(0X1D0000,DAC3484_REG_LENGTH);           //defaut
	emif_epld_spi_write(0X1E1111,DAC3484_REG_LENGTH);           //qmc sync source sel 
	emif_epld_spi_write(0X1F8884,DAC3484_REG_LENGTH);           // mixer,nco sync source sel ,sif_sync
	emif_epld_spi_write(0X201101,DAC3484_REG_LENGTH);           //fifo sync source sel =  sync
	emif_epld_spi_write(0X221B1B,DAC3484_REG_LENGTH);
	emif_epld_spi_write(0X23FFFF,DAC3484_REG_LENGTH);
	emif_epld_spi_write(0X240C00,DAC3484_REG_LENGTH);           // dataclk delay  3bit 011= 3*50 ps=150 ps 000 000 
	emif_epld_spi_write(0X257A7A,DAC3484_REG_LENGTH);
	emif_epld_spi_write(0X26B6B6,DAC3484_REG_LENGTH);
	emif_epld_spi_write(0X27EAEA,DAC3484_REG_LENGTH);
	emif_epld_spi_write(0X284545,DAC3484_REG_LENGTH);
	emif_epld_spi_write(0X291A1A,DAC3484_REG_LENGTH);
	emif_epld_spi_write(0X2A1616,DAC3484_REG_LENGTH);
	emif_epld_spi_write(0X2BAAAA,DAC3484_REG_LENGTH);
	emif_epld_spi_write(0X2CC6C6,DAC3484_REG_LENGTH);
	emif_epld_spi_write(0X2D0002,DAC3484_REG_LENGTH);
	emif_epld_spi_write(0X2E0000,DAC3484_REG_LENGTH);
	emif_epld_spi_write(0X2F0000,DAC3484_REG_LENGTH);
	emif_epld_spi_write(0X300E5C,DAC3484_REG_LENGTH);
	emif_epld_spi_write(0X7F0000,DAC3484_REG_LENGTH);      //version read only
	emif_epld_spi_write(0X1F8884,DAC3484_REG_LENGTH);      // soft sync
	emif_epld_spi_write(0X00f89b,DAC3484_REG_LENGTH);      // disable clock_divider_sync
	emif_epld_spi_write(0X1F8886,DAC3484_REG_LENGTH);      //clear soft sync
	dac2_txen(1);
	return 0;
}
