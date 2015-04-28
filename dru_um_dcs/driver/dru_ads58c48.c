/*******************************************************************************
********************************************************************************
* 文件名称:  dru_ads58c48.c
* 功能描述:  ads58c48的配置操作函数
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
#include "../../driver/dru_ads58c48.h"
/*******************************************************************************
* 函数名称: dru_ads58c48_init
* 功    能: ads58c48的配置
* 参    数:
* 参数名称         类型                描述
*	无
* 返回值:
* 无
* 说   明:
* 日   期     版本    作者   修改人      DEBUG
* -----------------------------------------------------------------
* 2012/06/18  V1.0     H4     无       ：
*******************************************************************************/
int dru_ads58c48_init(void)
{
	//spi_channel(SELECT_ADS58C48);
	//usleep(1000);
	emif_epld_adc_spi_write(0X0002,ADS58C48_REG_LENGTH);
	usleep(500);
	emif_epld_adc_spi_write(0X0000,ADS58C48_REG_LENGTH);
	usleep(1000);
	emif_epld_adc_spi_write(0X2900,ADS58C48_REG_LENGTH);   // 2s complement channel AB
	emif_epld_adc_spi_write(0X3500,ADS58C48_REG_LENGTH);   // 2s complement channel CD
	emif_epld_adc_spi_write(0XED08,ADS58C48_REG_LENGTH);
	emif_epld_adc_spi_write(0X4400,ADS58C48_REG_LENGTH);   // DIGITAL MODE2 EN=0
	emif_epld_adc_spi_write(0X4200,ADS58C48_REG_LENGTH);   // 可调整接口输出时钟和数据的相位关系
	emif_epld_adc_spi_write(0X2D1e,ADS58C48_REG_LENGTH);   // CHA SNRBoost FILter  0x28 = 40M 带宽  0x1E = 20M 带宽
	emif_epld_adc_spi_write(0X261e,ADS58C48_REG_LENGTH);   // CHB SNRBoost FILter
	emif_epld_adc_spi_write(0X321e,ADS58C48_REG_LENGTH);   // CHC SNRBoost FILter
	emif_epld_adc_spi_write(0X391e,ADS58C48_REG_LENGTH);   // CHD SNRBoost FILter
	emif_epld_adc_spi_write(0X3D00,ADS58C48_REG_LENGTH);   // OFFSET CORR 使能 =3d20
	emif_epld_adc_spi_write(0X4401,ADS58C48_REG_LENGTH);   // DIGITAL MODE2 EN=1
	emif_epld_adc_spi_write(0XEA80,ADS58C48_REG_LENGTH);   // OVERRID SNRB PINS =1 去掉SNRB 管脚的控制
	emif_epld_adc_spi_write(0X2E40,ADS58C48_REG_LENGTH);   // CHA SNRBOOST ON=1     
	emif_epld_adc_spi_write(0X2840,ADS58C48_REG_LENGTH);   // CHB SNRBOOST ON=1     
	emif_epld_adc_spi_write(0X3440,ADS58C48_REG_LENGTH);   // CHC SNRBOOST ON=1     
	emif_epld_adc_spi_write(0X3A40,ADS58C48_REG_LENGTH);   // CHD SNRBOOST ON=1
	return 1;
}

void dru_ads58c48_clkout_delay_0p5_ns()
{
	
emif_epld_adc_spi_write(0X4258,ADS58C48_REG_LENGTH);	
	
}
void dru_ads58c48_clkout_delay_1_ns()
{
	
emif_epld_adc_spi_write(0X42f8,ADS58C48_REG_LENGTH);	
	
}


void dru_ads58c48_test_on()
{
	
emif_epld_adc_spi_write(0X4208,ADS58C48_REG_LENGTH);	

emif_epld_adc_spi_write(0X2504,ADS58C48_REG_LENGTH);
emif_epld_adc_spi_write(0X2b04,ADS58C48_REG_LENGTH);
emif_epld_adc_spi_write(0X3104,ADS58C48_REG_LENGTH);
emif_epld_adc_spi_write(0X3704,ADS58C48_REG_LENGTH);	
	
}
void dru_ads58c48_test_off()
{
	
emif_epld_adc_spi_write(0X4200,ADS58C48_REG_LENGTH);	
	
	
}
