/********************  COPYRIGHT(C)***************************************
**--------------文件信息--------------------------------------------------------
**文   件   名: modem.h
**创   建   人: 于宏图
**创 建  日 期: 
**程序开发环境：
**描        述: Modem Gprs数据通讯程序的头文件
**--------------历史版本信息----------------------------------------------------
** 创建人: 于宏图
** 版  本: v1.0
** 日　期: 
** 描　述: 原始版本
**--------------当前版本修订----------------------------------------------------
** 修改人:
** 版  本:
** 日　期:
** 描　述:
**------------------------------------------------------------------------------
**
*******************************************************************************/

#ifndef _MODEM_H
#define _MODEM_H

#include "../../common/druheader.h"

//定义Modem Gprs连接状态
#define MODEM_SMS           0x01
#define GPRS_SMS            0x11
#define GPRS_DIAL           0x12
#define GPRS_DIALOK         0x13
#define GPRS_DIALFAILURE    0x14
#define GPRS_ONLINE         0x15
#define GPRS_OFFLINE        0x1C
#define GPRS_DISCONNET      0x1D

#define MODEM_ERROR         0xFE

#define DIALINTERVAL        60  //gprs拨号间隔时间60s

#define GPRS_PPPDIR         	"/flashDev/program/ppp"
#define GPRS_PPPSCRIPT      	"/flashDev/program/ppp/gprs-gsm"
#define GPRS_CONNECT        	"/flashDev/program/ppp/gprs-connect"
#define GPRS_DISCONNECT     	"/flashDev/program/ppp/gprs-disconnect"
#define GPRS_TMPFILE		    	"/tmp/gprs-tmpfile"
#define GPRS_DIALON          	"pppd file /flashDev/program/ppp/gprs-gsm &"
#define GPRS_DIALOFF         	"killall pppd"
//#define GPRS_KILL           "killall pppd file /flashDev/program/ppp/gprs-gsm"
#define RM_PPP0_PID         	"rm -rf  /var/run/ppp0.pid"
#define RM_LCK_TTY          	"rm -rf  /var/lock/LCK..ttyS2"
#define ROUTE_ON 							"route add default ppp0"
#define ROUTE_OFF							"route del default"

int ModemInit(void);
int ModemSendPackSms(ComBuf_t *p_combuf, char *tel, APPack_t *p_packbuf);
int ModemSmsReceiveData(ComBuf_t *p_combuf, char *tel);
void *GprsCom_Thread(void *pvoid);
void GprsComThreadStart(void);
int str1nstr2(char *s1, char *s2, int n);
/*
int ModemAtChat(struct combuf *pcombuf, char *atcmd, char *rc, int timeo);
int ModemSendSms(struct combuf *pcombuf, char *tel, struct cmcc_pack *p_packbuf);

void ModemGprsInit(struct combuf *pcombuf);
int GprsModeSendSms(struct combuf *pcombuf, char *tel, struct cmcc_pack *p_packbuf);
void GprsDisconnet(struct combuf *pcombuf);
int GprsDial(char *tbl_name);

void GprsComThreadInit(void);
void *GprsCom_Thread(void *pvoid);
void GprsComThreadStart(void);
void GprsComThreadStop(void);

int ModemSmsModeInit(struct combuf *pcombuf, char *tel);
void *ModemSms_Thread(void *pvoid);
void ModemSmsThreadStart(void);
void ModemSmsThreadStop(void);
*/
#endif /* _MODEM_H */
/******************* (C) COPYRIGHT************************END OF FILE****/
