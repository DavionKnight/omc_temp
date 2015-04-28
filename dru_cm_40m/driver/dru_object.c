#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <sys/mman.h>
#include <string.h>
#include <time.h>
#include "../../driver/dru_object.h"
#include "../../driver/dru_spi.h"
#include "../../driver/dru_mmap.h"
#include "../../driver/omap_pinmux.h"
#include "../../driver/dru_fpga_download.h"
#include "../../driver/dru_ksz8873.h"
#include "../../driver/omap_gpio.h"
#include "../../driver/dru_reg.h"
#include "../../driver/omap_fpga.h"
#include "../../driver/omap_epld.h"
#include "../../driver/dru_iic.h"
#include "../../driver/dru_lmh6517.h"
#include "../../driver/dru_fpga_ram.h"
#include "../../driver/dru_lmx2531.h"
#include "../../driver/dru_ads58c48.h"
#include "../../driver/dru_dac3484.h"
#include "../../driver/dru_ad9523.h"
#include "common.h"
#include "status.h"

//const char * g_ver = "CM-DCS-0004.0";
const char * g_type_ver = "MG/D"; 
const char * g_drv_ver = "05";
const char * g_test_ver = "02"; // 0:正式版 奇数：待测试版 偶数：已测试版本
extern const char * g_comm_ver;
extern const char * g_comm_test_ver;
void dru_init(void)
{
	unsigned int i=0,data;
	unsigned char gain_test,en_test,lo;
	unsigned short temp_test,epld_test;

	power_sw_init();
	power_sw_open();
	
#ifdef CONFIG_KSZ8873
	dru_ksz8873_init();
 	printf("configure ksz8873\n");
#endif

#ifdef CONFIG_AD9523
	if(get_device_type()==DEVICE_TYPE_MAIN)
	{
		dru_ad9523_init_m();
	}
	else
	{
		dru_ad9523_init_s();
	}
	//dru_ad9523_init();
	spi_delay(SPI_CONFIG_DELAY);
	printf("configure ad9523\n");
#endif

#ifdef DOWNLOAD_FPGA
	if(get_device_type()==DEVICE_TYPE_MAIN)
	{
		dru_fpga_download(FPGA_FILE_LENGTH,"/ramDisk/fpga_main.rbf");//1357101
	}
	else if(get_device_type()==DEVICE_TYPE_RAU)
	{
		dru_fpga_download(FPGA_FILE_LENGTH,"/ramDisk/fpga_rrs.rbf");//1357101
	}
	else if(get_device_type()==DEVICE_TYPE_EXPEND)
	{
		dru_fpga_download(FPGA_FILE_LENGTH,"/ramDisk/fpga_exp.rbf");//1357101
	}
	printf("configure fpga\n");
#endif

#ifdef CONFIG_LMX2531
	if(get_net_group() == 0x331)	 //电信三网
	{
		dru_lmx2531_cdma_init_m();	
		dru_lmx2531_fdd_lte1_config(1711400);
		dru_lmx2531_fdd_lte2_config(1711400);
	}
	else
	{
		if(get_device_type()==DEVICE_TYPE_MAIN)	//main
		{
			dru_lmx2531_wcdma_config(1925340);
		}
		else
		{
			dru_lmx2531_wcdma_config(1925340); //RAU
		}

		if(get_net_type() == 1)				// 1:gsm 2:dcs
		{ 
			dru_lmx2531_gsm_init_m();
		}
		else
		{
			dru_lmx2531_dcs_init();
		}

		if(get_device_type()==DEVICE_TYPE_MAIN)
		{
			dru_lmx2531_lte1_config(2252840);// main
			dru_lmx2531_lte2_config(2252840);// main
		}
		else
		{
			dru_lmx2531_lte1_config(2252840); // RAU
			dru_lmx2531_lte2_config(2252840);
		}
	}

	printf("%x\n",epld_test);
	emif_epld_spi_channel(10);
	emif_epld_spi_channel(10);

	emif_fpga_ddc_carrier_f8_write(0,25);
	emif_fpga_ddc_carrier_f8_write(1,50);
	emif_fpga_ddc_carrier_f8_write(2,75);
	emif_fpga_ddc_carrier_f8_write(3,100);
	emif_fpga_ddc_carrier_f8_write(4,125);
	emif_fpga_ddc_carrier_f8_write(5,150);
	emif_fpga_ddc_carrier_f8_write(6,175);
	emif_fpga_ddc_carrier_f8_write(7,200);
	emif_fpga_ddc_carrier_m8_write(0,768-200);
	emif_fpga_ddc_carrier_m8_write(1,768-175);
	emif_fpga_ddc_carrier_m8_write(2,768-150);
	emif_fpga_ddc_carrier_m8_write(3,768-125);
	emif_fpga_ddc_carrier_m8_write(4,768-100);
	emif_fpga_ddc_carrier_m8_write(5,768-75);
	emif_fpga_ddc_carrier_m8_write(6,768-50);
	emif_fpga_ddc_carrier_m8_write(7,768-25);
	//
	emif_fpga_duc_carrier_f8_write(0,25);
	emif_fpga_duc_carrier_f8_write(1,50);
	emif_fpga_duc_carrier_f8_write(2,75);
	emif_fpga_duc_carrier_f8_write(3,100);
	emif_fpga_duc_carrier_f8_write(4,125);
	emif_fpga_duc_carrier_f8_write(5,150);
	emif_fpga_duc_carrier_f8_write(6,175);
	emif_fpga_duc_carrier_f8_write(7,200);
	emif_fpga_duc_carrier_m8_write(0,768-200);
	emif_fpga_duc_carrier_m8_write(1,768-175);
	emif_fpga_duc_carrier_m8_write(2,768-150);
	emif_fpga_duc_carrier_m8_write(3,768-125);
	emif_fpga_duc_carrier_m8_write(4,768-100);
	emif_fpga_duc_carrier_m8_write(5,768-75);
	emif_fpga_duc_carrier_m8_write(6,768-50);
	emif_fpga_duc_carrier_m8_write(7,768-25);

	//*DRU_REGADDR(fpga_base_addr, ADC_SOURCE_SEL)= 1;//enable internal dds for adc_source
	//*DRU_REGADDR(fpga_base_addr, LOOP_BACK_SEL)= 1; //enable loop_back_sel
	*DRU_REGADDR(fpga_base_addr, MASK_CARRIER)= 0xfffe; 
	*DRU_REGADDR(fpga_base_addr, DUC_RST)= 0; 
	*DRU_REGADDR(fpga_base_addr, DUC_RST)= 1;       // disable duc rst
	spi_delay(10000);
	*DRU_REGADDR(fpga_base_addr, DUC_RST)= 0; 
	*DRU_REGADDR(fpga_base_addr, LTE1_ATT)= 13;
	*DRU_REGADDR(fpga_base_addr, LTE2_ATT)= 13; 
	*DRU_REGADDR(fpga_base_addr, WCDMA_ATT)= 13; 
	*DRU_REGADDR(fpga_base_addr, GSM_ATT)= 13; 


	*DRU_REGADDR(fpga_base_addr, CPU_AGC_EN)= 0; 
	*DRU_REGADDR(fpga_base_addr, CPU_AGC_EN)= 1; 
	spi_delay(1000);
	*DRU_REGADDR(fpga_base_addr, CPU_AGC_EN)= 0;  
#endif

#ifdef CONFIG_ADS58C48
	dru_ads58c48_init();
	printf("configure ads58c48\n");
#endif

	dru_dac3484_init();
	*DRU_REGADDR(fpga_base_addr, DAC_SYNC)=0;
	*DRU_REGADDR(fpga_base_addr, DAC_SYNC)=1;
	printf("configure dac3484\n");

	*DRU_REGADDR(fpga_base_addr, LOOP_BACK_SEL)=1;
	*DRU_REGADDR(fpga_base_addr, AGC_STEP)=1;

	//dac3484_td_qmc_phase_cd(10);
	*DRU_REGADDR(fpga_base_addr, DAC_SYNC)=0;
	*DRU_REGADDR(fpga_base_addr, DAC_SYNC)=1;	

	*DRU_REGADDR(fpga_base_addr, LTE1_GAIN)=8192;
	*DRU_REGADDR(fpga_base_addr, LTE2_GAIN)= 8192;
	*DRU_REGADDR(fpga_base_addr, TD_GAIN)= 8192;
	*DRU_REGADDR(fpga_base_addr, GSM_GAIN)= 8192;
	*DRU_REGADDR(fpga_base_addr, ADC_SOURCE_FREQENCY)= 192+10;


	*DRU_REGADDR(fpga_base_addr, ADC_SOURCE_SEL)= 0;

	*DRU_REGADDR(fpga_base_addr, LOOP_BACK_SEL)= 1; //enable loop_back_sel



	lo=*DRU_REGADDR(epld_base_addr, 0x12);
	if((lo&(1<<0))!=0)
		printf("本振1锁定\n");
	else	
		printf("本振1失锁\n");
	if((lo&(1<<1))!=0)
		printf("本振2锁定\n");
	else	
		printf("本振2失锁\n");
	if((lo&(1<<2))!=0)
		printf("本振3锁定\n");
	else	
		printf("本振3失锁\n");
	if((lo&(1<<3))!=0)
		printf("本振4锁定\n");
	else	
		printf("本振4失锁\n");
}
void init_3484_ex(void)
{
	*DRU_REGADDR(fpga_base_addr, DAC_SYNC)=0;
	*DRU_REGADDR(fpga_base_addr, DAC_SYNC)=1;
	*DRU_REGADDR(fpga_base_addr, DAC_SYNC)=0;
}
//int main(void)
//void  hardware_init(void )

// 加载射频驱动
void * dru_obj_main(void *arg)
{
	
	if(dru_get_val()==-1)
	{
		exit(-1);
	}
	dru_init();
	init_3484_ex();	//dac初始化
//	while(1)
//	{
		//i++;
//		gpio_low();
		//spi_delay(SPI_CONFIG_DELAY);
	//	usleep(10000);
	//dru_iic_8873_write_byte(0xaa);
//	dru_iic_8873_ack();
	/*DRU_ICC_8873_SDA_OUT_HIGH();
	iic_delay(IIC_DELAY_TIME);
	DRU_ICC_8873_SCL_OUT_HIGH();
	iic_delay(IIC_DELAY_TIME);
	DRU_ICC_8873_SDA_OUT_LOW();
	iic_delay(IIC_DELAY_TIME);
	DRU_ICC_8873_SCL_OUT_LOW();
	iic_delay(IIC_DELAY_TIME);*/
//	gpio_high();
	//sleep(1);
	//spi_delay(SPI_CONFIG_DELAY);
	// usleep(10000);
		//data=REGADDR(emifacs3_mmap_val,SPI_CLK_ADDR);
		//printf("spi clk is %x\n",data); 
		//spi_write(0xaaaa,16);
		//printf("wtrie 0xaaaa to spi %d\n",i);
	//}//
//	dru_rm_val();

	return ((void *)0);
}

int dru_init_change(int argc, char * argv[])
{
	msg_tmp.mtype = MSG_FUN_RESULT;	
	if(argc != 1){
		printf("input para cnt is not 1.\r\n");
		sprintf(msg_tmp.mtext, "input para cnt is not 1.\r\n");
		msg_send(TASK2_MODULE, (char *)&msg_tmp, strlen(msg_tmp.mtext));
		return 1;
	}
	dru_init();
	sprintf(msg_tmp.mtext, "dru_init is ok.\r\n");
	msg_send(TASK2_MODULE, (char *)&msg_tmp, strlen(msg_tmp.mtext));
	return 0;
}
int set_lo_change(int argc, char * argv[])
{
	msg_tmp.mtype = MSG_FUN_RESULT;	
	if(argc != 1){
		printf("input para cnt is not 1.\r\n");
		sprintf(msg_tmp.mtext, "input para cnt is not 1.\r\n");
		msg_send(TASK2_MODULE, (char *)&msg_tmp, strlen(msg_tmp.mtext));
		return 1;
	}
	dru_lmx2531_wcdma_config(0);// main
	sprintf(msg_tmp.mtext, "dru_init is ok.\r\n");
	msg_send(TASK2_MODULE, (char *)&msg_tmp, strlen(msg_tmp.mtext));
	return 0;
}
int dru_get_version_change(int argc, char * argv[])
{
	char type[16];

	memset(type, 0, 16);
	if(get_device_type()==DEVICE_TYPE_MAIN){
		memcpy(type, "MU", 2);
	}else if(get_device_type()==DEVICE_TYPE_EXPEND){
		memcpy(type, "EU", 2);
	}else if(get_device_type()==DEVICE_TYPE_RAU){
		memcpy(type, "RU", 2);
	}else{
		memcpy(type, "NULL", 2);
	}
	msg_tmp.mtype = MSG_FUN_RESULT;	
	if(argc != 1){
		printf("input para cnt is not 1.\r\n");
		sprintf(msg_tmp.mtext, "input para cnt is not 1.\r\n");
		msg_send(TASK2_MODULE, (char *)&msg_tmp, strlen(msg_tmp.mtext));
		return 1;
	}
	sprintf(msg_tmp.mtext, "%s-%s-%s.%s.%s%s\r\n", type, g_type_ver, g_comm_ver, g_drv_ver, g_comm_test_ver, g_test_ver);
	msg_send(TASK2_MODULE, (char *)&msg_tmp, strlen(msg_tmp.mtext));
	return 0;
}
int download_fpga(void)
{
	if(get_device_type()==DEVICE_TYPE_MAIN){
		dru_fpga_download(FPGA_FILE_LENGTH,"/ramDisk/fpga_main.rbf");//1357101
	}else if(get_device_type()==DEVICE_TYPE_RAU){
		dru_fpga_download(FPGA_FILE_LENGTH,"/ramDisk/fpga_rrs.rbf");//1357101
	}else if(get_device_type()==DEVICE_TYPE_EXPEND){
		dru_fpga_download(FPGA_FILE_LENGTH,"/ramDisk/fpga_exp.rbf");//1357101
	}
}
int download_fpga_change(int argc, char * argv[])
{
	msg_tmp.mtype = MSG_FUN_RESULT;	
	if(argc != 1){
		printf("input para cnt is not 1.\r\n");
		sprintf(msg_tmp.mtext, "input para cnt is not 1.\r\n");
		msg_send(TASK2_MODULE, (char *)&msg_tmp, strlen(msg_tmp.mtext));
		return 1;
	}
	download_fpga();
	sprintf(msg_tmp.mtext, "download fpga.\r\n");
	msg_send(TASK2_MODULE, (char *)&msg_tmp, strlen(msg_tmp.mtext));
	return 0;
}
