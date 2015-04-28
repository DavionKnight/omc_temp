/*************************************************************************
***********************************************************************
** FILE NAME: hdalsrm.c
** AUTHOR:h4
** MAIL: harry.zhu.h4@gmail.com
** TIME: 2013å¹´03æœˆ04æ—¥ æ˜ŸæœŸä¸€ 10æ—¶50åˆ†50ç§’
************************************************************************
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
#include <pthread.h>
#include "../../common/status.h"
#include "../../common/type_def.h"
#include "../../net/dru_connect.h"
#include "../../net/socketcommon.h"
#include "../../driver/drv_api.h"
#include "../../driver/dru_psw.h"
#include "../../sqlite/drudatabase.h"
#include "../log/thread_log.h"
#include "hdalarm.h"
/***********************************************************************
** Funcation Name : create_hardware_alarm_thread
** Author         : H4
** Description    : 
** Input          : 
** Output         : 
** date           : 2013å¹´03æœˆ05æ—¥ æ˜ŸæœŸäºŒ 10æ—¶02åˆ†55ç§’
** Version        : V1.0
** Modify         : 
***********************************************************************/
int create_hardware_alarm_thread(void)
{
	pthread_t pid;
	create_irq_thread();
    if(pthread_create(&pid, NULL,hardware_alarm_thread,NULL)){
		DEBUG_PRINTF("pthread_creat hardware_alarm_thread error.\r\n");
		return 1;
	}
	DEBUG_PRINTF("\nHARDWARE_ALARM_THREAD RUN\r\n");
	return 0;
}
/***********************************************************************
** Funcation Name : hardware_alarm_thread
** Author         : H4
** Description    : ç¡¬ä»¶å‘Šè­¦å¤„ç†çº¿ç¨‹
** Input          : 
** Output         : 
** date           : 2013å¹´03æœˆ04æ—¥ æ˜ŸæœŸä¸€ 11æ—¶24åˆ†10ç§’
** Version        : V1.0
** Modify         : 
***********************************************************************/
extern volatile unsigned char connectflag;	
void *hardware_alarm_thread(void *pvoid)
{
	drv_write_fpga(0x0143,0xffff);
	while(1){
		sleep(3);
		if(connectflag==CLIENT_CONNECTED){
			light_alarm();
		}
		DEBUG_PRINTF("CONFIG FLAG %d \r\n",connectflag);
	}
}
/***********************************************************************
** Funcation Name : light_alarm
** Author         : H4
** Description    :	å…‰æ¨¡å—åŠå…‰é“¾è·¯å‘Šè­¦ 
** Input          : 
** Output         : 
** date           : 2013å¹´03æœˆ04æ—¥ æ˜ŸæœŸä¸€ 11æ—¶28åˆ†45ç§’
** Version        : V1.0
** Modify         : 
***********************************************************************/
void light_alarm(void)
{
	U16 i;
	U16 sig_alarm,syn_alarm,sig_en,syn_en,sig_clr,syn_clr;
	U16 syn;
	U8 alarm_tmp[8];
	drv_read_fpga(0x146,&sig_alarm);	
	drv_read_fpga(0x100,&syn);
	DEBUG_PRINTF("THE SIG_ALARM IS %x syn is %x\r\n",sig_alarm,syn);
	for(i=0;i<8;i++){
		if((syn&(1<<i))!=0){
			DEBUG_PRINTF("THE RECONSTRUCT PORT IS %d\r\n",i);
			reconstruct_signal_deal(i);
		}
	}
	for(i=0;i<8;i++){
		if((sig_alarm&(1<<i*2))!=0){
			lost_signal_deal(i);
			drv_write_fpga(0x149,1<<i*2);
			drv_write_fpga(0x149,0);
			DEBUG_PRINTF("THE %d LITHT PORT LOST SIGNAL \r\n",i);
		}
		if((sig_alarm&(1<<(i*2+1)))!=0){
			reconstruct_signal_deal(i);
			drv_write_fpga(0x149,1<<(i*2+1));
			drv_write_fpga(0x149,0);
			DEBUG_PRINTF("THE %d LITHT PORT RECONSTRUCT SIGNAL \r\n",i);
		}
	}
}
/*******************************************************************************
* º¯ÊıÃû³Æ: power_lost_deal
* ¹¦    ÄÜ:µçÔ´µôµç´¦Àí
* ²Î    Êı:
* ²ÎÊıÃû³Æ         ÀàĞÍ                ÃèÊö
* ·µ»ØÖµ:
* 
* Ëµ   Ã÷:
* ÈÕ   ÆÚ     °æ±¾    ×÷Õß   ĞŞ¸ÄÈË      DEBUG
* -----------------------------------------------------------------------------
* 2013/08/08  V1.0     H4     ÎŞ       £º
*******************************************************************************/
void power_lost_deal(void)
{
	int i;
	DbGetThisIntPara(0x0301,&i);
	i|=0x01;
	DbSaveThisIntPara(0x0301,i);
	power_sw_close();
}
/*******************************************************************************
* º¯ÊıÃû³Æ: power_get_deal
* ¹¦    ÄÜ:µçÔ´µôµç´¦Àí
* ²Î    Êı:
* ²ÎÊıÃû³Æ         ÀàĞÍ                ÃèÊö
* ·µ»ØÖµ:
* 
* Ëµ   Ã÷:
* ÈÕ   ÆÚ     °æ±¾    ×÷Õß   ĞŞ¸ÄÈË      DEBUG
* -----------------------------------------------------------------------------
* 2013/08/08  V1.0     H4     ÎŞ       £º
*******************************************************************************/
void power_get_deal(void)
{
	int i;
	DbGetThisIntPara(0x0301,&i);
	i&=0xfe;
	DbSaveThisIntPara(0x0301,i);
	drv_write_epld(0x24,0x55);//reboot the system
}
/*******************************************************************************
* º¯ÊıÃû³Æ: power_error_deal
* ¹¦    ÄÜ:µçÔ´µôµç´¦Àí
* ²Î    Êı:
* ²ÎÊıÃû³Æ         ÀàĞÍ                ÃèÊö
* ·µ»ØÖµ:
* 
* Ëµ   Ã÷:
* ÈÕ   ÆÚ     °æ±¾    ×÷Õß   ĞŞ¸ÄÈË      DEBUG
* -----------------------------------------------------------------------------
* 2013/08/08  V1.0     H4     ÎŞ       £º
*******************************************************************************/
void power_error_deal(void)
{
	int i;
	DbGetThisIntPara(0x0302,&i);
	i|=0x01;
	DbSaveThisIntPara(0x0302,i);
	power_sw_close();
}
/*******************************************************************************
* º¯ÊıÃû³Æ: power_lost_deal
* ¹¦    ÄÜ:µçÔ´µôµç´¦Àí
* ²Î    Êı:
* ²ÎÊıÃû³Æ         ÀàĞÍ                ÃèÊö
* ·µ»ØÖµ:
* 
* Ëµ   Ã÷:
* ÈÕ   ÆÚ     °æ±¾    ×÷Õß   ĞŞ¸ÄÈË      DEBUG
* -----------------------------------------------------------------------------
* 2013/08/08  V1.0     H4     ÎŞ       £º
*******************************************************************************/
void power_reconstruct_deal(void)
{
	int i;
	DbGetThisIntPara(0x0302,&i);
	i&=0xfe;
	DbSaveThisIntPara(0x0302,i);
	drv_write_epld(0x24,0x55);//reboot the system
}
/*******************************************************************************
* º¯ÊıÃû³Æ: battery_error_deal
* ¹¦    ÄÜ:µçÔ´µôµç´¦Àí
* ²Î    Êı:
* ²ÎÊıÃû³Æ         ÀàĞÍ                ÃèÊö
* ·µ»ØÖµ:
* 
* Ëµ   Ã÷:
* ÈÕ   ÆÚ     °æ±¾    ×÷Õß   ĞŞ¸ÄÈË      DEBUG
* -----------------------------------------------------------------------------
* 2013/08/08  V1.0     H4     ÎŞ       £º
*******************************************************************************/
void battery_error_deal(void)
{
	int i;
	DbGetThisIntPara(0x0304,&i);
	i|=0x01;
	DbSaveThisIntPara(0x0304,i);
}
/*******************************************************************************
* º¯ÊıÃû³Æ: battery_reconstruct_deal
* ¹¦    ÄÜ:µçÔ´µôµç´¦Àí
* ²Î    Êı:
* ²ÎÊıÃû³Æ         ÀàĞÍ                ÃèÊö
* ·µ»ØÖµ:
* 
* Ëµ   Ã÷:
* ÈÕ   ÆÚ     °æ±¾    ×÷Õß   ĞŞ¸ÄÈË      DEBUG
* -----------------------------------------------------------------------------
* 2013/08/08  V1.0     H4     ÎŞ       £º
*******************************************************************************/
void battery_reconstruct_deal(void)
{
	int i;
	DbGetThisIntPara(0x0304,&i);
	i&=0xfe;
	DbSaveThisIntPara(0x0304,i);
}
/*******************************************************************************
* º¯ÊıÃû³Æ: site_change_deal
* ¹¦    ÄÜ:µçÔ´µôµç´¦Àí
* ²Î    Êı:
* ²ÎÊıÃû³Æ         ÀàĞÍ                ÃèÊö
* ·µ»ØÖµ:
* 
* Ëµ   Ã÷:
* ÈÕ   ÆÚ     °æ±¾    ×÷Õß   ĞŞ¸ÄÈË      DEBUG
* -----------------------------------------------------------------------------
* 2013/08/08  V1.0     H4     ÎŞ       £º
*******************************************************************************/
void site_change_deal(void)
{
	int i;
	DbGetThisIntPara(0x0305,&i);
	i|=0x01;
	DbSaveThisIntPara(0x0305,i);
}
/*******************************************************************************
* º¯ÊıÃû³Æ: site_back_deal
* ¹¦    ÄÜ:µçÔ´µôµç´¦Àí
* ²Î    Êı:
* ²ÎÊıÃû³Æ         ÀàĞÍ                ÃèÊö
* ·µ»ØÖµ:
* 
* Ëµ   Ã÷:
* ÈÕ   ÆÚ     °æ±¾    ×÷Õß   ĞŞ¸ÄÈË      DEBUG
* -----------------------------------------------------------------------------
* 2013/08/08  V1.0     H4     ÎŞ       £º
*******************************************************************************/
void site_back_deal(void)
{
	int i;
	DbGetThisIntPara(0x0305,&i);
	i&=0xfe;
	DbSaveThisIntPara(0x0305,i);
}
/***********************************************************************
** Funcation Name : lost_signal_deal
** Author         : H4
** Description    : å¤„ç†ä¸¢å¤±ä¿¡å·å‘Šè­¦ 
** Input          :	å‘Šè­¦çš„å…‰å£å·0-7 
** Output         : 
** date           : 2013å¹´03æœˆ05æ—¥ æ˜ŸæœŸäºŒ 09æ—¶34åˆ†02ç§’
** Version        : V1.0
** Modify         : 
***********************************************************************/
void lost_signal_deal(U8 index)
{
	int i;
	DbGetThisIntPara(0x03A8+index,&i);
	i|=0x01;
	DbSaveThisIntPara(0x03A8+index,i);
	
}
/***********************************************************************
** Funcation Name : reconstruct_signal_deal
** Author         : H4
** Description    : ä¿¡å·å›å¤å‘Šè­¦å¤„ç† 
** Input          :	å‘Šè­¦çš„å…‰å£0-7 
** Output         : 
** date           : 2013å¹´03æœˆ06æ—¥ æ˜ŸæœŸä¸‰ 10æ—¶21åˆ†44ç§’
** Version        : V1.0
** Modify         : 
***********************************************************************/
void reconstruct_signal_deal(U8 index)
{
	int i;
	DbGetThisIntPara(0x03A8+index,&i);
	i&=0xFE;
	DbSaveThisIntPara(0x03A8+index,i);
}
/***********************************************************************
** Funcation Name : alarm_irq_thread
** Author         : H4
** Description    :  
** Input          : 
** Output         : 
** date           : 2013å¹´05æœˆ29æ—¥ æ˜ŸæœŸä¸‰ 10æ—¶38åˆ†02ç§’
** Version        : V1.0
** Modify         : 
***********************************************************************/
void *alarm_irq_thread(void *args)
{
	int ret;
	int fd;
	unsigned int arg;
	fd=open("/dev/gpio_int",O_RDONLY);
	if(fd==-1){
		LOG("ERROR:init gpio irq error\r\n!");	
	}
	DEBUG_PRINTF("IRQ THREAD IS RUN \r\n");
	while(1){
		ret=read(fd,(void *)&arg,4);
		if(ret<0){
			LOG("ERROR:read gpio irq error\r\n!");	
			close(fd);
			return ;
		}
		if((arg&0x02)==0x02){//cpld irq
			DEBUG_PRINTF("IRQ:cpld irq\r\n");
		}
		if((arg&0x01)==0x01){//fpga irq
			LOG("IRQ:fpga irq\r\n");
		}
		//waiting for gaoyuan
	}
} 
/***********************************************************************
** Funcation Name : create_irq_thread
** Author         : H4
** Description    : 
** Input          : 
** Output         : 
** date           : 2013å¹´03æœˆ05æ—¥ æ˜ŸæœŸäºŒ 10æ—¶02åˆ†55ç§’
** Version        : V1.0
** Modify         : 
***********************************************************************/
int create_irq_thread(void)
{
	pthread_t pid;
    if(pthread_create(&pid,NULL,alarm_irq_thread,NULL)){
		return 1;
	}
	return 0;
}
