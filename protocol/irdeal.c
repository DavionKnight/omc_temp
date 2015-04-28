/*************************************************************************
	> File Name: irdeal.c
	> Author:h4 
	> Mail:h4@sina.cn 
	> Created Time: 2012年12月14日 星期五 14时57分20秒
 ************************************************************************/
#include <stdlib.h>
#include <stdio.h>
#include <errno.h>
#include <string.h>
#include <unistd.h>
#include <netdb.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <sys/types.h>
#include <arpa/inet.h>
#include "ir_def.h"
#include "../common/status.h"
#include "../common/type_def.h"
#include "../net/dru_connect.h"
#include "../net/socketcommon.h"
#include "../driver/drv_api.h"
#include "ir.h"
#include "irdeal.h"
#include "apue.h"
#include "common.h"

U32 reboot_count=0;
extern rru_series dru_rru_series;
extern volatile unsigned char connectflag;

/*
 * 读取硬件信息
 * 2012 12 17
 * h4
*/
int get_port_info(void)
{
	U64 data;
	U64 d1,d2,d3;
	U16 data_h,data_l,data_2;
	U16 i,j;
	U8 flag,flag_1,flag_2;
	while(1){
	//	drv_read_fpga(FPGA_SYNC_ADDR,&data_l);
	//	DEBUG_PRINTF("SYNC REG IS %x \n",data_l);
		//j++;
		drv_read_fpga(FPGA_PORT_ID_L_0_ADDR,&data_l);
		drv_read_fpga(FPGA_PORT_ID_H_0_ADDR,&data_h);
		drv_read_fpga(FPGA_PORT_ID_2_0_ADDR,&data_2);
		d1=data_l;
		d2=data_h;
		d3=data_2;
		data=(d3<<32)|(d2<<16)|d1;
		DEBUG_PRINTF("PORT 0 NUMBER %llx \n",data);
		dru_rru_series.port_number[0]=data;
		drv_read_fpga(FPGA_PORT_ID_L_1_ADDR,&data_l);
		drv_read_fpga(FPGA_PORT_ID_H_1_ADDR,&data_h);
		drv_read_fpga(FPGA_PORT_ID_2_1_ADDR,&data_2);
		d1=data_l;
		d2=data_h;
		d3=data_2;
		data=(d3<<32)|(d2<<16)|d1;
		//data=(U64)(data_2<<32)|(data_h<<16)|data_l;
		DEBUG_PRINTF("PORT 1 NUMBER %llx \n",data);
		dru_rru_series.port_number[1]=data;
		drv_read_fpga(FPGA_PORT_ID_L_2_ADDR,&data_l);
		drv_read_fpga(FPGA_PORT_ID_H_2_ADDR,&data_h);
		drv_read_fpga(FPGA_PORT_ID_2_2_ADDR,&data_2);
		d1=data_l;
		d2=data_h;
		d3=data_2;
		data=(d3<<32)|(d2<<16)|d1;
		//data=(U64)(data_2<<32)|(data_h<<16)|data_l;
		DEBUG_PRINTF("PORT 2 NUMBER %llx \n",data);
		dru_rru_series.port_number[2]=data;
		drv_read_fpga(FPGA_PORT_ID_L_3_ADDR,&data_l);
		drv_read_fpga(FPGA_PORT_ID_H_3_ADDR,&data_h);
		drv_read_fpga(FPGA_PORT_ID_2_3_ADDR,&data_2);
		d1=data_l;
		d2=data_h;
		d3=data_2;
		data=(d3<<32)|(d2<<16)|d1;
		//data=(U64)(data_2<<32)|(data_h<<16)|data_l;
		DEBUG_PRINTF("PORT 3 NUMBER %llx \n",data);
		dru_rru_series.port_number[3]=data;
		drv_read_fpga(FPGA_PORT_ID_L_4_ADDR,&data_l);
		drv_read_fpga(FPGA_PORT_ID_H_4_ADDR,&data_h);
		drv_read_fpga(FPGA_PORT_ID_2_4_ADDR,&data_2);
		d1=data_l;
		d2=data_h;
		d3=data_2;
		data=(d3<<32)|(d2<<16)|d1;
		//data=(U64)(data_2<<32)|(data_h<<16)|data_l;
		DEBUG_PRINTF("PORT 4 NUMBER %llx \n",data);
		dru_rru_series.port_number[4]=data;
		drv_read_fpga(FPGA_PORT_ID_L_5_ADDR,&data_l);
		drv_read_fpga(FPGA_PORT_ID_H_5_ADDR,&data_h);
		drv_read_fpga(FPGA_PORT_ID_2_5_ADDR,&data_2);
		d1=data_l;
		d2=data_h;
		d3=data_2;
		data=(d3<<32)|(d2<<16)|d1;
		//data=(U64)(data_2<<32)|(data_h<<16)|data_l;
		DEBUG_PRINTF("PORT 5 NUMBER %llx \n",data);
		dru_rru_series.port_number[5]=data;
		drv_read_fpga(FPGA_PORT_ID_L_6_ADDR,&data_l);
		drv_read_fpga(FPGA_PORT_ID_H_6_ADDR,&data_h);
		drv_read_fpga(FPGA_PORT_ID_2_6_ADDR,&data_2);
		d1=data_l;
		d2=data_h;
		d3=data_2;
		data=(d3<<32)|(d2<<16)|d1;
		//data=(U64)(data_2<<32)|(data_h<<16)|data_l;
		DEBUG_PRINTF("PORT 6 NUMBER %llx \n",data);
		dru_rru_series.port_number[6]=data;
		drv_read_fpga(FPGA_PORT_ID_L_7_ADDR,&data_l);
		drv_read_fpga(FPGA_PORT_ID_H_7_ADDR,&data_h);
		drv_read_fpga(FPGA_PORT_ID_2_7_ADDR,&data_2);
		d1=data_l;
		d2=data_h;
		d3=data_2;
		data=(d3<<32)|(d2<<16)|d1;
		//data=(U64)(data_2<<32)|(data_h<<16)|data_l;
		DEBUG_PRINTF("PORT 7 NUMBER %llx \n",data);
		dru_rru_series.port_number[7]=data;
		dru_rru_series.series=get_device_type();
		flag=0;
		flag_1=0;
		flag_2=0;
		for(i=0;i<8;i++){
			if(dru_rru_series.port_number[i]>0xf00000000000){
				flag++;
				if(flag==1){
					flag_1=i;
				}else{
					flag_2=i;
				}
			}
		}
		if(flag==0){
			sleep(1);
		}else if(flag==1){
			drv_write_fpga(FPGA_OPT_SEL_ADDR ,(1<<flag_1));
			if(get_device_type()==DEVICE_TYPE_EXPEND){
				drv_write_fpga(FPGA_IQ_EN_ADDR ,(1<<flag_1));
			}
			DEBUG_PRINTF("SET PORT %d WITH UP LINK\n",flag_1);
			sleep(1);
			drv_read_fpga(FPGA_SYNC_ADDR,&data_l);
			if((data_l&(1<<flag_1))!=0){
				DEBUG_PRINTF("PORT %d SYNC OK \n",flag_1);
				break;
			}else{
				DEBUG_PRINTF("PORT SYNC REG %x \n",data_l);
				sleep(1);
			}
		}else{
			if((j%2)==0){
				drv_write_fpga(FPGA_OPT_SEL_ADDR ,(1<<flag_1));
				if(get_device_type()==DEVICE_TYPE_EXPEND){
					drv_write_fpga(FPGA_IQ_EN_ADDR ,(1<<flag_1));
				}				
				DEBUG_PRINTF("SET PORT %d WITH UP LINK\n",flag_1);
				sleep(1);
				drv_read_fpga(FPGA_SYNC_ADDR,&data_l);
				if((data_l&(1<<flag_1))!=0){
					DEBUG_PRINTF("PORT %d SYNC OK \n",flag_1);
					break;
				}else{
					DEBUG_PRINTF("PORT SYNC REG %x \n",data_l);
					sleep(1);
				}
			}else{
				drv_write_fpga(FPGA_OPT_SEL_ADDR ,(1<<flag_2));
				if(get_device_type()==DEVICE_TYPE_EXPEND){
					drv_write_fpga(FPGA_IQ_EN_ADDR ,(1<<flag_2));
				}							
				DEBUG_PRINTF("SET PORT %d WITH UP LINK\n",flag_2);
				sleep(1);
				drv_read_fpga(FPGA_SYNC_ADDR,&data_l);
				if((data_l&(1<<flag_2))!=0){
					DEBUG_PRINTF("PORT %d SYNC OK \n",flag_2);
					break;
				}else{
					DEBUG_PRINTF("PORT SYNC REG %x \n",data_l);
					sleep(1);
				}
			}
		}
		j++;
	}
}
// 获取本机序列号
unsigned long long get_dev_port_num(void)
{
	int i = 0;
	unsigned short val1, val2, val3;
	unsigned long long tmp;
	drv_read_fpga(FPGA_PORT_ID_L_0_ADDR,&val1);
	drv_read_fpga(FPGA_PORT_ID_H_0_ADDR,&val2);
	drv_read_fpga(FPGA_PORT_ID_2_0_ADDR,&val3);
	tmp = 0;
	tmp |= val3;
	tmp <<= 16;
	tmp |= val2;
	tmp <<= 16;
	tmp |= val1;
	return tmp;
}
/*
 * 硬件初始化
 * 2012 12 17
 * h4
*/
void hardware_init(void)
{
	if(get_device_type()!=DEVICE_TYPE_MAIN){
		get_port_info();	
	}
}
/*******************************************************************************
* ????????: reboot_timing
* ??    ??:??ʱ???����??⵽?嵽ʮ????ʧȥ��?????帴λ
*			
*			
* ??    ??:
* ????????         ????                ????
* ????ֵ:
* 
* ˵   ??:
* ??   ??     ?汾    ????   ?޸???      DEBUG
* -----------------------------------------------------------------------------
* 2013/3/29  V1.0     H4     ??       ??
*******************************************************************************/
int reboot_flag = 1;
void reboot_timing(void)
{
	if(get_device_type()!=DEVICE_TYPE_MAIN){
		if(connectflag==CLIENT_CONNECTED){
			reboot_count=0;
		}else{
			reboot_count++;
			if((reboot_count%1000)==0){
				DEBUG_PRINTF("\r\n");
				DEBUG_PRINTF("REBOOT COUNT %d\r\n",reboot_count);
				DEBUG_PRINTF("\r\n");
			}
			if(reboot_count>MAX_REBOOT_TIME){
				DEBUG_PRINTF("\r\n");
				DEBUG_PRINTF("\r\n");
				DEBUG_PRINTF("\r\n");
				DEBUG_PRINTF("SYSTEM IS REBOOT\r\n");
				sleep(1);
				if(reboot_flag == 0x0){
					drv_write_epld(CPLD_REBOOT_ADDR,CPLD_REBOOT_DATA);
				}
			}
		}
	}
}
int set_reboot_change(int argc, char * argv[])
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
	reboot_flag = para1;
	sprintf(msg_tmp.mtext, "reboot_flag=%d.\r\n", reboot_flag);
	msg_send(TASK2_MODULE, (char *)&msg_tmp, strlen(msg_tmp.mtext));
	return 0;
}
