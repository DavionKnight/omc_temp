/********************  COPYRIGHT(C) ***************************************
**--------------文件信息--------------------------------------------------------
**文   件   名: localcom.c
**创   建   人: 于宏图
**创 建  日 期: 
**程序开发环境：
**描        述: 本地通讯控制程序，用于与本地OMC监控使用
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
#include "../../common/commonfun.h"
#include "../../common/druheader.h"
#include "../../protocol/approtocol.h"
#include "../../protocol/apbprotocol.h"
#include "../../protocol/apcprotocol.h"
#include "uartdev.h"
#include "localcom.h"

ComBuf_t g_LocalCom;
SelfThread_t g_LocalComThread;

extern DevicePara_t g_DevicePara;

/*******************************************************************************
*函数名称 : int LocalComInit(void)
*功    能 : 本地通讯UART口配置函数
*输入参数 : None
*输出参数 : Fd
*******************************************************************************/
int LocalComInit(void)
{
  UartInit(&g_LocalCom, LOCALCOM);
  LocalComThreadInit();
  return g_LocalCom.Fd;
}

/*******************************************************************************
*函数名称 : void LocalComThreadInit(void)
*功    能 : 本地通讯接口线程初始化
*输入参数 : none
*输出参数 : none
*******************************************************************************/ 
void LocalComThreadInit(void)
{
  g_LocalComThread.Tid = 0;
  g_LocalComThread.ThreadStatus = THREAD_STATUS_EXIT;
}

/*******************************************************************************
*函数名称 : void *LocalCom_Thread(void *pvoid)
*功    能 : 本地通讯接口线程
*输入参数 : none
*输出参数 : none
*******************************************************************************/ 
void *LocalCom_Thread(void *pvoid)
{
	int rcsum, res;
	APPack_t PackBuf, *p_PackBuf;//数据协议缓冲数据结构
	DevInfo_t DevInfo, *p_DevInfo;
	DevicePara_t *p_DevicePara;

	pthread_detach(pthread_self());
	pthread_setcancelstate(PTHREAD_CANCEL_ENABLE, NULL);
	pthread_setcanceltype(PTHREAD_CANCEL_ASYNCHRONOUS, NULL);//线程设置为立即取消

	printf("LocalCom_Thread Run!\r\n");
	while(1)
	{
		//本地监控接收
		rcsum = UartReceiveData(&g_LocalCom, 500);
		if (rcsum > 0)
		{
			DEBUGOUT("Fd:%d.UartReceiveData:%d.\r\n", g_LocalCom.Fd, g_LocalCom.RecvLen);
			ComDataHexDis(g_LocalCom.Buf, g_LocalCom.RecvLen);
			//一个完整的数据包,按照APC协议解包,使用06D2中按照地址->模块类型顺序进行分别处理
			p_PackBuf = &PackBuf;
			p_DevInfo = &DevInfo;
			p_DevicePara = &g_DevicePara;

			//res = APCUnpack(g_LocalCom.Buf, g_LocalCom.RecvLen, p_PackBuf);
			res = APUnpack(g_LocalCom.Buf, g_LocalCom.RecvLen, p_PackBuf);
			if(res > 0)
			{
				DEBUGOUT("LocalCom:ReceiveData WriteLogBook.\r\n");
				ComDataWriteLog(g_LocalCom.Buf, g_LocalCom.RecvLen);//存日志
				GetDevInfo(p_DevInfo, p_PackBuf);

				if (APProcess(p_PackBuf, p_DevicePara) > 0)
				{
					if (p_PackBuf->PackLen > 0)
						UartSendPack(&g_LocalCom, p_PackBuf);//打包数据发送
				}
				ClearComBuf(&g_LocalCom);
			}
			else
			{
				DEBUGOUT("LocalCom APC Unpacked Error!\r\n");//存错误数据包标识
				ClearComBuf(&g_LocalCom);
			}
		}
		usleep(1000);
	}
	g_LocalComThread.ThreadStatus = THREAD_STATUS_EXIT;
	g_LocalComThread.Tid = 0;
	pthread_exit(NULL);
}

/*******************************************************************************
*函数名称 : void LocalComThreadStart(void)
*功    能 : 创建本地通讯接口线程
*输入参数 : none
*输出参数 : none
*******************************************************************************/ 
void LocalComThreadStart(void)
{
  if (g_LocalComThread.ThreadStatus != THREAD_STATUS_RUNNING)
  {
    pthread_create(&g_LocalComThread.Tid, NULL, LocalCom_Thread, NULL);
    g_LocalComThread.ThreadStatus = THREAD_STATUS_RUNNING;
    printf("LocalCom_Thread ID: %lu.\n", g_LocalComThread.Tid);
  }
}

/*******************************************************************************
*函数名称 : void LocalComThreadStop(void)
*功    能 : 停止本地通讯接口线程
*输入参数 : none
*输出参数 : none
*******************************************************************************/ 
void LocalComThreadStop(void)
{
  if (g_LocalComThread.ThreadStatus != THREAD_STATUS_EXIT)
  {
    pthread_cancel(g_LocalComThread.Tid);
    g_LocalComThread.Tid = 0;
    g_LocalComThread.ThreadStatus = THREAD_STATUS_EXIT;
    printf("LocalCom_Thread Stop!\r\n");
  }
}

/*********************************End Of File*************************************/
