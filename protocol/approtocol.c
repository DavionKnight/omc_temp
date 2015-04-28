/********************  COPYRIGHT(C)***************************************
**--------------文件信息--------------------------------------------------------
**文   件   名: approtocol.c
**创   建   人: 于宏图
**创 建  日 期:
**程序开发环境：
**描        述: 中国移动接入层（AP）处理参数查询设置程序
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
#include "../common/commonfun.h"
#include "../net/netcom.h"
#include "../sqlite/drudatabase.h"
#include "approtocol.h"
#include "mcpb_protocol.h"
#include <sys/wait.h>
#include <sys/types.h>
extern int system_time_config(void);
extern int g_OMCLoginFlag, g_OMCHeartBeatFlag, g_OMCSetParaFlag;
extern int g_OmcBroadcastFlag, g_DevType;;
extern volatile int	g_alarm_report_cnt;
extern volatile int	g_alarm_report_time; 
extern DevicePara_t g_DevicePara;
extern int settimeflag;
extern int g_OMCSetParaRs485Flag;

/*******************************************************************************
*函数名称 : void  ClearAPPackBuf(APPack_t *p_packbuf)
*功    能 : 清除通讯缓存为初始状态
*输入参数 : None
*输出参数 : None
*******************************************************************************/
void ClearAPPackBuf(APPack_t *p_packbuf)
{
/*
  p_packbuf->StartFlag = ' '; //清通讯起始标志接收
  p_packbuf->EndFlag = ' ';   //清通讯停止标志接收
  p_packbuf->ComFlag = 0; 		//通讯状态:空闲//COMIDLE;
  p_packbuf->ComBufOverflow = 0;	//通讯缓冲区溢出标志
  p_packbuf->PtrComWr = &p_packbuf->APType;  //指向下一字符将被插入的位置
  p_packbuf->PtrComRd = &p_packbuf->APType;  //指向下一字符将被提取的位置
  p_packbuf->PackLen = 0;  //通讯缓冲中字符数
  memset(p_packbuf->PackValue, 0, PACK_VALUE_SIZE);	//将接收缓冲全置为0
*/
  memset(p_packbuf, 0, sizeof(APPack_t));//清除通讯数据包内容
}

/*******************************************************************************
*函数名称 : int GetDevInfo(DevInfo_t *p_devinfo, APPack_t *p_packbuf)
*功    能 : 从数据包中解析DEVICETYPE_ID信息
*输入参数 : DevInfo_t *p_devinfo:设备信息结构
*           APPack_t *p_packbuf:协议数据包结构指针
*输出参数 : 返回数据单元起始地址
*******************************************************************************/
int GetDevInfo(DevInfo_t *p_devinfo, APPack_t *p_packbuf)
{
int objectid;

  p_devinfo->DeviceNo = p_packbuf->DeviceNo;
  objectid = p_packbuf->PackValue[1] + (p_packbuf->PackValue[2] * 0x100);
  //设备类型判断
  if (objectid == DEVICETYPE_ID)//查询设备下模块
  {
    p_devinfo->ModuleAddr = p_packbuf->PackValue[3];//远端单元及其模块
    p_devinfo->ModuleType = p_packbuf->PackValue[4] + (p_packbuf->PackValue[5] * 0x100);
    p_devinfo->PduStart = 6;
  }
  else//设备主机
  {
    p_devinfo->ModuleAddr = 0;
    p_devinfo->ModuleType = 0;
    p_devinfo->PduStart = 0;
  }
  return p_devinfo->PduStart;
}

/*******************************************************************************
*函数名称 : APPack(APPack_t *p_packbuf, char *sdbuf)
*功    能 : 按照中国移动AP协议，对协议数据包进行打包的函数
*输入参数 : APPack_t *p_packbuf：协议数据包结构指针
*           char  *sdbuf:打包后数据缓存
*输出参数 : 打包后需要发送的数据包长度
*******************************************************************************/
int APPack(APPack_t *p_packbuf, char *sdbuf)
{
int sd, rd;

  sd = 0;
  sdbuf[sd++] = p_packbuf->StartFlag; //通讯起始标志接收
  sdbuf[sd++] = p_packbuf->APType;		//AP:层协议类型
  sdbuf[sd++] = p_packbuf->VPType;		//VP:A承载协议类型
  sdbuf[sd++] = (char)p_packbuf->StationNo;//站点编号
  sdbuf[sd++] = (char)(p_packbuf->StationNo/0x100);
  sdbuf[sd++] = (char)(p_packbuf->StationNo/0x10000);
  sdbuf[sd++] = (char)(p_packbuf->StationNo/0x1000000);
  sdbuf[sd++] = p_packbuf->DeviceNo;	    //设备编号
  sdbuf[sd++] = (char)p_packbuf->PackNo;	//通讯包标识号
  sdbuf[sd++] = (char)(p_packbuf->PackNo/0x100);
  sdbuf[sd++] = p_packbuf->VPInteractFlag;  //VP层交互标志
  sdbuf[sd++] = p_packbuf->MCPFlag;			    //MCP层协议标识
  sdbuf[sd++] = p_packbuf->CommandFlag;	    	//命令标识
  sdbuf[sd++] = p_packbuf->ResponseFlag;	  	//应答标志
	//数据
  for(rd = 0; rd < (p_packbuf->PackLen - AP_MSG_HEAD_TAIL_LEN); rd++)
    sdbuf[sd++] = p_packbuf->PackValue[rd];

  p_packbuf->CRCData = CCITT_CRC16(&sdbuf[1], (p_packbuf->PackLen - AP_MSG_UNCRC_LEN));//CRC校验
  sdbuf[sd++] = (char)p_packbuf->CRCData;//CRC校验码
  sdbuf[sd++] = (char)(p_packbuf->CRCData/0x100);
  sdbuf[sd++] = p_packbuf->EndFlag;
  
  return sd;
}

/*******************************************************************************
*函数名称 : int	APUnpack(char *rcbuf, int rcsum, APPack_t *p_packbuf)
*功    能 : 当完成一次数据接收时，调用此函数，按照中国移动AP协议解包
*输入参数 : char  *rcbuf:接收数据缓冲
*						int   rcsum：接收数据数
*						APPack_t *p_packbuf：按照包格式解析接收数据
*输出参数 : 正确,数据包长度,错误返回MSG_CRC_ERR
*******************************************************************************/
int	APUnpack(char *rcbuf, int rcsum, APPack_t *p_packbuf)
{
int j;

  ClearAPPackBuf(p_packbuf);
  p_packbuf->PackLen = rcsum;//通讯数据包长度
  p_packbuf->CRCData = rcbuf[rcsum-3] + rcbuf[rcsum-2]*0x100;//CRC校验码
  p_packbuf->EndFlag = rcbuf[rcsum-1];//通讯停止标志接收
  //AP层的CRC校验,当出错时应丢弃该包
  if(CCITT_CRC16(&rcbuf[1], (rcsum-AP_MSG_UNCRC_LEN)) == p_packbuf->CRCData)
  {
	  p_packbuf->StartFlag = rcbuf[0];    //通讯起始标志接收
	  p_packbuf->APType = rcbuf[1];     //AP层协议类型
	  p_packbuf->VPType = rcbuf[2];     //VP:A承载协议类型
	  p_packbuf->StationNo = rcbuf[3] + rcbuf[4]*0x100//站点编号
		     				 				 + rcbuf[5]*0x10000 + rcbuf[6]*0x1000000;
	  p_packbuf->DeviceNo = rcbuf[7];		//设备编号
	  p_packbuf->PackNo = rcbuf[8] + rcbuf[9]*0x100;//通讯包标识号
	  p_packbuf->VPInteractFlag = rcbuf[10];   //VP层交互标志
	  p_packbuf->MCPFlag = rcbuf[11];	     //MCP层协议标识
	  p_packbuf->CommandFlag = rcbuf[12];  //命令标识
	  p_packbuf->ResponseFlag = rcbuf[13]; //应答标志

    for(j = 0; j < (rcsum - AP_MSG_HEAD_TAIL_LEN); j++)
		  p_packbuf->PackValue[j] = rcbuf[14 + j];//数据,根据需要确定是数据还是字符串 
		return p_packbuf->PackLen;
	}
	else
	  return MSG_CRC_ERR;
}

/*******************************************************************************
*函数名称 : int APProcess(APPack_t *p_packbuf, DevicePara_t *p_dev)
*功    能 : 当完成一次数据接收时，调用此函数，按照中国移动协议解包
*           当包数据正确，是本模块的数据，返回MSG_RIGHT，否则根据中国移动“通信错误检查优先级返回错误信息
*           错误信息：AP协议类型错误，AP CRC校验错误，AP承载层错误，VP站点编号错误，设备编号错误
*           VP交互标志错误，VP层监控控制层协议错误，通信安全错误.........
*输入参数 : APPack_t *p_packbuf：接收数据包缓存结构指针
*           DevicePara_t *p_dev:本机设备参数
*输出参数 : 解包结果,解包正确或包数据错误的返回错误信息
*******************************************************************************/
int APProcess(APPack_t *p_packbuf, DevicePara_t *p_dev)
{
int i;

  //AP层协议类型出错,当出错时应丢弃该包
  if (p_packbuf->APType == AP_B )
  {
  	if(p_packbuf->StartFlag != APB_STARTFLAG)
  	{
    	ClearAPPackBuf(p_packbuf);
    	return MSG_AP_ERR;
    }
  }
  if (p_packbuf->APType == AP_C )
  {
  	if(p_packbuf->StartFlag != APC_STARTFLAG)
  	{
    	ClearAPPackBuf(p_packbuf);
    	return MSG_AP_ERR;
    }
  }
  //(2)AP层的承载协议类型,当出错时应丢弃该包
  if(p_packbuf->VPType != VP_A )
  {
    ClearAPPackBuf(p_packbuf);
    return  MSG_VP_ERR;
  }
  //(3)VP层的站点编号校验,当出错时应丢弃该包
  //if ((p_packbuf->StationNo != BROADCAST_STATION)//广播站点编号200.200.200
   // &&(p_packbuf->StationNo != p_dev->StationNo))
  if (p_packbuf->StationNo != p_dev->StationNo)//广播站点编号200.200.200
  {
    ClearAPPackBuf(p_packbuf);//站点鉴权失败，对收到的命令不予回应
    return MSG_STATION_ERR;
  }
  //设备编号校验,如果设备对应为扩展单元，进行设备编号校验,对于主单元远程监控单元不校验
  //if ((p_packbuf->DeviceNo != BROADCAST_DEV)//广播设备编号0xFF
   //&& (p_packbuf->DeviceNo != p_dev->DeviceNo))
   if (p_packbuf->DeviceNo != p_dev->DeviceNo)
  {
    ClearAPPackBuf(p_packbuf);//设备鉴权失败，对收到的命令不予回应
    return MSG_DEV_ERR;
  }

  //(4)VP层交互标志。当出错时应丢弃该包
  if ((p_packbuf->VPInteractFlag != VP_INTERACT_NORMAL)  //通信请求的执行结果，表示执行正常
    &&(p_packbuf->VPInteractFlag != VP_INTERACT_REQUEST))//正常的命令请求
    //&&(p_packbuf->VPInteractFlag != VP_INTERACT_BUSY)    //通信请求的执行结果，表示设备忙，无法处理命令请求
  {
    ClearAPPackBuf(p_packbuf);
    return MSG_VP_INTERACT_ERR;
  }
  //(5)VP层的监控控制层协议标识。当出错时应丢弃该包
  if ((p_packbuf->MCPFlag != MCP_A)
    &&(p_packbuf->MCPFlag != MCP_B)
    &&(p_packbuf->MCPFlag != MCP_C))
  {
    ClearAPPackBuf(p_packbuf);
    return MSG_MCP_ERR;
  }
  //设备部支持MCP:A 协议，返回0x05错误
  if (p_packbuf->MCPFlag == MCP_A)
  {
  	p_packbuf->VPInteractFlag = VP_INTERACT_NORMAL;//通讯请求执行正常,VP层交互标志,0x00
    p_packbuf->ResponseFlag = RESPONSE_MCPAC_ERR;
    return MSG_QUERY_OK;
  }
  //(6)通信安全处理（电话号码鉴权）。当出错时应丢弃该包
  
  //(7)MCP层命令单元中的应答标志。当不为0xFF时应丢弃该包
  if ((p_packbuf->ResponseFlag != RESPONSE_COMMAND)   //0xFF
    &&(p_packbuf->ResponseFlag != RESPONSE_SUCCESS))  //0x00
  {
    ClearAPPackBuf(p_packbuf);
    return MSG_MCP_RESPONSE_ERR;
  }
  //(8)MCP层数据单元中的监控数据总长度
  //长度错,接收到的实际长度与数据包中描述的不相符（有不完整的参数）
	for (i = 0; i < (p_packbuf->PackLen - AP_MSG_HEAD_TAIL_LEN);)
 	{
 		if (p_packbuf->MCPFlag == MCP_B)
 		{
 			i = (p_packbuf->PackValue[i]+p_packbuf->PackValue[i+1]*0x100) + i;
 		}
 		else
 			i = p_packbuf->PackValue[i] + i;
 	}
 	if (i != (p_packbuf->PackLen - AP_MSG_HEAD_TAIL_LEN))
 	{
 		p_packbuf->VPInteractFlag = VP_INTERACT_NORMAL;//通讯请求执行正常,VP层交互标志,0x00
 		p_packbuf->ResponseFlag = RESPONSE_LENGTH_ERR;
 		return MSG_QUERY_OK;
 	}

  if (p_packbuf->MCPFlag == MCP_A) //0x01
  {
    if (p_packbuf->CommandFlag != COMMAND_REPORT)//上报回复无数据单元，不做长度检验错误
    {
      //设置、查询命令，进行长度检验错误
    }
  }
  //(9)MCP层命令单元中的命令标识
  //上报命令 
  if (p_packbuf->CommandFlag == COMMAND_REPORT)
  {
    //本机为主机,接收到从机上报命令,需要解析上报参数,需要包返回
    if (p_packbuf->ResponseFlag == RESPONSE_COMMAND)
    {
      if (p_packbuf->VPInteractFlag == VP_INTERACT_REQUEST)
      {
        ReportCommand(p_packbuf);//上报数据包
        return MSG_REPORT_OK;
      }
      else
        return MSG_REPORT_ERR;
    }
    //本机为从机,发送完上报包后，主机对上报包处理后的返回包
    else if (p_packbuf->ResponseFlag == RESPONSE_SUCCESS)
    {
      ReportCommandBack(p_packbuf);//上报返回数据包
      return MSG_REPORTBACK_OK;
    }
  }
  //查询命令(MCP_A:查询命令;MCP_B:读取命令)
  else if (p_packbuf->CommandFlag == COMMAND_QUERY)
  {
    //MCP_A:查询命令
    if (p_packbuf->MCPFlag == MCP_A)
    {
      //本机为从机,接收到查询命令,需要获取参数,打包返回
      if (p_packbuf->ResponseFlag == RESPONSE_COMMAND)
      {
        QueryCommand(p_packbuf);//查询数据
        return MSG_QUERY_OK;
      }
      //本机为主机,接收到查询命令返回数据,根据需要写入到数据库
      else if (p_packbuf->ResponseFlag == RESPONSE_SUCCESS)
      {
        QueryCommandBack(p_packbuf);//查询,数据返回
        return MSG_QUERYBACK_OK;
      }
    }
    //MCP_B:读取命令,软件升级使用
	else if (p_packbuf->MCPFlag == MCP_B)
	{
		printf("recv mcp_b read data\n");
		MCP_B_QueryCommand(p_packbuf);//更新软件回传,参数查询
		return MSG_SW_UPDATE_QUERY_OK;
	}
	else if (p_packbuf->MCPFlag == MCP_C)
	{
		//本机为从机,接收到查询命令,需要获取参数,打包返回
		if (p_packbuf->ResponseFlag == RESPONSE_COMMAND)
		{
			QueryCommand_MCP_C(p_packbuf);//查询数据
			return MSG_QUERY_OK;
		}
	}
  }
  //设置命令, MCP_A:设置命令;MCP_B:写入命令
  else if (p_packbuf->CommandFlag == COMMAND_SET)
  {
    //MCP_A:设置命令
    if (p_packbuf->MCPFlag == MCP_A)
    {
      //本机为从机,接收到设置命令,需要设置参数,并打包返回
      if (p_packbuf->ResponseFlag == RESPONSE_COMMAND)
      {
        SetCommand(p_packbuf);//设置数据
        return MSG_SET_OK;
      }
      //本机为主机,接收到设置命令返回数据
      else if (p_packbuf->ResponseFlag == RESPONSE_SUCCESS)
      {
        SetCommandBack(p_packbuf);//查询，数据返回
        return MSG_SETBACK_OK;
      }
    }
    //MCP_B:写入命令0x03，软件升级使用
    else if (p_packbuf->MCPFlag == MCP_B)
    {
		printf("recv mcp_b set data\n");
      MCP_B_SetCommand(p_packbuf);//软件升级
      return MSG_SW_UPDATE_OK;
    }
		else if(p_packbuf->MCPFlag == MCP_C)
		{
			//本机为从机,接收到设置命令,需要设置参数,并打包返回
			if (p_packbuf->ResponseFlag == RESPONSE_COMMAND)
			{
				SetCommand_MCP_C(p_packbuf);//设置数据
				return MSG_SET_OK;
			}
		}
  }
  //转换到软件升级模式
  else if (p_packbuf->CommandFlag == COMMAND_SW_UPDATE_MOD)
  {
    TurnToUpdateMode(p_packbuf);//切换到升级状态
    return MSG_SW_UPDATE_MOD_OK;
  }
  //切换监控软件版本
  else if(p_packbuf->CommandFlag == COMMAND_SWVERISONSWITCH)
  {
    SWVerisonSwitch(p_packbuf);//切换监控软件版本
    return MSG_SW_UPDATE_VER_OK;
  }
  // 命令编号错,无效的命令
  else
  {
  	p_packbuf->VPInteractFlag = VP_INTERACT_NORMAL;//通讯请求执行正常,VP层交互标志,0x00
    p_packbuf->ResponseFlag = RESPONSE_COMMANDNO_ERR;
    return MSG_QUERY_OK;
//  	p_packbuf->ResponseFlag = RESPONSE_COMMANDNO_ERR;
    //ClearAPPackBuf(p_packbuf);//命令标识错,无效的命令
    //return MSG_MCP_COMMAND_ERR;
  }
  return MSG_PACK_ERR;
}

/*******************************************************************************
*函数名称 : int QueryIDList(DevInfo_t *p_devinfo, INT16U pdustart, APPack_t *p_packbuf)
*功    能 : 根据主机发送的查询数据命令,获取监控数据列表,查询结果存储在*p_packbuf中
*输入参数 : DevInfo_t *p_devinfo:设备编号,设备模块地址,设备模块类型
*           INT16U start:数据存储开始位置
*           APPack_t *p_packbuf：移动协议数据包数据缓存结构指针
*输出参数 : 正常返回数据包长度，错误返回<0
*******************************************************************************/
int QueryIDList(DevInfo_t *p_devinfo, INT16U pdustart, APPack_t *p_packbuf)
{
int idlist[250];
int i, totalsum, idmaxsum, sdsum;

  if(p_packbuf->APType == AP_B)
    idmaxsum = APB_IDPACK_MAX_LEN/2;
  else
    idmaxsum = APC_IDPACK_MAX_LEN/2;

  memset(idlist, 0, sizeof(idlist));
  totalsum = DbGetIDList(p_devinfo, idlist);//读参数列表
  if(totalsum < 0)//查询错误
  {
    DEBUGOUT("Dev:%d,Maddr:%d,Mtype:%d IDList Query Failure!\r\n", p_devinfo->DeviceNo, p_devinfo->ModuleAddr, p_devinfo->ModuleType);
    return -3;
  }
 DEBUGOUT("DbGetIDList totalsum:%d!\r\n",totalsum);
  //第一次查询,计算总包数
  if(p_packbuf->PackValue[pdustart+3] == 1)
  {
    p_packbuf->PackValue[pdustart+3] = (totalsum+idmaxsum-1)/idmaxsum;
  }
  //已经发送数据数量
  sdsum = (p_packbuf->PackValue[pdustart+4]-1)*idmaxsum;

  //数据单元数据长度 监控数据长度(1)+监控数据标识(2)+总的查询次数(1)当前的查询编号(1)
  p_packbuf->PackValue[pdustart] = 0x05;//初始长度,xx00090101
  for(i = 0; i < idmaxsum; i++)//参数列表数量 数据长度(1)查询ID(2)总包数(1)查询包编号(1)
  {
    if(sdsum < totalsum)
    {
      p_packbuf->PackValue[pdustart+5+2*i] =(char)idlist[sdsum];
      p_packbuf->PackValue[pdustart+6+2*i] = (char)(idlist[sdsum]>>8);
      p_packbuf->PackValue[pdustart] = p_packbuf->PackValue[pdustart]+2;//数据长度家2
    }
    else
      break;
    sdsum++;
  }
  //参数列表数据包长度
  p_packbuf->PackLen = p_packbuf->PackValue[pdustart]+pdustart+AP_MSG_HEAD_TAIL_LEN;
  return p_packbuf->PackLen;
}
int QueryIDList_MCP_C(DevInfo_t *p_devinfo, INT16U pdustart, APPack_t *p_packbuf)
{
unsigned int idlist[1024];
int i, totalsum, idmaxsum, sdsum;

  if(p_packbuf->APType == AP_B)
    idmaxsum = APB_IDPACK_MAX_LEN/4;
  else
    idmaxsum = APC_IDPACK_MAX_LEN/4;

  memset(idlist, 0, sizeof(idlist));
  totalsum = DbGetIDList_MCP_C(p_devinfo, idlist);//读参数列表
  if(totalsum < 0)//查询错误
  {
    DEBUGOUT("Dev:%d,Maddr:%d,Mtype:%d IDList Query Failure!\r\n", p_devinfo->DeviceNo, p_devinfo->ModuleAddr, p_devinfo->ModuleType);
    return -3;
  }
 DEBUGOUT("DbGetIDList totalsum:%d!\r\n",totalsum);
  //第一次查询,计算总包数
  if(p_packbuf->PackValue[pdustart+5] == 1)
  {
    p_packbuf->PackValue[pdustart+5] = (totalsum+idmaxsum-1)/idmaxsum;
  }
  //已经发送数据数量
  sdsum = (p_packbuf->PackValue[pdustart+6]-1)*idmaxsum;
  //数据单元数据长度 监控数据长度(1)+监控数据标识(4)+总的查询次数(1)当前的查询编号(1)
  p_packbuf->PackValue[pdustart] = 0x07;//初始长度,xx00090101
  for(i = 0; i < idmaxsum; i++)//参数列表数量 数据长度(1)查询ID(2)总包数(1)查询包编号(1)
  {
    if(sdsum < totalsum)
    {
      p_packbuf->PackValue[pdustart+7+4*i] =(char)idlist[sdsum];
      p_packbuf->PackValue[pdustart+8+4*i] = (char)(idlist[sdsum]>>8);
      p_packbuf->PackValue[pdustart+9+4*i] = (char)(idlist[sdsum]>>16);
      p_packbuf->PackValue[pdustart+10+4*i] = (char)(idlist[sdsum]>>24);
      p_packbuf->PackValue[pdustart] = p_packbuf->PackValue[pdustart]+4;//数据长度家4
    }
    else
      break;
    sdsum++;
  }
  //参数列表数据包长度
  p_packbuf->PackLen = p_packbuf->PackValue[pdustart]+pdustart+AP_MSG_HEAD_TAIL_LEN;
  return p_packbuf->PackLen;
}
/*******************************************************************************
*函数名称 : int QuerySampleData(APPack_t *p_packbuf)
*功    能 : 获取批采数据
*输入参数 : APPack_t *p_packbuf:通讯数据包数据结构指针
*输出参数 : 正常返回数据包长度，错误返回0
*******************************************************************************/
int QuerySampleData(APPack_t *p_packbuf)
{
  p_packbuf->PackLen = 0;
  return p_packbuf->PackLen;
}

/*******************************************************************************
*函数名称 : int QueryLog(APPack_t *p_packbuf)
*功    能 : 获取日志数据
*输入参数 : APPack_t *p_packbuf:通讯数据包数据结构指针
*输出参数 : 正常返回数据包长度，错误返回0
*******************************************************************************/
int QueryLog(APPack_t *p_packbuf)
{
  p_packbuf->PackLen = 0;
  return p_packbuf->PackLen;
}

/*******************************************************************************
*函数名称 : int QueryParaValue(DevInfo_t *p_devinfo, INT16U objectid, char *p_buf)
*功    能 : 从数据库中查询objectid对应的数据，返回查询结果存储在*buf中
*输入参数 : DevInfo_t *p_devinfo:设备编号,设备模块地址,设备模块类型
*           objectid：数据编号，p_buf数据缓存
*输出参数 : 正常返回1,数据库中无对应参数返回-1
*******************************************************************************/
int QueryParaValue(DevInfo_t *p_devinfo, INT16U objectid, char *p_buf)
{
Pdu_t pdu;

	if(DbGetParaValue(p_devinfo, objectid, &pdu) == 1)
	{
		if(*p_buf == (pdu.len+AP_PDU_HEAD_LEN))
		{
		  if (strcmp(pdu.var_type, "bit") == 0)//bit型数据
		    *(p_buf+AP_PDU_HEAD_LEN) = (pdu.var[0] & 0x01);
		  else
		  {
		    memcpy((p_buf+AP_PDU_HEAD_LEN), pdu.var, pdu.len);
		    if (objectid == DEVICETIME_ID)	//设备当前时间,7个字节组成
    		{
    		  GetDevTime(p_buf+AP_PDU_HEAD_LEN);
    		}
		  }
		}
		else
		{
			*(p_buf+2) |= 0x40;//4:监控数据标识与监控数据长度不匹配
		}
		return 1;
	}
	else//未查到此数据,监控数据标识无法识别错误
	{
		*(p_buf+2) |= 0x10;//1:监控数据标识无法识别
		DEBUGOUT("Dev:%d,Maddr:%d,Mtype:%d,ID:%d no Find!\r\n", 
		         p_devinfo->DeviceNo, p_devinfo->ModuleAddr, p_devinfo->ModuleType, objectid);
		return -1;
	}
}

int get_idx(int id)
{
	int ret = 0;

	switch((id>>12)&0xf){
		case 1:
			ret = 0;
			break;
		case 4:
			ret = 1;
			break;
		case 7:
			ret = 2;
			break;
		case 8:
			ret = 3;
			break;
	}
	return ret;
}
// 查询MCPC参数
extern int ru_dout_pw[8][4]; // 下行输出功率
extern int g_din_pw[4];      // 下行输入功率
int QueryParaValue_MCP_C(DevInfo_t *p_devinfo, unsigned int objectid, char *p_buf)
{
	Pdu_t pdu;
	int val = 0;
	int val1, val2;
	int idx = 0;
	int port_num = 0;

	if(DbGetParaValue_MCP_C(p_devinfo, objectid, &pdu) == 1)
	{
		if(*p_buf == (pdu.len+AP_PDU_HEAD_LEN))
		{
			if (strcmp(pdu.var_type, "bit") == 0)//bit型数据
				*(p_buf+AP_PDU_HEAD_LEN) = (pdu.var[0] & 0x01);
			else
			{
				memcpy((p_buf+AP_PDU_HEAD_LEN), pdu.var, pdu.len);
				if (objectid == DEVICETIME_ID)	//设备当前时间,7个字节组成
				{
					GetDevTime(p_buf+AP_PDU_HEAD_LEN);
				}
			}
		}
		else
		{
			*(p_buf+4) &= 0x0F;
			*(p_buf+4) |= MCPVAR_LEN_ERR;//4:监控数据标识与监控数据长度不匹配
		}
		if((objectid == 0x08001001) || (objectid == 0x08004001) 
				|| (objectid == 0x08007001) || (objectid == 0x08008001)){
			if(g_DevType == MAIN_UNIT){
				// 下行输入功率范围超限，和无法检测
				DbGetThisIntPara(PW_DOWN_ALARM_ID, &val1);
				DbGetThisIntPara(PW_ERROR_ALARM_ID, &val2);
				if(((val1&0x1) == 1) || ((val2&0x1) == 1)){ // 电源发生故障
					*(p_buf+4) &= 0x0F;
					*(p_buf+4) |= MCPVAR_DATA_ERR;//7: 无法检测
				}else{
					val = signed_1to4(pdu.var[0]);
					if(val > 5){
						*(p_buf+4) &= 0x0F;
						*(p_buf+4) |= MCPVAR_UP_ERR;//6: 高于上限
					}else if(val < -15){
						*(p_buf+4) &= 0x0F;
						*(p_buf+4) |= MCPVAR_UNDER_ERR;//5: 低于下限
					}
				}
			}
		}else if((objectid&0xffff0ff0) == 0x08000100){ // 远端下行输出功率超限，和无法检测
			if(g_DevType == EXPAND_UNIT){
				DbGetThisIntPara(PW_DOWN_ALARM_ID, &val1);
				DbGetThisIntPara(PW_ERROR_ALARM_ID, &val2);
				if(((val1&0x1) == 1) || ((val2&0x1) == 1)){ // 电源发生故障
					*(p_buf+4) &= 0x0F;
					*(p_buf+4) |= MCPVAR_DATA_ERR;//7: 无法检测
				}else{
					val = signed_1to4(pdu.var[0]);
					if(val > 29){
						*(p_buf+4) &= 0x0F;
						*(p_buf+4) |= MCPVAR_UP_ERR;//6: 高于上限
					}else if(val < 14){
						*(p_buf+4) &= 0x0F;
						*(p_buf+4) |= MCPVAR_UNDER_ERR;//5: 低于下限
					}
				}
			}
		}else if((objectid&0xffff0ff0) == 0x08000200){ // 远端下行实际增益超限，和无法检测
			if(g_DevType == EXPAND_UNIT){
				idx = get_idx(objectid);
				port_num = (objectid&0xf);
				DbGetThisIntPara(PW_DOWN_ALARM_ID, &val1);
				DbGetThisIntPara(PW_ERROR_ALARM_ID, &val2);
				if(((val1&0x1) == 1) || ((val2&0x1) == 1) // 电源发生故障
					|| (signed_1to4(g_din_pw[idx]) > 5) || (signed_1to4(g_din_pw[idx]) < -15) // 输入功率超限
					|| (signed_1to4(ru_dout_pw[port_num][idx]) > 29) || (signed_1to4(ru_dout_pw[port_num][idx]) < 9)){ // 输出功率超限
					*(p_buf+4) &= 0x0F;
					*(p_buf+4) |= MCPVAR_DATA_ERR;//7: 无法检测
				}
			}
		}else if(((objectid&0xfffffff0) == 0x08000500)||(objectid == 0x0000087c)){ // 设备温度无法检测
			//if(g_DevType == EXPAND_UNIT){
				DbGetThisIntPara(PW_DOWN_ALARM_ID, &val1);
				DbGetThisIntPara(PW_ERROR_ALARM_ID, &val2);
				if(((val1&0x1) == 1) || ((val2&0x1) == 1)){ // 电源发生故障
					*(p_buf+4) &= 0x0F;
					*(p_buf+4) |= MCPVAR_DATA_ERR;//7: 无法检测
				}
			//}
		}
			
		return 1;
	}
	else//未查到此数据,监控数据标识无法识别错误
	{
		*(p_buf+4) &= 0x0F;
		*(p_buf+4) |= MCPVAR_NOID_ERR;//1:监控数据标识无法识别
		DEBUGOUT("Dev:%d,Maddr:%d,Mtype:%d,ID:%d no Find!\r\n", 
				p_devinfo->DeviceNo, p_devinfo->ModuleAddr, p_devinfo->ModuleType, objectid);
		return 1;
	}
}
/*******************************************************************************
*函数名称 : int SetParaValue(DevInfo_t *p_devinfo, INT16U objectid, char *buf)
*功    能 : 设置objectid对应的数据，写数据库
*输入参数 : DevInfo_t *p_devinfo:设备编号,设备模块地址,设备模块类型
*           objectid：数据编号，buf数据缓存
*输出参数 : 正常返回1,数据库中无对应参数返回-1
*******************************************************************************/
int SetParaValue(DevInfo_t *p_devinfo, INT16U objectid, char *buf)
{
Pdu_t pdu;
INT32U ip, res;

  memset(pdu.var, 0, sizeof(pdu.var));
	pdu.id = objectid; 
	pdu.len = *buf - 5 ;
	printf("pdu.len=%d\n", pdu.len);
	memcpy(pdu.var, (buf + AP_PDU_HEAD_LEN), pdu.len);
	res = DbSaveParaValue(p_devinfo, objectid, &pdu);
	if(res == 1)//参数设置成功
	{
		if ((objectid == OMCIP_ID) || (objectid == OMCIPPORT_ID) || (objectid ==  GPRSAPN_ID)
		 || (objectid == GPRSUSER_ID) || (objectid == GPRSPASSWORD_ID) || (objectid == DEVICEIP_ID))
		{
		  memcpy(&ip, &pdu.var, pdu.len);
		  if (objectid == DEVICEIP_ID)
		  {
		    SetDevIPpara("eth0", ip, SET_IP);
		  }
		  if (objectid == DEVNETMASK_ID)
		  {
		    SetDevIPpara("eth0", ip, SET_NETMASK);
		  }
		  if (objectid == DEVDEFAULTGW_ID)
		  {
		    SetDevIPpara("eth0", ip, SET_GATEWAY);
		  }
			//gprs重联
		}
		if (objectid == DEVICETIME_ID)	//设备当前时间,7个字节组成
		{
		  SetDevTime(pdu.var);
		  system_time_config();
		}
		return  1;
	}
	else//设置参数过程有错
	{
		if(pdu.id != objectid)//未查到此数据,监控数据标识无法识别错误
		{
			*(buf + 2) |= 0x10;//1:监控数据标识无法识别
			return -1;
		}
		if((*buf - 3) != pdu.len)
		{
			*(buf + 2) |= 0x40;//4:监控数据标识与监控数据长度不匹配
			return -1;
		}
		if (res == -6)
		{
			*(buf+4) &= 0x0F;
			*(buf+4) |= MCPVAR_OVER_ERR; //设备编号设置错误,超范围2：监控数据的设置值超出范围
			return  1;
		}
	}
}
// 设置MCPC参数
int SetParaValue_MCP_C(DevInfo_t *p_devinfo, unsigned int objectid, char *buf, int flag)
{
	int i;
	Pdu_t pdu;
	INT32U ip;
	int val = 0;

	memset(pdu.var, 0, sizeof(pdu.var));
	pdu.id = objectid; 
	pdu.len = *buf - 5 ;
	memcpy(pdu.var, (buf + AP_PDU_HEAD_LEN), pdu.len);

	if(objectid == DEVICENO_ID)
	{
		if (g_DevType == MAIN_UNIT)
		{
			if(*(buf+5) > 0)
			{
				*(buf+4) &= 0x0F;
				*(buf+4) |= MCPVAR_OVER_ERR; //设备编号设置错误,超范围2：监控数据的设置值超出范围
				return  1;
			}
		}
		else
		{
			if((*(buf+5) == 0xff) || (*(buf+5) == 0x0))
			{
				*(buf+4) &= 0x0F;
				*(buf+4) |= MCPVAR_OVER_ERR; //设备编号设置错误,超范围2：监控数据的设置值超出范围
				return  1;
			}
		}
	}
	else if(objectid == 0x00000142){
		printf("\n\n>>>>>>>>>>>>>>>>>>>>>>\n");
		printf("objectid=0x%08x, val=%d\n", objectid, *(buf+5));
		if((*(buf+5) < 1) || (*(buf+5) > 2))
		{
			printf("ser id=0x%08x error\n", objectid);
			*(buf+4) &= 0x0F;
			*(buf+4) |= MCPVAR_OVER_ERR; //通信方式错误,超范围2：监控数据的设置值超出范围
			return  1;
		}
	}
	//else if((objectid & (~(INT32U)0x2ff))==0x200)
	//else if((objectid & 0xffffff00)==0x200)
	else if((objectid == 0x00000201))
	{
		if(*(buf+5) > 1)
		{
			*(buf+4) &= 0x0F;
			*(buf+4) |= MCPVAR_OVER_ERR; //设备编号设置错误,超范围2：监控数据的设置值超出范围
			return  1;
		}
	}else if(objectid == 0x00000172){ // 设备过温门限设置超出范围
		val = signed_1to4(*(buf+5));
		if((val > 125)||(val < -40)){
			*(buf+4) &= 0x0F;
			*(buf+4) |= MCPVAR_OVER_ERR; //超范围2：监控数据的设置值超出范围
			return  1;
		}
	}else if((objectid == 0x08001007) || (objectid == 0x08004007) || (objectid == 0x08007007) 
		  || (objectid == 0x08008007) || (objectid == 0x08001008) || (objectid == 0x08004008) 
		  || (objectid == 0x08007008) || (objectid == 0x08008008)) { // 下行输入功率门限设置超限
		if (g_DevType == MAIN_UNIT){
			val = signed_1to4(*(buf+5));
			if((val > 5)||(val < -15)){
				*(buf+4) &= 0x0F;
				*(buf+4) |= MCPVAR_OVER_ERR; //超范围2：监控数据的设置值超出范围
				return  1;
			}
		}
	}else if((objectid == 0x08001009) || (objectid == 0x08004009) || (objectid == 0x08007009) 
			|| (objectid == 0x08008009) || (objectid == 0x0800100a) || (objectid == 0x0800400a) 
			|| (objectid == 0x0800700a) || (objectid == 0x0800800a)) { // 下行输出功率门限设置超限
		if (g_DevType == EXPAND_UNIT){
			val = signed_1to4(*(buf+5));
			if((val > 29)||(val < -14)){
				*(buf+4) &= 0x0F;
				*(buf+4) |= MCPVAR_OVER_ERR; //超范围2：监控数据的设置值超出范围
				return  1;
			}
		}
	}else if(objectid == 0x00000872){ // 上行噪声抑制门限设置超限
		if (g_DevType == EXPAND_UNIT){
			val = signed_1to4(*(buf+5));
			printf("*buf+5=0x%02x, val=%d\n\n\n\n", *(buf+5), val);
			if((val > -40)||(val < -108)){
				*(buf+4) &= 0x0F;
				*(buf+4) |= MCPVAR_OVER_ERR; //超范围2：监控数据的设置值超出范围
				return  1;
			}

		}
	}else if((objectid == 0x08001005)||(objectid == 0x08004005)||(objectid == 0x08007005)||
			 (objectid == 0x08008005)||(objectid == 0x08001006)||(objectid == 0x08004006)||
			 (objectid == 0x08007006)||(objectid == 0x08008006))	//下行衰减值
	{
		if (g_DevType == MAIN_UNIT)
		{
			val = signed_1to4(*(buf+5));
			if((val > 15)||(val < 0))
			{
				*(buf+4) &= 0x0F;
				*(buf+4) |= MCPVAR_OVER_ERR; //超范围2：监控数据的设置值超出范围
				return  1;
			}
		}
	}else if(((objectid&0xffff0fff) == 0x08000300)||((objectid&0xffff0fff) == 0x08000301)||((objectid&0xffff0fff) == 0x08000302)||
			 ((objectid&0xffff0fff) == 0x08000303)||((objectid&0xffff0fff) == 0x08000304)||((objectid&0xffff0fff) == 0x08000305)||
			 ((objectid&0xffff0fff) == 0x08000306)||((objectid&0xffff0fff) == 0x08000307))	//远端上行衰减值
	{
		if (g_DevType == EXPAND_UNIT)
		{
			val = signed_1to4(*(buf+5));
			if((val > 15)||(val < 0))
			{
				*(buf+4) &= 0x0F;
				*(buf+4) |= MCPVAR_OVER_ERR; //超范围2：监控数据的设置值超出范围
				return  1;
			}
		}
	}else if(((objectid&0xffff0fff) == 0x08000380)||((objectid&0xffff0fff) == 0x08000381)||((objectid&0xffff0fff) == 0x08000382)||
			 ((objectid&0xffff0fff) == 0x08000383)||((objectid&0xffff0fff) == 0x08000384)||((objectid&0xffff0fff) == 0x08000385)||
			 ((objectid&0xffff0fff) == 0x08000386)||((objectid&0xffff0fff) == 0x08000387))	//远端下行衰减值
	{
		if (g_DevType == EXPAND_UNIT)
		{
			val = signed_1to4(*(buf+5));
			if((val > 15)||(val < 0))
			{
				*(buf+4) &= 0x0F;
				*(buf+4) |= MCPVAR_OVER_ERR; //超范围2：监控数据的设置值超出范围
				return  1;
			}
		}
	}
	//判断电话号码是否适合
	else if((objectid == QUERYTEL_ID) || (objectid == (QUERYTEL_ID+1)) || (objectid == (QUERYTEL_ID+2))
			||(objectid == (QUERYTEL_ID+3)) || (objectid == (QUERYTEL_ID+4)) || (objectid == NOTIFYTEL_ID))
	{
		for(i=0;i<20;i++)
		{
			if ((*(buf+5+i) < '0') || (*(buf+5+i) > '9'))
				//			if((('0'-1) < *(buf+5+i)) && (*(buf+5+i) < ('9'+1)))
			{
				if (*(buf+5+i) != 0x00)
				{
					*(buf+4) &= 0x0F;
					*(buf+4) |= MCPVAR_CODE_ERR; //设备编号设置错误,超范围2：监控数据的设置值超出范围
					return  1;
				}
			}
		}
	}
	if(DbSaveParaValue_MCP_C(p_devinfo, objectid, &pdu, flag) == 1)//参数设置成功
	{
		/*
		   if ((objectid == OMCIP_ID) || (objectid == OMCIPPORT_ID) || (objectid ==  GPRSAPN_ID)
		   || (objectid == GPRSUSER_ID) || (objectid == GPRSPASSWORD_ID) || (objectid == DEVICEIP_ID))
		   {
		   memcpy(&ip, &pdu.var, pdu.len);
		   if (objectid == DEVICEIP_ID)
		   {
		   SetDevIPpara("eth0", ip, SET_IP);
		   }
		   if (objectid == DEVNETMASK_ID)
		   {
		   SetDevIPpara("eth0", ip, SET_NETMASK);
		   }
		   if (objectid == DEVDEFAULTGW_ID)
		   {
		   SetDevIPpara("eth0", ip, SET_GATEWAY);
		   }
		//gprs重联
		}
		*/
		if (objectid == DEVICETIME_ID)	//设备当前时间,7个字节组成
		{
			SetDevTime(pdu.var);
			system_time_config();
		}
		if((g_DevType == EXPAND_UNIT) && (objectid == DEVICENO_ID)){
		//if((objectid == DEVICENO_ID)){
			UDPComThreadInit();
			UDPComThreadStart();
		}
		return 1;
	}
	else//设置参数过程有错
	{
		if(pdu.id != objectid)//未查到此数据,监控数据标识无法识别错误
			*(buf + 4) |= MCPVAR_NOID_ERR;//1:监控数据标识无法识别
		if((*buf - 5) != pdu.len)
			*(buf + 4) |= MCPVAR_LEN_ERR;//4:监控数据标识与监控数据长度不匹配
		return 1;
	}
}
/*******************************************************************************
*函数名称 : int APHeadPack(DevInfo_t *p_devinfo, int aptype, int commandid, int packno, APPack_t *p_packbuf)
*功    能 : 主动发起，从机、设备内部模块数据通讯包头组包
*输入参数 : DevInfo_t *p_devinfo:站点编号,设备编号,设备模块地址,设备模块类型
*           int aptype:AP协议类型 
*           int commandid：命令标识，查询、设置、上报......
*           int packno：数据包号
*           APPack_t *p_packbuf：移动协议数据包数据缓存结构指针
*输出参数 : 返回数据单元起始地址,错误返回-1
*******************************************************************************/
int APHeadPack(DevInfo_t *p_devinfo, int aptype, int commandid, int packno, APPack_t *p_packbuf)
{
int pvlen;

  if(aptype == AP_A)
  {
    p_packbuf->APType =AP_A;                //AP层协议类型,接入层协议:AP:A协议类型0x01
    p_packbuf->StartFlag = APA_STARTFLAG;   //通讯起始标志
    p_packbuf->EndFlag = APA_ENDFLAG;       //通讯停止标志
  }
  else if(aptype == AP_B)
  {
    p_packbuf->APType =AP_B;                //AP层协议类型,接入层协议:AP:B协议类型0x02
    p_packbuf->StartFlag = APB_STARTFLAG;   //通讯起始标志
    p_packbuf->EndFlag = APB_ENDFLAG;       //通讯停止标志
  }
  else if(aptype == AP_C)
  {
    p_packbuf->APType =AP_C;                //AP层协议类型,接入层协议:AP:C协议类型0x03
    p_packbuf->StartFlag = APC_STARTFLAG;   //通讯起始标志
    p_packbuf->EndFlag = APC_ENDFLAG;       //通讯停止标志
  }
  else
  {
    DEBUGOUT("AP Type Error!\r\n");
    return -1;//AP层协议类型错误
  }

  p_packbuf->VPType = VP_A;                   //3 承载层协议类型:VP:A0x01
  p_packbuf->StationNo = p_devinfo->StationNo;//4 站点编号 uint4,广播站点编号:200.200.200
  p_packbuf->DeviceNo = p_devinfo->DeviceNo;  //5 设备编号 uint1
  p_packbuf->PackNo = packno;                 //6 通信包标识号 uint2
  p_packbuf->VPInteractFlag = VP_INTERACT_REQUEST;//7 VP层交互标志0x80
  p_packbuf->MCPFlag = MCP_C;                 //8 监控控制层协议:MCP_A,0x01
  p_packbuf->CommandFlag = commandid;         //9 命令标识
  p_packbuf->ResponseFlag = RESPONSE_COMMAND; //10 应答标志,命令发起方该字段填充0xFF
  if (p_devinfo->ModuleType != 0)
  {
    pvlen = 0;
    p_packbuf->PackValue[pvlen++] = 0x06;//数据长度
    p_packbuf->PackValue[pvlen++] = (char)DEVICETYPE_ID;//0x06D2
    p_packbuf->PackValue[pvlen++] = (char)(DEVICETYPE_ID >> 8);
    p_packbuf->PackValue[pvlen++] = p_devinfo->ModuleAddr;
    p_packbuf->PackValue[pvlen++] = (char)p_devinfo->ModuleType;
    p_packbuf->PackValue[pvlen++] = (char)(p_devinfo->ModuleType >> 8);
    p_devinfo->PduStart = pvlen;
  }
  else
  {
    p_devinfo->PduStart = 0;
  }
  return p_devinfo->PduStart;
}

/*******************************************************************************
*函数名称 : int QueryCommand(APPack_t *p_packbuf)
*功    能 : 查询命令响应函数，对查询命令进行数据处理，将数据按要求放置到通讯数据包中并发送.
*输入参数 : APPack_t *p_packbuf:通讯数据包数据结构指针
*输出参数 : 数据包长度
*******************************************************************************/
int QueryCommand(APPack_t *p_packbuf)
{
int objectlen, objectid, pdustart, result, i;
DevInfo_t devinfo;

  //以下各项参数在回复时不变，起始、结束标志,AP层协议类型,VP:A承载协议类型,站点编号,设备编号,通讯包标识号不变,MCP层协议标识
  //StartFlag, EndFlag, APType, VPType, StationNo, DeviceNo, PackNo, MCPFlag
  //以下非数据单元各项根据协议进行修改
  p_packbuf->VPInteractFlag = VP_INTERACT_NORMAL;// 通讯请求执行正常,VP层交互标志,0x00
  p_packbuf->CommandFlag = COMMAND_QUERY;// 命令标识:查询
  //p_packbuf->ResponseFlag = RESPONSE_SUCCESS;// 应答标志:成功
  p_packbuf->ResponseFlag = 0x05;// 应答标志: mcp 错，仅支持MCP:C

    return  p_packbuf->PackLen;

  pdustart = GetDevInfo(&devinfo, p_packbuf);

  objectlen = p_packbuf->PackValue[pdustart];//数据长度
  objectid = p_packbuf->PackValue[pdustart+1] + (p_packbuf->PackValue[pdustart+2] * 0x100);
  if (objectid == IDLIST_ID) //设备监控列表查询
  {
    return QueryIDList(&devinfo, pdustart, p_packbuf);//正确查询到监控列表
  }
  else if (objectid == QUERYSAMPLEDATA_ID)//批采数据查询
  {
    return QuerySampleData(p_packbuf); //获取批采数据
  }
  else if (objectid == QUERYLOG_ID)//日志数据查询
  {
    return QueryLog(p_packbuf); //日志数据
  }
  else  //正常参数查询
  {
    result = 0;
    //数据单元根据协议数据ID号进行修改
    for (i = pdustart; i < (p_packbuf->PackLen - AP_MSG_HEAD_TAIL_LEN); )//AP_MSG_HEAD_TAIL_LEN:17,数据包控制协议字节数(除数据单元外所有数据)
    {
	  	if (p_packbuf->PackValue[i] == 0)//数据长度错误
	  	{
	  		DEBUGOUT("QueryCommand:PackValue Len Error...");
	  		goto _QUERYFAILURE;
	  	}
      //数据长度
      objectlen = p_packbuf->PackValue[i] - 3;
      // 数据单元数据标识,ID
      objectid = p_packbuf->PackValue[i + 1] + (p_packbuf->PackValue[i + 2] * 0x100);
      //读取数据库中ID对应值
      if (QueryParaValue(&devinfo, objectid, &p_packbuf->PackValue[i]) > 0)
        result = 1;
      i = i + p_packbuf->PackValue[i];//数据单元数据长度
    }
    if (result == 1)//查询数据成功
    {
      return  p_packbuf->PackLen;
    }
    else
    {
_QUERYFAILURE:
      DEBUGOUT("Query Command Failure!\r\n");
      ClearAPPackBuf(p_packbuf);
      return 0;
    }
  }
}
int is_alarm(int id)
{
	if(((id&0xfffffff0) == 0x08000060) || ((id&0xfffffff0) == 0x08000600) || ((id&0xffff0ff0) == 0x08000480) 
			|| ((id&0xfffffff0) == 0x08001580) || ((id&0xfffffff0) == 0x08004580) || ((id&0xfffffff0) == 0x08007580)
			|| ((id&0xfffffff0) == 0x08008580) || ((id&0xffffff00) == 0x00000300) || ((id&0xffff0fff) == 0x0800000e)
			|| ((id&0xffff0fff) == 0x0800000f) || ((id > 0x00000050) && (id < 0x00000075))){
		printf("is alarm id\n");
		return 1;
	}
	return 0;
}
int QueryCommand_MCP_C(APPack_t *p_packbuf)
{
	unsigned int objectlen, objectid, pdustart;
	int result, i;
	DevInfo_t devinfo;
	int tmp;

	//以下各项参数在回复时不变，起始、结束标志,AP层协议类型,VP:A承载协议类型,站点编号,设备编号,通讯包标识号不变,MCP层协议标识
	//StartFlag, EndFlag, APType, VPType, StationNo, DeviceNo, PackNo, MCPFlag
	//以下非数据单元各项根据协议进行修改
	p_packbuf->VPInteractFlag = VP_INTERACT_NORMAL;// 通讯请求执行正常,VP层交互标志,0x00
	p_packbuf->CommandFlag = COMMAND_QUERY;// 命令标识:查询
	p_packbuf->ResponseFlag = RESPONSE_SUCCESS;// 应答标志:成功

	pdustart = GetDevInfo(&devinfo, p_packbuf);

	objectlen = p_packbuf->PackValue[pdustart];//数据长度
	//objectid = p_packbuf->PackValue[pdustart+1] + (p_packbuf->PackValue[pdustart+2] * 0x100);
	objectid = p_packbuf->PackValue[pdustart+1] + ((p_packbuf->PackValue[pdustart+2]) <<  8) + 
		((p_packbuf->PackValue[pdustart+3]) << 16) + ((p_packbuf->PackValue[pdustart+4]) << 24); 
	if (objectid == IDLIST_ID) //设备监控列表查询
	{
		return QueryIDList_MCP_C(&devinfo, pdustart, p_packbuf);//正确查询到监控列表
	}
	else if (objectid == QUERYSAMPLEDATA_ID)//批采数据查询
	{
		return QuerySampleData(p_packbuf); //获取批采数据
	}
	else if (objectid == QUERYLOG_ID)//日志数据查询
	{
		return QueryLog(p_packbuf); //日志数据
	}
	else  //正常参数查询
	{
		result = 0;
		//数据单元根据协议数据ID号进行修改
		for (i = pdustart; i < (p_packbuf->PackLen - AP_MSG_HEAD_TAIL_LEN); )//AP_MSG_HEAD_TAIL_LEN:17,数据包控制协议字节数(除数据单元外所有数据)
		{
			if (p_packbuf->PackValue[i] == 0)//数据长度错误
			{
				DEBUGOUT("QueryCommand:PackValue Len Error...");
				goto _QUERYFAILURE;
			}
			//数据长度
			objectlen = p_packbuf->PackValue[i] - 5;
			// 数据单元数据标识,ID
			//objectid = p_packbuf->PackValue[i + 1] + (p_packbuf->PackValue[i + 2] * 0x100);
			objectid = p_packbuf->PackValue[i+1] + ((p_packbuf->PackValue[i+2]) <<  8) + 
				((p_packbuf->PackValue[i+3]) << 16) + ((p_packbuf->PackValue[i+4]) << 24); 
			//读取数据库中ID对应值
			if (QueryParaValue_MCP_C(&devinfo, objectid, &p_packbuf->PackValue[i]) > 0)
				result = 1;
			if(is_alarm(objectid)){
				DbGetThisIntPara(objectid, &tmp);
				printf("id=0x%08x, val=%d\n", objectid, tmp);
				if(tmp == 1){
					printf("val=1, write=129\n");
					p_packbuf->PackValue[i+5] = 129;
					SetParaValue_MCP_C(&devinfo, objectid, &p_packbuf->PackValue[i], 0); 
				}else if(tmp == 128){
					printf("val=128, write=0\n");
					p_packbuf->PackValue[i+5] = 0;
					SetParaValue_MCP_C(&devinfo, objectid, &p_packbuf->PackValue[i], 0); 
				}
			}
			i = i + p_packbuf->PackValue[i];//数据单元数据长度
		}
		if (result == 1)//查询数据成功
		{
			return  p_packbuf->PackLen;
		}
		else
		{
_QUERYFAILURE:
			DEBUGOUT("Query Command Failure!\r\n");
			ClearAPPackBuf(p_packbuf);
			return 0;
		}
	}
}

/*******************************************************************************
*函数名称 : int QueryCommandBack(APPack_t *p_packbuf)
*功    能 : 查询从机参数返回处理，将查询参数写到相应数据库中
*输入参数 : APPack_t *p_packbuf:通讯数据包数据结构指针
*输出参数 : 查询数据更新正确返回1,需要继续查询返回数据包长度,错误返回-1
*******************************************************************************/
int QueryCommandBack(APPack_t *p_packbuf)
{
int objectlen, objectid, pdustart, i, j;
DevInfo_t devinfo;
Pdu_t pdu;

  pdustart = GetDevInfo(&devinfo, p_packbuf);
  
  objectlen = p_packbuf->PackValue[pdustart];//数据长度
  objectid = p_packbuf->PackValue[pdustart+1] + (p_packbuf->PackValue[pdustart+2] * 0x100);
  if(objectid == IDLIST_ID)//设备监控列表返回,在数据库中参加参数
  {
    //参数列表处理
    if (p_packbuf->PackValue[pdustart+3] < p_packbuf->PackValue[pdustart+4])//ID查询包号小于ID总包数
    {
      QueryIDPack(&devinfo, p_packbuf->PackNo++, p_packbuf->PackValue[pdustart+3]++, p_packbuf->PackValue[pdustart+4], p_packbuf);
      return p_packbuf->PackLen;
    }
    else
    {
      ClearAPPackBuf(p_packbuf);
      return 1;//完成参数列表查询
    }
  }
  else//数据查询返回
  {
    for (i = pdustart; i < (p_packbuf->PackLen - AP_MSG_HEAD_TAIL_LEN); )//AP_MSG_HEAD_TAIL_LEN:17,数据包控制协议字节数(除数据单元外所有数据)
    {
      //数据长度
      pdu.len = p_packbuf->PackValue[i] - 3;
      // 数据单元数据标识,ID
      pdu.id = p_packbuf->PackValue[i+1] + (p_packbuf->PackValue[i+2] * 0x100);
      memset(pdu.var, 0, sizeof(pdu.var));
      for(j = 0; j < pdu.len; j++)
		    pdu.var[j] = p_packbuf->PackValue[i+3+j];
      //读取数据库中ID对应值
      if (DbSaveParaValue(&devinfo, objectid, &pdu) != 1)//参数设置不成功
      {
        DEBUGOUT("Query ID:%d Update Failure!\r\n", objectid);
      }
      i = i + p_packbuf->PackValue[i];//数据单元数据长度
    }
    ClearAPPackBuf(p_packbuf);
    return 1;
  }
}

/*******************************************************************************
*函数名称 : int QueryIDPack(DevInfo_t *p_devinfo, int packno, int packsum, APPack_t *p_packbuf)
*功    能 : 查询devno_port_mtype的第packno包监控列表数据
*输入参数 : DevInfo_t *p_devinfo:站点编号,设备编号,设备模块地址,设备模块类型
*           packno:查询包编号
*           idqueryno:参数列表包号(多包),idpacksum:参数列表总包数
*           APPack_t *p_packbuf：移动协议数据包数据缓存结构指针
*输出参数 : 正常查询返回数据包长度;错误设备返回-1
*******************************************************************************/
int QueryIDPack(DevInfo_t *p_devinfo, int packno, int idpackno, int idpacksum, APPack_t *p_packbuf)
{
int pvlen;

  //按照协议组包头
  pvlen = APHeadPack(p_devinfo, AP_C, COMMAND_QUERY, packno, p_packbuf);
  if (pvlen < 0)
  {
    DEBUGOUT("QueryIDPack Error!\r\n");
    return -1;//AP层协议类型错误
  }
  //数据单元添加数据
  p_packbuf->PackValue[pvlen++] = 0x05;
  p_packbuf->PackValue[pvlen++] = 0x09;
  p_packbuf->PackValue[pvlen++] = 0x00;
  p_packbuf->PackValue[pvlen++] = idpackno;
  p_packbuf->PackValue[pvlen++] = idpacksum;

  //数据包长度
  p_packbuf->PackLen = pvlen + AP_MSG_HEAD_TAIL_LEN;//AP_MSG_HEAD_TAIL_LEN:17,数据包控制协议字节数(除数据单元外所有数据)
  return p_packbuf->PackLen;
}

/*******************************************************************************
*函数名称 : int QueryParaPack(DevInfo_t *p_devinfo, int packno, int *p_idlist, APPack_t *p_packbuf)
*功    能 : 查询deviceno_port_mtype数据
*输入参数 : DevInfo_t *p_devinfo:站点编号,设备编号,设备模块地址,设备模块类型
*           packno:包编号
*           int *p_idlist:查询参数id列表
*           APPack_t *p_packbuf：移动协议数据包数据缓存结构指针
*输出参数 : 正常查询返回数据包长度;错误设备返回-1
*******************************************************************************/
int QueryParaPack(DevInfo_t *p_devinfo, int packno, int *p_idlist, APPack_t *p_packbuf)
{
int pdustart, pvlen, i;
Pdu_t pdu;

  //按照协议组包头
  pdustart = APHeadPack(p_devinfo, AP_C, COMMAND_QUERY, packno, p_packbuf);
  if (pdustart < 0)
  {
    DEBUGOUT("QueryParaPack Error!\r\n");
    return -1;//AP层协议类型错误
  }

  memset(&p_packbuf->PackValue[pdustart], 0, (sizeof(p_packbuf->PackValue)-pdustart));
  pvlen = pdustart;
  while (*p_idlist != 0x00)
  {
    if (DbGetParaValue(p_devinfo, *p_idlist, &pdu) == 1)
    {
      p_packbuf->PackValue[pvlen++] = pdu.len + AP_PDU_HEAD_LEN;
      p_packbuf->PackValue[pvlen++] = (char)pdu.id;
      p_packbuf->PackValue[pvlen++] = (char)(pdu.id >> 8);
      for (i = 0; i < pdu.len; i++)
        p_packbuf->PackValue[pvlen++] = 0;
    }
    if (pvlen > (APC_MSG_MAX_LEN-pdustart-AP_MSG_HEAD_TAIL_LEN))
    {
      DEBUGOUT("QueryParaPackLen More than APC_MSG_MAX_LEN!\r\n");
      break;
    }
    p_idlist++;
  }
  //参数列表数据包长度
  p_packbuf->PackLen = pvlen + pdustart + AP_MSG_HEAD_TAIL_LEN;
  return p_packbuf->PackLen;
}

/*******************************************************************************
*函数名称 : int SetCommand(APPack_t *p_packbuf)
*功    能 : 设置命令响应函数，对设置命令进行数据处理，将数据按要求放置到通讯数据包中并发送.
*输入参数 : APPack_t *p_packbuf:通讯数据包数据结构指针
*输出参数 : 数据包长度
*******************************************************************************/
int SetCommand(APPack_t *p_packbuf)
{
int objectlen, objectid, pdustart, result, i;
DevInfo_t devinfo;

  //以下各项参数在回复时不变，起始、结束标志,AP层协议类型,VP:A承载协议类型,站点编号,设备编号,通讯包标识号不变,MCP层协议标识
  //StartFlag, EndFlag, APType, VPType, StationNo, DeviceNo, PackNo, MCPFlag

  //以下非数据单元各项根据协议进行修改
  p_packbuf->VPInteractFlag = VP_INTERACT_NORMAL;// 通讯请求执行正常,VP层交互标志,0x00
  p_packbuf->CommandFlag = COMMAND_SET; // 命令标识:设置
  p_packbuf->ResponseFlag = RESPONSE_SUCCESS;// 应答标志:成功

  pdustart = GetDevInfo(&devinfo, p_packbuf);

  result = 0;
  //数据单元根据协议数据ID号进行修改
  for(i = pdustart; i < (p_packbuf->PackLen - AP_MSG_HEAD_TAIL_LEN); )//AP_MSG_HEAD_TAIL_LEN:17,数据包控制协议字节数(除数据单元外所有数据)
  {
  	if (p_packbuf->PackValue[i] == 0)//数据长度错误
  	{
  		DEBUGOUT("SetCommand:PackValue Len Error...");
  		goto _SETFAILURE;
  	}
    //数据长度
    objectlen = p_packbuf->PackValue[i] - 3;
    // 数据单元数据标识,ID
    objectid = p_packbuf->PackValue[i + 1] + (p_packbuf->PackValue[i + 2] * 0x100);
    //读取数据库中ID对应值
    if (SetParaValue(&devinfo, objectid, &p_packbuf->PackValue[i]) > 0)
      result = 1;
    i = i + p_packbuf->PackValue[i];//数据单元数据长度
  }
  if (result)//设置数据成功
  {
    g_OMCSetParaFlag = 1;
    return  p_packbuf->PackLen;
  }
  else
  {
_SETFAILURE:
    DEBUGOUT("Set Command Failure!\r\n");
    ClearAPPackBuf(p_packbuf);
    return 0;
  }
}

// MCPC数据处理
int SetCommand_MCP_C(APPack_t *p_packbuf)
{
	unsigned int objectlen, objectid, pdustart;
	int result, i;
	DevInfo_t devinfo;
	int temp;
	char cbuf[128];

	//以下各项参数在回复时不变，起始、结束标志,AP层协议类型,VP:A承载协议类型,站点编号,设备编号,通讯包标识号不变,MCP层协议标识
	//StartFlag, EndFlag, APType, VPType, StationNo, DeviceNo, PackNo, MCPFlag

	//以下非数据单元各项根据协议进行修改
	p_packbuf->VPInteractFlag = VP_INTERACT_NORMAL;// 通讯请求执行正常,VP层交互标志,0x00
	p_packbuf->CommandFlag = COMMAND_SET; // 命令标识:设置
	p_packbuf->ResponseFlag = RESPONSE_SUCCESS;// 应答标志:成功

	pdustart = GetDevInfo(&devinfo, p_packbuf);

	result = 0;
	//数据单元根据协议数据ID号进行修改
	for(i = pdustart; i < (p_packbuf->PackLen - AP_MSG_HEAD_TAIL_LEN); )//AP_MSG_HEAD_TAIL_LEN:17,数据包控制协议字节数(除数据单元外所有数据)
	{
		if (p_packbuf->PackValue[i] == 0)//数据长度错误
		{
			DEBUGOUT("SetCommand:PackValue Len Error...");
			goto _SETFAILURE;
		}
		//数据长度
		objectlen = p_packbuf->PackValue[i] - 5;
		// 数据单元数据标识,ID
		objectid = p_packbuf->PackValue[i+1] + ((p_packbuf->PackValue[i+2]) <<  8) + 
			((p_packbuf->PackValue[i+3]) << 16) + ((p_packbuf->PackValue[i+4]) << 24); 
		//读取数据库中ID对应值

		cbuf[0] = objectlen+AP_PDU_HEAD_LEN;
		QueryParaValue_MCP_C(&devinfo, objectid, cbuf);
		if(memcmp(&cbuf[5],&p_packbuf->PackValue[5], objectlen) != 0)
		{
			if((g_DevType == EXPAND_UNIT) && (objectid == 0x00000101)){   // 站点编号
				result = 1;
			}else{
				if (SetParaValue_MCP_C(&devinfo, objectid, &p_packbuf->PackValue[i], 0) > 0)
					result = 1;
			}
		}
		else
		{
			result = 1;
		}
		i = i + p_packbuf->PackValue[i];//数据单元数据长度
		temp = SqliteGetPro(&devinfo, objectid); // 读取属性字段，并判断个位是否是1，是则触发广播
		if(temp > 0)
		{    // 广播参数
			if((temp % 10) == 1)
			{
				printf("broadcast para\n");
				g_OmcBroadcastFlag = 1;
				result = 1;
			}
		}
	}
	if (result)//设置数据成功
	{
		g_OMCSetParaFlag = 1;
		g_OMCSetParaRs485Flag = 1;
		return  p_packbuf->PackLen;
	}
	else
	{
_SETFAILURE:
		DEBUGOUT("Set Command Failure!\r\n");
		ClearAPPackBuf(p_packbuf);
		return 0;
	}
}
/*******************************************************************************
*函数名称 : int SetCommandBack(APPack_t *p_packbuf)
*功    能 : AP协议设置命令返回处理函数
*输入参数 : APPack_t *p_packbuf:通讯数据包数据结构指针
*输出参数 : 设置命令返回正确返回1,需要继续查询返回数据包长度,错误返回-1
*******************************************************************************/
int SetCommandBack(APPack_t *p_packbuf)
{
  ClearAPPackBuf(p_packbuf);
  return 1;
}

/*******************************************************************************
*函数名称 : void SetPack(int APType, int notify_type, struct cmcc_pack *p_packbuf)
*功    能 : AP协议设备信息上报函数,监控数据标识:0x0141
*输入参数 : APType:AP协议类型;int NotifyType:上报的类型;struct cmcc_pack *p_packbuf:通讯数据包数据结构指针
           struct ras_sys *psys:参数结构指针
*输出参数 : None
*******************************************************************************/
/*******************************************************************************
*函数名称 : int SetParaPack(DevInfo_t *p_devinfo, int packno, char *buf, APPack_t *p_packbuf)
*功    能 : 设置deviceno_port_mtype数据
*输入参数 : DevInfo_t *p_devinfo:站点编号,设备编号,设备模块地址,设备模块类型
*           packno:包编号
*           char *buf:设置参数buf缓存
*           APPack_t *p_packbuf：移动协议数据包数据缓存结构指针
*输出参数 : 正常返回数据包长度;错误设备返回-1
*******************************************************************************/
int SetParaPack(DevInfo_t *p_devinfo, int packno, char *p_buf, int buflen, APPack_t *p_packbuf)
{
int pdustart, pvlen;

  //按照协议组包头
  pdustart = APHeadPack(p_devinfo, AP_C, COMMAND_SET, packno, p_packbuf);
  if (pdustart < 0)
  {
    DEBUGOUT("QueryParaPack Error!\r\n");
    return -1;//AP层协议类型错误
  }

  memset(&p_packbuf->PackValue[pdustart], 0, (sizeof(p_packbuf->PackValue)-pdustart));
  pvlen = pdustart;
  while (buflen--)
  {
    p_packbuf->PackValue[pvlen++] = *p_buf--;
  }
  //参数列表数据包长度
  p_packbuf->PackLen = pvlen + pdustart + AP_MSG_HEAD_TAIL_LEN;
  return p_packbuf->PackLen;
}

/*******************************************************************************
*函数名称 : int ReportCommand(APPack_t *p_packbuf)
*功    能 : 最为主机对从机上报命令响应函数，对上报命令相应进行处理
*输入参数 : APPack_t *p_packbuf:通讯数据包数据结构指针
*输出参数 : 数据包长度
*******************************************************************************/
int ReportCommand(APPack_t *p_packbuf)
{
int objectlen, objectid, pdustart, i, reporttype;
DevInfo_t devinfo;

  //以下各项参数在回复时不变，起始、结束标志,AP层协议类型,VP:A承载协议类型,站点编号,设备编号,通讯包标识号不变,MCP层协议标识
  //StartFlag, EndFlag, APType, VPType, StationNo, DeviceNo, PackNo, MCPFlag
  //以下非数据单元各项根据协议进行修改
  p_packbuf->VPInteractFlag = VP_INTERACT_NORMAL;// 通讯请求执行正常,VP层交互标志,0x00
  p_packbuf->CommandFlag = COMMAND_REPORT;// 命令标识:上报
  p_packbuf->ResponseFlag = RESPONSE_SUCCESS;// 应答标志:成功

  pdustart = GetDevInfo(&devinfo, p_packbuf);

  reporttype = 0;
  objectlen = p_packbuf->PackValue[pdustart];//数据长度
  objectid = p_packbuf->PackValue[pdustart+1] + (p_packbuf->PackValue[pdustart+2] * 0x100);
  if (objectid == REPORTTYPE_ID)//上报类型,明确上报的类型(在上报时，放在数据单元的最前边)
  {
    reporttype = p_packbuf->PackValue[pdustart+3];
    switch(reporttype)
    {
      case REPORT_ALARM://1:告警上报
        //保存从机告警上报
        for (i = (pdustart+4); i < (p_packbuf->PackLen - AP_MSG_HEAD_TAIL_LEN); )//AP_MSG_HEAD_TAIL_LEN:17,数据包控制协议字节数(除数据单元外所有数据)
        {
          //数据长度
          objectlen = p_packbuf->PackValue[i] - 3;
          // 数据单元数据标识,ID
          objectid = p_packbuf->PackValue[i + 1] + (p_packbuf->PackValue[i + 2] * 0x100);
          /*if (SetObjectValue(devno, port, mtype, ObjectId, &p_packbuf->PackValue[i]) > 0)
            result = 1;*/
          i = i + p_packbuf->PackValue[i];//数据单元数据长度
        }
        return p_packbuf->PackLen;
      break;
      case REPORT_CONFIG_CREATE://2:开站上报
      break;
      case REPORT_CHECK://3:巡检上报
      break;
      case REPORT_RESTORE://4:故障修复上报
      break;
      case REPORT_CONFIG_CHANGE://5:配置变更上报
      break;
      case REPORT_LOGIN_OMC://6:登录到监控中心上报
      break;
      case REPORT_HEART_BEAT://7:心跳上报
      break;
      case REPORT_SW_UPDATE://8:设备监控软件更新上报
      break;
      case REPORT_PSLOGIN_FAIL://9:GPRS登录失败上报
      break;
      case REPORT_SAMPLEJOB_END://10:批采结束上报
      break;
      default:
        DEBUGOUT("Invalid Report Command!\r\n");
        ClearAPPackBuf(p_packbuf);
      break;
    }
  }
  return p_packbuf->PackLen;
}

/*******************************************************************************
*函数名称 : int ReportCommandBack(APPack_t *p_packbuf)
*功    能 : 向主机上报返回处理函数，将上报结果写到相应数据库中
*输入参数 : APPack_t *p_packbuf:通讯数据包数据结构指针
*输出参数 : 数据包长度
*******************************************************************************/
int ReportCommandBack(APPack_t *p_packbuf)
{
int objectlen, objectid, pdustart, i, reporttype;
DevInfo_t devinfo;

  pdustart = GetDevInfo(&devinfo, p_packbuf);

  objectlen = p_packbuf->PackValue[pdustart];//数据长度
  objectid = p_packbuf->PackValue[pdustart+1] + ((p_packbuf->PackValue[pdustart+2]) <<  8) + 
				((p_packbuf->PackValue[pdustart+3]) << 16) + ((p_packbuf->PackValue[pdustart+4]) << 24); 

  if (objectid == REPORTTYPE_ID)//上报类型,明确上报的类型(在上报时，放在数据单元的最前边)
  {
    reporttype = p_packbuf->PackValue[pdustart+5];
	  switch(reporttype)
	  {
	    case REPORT_ALARM://1:告警上报
			//保存已经完成的告警上报,或清除上报参数
			for (i = (pdustart+6); i < (p_packbuf->PackLen - AP_MSG_HEAD_TAIL_LEN); )//AP_MSG_HEAD_TAIL_LEN:17,数据包控制协议字节数(除数据单元外所有数据)
			{
				if (p_packbuf->PackValue[i] == 0)//数据长度错误
				{
					DEBUGOUT("ReportCommandBack:PackValue Len Error...");
					goto _REPORTBACKFAILURE;
				}
				//数据长度
				objectlen = p_packbuf->PackValue[i] - 5;
				// 数据单元数据标识,ID
				objectid = p_packbuf->PackValue[i+1] + ((p_packbuf->PackValue[i+2]) <<  8) + 
					((p_packbuf->PackValue[i+3]) << 16) + ((p_packbuf->PackValue[i+4]) << 24); 

				if (p_packbuf->PackValue[i+5] == 1)//告警上报成功
				{
					p_packbuf->PackValue[i+5] = p_packbuf->PackValue[i+5] | ALARMSUCCESSFLAG;
					printf("recv alarm back!!!\n");
				}
				else if(p_packbuf->PackValue[i+5] == 0)//告警恢复上报成功
				{
					p_packbuf->PackValue[i+5] = 0;
				}
				SetParaValue_MCP_C(&devinfo, objectid, &p_packbuf->PackValue[i], 0);
				i = i + p_packbuf->PackValue[i];//数据单元数据长度
				g_alarm_report_cnt = 0;
				g_alarm_report_time = 0xffffffff;
			}
_REPORTBACKFAILURE:
        ClearAPPackBuf(p_packbuf);
        return p_packbuf->PackLen;
	    break;
      case REPORT_CONFIG_CREATE://2:开站上报
      case REPORT_CHECK://3:巡检上报
      case REPORT_RESTORE://4:故障修复上报
      case REPORT_CONFIG_CHANGE://5:配置变更上报
      break;
	    case REPORT_LOGIN_OMC://6:登录到监控中心上报
	      g_OMCLoginFlag = 1;//登陆成功
	    break;
	    case REPORT_HEART_BEAT://7:心跳上报
	      g_OMCHeartBeatFlag = 0;
	    break;
      case REPORT_SW_UPDATE://8:设备监控软件更新上报
      break;
	    case REPORT_PSLOGIN_FAIL://9:GPRS登录失败上报
	    break;
	    case REPORT_SAMPLEJOB_END://10:批采结束上报
		case 11:
	    break;
      default:
        DEBUGOUT("Invalid Report Command!\r\n");
        ClearAPPackBuf(p_packbuf);
        return 0;
      break;
	  }
    ClearAPPackBuf(p_packbuf);
  }
  return p_packbuf->PackLen;
}

/*******************************************************************************
*函数名称 : int ReportParaPack(int APType, DevInfo_t *p_devinfo, int reporttype, int packno, APPack_t *p_packbuf)
*功    能 : AP协议设备信息上报数据打包函数,监控数据标识:0x0141
*输入参数 : APType:AP协议类型;DevInfo_t *p_devinfo:站点编号,设备编号,设备模块地址,设备模块类型
*           int reporttype:上报的类型;packno:包编号;
*           APPack_t *p_packbuf：移动协议数据包数据缓存结构指针
*输出参数 : 正常查询返回数据包长度;错误设备返回-1
*******************************************************************************/
extern void set_alarm(unsigned int id, unsigned int val, int en);
int ReportParaPack(int APType, DevInfo_t *p_devinfo, int reporttype, int packno, APPack_t *p_packbuf)
{
int i, pvlen, allen;
char var[20];

  //按照协议组包头
  pvlen = APHeadPack(p_devinfo, APType, COMMAND_REPORT, packno, p_packbuf);
  //0x0141:上报
  p_packbuf->PackValue[pvlen++] = 6;//数据长度
  p_packbuf->PackValue[pvlen++] = (char)REPORTTYPE_ID;//监控数据标识
  p_packbuf->PackValue[pvlen++] = (char)(REPORTTYPE_ID >> 8);
  p_packbuf->PackValue[pvlen++] = (char)(REPORTTYPE_ID >> 16);
  p_packbuf->PackValue[pvlen++] = (char)(REPORTTYPE_ID >> 24);
  p_packbuf->PackValue[pvlen++] = reporttype;//上报的类型
  switch(reporttype)
  {
    case REPORT_ALARM://1:告警上报
      //获取告警数据
      allen = DbGetAlarmValue(p_devinfo, &p_packbuf->PackValue[pvlen]);
      if (allen > 0)
      {
      	p_packbuf->PackLen = pvlen + allen + AP_MSG_HEAD_TAIL_LEN;
      }
      else
      {
      	p_packbuf->PackLen = 0;
      }
    break;
    case REPORT_CONFIG_CREATE://2:开站上报
    case REPORT_CHECK://3:巡检上报
    case REPORT_RESTORE://4:故障修复上报                    
		if(reporttype == REPORT_RESTORE){
			set_alarm(BAT_ERROR_ALARM_ID, 0, 0);
		}
    case REPORT_CONFIG_CHANGE://5:配置变更上报                
    case REPORT_LOGIN_OMC://6:登录到监控中心上报              
    case REPORT_HEART_BEAT://7:心跳上报
    	p_packbuf->PackLen = pvlen + AP_MSG_HEAD_TAIL_LEN;
    break;                     
    case REPORT_SW_UPDATE://8:设备监控软件更新上报
      //0x000A:当前监控软件版本
      p_packbuf->PackValue[pvlen++] = 25;//数据长度
		  p_packbuf->PackValue[pvlen++] = (char)SWRUNVER_ID;//监控数据标识
		  p_packbuf->PackValue[pvlen++] = (char)(SWRUNVER_ID >> 8);
		  p_packbuf->PackValue[pvlen++] = (char)(SWRUNVER_ID >> 16);
		  p_packbuf->PackValue[pvlen++] = (char)(SWRUNVER_ID >> 24);
		  DbGetThisStrPara(SWRUNVER_ID, var);
      for(i = 0; i < 20; i++)
      {
        p_packbuf->PackValue[pvlen++] = var[i];
      }
      //0x0018:设备执行软件升级的结果
      p_packbuf->PackValue[pvlen++] = 6;//数据长度
		  p_packbuf->PackValue[pvlen++] = (char)SWUPDATERESULT_ID;//监控数据标识
		  p_packbuf->PackValue[pvlen++] = (char)(SWUPDATERESULT_ID >> 8);
		  p_packbuf->PackValue[pvlen++] = (char)(SWUPDATERESULT_ID >> 16);
		  p_packbuf->PackValue[pvlen++] = (char)(SWUPDATERESULT_ID >> 24);

      p_packbuf->PackValue[pvlen++] = g_DevicePara.SWUpdateResult;//设备执行软件升级的结果
      
      p_packbuf->PackLen = pvlen + AP_MSG_HEAD_TAIL_LEN;
    break;
    case REPORT_PSLOGIN_FAIL://9:GPRS登录失败上报
    	p_packbuf->PackLen = pvlen + AP_MSG_HEAD_TAIL_LEN;
    break;
    case REPORT_SAMPLEJOB_END://10:批采结束上报
	  case 11:  // 软件异常复位上报
      p_packbuf->PackLen = pvlen + AP_MSG_HEAD_TAIL_LEN;
    break;
    default:
      DEBUGOUT("Invalid Report Command!\r\n");
      ClearAPPackBuf(p_packbuf);
    break;
  }
  return p_packbuf->PackLen;
}

/*******************************************************************************
*函数名称 : int BinToBcd(int data)
*功    能 : Bin码转换为BCD数据函数
*输入参数 : int data：Bin码数据
*输出参数 : BCD 码数据
*******************************************************************************/
int BinToBcd(int data)
{
int	temp;

  temp = data / 10;
  temp = temp * 0x10 + (data % 10);
  return temp;
}

/*******************************************************************************
*函数名称 : int	BcdToBin(int data)
*功    能 : BCD码转换为Bin数据函数
*输入参数 : int data：BCD码数据
*输出参数 : BIN 码数据
*******************************************************************************/
int	BcdToBin(int data)
{
int temp;

  temp = (data >> 4) & 0x0F;
  temp = temp * 10 + (data & 0x0F);
  return temp;
}

/*******************************************************************************
*函数名称 : int	GetDevTime(char *tbuf)  
*功    能 : 获得AP协议时间格式字符串
*输入参数 : None
*输出参数 : 返回1
*******************************************************************************/
int	GetDevTime(char *tbuf)
{
time_t timep;
struct tm *p;

	time(&timep);
	p = gmtime(&timep);
	tbuf[0] = BinToBcd((1900 + p->tm_year) / 100);
	tbuf[1] = BinToBcd((1900 + p->tm_year) % 100);
	tbuf[2] = BinToBcd(1 + p->tm_mon);
	tbuf[3] = BinToBcd(p->tm_mday);
	tbuf[4] = BinToBcd(p->tm_hour);
	tbuf[5] = BinToBcd(p->tm_min);
	tbuf[6] = BinToBcd(p->tm_sec);
	return	1;
}

/*******************************************************************************
*函数名称 : int	SetDevTime(char *tbuf)  
*功    能 : 按照CMCC协议时间格式设置时间
*输入参数 : None
*输出参数 : 返回1
*******************************************************************************/
int	SetDevTime(char *tbuf)
{
time_t timep;
struct tm tmp;

	tmp.tm_year = BcdToBin(tbuf[0])*100 + BcdToBin(tbuf[1]) - 1900;
	tmp.tm_mon = BcdToBin(tbuf[2]) - 1;
	tmp.tm_mday = BcdToBin(tbuf[3]);
	tmp.tm_hour = BcdToBin(tbuf[4]);
	tmp.tm_min = BcdToBin(tbuf[5]);
	tmp.tm_sec = BcdToBin(tbuf[6]);

	if(settimeflag==1)
	{
		settimeflag = 0;
		tmp.tm_min += 2 ;
		if(tmp.tm_min>60)
		{
			tmp.tm_hour++;
			tmp.tm_min -= 60;
		}
		
	}
	

	timep = mktime(&tmp);
	stime(&timep);
	system("hwclock -w");
	return	1;
}
// FTP升级处理函数，升级过程,调用脚本
void update(void)
{
	int status = 0;
	int val = 0;
	char ftp_ip[20]; 
	unsigned short ftp_port;
	char ftp_user[40];
	char ftp_password[40];
	char ftp_path[48];
	char ftp_filename[48];
	char str_shell[256];

    if (DbGetThisIntPara(0x00000166, &val) == 1)// 是否接收到升级命令
	{
		printf("val=%d\n", val);
		if(val == 1){ // 启动升级
			DbSaveThisIntPara(0x00000166, 0);
			sqlite_read_data_ex(0x00000160, ftp_ip);
			DbGetThisIntPara(0x00000161, &ftp_port);
			sqlite_read_data_ex(0x00000162, ftp_user);
			sqlite_read_data_ex(0x00000163, ftp_password);
			sqlite_read_data_ex(0x00000164, ftp_path);
			sqlite_read_data_ex(0x00000165, ftp_filename);
			sprintf(str_shell, "/ramDisk/update.sh %s %d %s %s %s %s", ftp_ip, ftp_port, ftp_user, ftp_password, ftp_path, ftp_filename);
			printf(str_shell);
			status = system(str_shell);
			if(-1 == status){
				printf("system error!\n");
			}else{
				if(WIFEXITED(status)){
					if (0 == WEXITSTATUS(status)){
						printf("run shell script successfully\n");
						system("reboot");
					}else{
						printf("run shell script fail, script exit code: %d\n", WEXITSTATUS(status));
					}
				}else{
					printf("exit status = [%d]\n", WEXITSTATUS(status));
				}
			}
		}
	}else{
		printf("read 0x00000166 error!!!!!!!!!!!!\n");
	}
}
/*********************************End Of File*************************************/

