/*******************************************************************************
********************************************************************************
* ?ļ?????:  dru_ad9523.c
* ????????:  ad9523?????ò???????
* ʹ??˵??:  
* ?ļ?????:	H4
* ??д????: ??2012/06/18??
* ?޸???ʷ:
* ?޸?????    ?޸???       ?޸?????
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
#include "dru_spi.h"
#include "dru_ad9523.h"
/*******************************************************************************
* ????????: dru_ad9523_init
* ??    ??: ??ʼ??ad9523
* ??    ??:
* ????????         ????                ????
* ????ֵ:
* 
* ˵   ??:
* ??   ??     ?汾    ????   ?޸???      DEBUG
* -----------------------------------------------------------------
* 2012/06/18  V1.0     H4     ??       ??
*******************************************************************************/
//#if DEV_TYPE == 1
int dru_ad9523_init_s(void)
{
	printf("dru rau.\r\n");
	emif_epld_spi_channel(SELECT_AD9523);
	usleep(1000);
	//spi_delay(100);
emif_epld_spi_write(0X000080,AD9523_REG_LENGTH);     // ?1?7?1?7?1?7?1?7spi?1?7?1?7 ?0?0?0?4 MSB first
	emif_epld_spi_write(0X023401,AD9523_REG_LENGTH);     // ?1?7?1?7?1?7?0?4?0?6?1?7?1?7?1?7
	emif_epld_spi_write(0X000080,AD9523_REG_LENGTH);
	emif_epld_spi_write(0X000250,AD9523_REG_LENGTH);
	emif_epld_spi_write(0X000302,AD9523_REG_LENGTH);
	emif_epld_spi_write(0X000400,AD9523_REG_LENGTH);    
	emif_epld_spi_write(0X000500,AD9523_REG_LENGTH);
	emif_epld_spi_write(0X000600,AD9523_REG_LENGTH);
	emif_epld_spi_write(0X001020,AD9523_REG_LENGTH);     //REFA R divider 7-0
	emif_epld_spi_write(0X001100,AD9523_REG_LENGTH);     //REFA R divider 9-8  
	emif_epld_spi_write(0X001208,AD9523_REG_LENGTH);     //REFB R divider 7-0
	emif_epld_spi_write(0X001300,AD9523_REG_LENGTH);    // REFB R divider 9-8
	emif_epld_spi_write(0X001400,AD9523_REG_LENGTH);     
	emif_epld_spi_write(0X001500,AD9523_REG_LENGTH);     //reserved
	emif_epld_spi_write(0X001608,AD9523_REG_LENGTH);    // REFB N divider 7-0
	emif_epld_spi_write(0X001700,AD9523_REG_LENGTH);     //REFB N divider 9-8
	emif_epld_spi_write(0X00187F,AD9523_REG_LENGTH);    // pll1 charge pump current max
	emif_epld_spi_write(0X001913,AD9523_REG_LENGTH);
	emif_epld_spi_write(0X001A78,AD9523_REG_LENGTH);    /*0x78 enable refa,refb,  0x60 disable refa,refb*/
	emif_epld_spi_write(0X001B14,AD9523_REG_LENGTH);
	emif_epld_spi_write(0X001C88,AD9523_REG_LENGTH);   /*select refa,0x8c refb*/
	emif_epld_spi_write(0X001D04,AD9523_REG_LENGTH);
	emif_epld_spi_write(0X001E00,AD9523_REG_LENGTH);   /*loopfilter bandwith*/
	emif_epld_spi_write(0X001F00,AD9523_REG_LENGTH);
	emif_epld_spi_write(0X002000,AD9523_REG_LENGTH);
	emif_epld_spi_write(0X00F0FF,AD9523_REG_LENGTH);   
	emif_epld_spi_write(0X00F110,AD9523_REG_LENGTH);    /* N divider 64 */
	emif_epld_spi_write(0X00F217,AD9523_REG_LENGTH);   /* PLL2 lock,charge pump, frequence doubler*/
	emif_epld_spi_write(0X00F303,AD9523_REG_LENGTH);  /* initial vco calibration*/
	emif_epld_spi_write(0X00F400,AD9523_REG_LENGTH);  /*vco divider 4*/
	emif_epld_spi_write(0X00F5D1,AD9523_REG_LENGTH);  /* LOOP filter*/
	emif_epld_spi_write(0X00F600,AD9523_REG_LENGTH);
	emif_epld_spi_write(0X00F700,AD9523_REG_LENGTH);
	emif_epld_spi_write(0X00F800,AD9523_REG_LENGTH);
	emif_epld_spi_write(0X00F900,AD9523_REG_LENGTH);
	emif_epld_spi_write(0X00FA00,AD9523_REG_LENGTH);
	
	emif_epld_spi_write(0X019082,AD9523_REG_LENGTH);   /*lvds 3.5mA*/
	emif_epld_spi_write(0X019107,AD9523_REG_LENGTH);  /* divider 7+1 122.88M*/	
	emif_epld_spi_write(0X019204,AD9523_REG_LENGTH);
	
	emif_epld_spi_write(0X019382,AD9523_REG_LENGTH);  /*LVPECL 8mA*/
	emif_epld_spi_write(0X019407,AD9523_REG_LENGTH);
	emif_epld_spi_write(0X019504,AD9523_REG_LENGTH);
	
	emif_epld_spi_write(0X019682,AD9523_REG_LENGTH);
	emif_epld_spi_write(0X019707,AD9523_REG_LENGTH);
	emif_epld_spi_write(0X019804,AD9523_REG_LENGTH);
	
	emif_epld_spi_write(0X019982,AD9523_REG_LENGTH);//3
	emif_epld_spi_write(0X019A07,AD9523_REG_LENGTH);
	emif_epld_spi_write(0X019B04,AD9523_REG_LENGTH);
	
	emif_epld_spi_write(0X019Ca0,AD9523_REG_LENGTH);//4
	emif_epld_spi_write(0X019D07,AD9523_REG_LENGTH);
	emif_epld_spi_write(0X019E04,AD9523_REG_LENGTH);
	
	emif_epld_spi_write(0X019F81,AD9523_REG_LENGTH);//5
	emif_epld_spi_write(0X01A007,AD9523_REG_LENGTH);
	emif_epld_spi_write(0X01A104,AD9523_REG_LENGTH);
	
	emif_epld_spi_write(0X01A2a0,AD9523_REG_LENGTH);//6
	emif_epld_spi_write(0X01A307,AD9523_REG_LENGTH);
	emif_epld_spi_write(0X01A404,AD9523_REG_LENGTH);
	
	emif_epld_spi_write(0X01A581,AD9523_REG_LENGTH);//7
	emif_epld_spi_write(0X01A60F,AD9523_REG_LENGTH);
	emif_epld_spi_write(0X01A704,AD9523_REG_LENGTH);
	
	emif_epld_spi_write(0X01A881,AD9523_REG_LENGTH);//8
	emif_epld_spi_write(0X01A90F,AD9523_REG_LENGTH);   /*61.44M*/
	emif_epld_spi_write(0X01AA8C,AD9523_REG_LENGTH); 
	
	emif_epld_spi_write(0X01AB81,AD9523_REG_LENGTH); //9
	emif_epld_spi_write(0X01AC0F,AD9523_REG_LENGTH); 
	emif_epld_spi_write(0X01AD28,AD9523_REG_LENGTH); 
	
	emif_epld_spi_write(0X01AE81,AD9523_REG_LENGTH); //10
	emif_epld_spi_write(0X01AF0F,AD9523_REG_LENGTH);  /*61.44M*/
	emif_epld_spi_write(0X01B004,AD9523_REG_LENGTH);
	
	emif_epld_spi_write(0X01B182,AD9523_REG_LENGTH);//11
	emif_epld_spi_write(0X01B207,AD9523_REG_LENGTH);
	emif_epld_spi_write(0X01B304,AD9523_REG_LENGTH);
	
	emif_epld_spi_write(0X01B481,AD9523_REG_LENGTH);//12
	emif_epld_spi_write(0X01B500,AD9523_REG_LENGTH);//491.52M
	emif_epld_spi_write(0X01B604,AD9523_REG_LENGTH);
	
	emif_epld_spi_write(0X01B781,AD9523_REG_LENGTH);//13
	emif_epld_spi_write(0X01B800,AD9523_REG_LENGTH);//491.52M
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
	 printf("the ad9523 for rau\n");
	return 1;
}
//#endif

