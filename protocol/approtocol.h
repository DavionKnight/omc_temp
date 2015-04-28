/********************  COPYRIGHT(C) ***************************************
**--------------文件信息--------------------------------------------------------
**文   件   名: approtocol.h
**创   建   人: 于宏图
**创 建  日 期: 
**程序开发环境:
**描        述: APx协议处理程序头文件
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
#ifndef _APPROTOCOL_H
#define _APPROTOCOL_H

#include "../common/druheader.h"

int APHeadPack(DevInfo_t *p_devinfo, int aptype, int commandid, int packno, APPack_t *p_packbuf);
int APPack(APPack_t *p_packbuf, char *sdbuf);
int APProcess(APPack_t *p_packbuf, DevicePara_t *p_dev);
int	APUnpack(char *rcbuf, int rcsum, APPack_t *p_packbuf);
void ClearAPPackBuf(APPack_t *p_packbuf);
int GetDevInfo(DevInfo_t *p_devinfo, APPack_t *p_packbuf);
int QueryCommand(APPack_t *p_packbuf);
int QueryCommandBack(APPack_t *p_packbuf);
int QueryIDList(DevInfo_t *p_devinfo, INT16U pdustart, APPack_t *p_packbuf);
int QueryIDPack(DevInfo_t *p_devinfo, int packno, int idpackno, int idpacksum, APPack_t *p_packbuf);
int QueryLog(APPack_t *p_packbuf);
int QueryParaPack(DevInfo_t *p_devinfo, int packno, int *p_idlist, APPack_t *p_packbuf);
int QueryParaValue(DevInfo_t *p_devinfo, INT16U objectid, char *p_buf);
int QuerySampleData(APPack_t *p_packbuf);
int ReportCommand(APPack_t *p_packbuf);
int ReportCommandBack(APPack_t *p_packbuf);
int ReportParaPack(int APType, DevInfo_t *p_devinfo, int reporttype, int packno, APPack_t *p_packbuf);
int SetCommand(APPack_t *p_packbuf);
int SetCommandBack(APPack_t *p_packbuf);
int SetParaPack(DevInfo_t *p_devinfo, int packno, char *p_buf, int buflen, APPack_t *p_packbuf);
int SetParaValue(DevInfo_t *p_devinfo, INT16U objectid, char *buf);

int BinToBcd(int data);
int	BcdToBin(int data);
int	GetDevTime(char *tbuf);
int	SetDevTime(char *tbuf);
void update(void);

int SetParaValue_MCP_C(DevInfo_t *p_devinfo, unsigned int objectid, char *buf, int flag);
int QueryParaValue_MCP_C(DevInfo_t *p_devinfo, unsigned int objectid, char *p_buf);
int GetParaPro_MCP_C(DevInfo_t *p_devinfo, unsigned int objectid);
#endif  // _APPROTOCOL_H

/*********************************End Of File*************************************/
