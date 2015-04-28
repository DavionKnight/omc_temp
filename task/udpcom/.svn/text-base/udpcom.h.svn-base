/********************  COPYRIGHT(C)***************************************
**--------------文件信息--------------------------------------------------------
**文   件   名: udpcom.h
**创   建   人: 于宏图
**创 建  日 期: 
**程序开发环境：
**描        述: IR协议udp广播通讯处理程序的头文件
**--------------历史版本信息----------------------------------------------------
** 创建人: 于宏图
** 版  本:
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

#ifndef _UDPCOM_H
#define _UDPCOM_H

#include "../../common/druheader.h"

int MACCompare(char *mac1, char *mac2);
int MACToStr(char *p_mac, char *p_macstr);
int GetClientIP(char devno);
INT32U UDPBroadcastMsnDeal(char *mac);
void *UDPCom_Thread(void *pvoid);
void UDPComInit(void);
void UDPComThreadInit(void);
void UDPComThreadStart(void);
void UDPComThreadStop(void);
int ClientDbTableInit(char *tblname);

extern int creat_udp_relay(void);
#endif //_UDPCOM_H

/**************END OF FILE*********************************/
