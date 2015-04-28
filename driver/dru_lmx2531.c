/*******************************************************************************
********************************************************************************
* 文件名称:  dru_lmx2531.c
* 功能描述:  对lmx2531的配置操作。
*						
* 使用说明: 
* 文件作者:	H4
* 编写日期: （2012/06/18）
* 修改历史:
* 修改日期    修改人       修改内容
*-------------------------------------------------------------------------------

*******************************************************************************/
#include <sys/mman.h>
#include <string.h>
#include <time.h>
#include "dru_spi.h"
#include "dru_lmx2531.h"
#include "common.h"
#include "status.h"
/*******************************************************************************
* 函数名称: dru_lmx2531_init
* 功    能: 初始化lmx2531 
* 参    数:
* 参数名称         类型                描述
* 无
* 返回值:
* 成功返回1                           将来用于函数状态描述						
* 说   明:
* 日   期     版本    作者   修改人      DEBUG
* -----------------------------------------------------------------
* 2012/06/18  V1.0     H4     无       ：
*******************************************************************************/

int dru_lmx2531_init(unsigned int Freq)
{
	
	//unsigned int Flag,Mod,RfDriver,F_int,F_frac; 
	emif_epld_spi_channel(3);//(SELECT_LMX2531);
	usleep(1000);
	//spi_delay(100);
	/*
	if(Freq > 2200000)
	{
		RfDriver = 1;
		Flag = 0xffefffff;
		Mod = 384*4;
	}
	else
	{
		RfDriver = 2;
		Flag = 0xffffffff;
		Mod = 384;
	}
	
	F_int = Freq*RfDriver/30720;*/

	
	emif_epld_spi_write(0x5807F5,LMX2531_REG_LENGTH );
	usleep(100);    
//	spi_delay(100);              
	emif_epld_spi_write(0x000004,LMX2531_REG_LENGTH );
	emif_epld_spi_write(0x6CC003,LMX2531_REG_LENGTH );
	emif_epld_spi_write(0x580022,LMX2531_REG_LENGTH );
	emif_epld_spi_write(0x02c0001,LMX2531_REG_LENGTH );
	emif_epld_spi_write(0x0492020,LMX2531_REG_LENGTH );
	//emif_epld_spi_write(0x2C0001,LMX2531_REG_LENGTH );//

	return 1;
}
int dru_lmx2531_gsm_init(void)
{
	if(get_device_type()==DEVICE_TYPE_MAIN){
		dru_lmx2531_gsm_init_m();
	}else{
		dru_lmx2531_gsm_init_s();
	}
}
int dru_lmx2531_gsm_init_s()
//#if DEV_TYPE == 1
{
    unsigned int R6,R7,R8,R9,R12;
    R6 = 0x0280006;
    R7 = 0x0000307;
    R8 = 0x0028008;
    R9 = 0x0000ba9;
    R12= 0x001048c;

	emif_epld_spi_channel(1);
	usleep(10000);
//	spi_delay(10000);
	emif_epld_spi_write(0x840005,LMX2531_REG_LENGTH );
	emif_epld_spi_write(0x800005,LMX2531_REG_LENGTH );	
	emif_epld_spi_write(0x5807F5,LMX2531_REG_LENGTH );
	usleep(10000);    
	//spi_delay(10000);  
	
	emif_epld_spi_write(R12,LMX2531_REG_LENGTH );
	emif_epld_spi_write(R9,LMX2531_REG_LENGTH );
	emif_epld_spi_write(R8,LMX2531_REG_LENGTH );
	emif_epld_spi_write(R7,LMX2531_REG_LENGTH );
	emif_epld_spi_write(R6,LMX2531_REG_LENGTH );
	
	            
	emif_epld_spi_write(0x000004,LMX2531_REG_LENGTH );
	emif_epld_spi_write(0xecc003,LMX2531_REG_LENGTH );
	emif_epld_spi_write(0x430042,LMX2531_REG_LENGTH );
	emif_epld_spi_write(0x03e0001,LMX2531_REG_LENGTH );
	emif_epld_spi_write(0x06a0900,LMX2531_REG_LENGTH );
	//emif_epld_spi_write(0x2C0001,LMX2531_REG_LENGTH );//

	return 1;
}

int dru_lmx2531_gsm_init_m()
//#if DEV_TYPE == 0
{
    unsigned int R6,R7,R8,R9,R12;
    R6 = 0x0280006;
    R7 = 0x0000307;
    R8 = 0x0028008;
    R9 = 0x0000ba9;
    R12= 0x001048c;

	emif_epld_spi_channel(1);
	usleep(10000);
//	spi_delay(10000);
	emif_epld_spi_write(0x840005,LMX2531_REG_LENGTH );
	emif_epld_spi_write(0x800005,LMX2531_REG_LENGTH );	
	emif_epld_spi_write(0x5807F5,LMX2531_REG_LENGTH );
	usleep(10000);    
	//spi_delay(10000);  
	
	emif_epld_spi_write(R12,LMX2531_REG_LENGTH );
	emif_epld_spi_write(R9,LMX2531_REG_LENGTH );
	emif_epld_spi_write(R8,LMX2531_REG_LENGTH );
	emif_epld_spi_write(R7,LMX2531_REG_LENGTH );
	emif_epld_spi_write(R6,LMX2531_REG_LENGTH );
	
	            
	emif_epld_spi_write(0x000004,LMX2531_REG_LENGTH );
	emif_epld_spi_write(0xecc003,LMX2531_REG_LENGTH );
	emif_epld_spi_write(0x430042,LMX2531_REG_LENGTH );
	emif_epld_spi_write(0x03e0001,LMX2531_REG_LENGTH );
	emif_epld_spi_write(0x0680750,LMX2531_REG_LENGTH );// 803.4M
	//emif_epld_spi_write(0x06a0450,LMX2531_REG_LENGTH );
	//emif_epld_spi_write(0x2C0001,LMX2531_REG_LENGTH );//

	return 1;
}
//#endif

int dru_lmx2531_gsm1_init()
{
    unsigned int R6,R7,R8;
    R6 = 0x0080006;
    R7 = 0x0000107;
    R8 = 0x0028008;

	emif_epld_spi_channel(4);
	//usleep(1000);
	spi_delay(10000);
		
    emif_epld_spi_write(0x840005,LMX2531_REG_LENGTH );
	emif_epld_spi_write(0x800005,LMX2531_REG_LENGTH );	
	emif_epld_spi_write(0x8007F5,LMX2531_REG_LENGTH );
	//usleep(1000);    
	spi_delay(1000);  
	emif_epld_spi_write(R8,LMX2531_REG_LENGTH );
	emif_epld_spi_write(R7,LMX2531_REG_LENGTH );
	emif_epld_spi_write(R6,LMX2531_REG_LENGTH );
	
	            
	emif_epld_spi_write(0x000004,LMX2531_REG_LENGTH );
	emif_epld_spi_write(0xecc003,LMX2531_REG_LENGTH );
	emif_epld_spi_write(0x430042,LMX2531_REG_LENGTH );
	emif_epld_spi_write(0x03e0001,LMX2531_REG_LENGTH );
	emif_epld_spi_write(0x0690240,LMX2531_REG_LENGTH );
	//emif_epld_spi_write(0x2C0001,LMX2531_REG_LENGTH );//

	return 1;
}

int dru_lmx2531_wcdma_init()
{
	// for LMX2531_2050E, XTLSEL = manu; XDIV = 8; osc =61.44; 
    unsigned int R6,R7,R8,R9,R12;
    R6 = 0x0288056;
    R7 = 0x0000307;
    R8 = 0x0028008;
    R9 = 0x0000ba9;
    R12= 0x001048c;
	emif_epld_spi_channel(2);
	//usleep(1000);
	spi_delay(10000);
	emif_epld_spi_write(0x840005,LMX2531_REG_LENGTH );
	emif_epld_spi_write(0x800005,LMX2531_REG_LENGTH );	
	emif_epld_spi_write(0x5807F5,LMX2531_REG_LENGTH );//5807f5 8007f5
	//usleep(1000);    
	spi_delay(10000);   

	emif_epld_spi_write(R12,LMX2531_REG_LENGTH );
	emif_epld_spi_write(R9,LMX2531_REG_LENGTH );
	emif_epld_spi_write(R8,LMX2531_REG_LENGTH );
	emif_epld_spi_write(R7,LMX2531_REG_LENGTH );
	emif_epld_spi_write(R6,LMX2531_REG_LENGTH );
	          
	emif_epld_spi_write(0x0000004,LMX2531_REG_LENGTH );
	emif_epld_spi_write(0x06cc003,LMX2531_REG_LENGTH );
	emif_epld_spi_write(0x0580022,LMX2531_REG_LENGTH );
	emif_epld_spi_write(0x03e0001,LMX2531_REG_LENGTH );
	emif_epld_spi_write(0x03c25f0,LMX2531_REG_LENGTH );
	//emif_epld_spi_write(0x2C0001,LMX2531_REG_LENGTH );//

	return 1;
}



int dru_lmx2531_td_init()
{
	// for LMX2531_2050E, XTLSEL = manu; XDIV = 8; osc =61.44; 
    unsigned int R6,R7,R8,R9,R12;
    R6 = 0x0488006;
    R7 = 0x000d307;
    R8 = 0x0028018;
    R9 = 0x0000ba9;
    R12= 0x001048c;
	emif_epld_spi_channel(2);
	usleep(10000);
	//spi_delay(10000);
	emif_epld_spi_write(0x840005,LMX2531_REG_LENGTH );
	emif_epld_spi_write(0x800005,LMX2531_REG_LENGTH );	
	emif_epld_spi_write(0x5807F5,LMX2531_REG_LENGTH );
	usleep(10000);    
	//spi_delay(10000);   

	emif_epld_spi_write(R12,LMX2531_REG_LENGTH );
	emif_epld_spi_write(R9,LMX2531_REG_LENGTH );
	emif_epld_spi_write(R8,LMX2531_REG_LENGTH );
	emif_epld_spi_write(R7,LMX2531_REG_LENGTH );
	emif_epld_spi_write(R6,LMX2531_REG_LENGTH );
	          
	emif_epld_spi_write(0x0000004,LMX2531_REG_LENGTH );
	emif_epld_spi_write(0x06cc003,LMX2531_REG_LENGTH );
	emif_epld_spi_write(0x04c0022,LMX2531_REG_LENGTH );
	emif_epld_spi_write(0x02c0001,LMX2531_REG_LENGTH );
	emif_epld_spi_write(0x03e2120,LMX2531_REG_LENGTH );
	//emif_epld_spi_write(0x2C0001,LMX2531_REG_LENGTH );//

	return 1;
}



int dru_lmx2531_lte1_init()
{
	
    unsigned int R6,R7,R8,R9,R12;
    R6 = 0x0260006;
    R7 = 0x0000307;
    R8 = 0x0028008;
    R9 = 0x0000ba9;
    R12= 0x001048c;
	emif_epld_spi_channel(3);
	usleep(10000);
	//spi_delay(10000);
	emif_epld_spi_write(0x840005,LMX2531_REG_LENGTH );
	emif_epld_spi_write(0x800005,LMX2531_REG_LENGTH );
	emif_epld_spi_write(0x5807F5,LMX2531_REG_LENGTH );


	usleep(10000);    
//	spi_delay(10000);   
	emif_epld_spi_write(R12,LMX2531_REG_LENGTH );
	emif_epld_spi_write(R9,LMX2531_REG_LENGTH );
	emif_epld_spi_write(R8,LMX2531_REG_LENGTH );
	emif_epld_spi_write(R7,LMX2531_REG_LENGTH );
	emif_epld_spi_write(R6,LMX2531_REG_LENGTH );   
	        
	emif_epld_spi_write(0x000004,LMX2531_REG_LENGTH );
	emif_epld_spi_write(0x6cc003,LMX2531_REG_LENGTH );
	emif_epld_spi_write(0x580022,LMX2531_REG_LENGTH );
	emif_epld_spi_write(0x2c0001,LMX2531_REG_LENGTH );
	emif_epld_spi_write(0x492020,LMX2531_REG_LENGTH );
	//emif_epld_spi_write(0x2C0001,LMX2531_REG_LENGTH );//

	return 1;
}

int dru_lmx2531_wcdma_config(unsigned int freq)
{
	// for LMX2531_2050E, XTLSEL = manu; XDIV = 8; osc =61.44; 
    unsigned int R6,R7,R8,R9,R12,R0=0;
    unsigned short NUM=0;
    unsigned char N=0;
    R6 = 0x0288056;
    R7 = 0x0000307;
    R8 = 0x0028008;
    R9 = 0x0000ba9;
    R12= 0x001048c;
	emif_epld_spi_channel(2);
	//usleep(1000);
	spi_delay(10000);
	N= freq/30720;
	NUM=(freq-N*30720)/20;
	
	
	
	R0= (N <<16) |(NUM<<4);
	
	
	emif_epld_spi_write(0x840005,LMX2531_REG_LENGTH );
	emif_epld_spi_write(0x800005,LMX2531_REG_LENGTH );	
	emif_epld_spi_write(0x5807F5,LMX2531_REG_LENGTH );//5807f5 8007f5
	//usleep(1000);    
	spi_delay(10000);   

	emif_epld_spi_write(R12,LMX2531_REG_LENGTH );
	emif_epld_spi_write(R9,LMX2531_REG_LENGTH );
	emif_epld_spi_write(R8,LMX2531_REG_LENGTH );
	emif_epld_spi_write(R7,LMX2531_REG_LENGTH );
	emif_epld_spi_write(R6,LMX2531_REG_LENGTH );
	          
	emif_epld_spi_write(0x0000004,LMX2531_REG_LENGTH );
	emif_epld_spi_write(0x06cc003,LMX2531_REG_LENGTH );
	emif_epld_spi_write(0x0580022,LMX2531_REG_LENGTH );
	emif_epld_spi_write(0x03e0001,LMX2531_REG_LENGTH );
	emif_epld_spi_write(R0,LMX2531_REG_LENGTH );
	//emif_epld_spi_write(0x03c25f0,LMX2531_REG_LENGTH );
	//emif_epld_spi_write(0x2C0001,LMX2531_REG_LENGTH );//

	return 1;
}
int dru_lmx2531_wcdma_config_change(int argc, char * argv[])
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
	dru_lmx2531_wcdma_config((unsigned int)para1);
	sprintf(msg_tmp.mtext, "dru_lmx2531_wcdma_config=%d\r\n", para1);
	msg_send(TASK2_MODULE, (char *)&msg_tmp, strlen(msg_tmp.mtext));
	return 0;
}

int dru_lmx2531_lte1_config(unsigned int freq)
{
	
    unsigned int R6,R7,R8,R9,R12,R0=0;
    unsigned short NUM=0;
    unsigned char N=0;
    R6 = 0x0260006;
    R7 = 0x0000307;
    R8 = 0x0028008;
    R9 = 0x0000ba9;
    R12= 0x001048c;
	emif_epld_spi_channel(3);
	usleep(10000);
//	spi_delay(10000);
	N= freq/30720;
	NUM=(freq-N*30720)/40;
	
	
	
	R0= (N <<16) |(NUM<<4);

	
	
	
	emif_epld_spi_write(0x840005,LMX2531_REG_LENGTH );
	emif_epld_spi_write(0x800005,LMX2531_REG_LENGTH );
	emif_epld_spi_write(0x5807F5,LMX2531_REG_LENGTH );


	usleep(10000);    
	//spi_delay(10000);   
	emif_epld_spi_write(R12,LMX2531_REG_LENGTH );
	emif_epld_spi_write(R9,LMX2531_REG_LENGTH );
	emif_epld_spi_write(R8,LMX2531_REG_LENGTH );
	emif_epld_spi_write(R7,LMX2531_REG_LENGTH );
	emif_epld_spi_write(R6,LMX2531_REG_LENGTH );   
	        
	emif_epld_spi_write(0x000004,LMX2531_REG_LENGTH );
	emif_epld_spi_write(0x6cc003,LMX2531_REG_LENGTH );
	
	
	
	emif_epld_spi_write(0x4c0022,LMX2531_REG_LENGTH );
	emif_epld_spi_write(0x2c0001,LMX2531_REG_LENGTH );
	emif_epld_spi_write(R0,LMX2531_REG_LENGTH );
	//emif_epld_spi_write(0x492020,LMX2531_REG_LENGTH );
	//emif_epld_spi_write(0x2C0001,LMX2531_REG_LENGTH );//

	return 1;
}
int dru_lmx2531_lte1_config_change(int argc, char * argv[])
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
	dru_lmx2531_lte1_config((unsigned int)para1);
	sprintf(msg_tmp.mtext, "dru_lmx2531_lte1_config=%d\r\n", para1);
	msg_send(TASK2_MODULE, (char *)&msg_tmp, strlen(msg_tmp.mtext));
	return 0;
}

int dru_lmx2531_lte2_config(unsigned int freq)
{
	
    unsigned int R6,R7,R8,R9,R12,R0=0;
    unsigned short NUM=0;
    unsigned char N=0;
    R6 = 0x0260006;
    R7 = 0x0000307;
    R8 = 0x0028008;
    R9 = 0x0000ba9;
    R12= 0x001048c;
	emif_epld_spi_channel(4);
	usleep(1000);
	//spi_delay(10000);
	N= freq/30720;
	NUM=(freq-N*30720)/40;
	
	
	
	R0= (N <<16) |(NUM<<4);

	
	
	
	emif_epld_spi_write(0x840005,LMX2531_REG_LENGTH );
	emif_epld_spi_write(0x800005,LMX2531_REG_LENGTH );
	emif_epld_spi_write(0x5807F5,LMX2531_REG_LENGTH );


	usleep(1000);    
	//spi_delay(10000);   
	emif_epld_spi_write(R12,LMX2531_REG_LENGTH );
	emif_epld_spi_write(R9,LMX2531_REG_LENGTH );
	emif_epld_spi_write(R8,LMX2531_REG_LENGTH );
	emif_epld_spi_write(R7,LMX2531_REG_LENGTH );
	emif_epld_spi_write(R6,LMX2531_REG_LENGTH );   
	        
	emif_epld_spi_write(0x000004,LMX2531_REG_LENGTH );
	emif_epld_spi_write(0x6cc003,LMX2531_REG_LENGTH );
	
	
	
	emif_epld_spi_write(0x4c0022,LMX2531_REG_LENGTH );
	emif_epld_spi_write(0x2c0001,LMX2531_REG_LENGTH );
	emif_epld_spi_write(R0,LMX2531_REG_LENGTH );
	//emif_epld_spi_write(0x492020,LMX2531_REG_LENGTH );
	//emif_epld_spi_write(0x2C0001,LMX2531_REG_LENGTH );//

	return 1;
}
int dru_lmx2531_lte2_config_change(int argc, char * argv[])
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
	printf("para1=%d. \r\n", para1);
	dru_lmx2531_lte2_config((unsigned int)para1);
	sprintf(msg_tmp.mtext, "dru_lmx2531_lte2_config=%d\r\n", para1);
	msg_send(TASK2_MODULE, (char *)&msg_tmp, strlen(msg_tmp.mtext));
	return 0;
}

	
int dru_lmx2531_lte2_init()
{
    unsigned int R6,R7,R8,R9,R12;
    R6 = 0x0260006;
    R7 = 0x0000307;
    R8 = 0x0028008;
    R9 = 0x0000ba9;
    R12= 0x001048c;
	emif_epld_spi_channel(4);
	usleep(1000);
//	spi_delay(10000);
	emif_epld_spi_write(0x840005,LMX2531_REG_LENGTH );
	emif_epld_spi_write(0x800005,LMX2531_REG_LENGTH );
	emif_epld_spi_write(0x5807F5,LMX2531_REG_LENGTH );


	usleep(1000);    
//	spi_delay(10000);   
	emif_epld_spi_write(R12,LMX2531_REG_LENGTH );
	emif_epld_spi_write(R9,LMX2531_REG_LENGTH );
	emif_epld_spi_write(R8,LMX2531_REG_LENGTH );
	emif_epld_spi_write(R7,LMX2531_REG_LENGTH );
	emif_epld_spi_write(R6,LMX2531_REG_LENGTH );   
	        
	emif_epld_spi_write(0x000004,LMX2531_REG_LENGTH );
	emif_epld_spi_write(0x6cc003,LMX2531_REG_LENGTH );
	emif_epld_spi_write(0x580022,LMX2531_REG_LENGTH );
	emif_epld_spi_write(0x2c0001,LMX2531_REG_LENGTH );
	emif_epld_spi_write(0x492020,LMX2531_REG_LENGTH );
	//emif_epld_spi_write(0x2C0001,LMX2531_REG_LENGTH );//

	return 1;
}

int dru_lmx2531_fdd_lte1_init()
{
    unsigned int R6,R7,R8,R9,R12;
    R6 = 0x0288006;
    R7 = 0x0000307;
    R8 = 0x0028008;
    R9 = 0x0000ba9;
    R12= 0x001048c;
	emif_epld_spi_channel(3);
	//usleep(1000);
	spi_delay(10000);
	emif_epld_spi_write(0x840005,LMX2531_REG_LENGTH );
	emif_epld_spi_write(0x800005,LMX2531_REG_LENGTH );
	emif_epld_spi_write(0x5807F5,LMX2531_REG_LENGTH );


	//usleep(1000);    
	spi_delay(10000);   
	emif_epld_spi_write(R12,LMX2531_REG_LENGTH );
	emif_epld_spi_write(R9,LMX2531_REG_LENGTH );
	emif_epld_spi_write(R8,LMX2531_REG_LENGTH );
	emif_epld_spi_write(R7,LMX2531_REG_LENGTH );
	emif_epld_spi_write(R6,LMX2531_REG_LENGTH );   
	        
	emif_epld_spi_write(0x000004,LMX2531_REG_LENGTH );
	emif_epld_spi_write(0x6cc003,LMX2531_REG_LENGTH );
	emif_epld_spi_write(0x4c0022,LMX2531_REG_LENGTH );
	emif_epld_spi_write(0x3e0001,LMX2531_REG_LENGTH );
	emif_epld_spi_write(0x361da0,LMX2531_REG_LENGTH );
	//emif_epld_spi_write(0x2C0001,LMX2531_REG_LENGTH );//

	return 1;
}

int dru_lmx2531_fdd_lte1_config(unsigned int freq)
{
    unsigned int R6,R7,R8,R9,R12,R0=0;
    unsigned short NUM=0;
    unsigned char N=0;
    R6 = 0x0288006;
    R7 = 0x0000307;
    R8 = 0x0028008;
    R9 = 0x0000ba9;
    R12= 0x001048c;
	emif_epld_spi_channel(3);
	//usleep(1000);
	spi_delay(10000);
	N= freq/30720;
	NUM=(freq-N*30720)/40;
	
	
	
	R0= (N <<16) |(NUM<<4);
	
	emif_epld_spi_write(0x840005,LMX2531_REG_LENGTH );
	emif_epld_spi_write(0x800005,LMX2531_REG_LENGTH );
	emif_epld_spi_write(0x5807F5,LMX2531_REG_LENGTH );


	//usleep(1000);    
	spi_delay(10000);   
	emif_epld_spi_write(R12,LMX2531_REG_LENGTH );
	emif_epld_spi_write(R9,LMX2531_REG_LENGTH );
	emif_epld_spi_write(R8,LMX2531_REG_LENGTH );
	emif_epld_spi_write(R7,LMX2531_REG_LENGTH );
	emif_epld_spi_write(R6,LMX2531_REG_LENGTH );   
	        
	emif_epld_spi_write(0x000004,LMX2531_REG_LENGTH );
	emif_epld_spi_write(0x6cc003,LMX2531_REG_LENGTH );
	emif_epld_spi_write(0x4c0022,LMX2531_REG_LENGTH );
	emif_epld_spi_write(0x3e0001,LMX2531_REG_LENGTH );
	emif_epld_spi_write(R0,LMX2531_REG_LENGTH ); 
	//emif_epld_spi_write(0x361da0,LMX2531_REG_LENGTH );
	//emif_epld_spi_write(0x2C0001,LMX2531_REG_LENGTH );//

	return 1;
}

int dru_lmx2531_fdd_lte2_config(unsigned int freq)
{
    unsigned int R6,R7,R8,R9,R12,R0=0;
    unsigned short NUM=0;
    unsigned char N=0;
    R6 = 0x0288006;
    R7 = 0x0000307;
    R8 = 0x0028008;
    R9 = 0x0000ba9;
    R12= 0x001048c;
	emif_epld_spi_channel(4);
	//usleep(1000);
	spi_delay(10000);
	N= freq/30720;
	NUM=(freq-N*30720)/40;
	
	
	
	R0= (N <<16) |(NUM<<4);
	
	emif_epld_spi_write(0x840005,LMX2531_REG_LENGTH );
	emif_epld_spi_write(0x800005,LMX2531_REG_LENGTH );
	emif_epld_spi_write(0x5807F5,LMX2531_REG_LENGTH );


	//usleep(1000);    
	spi_delay(10000);   
	emif_epld_spi_write(R12,LMX2531_REG_LENGTH );
	emif_epld_spi_write(R9,LMX2531_REG_LENGTH );
	emif_epld_spi_write(R8,LMX2531_REG_LENGTH );
	emif_epld_spi_write(R7,LMX2531_REG_LENGTH );
	emif_epld_spi_write(R6,LMX2531_REG_LENGTH );   
	        
	emif_epld_spi_write(0x000004,LMX2531_REG_LENGTH );
	emif_epld_spi_write(0x6cc003,LMX2531_REG_LENGTH );
	emif_epld_spi_write(0x4c0022,LMX2531_REG_LENGTH );
	emif_epld_spi_write(0x3e0001,LMX2531_REG_LENGTH );
	emif_epld_spi_write(R0,LMX2531_REG_LENGTH ); 
	//emif_epld_spi_write(0x361da0,LMX2531_REG_LENGTH );
	//emif_epld_spi_write(0x2C0001,LMX2531_REG_LENGTH );//

	return 1;
}

int dru_lmx2531_fdd_lte2_init()
{
    unsigned int R6,R7,R8,R9,R12;
    R6 = 0x0288006;
    R7 = 0x0000307;
    R8 = 0x0028008;
    R9 = 0x0000ba9;
    R12= 0x001048c;
	emif_epld_spi_channel(4);
	//usleep(1000);
	spi_delay(10000);
	emif_epld_spi_write(0x840005,LMX2531_REG_LENGTH );
	emif_epld_spi_write(0x800005,LMX2531_REG_LENGTH );
	emif_epld_spi_write(0x5807F5,LMX2531_REG_LENGTH );


	//usleep(1000);    
	spi_delay(10000);   
	emif_epld_spi_write(R12,LMX2531_REG_LENGTH );
	emif_epld_spi_write(R9,LMX2531_REG_LENGTH );
	emif_epld_spi_write(R8,LMX2531_REG_LENGTH );
	emif_epld_spi_write(R7,LMX2531_REG_LENGTH );
	emif_epld_spi_write(R6,LMX2531_REG_LENGTH );   
	        
	emif_epld_spi_write(0x000004,LMX2531_REG_LENGTH );
	emif_epld_spi_write(0x6cc003,LMX2531_REG_LENGTH );
	emif_epld_spi_write(0x4c0022,LMX2531_REG_LENGTH );
	emif_epld_spi_write(0x3e0001,LMX2531_REG_LENGTH );
	emif_epld_spi_write(0x361da0,LMX2531_REG_LENGTH );
	//emif_epld_spi_write(0x2C0001,LMX2531_REG_LENGTH );//

	return 1;
}


//
//int dru_lmx2531_init(unsigned int Freq)
//{
//	
//	unsigned int Flag,Mod,RfDriver,F_int,F_frac; 
//	unsigned int R0,R1,R4; 
//	emif_epld_spi_channel(SELECT_LMX2531);
//	//usleep(1000);
//	spi_delay(100);
//	
//	if(Freq > 2200000)
//	{
//		RfDriver = 1;
//		Flag = 0xffefffff;
//		Mod = 384*4;
//	}
//	else
//	{
//		RfDriver = 2;
//		Flag = 0xffffffff;
//		Mod = 384;
//	}
//	
//	F_int = Freq*RfDriver/30720;
//	F_frac = (((Freq%(30720/RfDriver))*Mod*RfDriver/30720));
//	
//	R4 = 0x009f603c & Flag & 0xffffffe7 | 0x08;
//	R1 = 0x00008001 | (Mod << 3);
//	R0 = (F_frac << 3) | (F_int << 15);
//	
//	emif_epld_spi_write(0x5807f5,LMX2531_REG_LENGTH );
//	//usleep(1000);    
//	spi_delay(100);              
//	emif_epld_spi_write(R4,LMX2531_REG_LENGTH );
//	emif_epld_spi_write(0x000004b3,LMX2531_REG_LENGTH );
//	emif_epld_spi_write(0x00008ec2,LMX2531_REG_LENGTH );
//	emif_epld_spi_write(R1,LMX2531_REG_LENGTH );
//	emif_epld_spi_write(R0,LMX2531_REG_LENGTH );
//	//emif_epld_spi_write(0x2C0001,LMX2531_REG_LENGTH );//
//
//	return 1;
//}
int dru_lmx2531_power_down(unsigned char chip)//chip =1 LTE1,2=LTE2,3=3G,4=GSM
{
   if(chip == 1)
    {
	emif_epld_spi_channel(3);
	spi_delay(10000);	
	emif_epld_spi_write(0x840005,LMX2531_REG_LENGTH );
	emif_epld_spi_write(0x800005,LMX2531_REG_LENGTH );
	emif_epld_spi_write(0x580005,LMX2531_REG_LENGTH );
    }
    else if(chip == 2)
    {
	emif_epld_spi_channel(4);
	spi_delay(10000);	
	emif_epld_spi_write(0x840005,LMX2531_REG_LENGTH );
	emif_epld_spi_write(0x800005,LMX2531_REG_LENGTH );
	emif_epld_spi_write(0x580005,LMX2531_REG_LENGTH );
    }
    else if(chip == 3)
    {
	emif_epld_spi_channel(2);
	spi_delay(10000);	
	emif_epld_spi_write(0x840005,LMX2531_REG_LENGTH );
	emif_epld_spi_write(0x800005,LMX2531_REG_LENGTH );
	emif_epld_spi_write(0x580005,LMX2531_REG_LENGTH );
    }
    else if(chip == 4)
    {
	emif_epld_spi_channel(1);
	spi_delay(10000);	
	emif_epld_spi_write(0x840005,LMX2531_REG_LENGTH );
	emif_epld_spi_write(0x800005,LMX2531_REG_LENGTH );
	emif_epld_spi_write(0x580005,LMX2531_REG_LENGTH );
    }
    else;
	return 1;
}
int dru_lmx2531_power_down_change(int argc, char * argv[])
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
	printf("para1=%d. \r\n", para1);
	dru_lmx2531_power_down((unsigned char)para1);
	sprintf(msg_tmp.mtext, "dru_lmx2531_power_down=%d\r\n", para1);
	msg_send(TASK2_MODULE, (char *)&msg_tmp, strlen(msg_tmp.mtext));
	return 0;
}

int dru_lmx2531_power_up(unsigned char chip)//chip =1 LTE1,2=LTE2,3=3G,4=GSM
{
   if(chip == 1)
    {
	emif_epld_spi_channel(3);
	spi_delay(10000);	
	emif_epld_spi_write(0x840005,LMX2531_REG_LENGTH );
	emif_epld_spi_write(0x800005,LMX2531_REG_LENGTH );
	emif_epld_spi_write(0x5807f5,LMX2531_REG_LENGTH );
    }
    else if(chip == 2)
    {
	emif_epld_spi_channel(4);
	spi_delay(10000);	
	emif_epld_spi_write(0x840005,LMX2531_REG_LENGTH );
	emif_epld_spi_write(0x800005,LMX2531_REG_LENGTH );
	emif_epld_spi_write(0x5807f5,LMX2531_REG_LENGTH );
    }
    else if(chip == 3)
    {
	emif_epld_spi_channel(2);
	spi_delay(10000);	
	emif_epld_spi_write(0x840005,LMX2531_REG_LENGTH );
	emif_epld_spi_write(0x800005,LMX2531_REG_LENGTH );
	emif_epld_spi_write(0x5807f5,LMX2531_REG_LENGTH );
    }
    else if(chip == 4)
    {
	emif_epld_spi_channel(1);
	spi_delay(10000);	
	emif_epld_spi_write(0x840005,LMX2531_REG_LENGTH );
	emif_epld_spi_write(0x800005,LMX2531_REG_LENGTH );
	emif_epld_spi_write(0x5807f5,LMX2531_REG_LENGTH );
    }
    else;
	return 1;
}
int dru_lmx2531_power_up_change(int argc, char * argv[])
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
	printf("para1=%d. \r\n", para1);
	dru_lmx2531_power_up((unsigned char)para1);
	sprintf(msg_tmp.mtext, "dru_lmx2531_power_up=%d\r\n", para1);
	msg_send(TASK2_MODULE, (char *)&msg_tmp, strlen(msg_tmp.mtext));
	return 0;
}
int dru_lmx2531_gsm_init_m_gz(void)
{
    unsigned int R6,R7,R8,R9,R12;
    R6 = 0x0288006;
    R7 = 0x0000307;
    R8 = 0x0028008;
    R9 = 0x0000ba9;
    R12= 0x001048c;

	emif_epld_spi_channel(1);
	usleep(10000);
	emif_epld_spi_write(0x840005,LMX2531_REG_LENGTH );
	emif_epld_spi_write(0x800005,LMX2531_REG_LENGTH );	
	emif_epld_spi_write(0x5807F5,LMX2531_REG_LENGTH );
	usleep(10000);    
	
	emif_epld_spi_write(R12,LMX2531_REG_LENGTH );
	emif_epld_spi_write(R9,LMX2531_REG_LENGTH );
	emif_epld_spi_write(R8,LMX2531_REG_LENGTH );
	emif_epld_spi_write(R7,LMX2531_REG_LENGTH );
	emif_epld_spi_write(R6,LMX2531_REG_LENGTH );
	
	emif_epld_spi_write(0x000004,LMX2531_REG_LENGTH );
	emif_epld_spi_write(0xec2623,LMX2531_REG_LENGTH );
	emif_epld_spi_write(0x568022,LMX2531_REG_LENGTH );
	emif_epld_spi_write(0x2c1551,LMX2531_REG_LENGTH );
	emif_epld_spi_write(0x37bbc0,LMX2531_REG_LENGTH );// 853.4M 联通GSM

	return 1;
}
extern int dru_dac3484_init_m_gz(void);
extern int dru_dac3484_init_m(void);
int gz_test_change(int argc, char * argv[])
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
	printf("para1=%d. \r\n", para1);
	if(para1 == 0){
		dru_dac3484_init_m();
		dru_lmx2531_gsm_init_m();
	}else{
		dru_dac3484_init_m_gz();
		dru_lmx2531_gsm_init_m_gz();
	}
	sprintf(msg_tmp.mtext, "test ok.\n");
	msg_send(TASK2_MODULE, (char *)&msg_tmp, strlen(msg_tmp.mtext));

	return 0;
}
