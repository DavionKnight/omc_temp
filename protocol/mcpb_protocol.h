/********************  COPYRIGHT(C) 2014 ***************************************
**                               北京汉铭信通科技有限公司
**                                     无线产品研发部
**
**                                 http:// www.aceway.com.cn
**--------------文件信息--------------------------------------------------------
**文   件   名: mcpb_protocol.h
**创   建   人: 于宏图
**创 建  日 期: 2014年4月26日
**程序开发环境：
**描        述: MCPB协议处理程序头文件
**--------------历史版本信息----------------------------------------------------
** 创建人: 于宏图
** 版  本: 
** 日　期: 
** 描　述: 
**--------------当前版本修订----------------------------------------------------
** 修改人:
** 版  本:
** 日　期:
** 描　述:
**------------------------------------------------------------------------------
**
*******************************************************************************/
#ifndef _MCPB_PROTOCOL_H
#define _MCPB_PROTOCOL_H

#include "usertype.h"
#include "../common/druheader.h"

//软件升级参数定义
#define MAXUPDATEFILEPACKSUM    5000  //监控软件升级文件最大包数,最大5000*1K
#define SOFT_UPDATEDIR          "/flashDev/program/app/update"
#define SOFT_BAKDIR         		"/flashDev/program/app/updatebak"
#define SOFT_UPDATEPARA        	"/flashDev/program/app/update/softupdatepara"
#define SOFT_UPDATETMP        	"/flashDev/program/app/update/update_tmp"
#define SOFT_UPDATEINFO        	"/flashDev/program/app/update/update_info"
#define MANUFACTURER_INFO       "wellhead-stone"
#define TEST_VERSION        	  "dam42-h0173a-xxx"
#define DEV_VERSION        	    "dam42-h0173a-xxx"
#define SOFT_VERSION        	  "/flashDev/program/app/au/dam42-h0173a-xxx"
	
#pragma pack(1)
typedef struct MCPBPara
{
INT8U   SWRunMode;       //0x0010,0x0201 设备监控软件运行模式 uint1型 0：监控模式，1：软件升级模式，其它值为系统保留 
INT32U  NextFilePackId;         //0x0202 下一个文件数据块序号	uint4型
INT16U  UpdatePackLen;          //0x0203  数据块长度	uint2型，单位为Byte
INT32U  UpdateFileLen;          //更新软件的长度(自定义)
char    SWUpdateFileID[20];   //0x0301 文件标识码,数字串，最大长度20个字节用16位CRC算法
//（与AP层校验单元使用相同的算法）产生。传输时，放在数字串的最前两个字节，并且第1个字节放CRC结果的低8bit。
INT8U   TranSoftFileFlag;       //0x0302 文件传输控制 uint1型1：表示文件传输开始，
//2：表示文件传输结束，3：表示OMC取消软件升级，4：表示软件升级正常结束
INT8U   SWUpdateResponseFlag; //0x0303	文件数据包应答
INT32U  UpdateFilePackId;       //0x0304	文件数据块序号	uint4型，该序号从0开始顺序进行编号
INT32U  UpdateFilePackAddr;     //0x0305	(此处为数据地址)文件数据块 数字串，长度仅受通信包的最大长度限制
INT8U   UpdateNotificationFlag; //软件更新结果上报标志，1：已经上报，0：未上报
} MCPBPara_t, *MCPBPara_tp;
#pragma pack()


//void  SaveUpdateData(void);
//void  LoadUpdateData(void);
//void  NewSoftUpdate(void);
//void  SoftUpdateRead(ComBuf_T *p_PackData, INT16U DataLen);
//INT8U SoftUpdateResponse(void);
//void  SoftUpdateWrite(ComBuf_T *p_PackData, INT16U DataLen);
//void  TurnToUpdateMode(ComBuf_T *p_PackData, INT16U  DataLen);
//void  UpdateNotification(void);
//void  CheckUpdateResult(void);
//void  UpdateModeApp(void);
//
//void  UpdateTest(void);

#endif  // _MCPB_PROTOCOL_H
/*********************************************************************************
**                            End Of File
*********************************************************************************/
