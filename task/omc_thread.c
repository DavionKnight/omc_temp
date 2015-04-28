/*******************************************************************************
*函数名称 : int main(void)
*功    能 : 
*输入参数 : 
*输出参数 : 
*******************************************************************************/
#include "./localcom/localcom.h"
#include "./omccom/omccom.h"
//#include "./tcpclient/tcpclient.h"
//#include "./tcpserver/tcpserver.h"
#include "./udpcom/udpcom.h"
#include "../common/druheader.h"
#include "../net/netcom.h"
#include "../protocol/approtocol.h"
#include "../sqlite/drudatabase.h"

int settimeflag;
int g_DevType;
DevicePara_t g_DevicePara;
extern unsigned char get_device_type(void);
/******i*************************************************************************
*函数名称 : int _Omc_Thread_Init(void)
*功    能 : 启动OMC相关进程初始化
*输入参数 : None
*输出参数 : Fd
*******************************************************************************/
int _Omc_Thread_Init(void)
{

	DevicePara_t *pdev;
	DevInfo_t *pdevinfo, deviceinfo;

	//------2015_03_09----------------
	Pdu_t pdu, *p_pdu;
  	p_pdu = &pdu;
	//--------------------------------
	
	//通过设备读取
	g_DevType = get_device_type();
	//g_DevType = EXPAND_UNIT;
	DEBUGOUT("OMCcom Test:%d.\r\n", g_DevType);

	DataBaseInit();
	pdevinfo = &deviceinfo;
	pdev = &g_DevicePara;

	pdevinfo->DeviceNo = 0;
	pdevinfo->ModuleAddr = 0;
	pdevinfo->ModuleType = 0;
	LoadDevicePara(pdevinfo, pdev);
  
	//------2015_03_09----------------
	settimeflag = 1;
	DbGetParaValue_MCP_C(pdevinfo, DEVICETIME_ID, p_pdu);
	SetDevTime(pdu.var);
	//--------------------------------
  return 1;
}

/*******************************************************************************
*函数名称 : int _Omc_Thread(void)
*功    能 : 启动OMC相关进程
*输入参数 : None
*输出参数 : Fd
*******************************************************************************/
int _Omc_Thread(void)
{
  //LocalComInit();//本地通讯初始化
  //LocalComThreadStart();
  if (g_DevType == MAIN_UNIT)
  {
  	ModemInit();//modem通讯初始化
  }

  OMCComInit();//OMC通讯初始化
  OMCComThreadStart();
  if (g_DevType == MAIN_UNIT)
  {
    //与OMC通讯线程

    //server线程
    //TCPServerComInit();
    //TCPServerComThreadStart();
    //UDP线程
    UDPComThreadInit();
    UDPComThreadStart();
  }
  else
  {
    //与server通讯client的线程
    UDPComThreadInit();
    UDPComThreadStart();
    //TCPClientComInit();
    //TCPClientComThreadStart();
  }
  return 1;
}
