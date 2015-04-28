/********************  COPYRIGHT(C)***************************************
**--------------文件信息--------------------------------------------------------
**文   件   名: omccom.h
**创   建   人: 于宏图
**创 建  日 期: 
**程序开发环境：
**描        述: 与OMC通讯控制程序的头文件
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

#ifndef _OMCCOM_H
#define _OMCCOM_H

#include "../../common/druheader.h"

void OMCComInit(void);
void OMCComThreadInit(void);
void OMCComThreadStart(void);
void OMCComThreadStop(void);
void *OMCCom_Thread(void *pvoid);
void OMCAlarmDeal(void);
void ApPackTransimtPack(APPack_t *p_packbuf, ComBuf_t *p_combuf);
void ApPackTransimtUnPack(APPack_t *p_packbuf, ComBuf_t *p_combuf);
void OMCReadHWInfo(void);
void SaveAlarmVal(unsigned int objectid, int val);
int OMCWriteFpga(unsigned int addr, int data);
void OMCDevParaDeal(void);
#endif //_OMCCOM_H

/**************END OF FILE*********************************/
