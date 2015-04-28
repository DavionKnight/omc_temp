/********************  COPYRIGHT(C) ***************************************
**--------------文件信息--------------------------------------------------------
**文   件   名: uartdev.h
**创   建   人: 于宏图
**创 建  日 期: 
**程序开发环境：
**描        述: uart头文件
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
#ifndef _UARTDEV_H
#define _UARTDEV_H

#include "../../common/druheader.h"

int OpenCommPortISR(int fd, int comport, struct sigaction saio);
int OpenCommPort(int fd, int comport);
int SetCommState(int fd, int nSpeed, int nBits, char nEvent, int nStop);
int UartInit(ComBuf_t *p_combuf, int ttyid);
int UartReceiveData(ComBuf_t *pcombuf, int waittime);
int UartSendData(int fd, char *sbuf, int len);
int UartSendPack(ComBuf_t *pcombuf, APPack_t *p_packbuf);

#endif//_UARTDEV_H

/*********************************End Of File*************************************/