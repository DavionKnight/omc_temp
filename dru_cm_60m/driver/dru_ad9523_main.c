/*******************************************************************************
********************************************************************************
* 文件名称:  dru_ad9523.c
* 功能描述:  ad9523的配置操作函数
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
#include "../../driver/dru_ad9523.h"
/*******************************************************************************
* 函数名称: dru_ad9523_init
* 功    能: 初始化ad9523
* 参    数:
* 参数名称         类型                描述
* 返回值:
* 
* 说   明:
* 日   期     版本    作者   修改人      DEBUG
* -----------------------------------------------------------------
* 2012/06/18  V1.0     H4     无       ：
*2015/02/06	  v1.1    h4               h4               60m主单元修改。频率为737.28m
*******************************************************************************/
//#if DEV_TYPE == 0
int dru_ad9523_init_m(void)
{
emif_epld_spi_channel(SELECT_AD9523);
	usleep(1000);
	//spi_delay(1000);
	emif_epld_spi_write(0X000080,AD9523_REG_LENGTH);     // ????spi?? ?? MSB first
	emif_epld_spi_write(0X023401,AD9523_REG_LENGTH);     // ????????
	emif_epld_spi_write(0X000080,AD9523_REG_LENGTH);
	emif_epld_spi_write(0X000250,AD9523_REG_LENGTH);
	emif_epld_spi_write(0X000302,AD9523_REG_LENGTH);
	emif_epld_spi_write(0X000400,AD9523_REG_LENGTH);    
	emif_epld_spi_write(0X000500,AD9523_REG_LENGTH);
	emif_epld_spi_write(0X000600,AD9523_REG_LENGTH);
	
	emif_epld_spi_write(0X001024,AD9523_REG_LENGTH);     //REFA R divider 7-0
	emif_epld_spi_write(0X001100,AD9523_REG_LENGTH);     //REFA R divider 9-8  
	emif_epld_spi_write(0X001204,AD9523_REG_LENGTH);     //REFB R divider 7-0
	emif_epld_spi_write(0X001300,AD9523_REG_LENGTH);    // REFB R divider 9-8
	
	emif_epld_spi_write(0X001400,AD9523_REG_LENGTH);     
	emif_epld_spi_write(0X001500,AD9523_REG_LENGTH);     //reserved
	emif_epld_spi_write(0X001608,AD9523_REG_LENGTH);    //  N divider 7-0
	emif_epld_spi_write(0X001700,AD9523_REG_LENGTH);     // N divider 9-8
	emif_epld_spi_write(0X00187F,AD9523_REG_LENGTH);    // pll1 charge pump current max
	emif_epld_spi_write(0X001913,AD9523_REG_LENGTH);
	
	emif_epld_spi_write(0X001A78,AD9523_REG_LENGTH);    /*0x78 enable refa,refb,  0x60 disable refa,refb*/
	emif_epld_spi_write(0X001B14,AD9523_REG_LENGTH);
	
	emif_epld_spi_write(0X001C8c,AD9523_REG_LENGTH);   /*select refa,0x8c refb*/
	
	emif_epld_spi_write(0X001D04,AD9523_REG_LENGTH);
	emif_epld_spi_write(0X001E00,AD9523_REG_LENGTH);   /*loopfilter bandwith*/
	emif_epld_spi_write(0X001F00,AD9523_REG_LENGTH);
	emif_epld_spi_write(0X002000,AD9523_REG_LENGTH);
	
	emif_epld_spi_write(0X00F0FF,AD9523_REG_LENGTH);   
	emif_epld_spi_write(0X00F10f,AD9523_REG_LENGTH);    /* N divider 60*///61.44*4*15=3686.4
	emif_epld_spi_write(0X00F217,AD9523_REG_LENGTH);   /* PLL2 lock,charge pump, frequence doubler*/
	emif_epld_spi_write(0X00F303,AD9523_REG_LENGTH);  /* initial vco calibration*/
	emif_epld_spi_write(0X00F401,AD9523_REG_LENGTH);  /*vco divider 5*///3686.4/5=737.28
	
	emif_epld_spi_write(0X00F5D1,AD9523_REG_LENGTH);  /* LOOP filter*/
	emif_epld_spi_write(0X00F600,AD9523_REG_LENGTH);
	emif_epld_spi_write(0X00F700,AD9523_REG_LENGTH);
	emif_epld_spi_write(0X00F800,AD9523_REG_LENGTH);
	emif_epld_spi_write(0X00F900,AD9523_REG_LENGTH);
	emif_epld_spi_write(0X00FA00,AD9523_REG_LENGTH);
	
	emif_epld_spi_write(0X019082,AD9523_REG_LENGTH);   /*lvds 3.5mA*/
	emif_epld_spi_write(0X019105,AD9523_REG_LENGTH);  /* divider 7+1 122.88M*/
	emif_epld_spi_write(0X019204,AD9523_REG_LENGTH);
	
	emif_epld_spi_write(0X019382,AD9523_REG_LENGTH);  /*LVPECL 8mA*/
	emif_epld_spi_write(0X019405,AD9523_REG_LENGTH);
	emif_epld_spi_write(0X019504,AD9523_REG_LENGTH);
	
	emif_epld_spi_write(0X019682,AD9523_REG_LENGTH);
	emif_epld_spi_write(0X019705,AD9523_REG_LENGTH);
	emif_epld_spi_write(0X019804,AD9523_REG_LENGTH);
	
	emif_epld_spi_write(0X019982,AD9523_REG_LENGTH);
	emif_epld_spi_write(0X019A05,AD9523_REG_LENGTH);
	emif_epld_spi_write(0X019B04,AD9523_REG_LENGTH);
	
	emif_epld_spi_write(0X019Ca0,AD9523_REG_LENGTH);
	emif_epld_spi_write(0X019D05,AD9523_REG_LENGTH);
	emif_epld_spi_write(0X019E04,AD9523_REG_LENGTH);
	
	emif_epld_spi_write(0X019F81,AD9523_REG_LENGTH);
	emif_epld_spi_write(0X01A003,AD9523_REG_LENGTH);
	emif_epld_spi_write(0X01A104,AD9523_REG_LENGTH);
	
	emif_epld_spi_write(0X01A2a0,AD9523_REG_LENGTH);
	emif_epld_spi_write(0X01A305,AD9523_REG_LENGTH);
	emif_epld_spi_write(0X01A404,AD9523_REG_LENGTH);
	
	emif_epld_spi_write(0X01A581,AD9523_REG_LENGTH);
	emif_epld_spi_write(0X01A60b,AD9523_REG_LENGTH);
	emif_epld_spi_write(0X01A704,AD9523_REG_LENGTH);
	
	emif_epld_spi_write(0X01A801,AD9523_REG_LENGTH);
	emif_epld_spi_write(0X01A90b,AD9523_REG_LENGTH);   /*61.44M*/
	emif_epld_spi_write(0X01AA8C,AD9523_REG_LENGTH); 
	
	emif_epld_spi_write(0X01AB81,AD9523_REG_LENGTH); 
	emif_epld_spi_write(0X01AC0b,AD9523_REG_LENGTH); 
	emif_epld_spi_write(0X01AD28,AD9523_REG_LENGTH); 
	
	emif_epld_spi_write(0X01AE81,AD9523_REG_LENGTH); 
	emif_epld_spi_write(0X01AF0b,AD9523_REG_LENGTH);  /*61.44M*/
	emif_epld_spi_write(0X01B004,AD9523_REG_LENGTH);
	
	emif_epld_spi_write(0X01B182,AD9523_REG_LENGTH);
	emif_epld_spi_write(0X01B205,AD9523_REG_LENGTH);
	emif_epld_spi_write(0X01B304,AD9523_REG_LENGTH);
	
	emif_epld_spi_write(0X01B481,AD9523_REG_LENGTH);
	emif_epld_spi_write(0X01B500,AD9523_REG_LENGTH);//737.28M
	emif_epld_spi_write(0X01B604,AD9523_REG_LENGTH);
	
	emif_epld_spi_write(0X01B781,AD9523_REG_LENGTH);
	emif_epld_spi_write(0X01B800,AD9523_REG_LENGTH);//737.28M
	//emif_epld_spi_write(0X01B80f,AD9523_REG_LENGTH);//491.52M
	emif_epld_spi_write(0X01B904,AD9523_REG_LENGTH);
	
	emif_epld_spi_write(0X01BA10,AD9523_REG_LENGTH);
	emif_epld_spi_write(0X01BB80,AD9523_REG_LENGTH);
	emif_epld_spi_write(0X022E00,AD9523_REG_LENGTH);
	emif_epld_spi_write(0X022F00,AD9523_REG_LENGTH);
	emif_epld_spi_write(0X023002,AD9523_REG_LENGTH);
	emif_epld_spi_write(0X023103,AD9523_REG_LENGTH);
	emif_epld_spi_write(0X023401,AD9523_REG_LENGTH);
	emif_epld_spi_write(0X023302,AD9523_REG_LENGTH);
	emif_epld_spi_write(0X00F301,AD9523_REG_LENGTH);
	emif_epld_spi_write(0X023401,AD9523_REG_LENGTH);
	emif_epld_spi_write(0X023300,AD9523_REG_LENGTH);
	emif_epld_spi_write(0X023401,AD9523_REG_LENGTH);
	emif_epld_spi_write(0X00F303,AD9523_REG_LENGTH);
	emif_epld_spi_write(0X023401,AD9523_REG_LENGTH);
	emif_epld_spi_write(0X023201,AD9523_REG_LENGTH);
	emif_epld_spi_write(0X023401,AD9523_REG_LENGTH);
	emif_epld_spi_write(0X023200,AD9523_REG_LENGTH);
	emif_epld_spi_write(0X023401,AD9523_REG_LENGTH);
	//return 1;
	printf("the ad9523 for main\n");
	return 1;
}
//#endif
