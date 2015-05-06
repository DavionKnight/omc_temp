/********************  COPYRIGHT(C) ***************************************
**--------------文件信息--------------------------------------------------------
**文   件   名: udpcom.c
**创   建   人: 
**创 建  日 期:  
**程序开发环境：
**描        述: IR协议udp广播通讯处理程序
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
#include "../localcom/localcom.h"
//#include "../tcpserver/tcpserver.h"
#include "../../common/commonfun.h"
#include "../../net/netcom.h"
#include "../../protocol/approtocol.h"
#include "../../sqlite/sqliteops.h"
#include "../../sqlite/drudatabase.h"
#include "udpcom.h"

ComBuf_t g_UDPCom;
SelfThread_t g_UDPComThread;

extern int g_DevType;
extern unsigned long serverip;
extern DevicePara_t g_DevicePara;
ClientBuf_t g_TCPClient[CONNECT_TOTALNUMBER];
//extern ClientBuf_t g_TCPClient[CONNECT_TOTALNUMBER];

/*******************************************************************************
*函数名称 : int MACCompare(char *mac1, char *mac2)
*功    能 : MAC地址比对,
*输入参数 : int newfd:新增连接fd;struct sockaddr_in addr:新连接IP地址
*输出参数 : 相同返回1,否则返回-1
*******************************************************************************/ 
int MACCompare(char *mac1, char *mac2)
{
int i;

  for (i = 0; i < MACSIZE; i++)
  {
    if (*mac1++ != *mac2++)
    {
      return -1;
    }
  }
  return 1;
}

/*******************************************************************************
*函数名称 : int MACToStr(char *p_mac, char *p_macstr)
*功    能 : MAC地址转换为字符串
*输入参数 : char *p_mac:HEX型MAC地址;char *p_macstr:字符型MAC地址
*输出参数 : 相同返回1,否则返回-1
*******************************************************************************/ 
int MACToStr(char *p_mac, char *p_macstr)
{
int i;

	for(i = 0; i < (MACSIZE-1); i++)
	{
		sprintf(p_macstr, "%02X:", *p_mac++);
		p_macstr = p_macstr + 3;
	}
	sprintf(p_macstr, "%02X", *p_mac);
	*(p_macstr+2) = 0;
	return 1;
}

/*******************************************************************************
*函数名称 : int GetClientIP(char devno)
*功    能 : 根据devno获取client对应的IP地址(最后一位)
*输入参数 : char devno: 设备编号
*输出参数 : 正确返回对应的IP地址(最后一位),否则返回0
*******************************************************************************/ 
int GetClientIP(char devno)
{
int res, devip[4];
SqlResult_t sqlres;
char sql[SQL_CMD_SIZE];

	memset(devip, 0, sizeof(devip));
	sprintf(sql, "SELECT * FROM %s WHERE DevNo=%d;", CLIENT_TBL, devno);
	res = SqliteSelect(sql, &sqlres);

	if (res == SQLITE_OK)
	{
		//SqlResultDis(&sqlres);
		if(sqlres.nRow > 0)//此DevNo存在,重新分配DevNo
		{
			sscanf(sqlres.dbResult[sqlres.nColumn + CLIENTTBL_IPADDR], "%d.%d.%d.%d",
             &devip[0], &devip[1], &devip[2], &devip[3]);
		}
		sqlite3_free_table(sqlres.dbResult);//释放掉sqlres.dbResult的内存空间
	}
	return devip[3];
}

/*******************************************************************************
*函数名称 : int UDPMainMsnDeal(UDPRequestMsg_t *p_reqmsg, UDPAckMsg_t *p_ackmsg)
*功    能 : UDPMainMsnDeal消息处理函数
*输入参数 : p_reqmsg:请求消息指针;p_ackmsg:应答消息指针
*输出参数 : 正确返回1,否则返回0
*******************************************************************************/ 
int UDPMainMsnDeal(UDPRequestMsg_t *p_reqmsg, UDPAckMsg_t *p_ackmsg)
{
int res;
Pdu_t pdu;
SqlResult_t sqlres;
INT32U selfip, devip;
DevInfo_t DevInfo, *pdevinfo;
char macbuf[20], ipbuf[20], sql[SQL_CMD_SIZE], newip4;
struct sockaddr_in saddr;

	//判断是否是由于主单元(server)IP信息变化引起重连
	GetSelfMac("eth0", ipbuf);
	MACToStr(ipbuf, macbuf);
	selfip = GetSelfIp("eth0");
	memcpy(&saddr.sin_addr, &selfip, 4);
	inet_ntop(AF_INET, (void *)&saddr.sin_addr, ipbuf, sizeof(ipbuf));
	//主单元信息更新
	sprintf(sql, "UPDATE %s SET IP=\'%s\' WHERE Mac=\'%s\';", CLIENT_TBL, ipbuf, macbuf);
	SqliteUpdate(sql);//更新sqtbl数据表中对应数据

	MACToStr(p_reqmsg->RRU_Mac, macbuf);
	sprintf(sql, "SELECT * FROM %s WHERE Mac=\"%s\";", CLIENT_TBL, macbuf);
	res = SqliteSelect(sql, &sqlres);

	if (res == SQLITE_OK)
	{
		//SqlResultDis(&sqlres);
		if(sqlres.nRow > 0)//此MAC存在
		{
			//与主单元MAC相同
			if (atoi(sqlres.dbResult[sqlres.nColumn + CLIENTTBL_DEVTYPE]) == MAIN_UNIT)
			{
				DEBUGOUT("%s与主单元MAC相同,系统错误!\r\n", macbuf);
				memset(p_ackmsg, 0, sizeof(UDPAckMsg_t));
				sqlite3_free_table(sqlres.dbResult);//释放掉sqlres.dbResult的内存空间
				return -1;
			}
			else
			{
				goto _CONFIGDEVIP;
			}
		}
		else//此MAC不存在
		{
			DEBUGOUT("此MAC不存在!\r\n");
			MACToStr(p_reqmsg->RRU_Mac, macbuf);
			//数据表中增加从机信息,(Fd,DevNo,Mac,IP,DevType,LastComTime)
			sprintf(sql, "INSERT INTO %s(Mac,DevType) VALUES (\'%s\',%d);", CLIENT_TBL, macbuf, p_reqmsg->DevType);
			SqliteInsert(sql);//记录插入数据库
		}
	}
	else
	{
		DEBUGOUT("数据库client_info表读错误!\r\n");
		memset(p_ackmsg, 0, sizeof(UDPAckMsg_t));
		sqlite3_free_table(sqlres.dbResult);//释放掉sqlres.dbResult的内存空间
		return -1;
	}
//IP地址处理
_CONFIGDEVIP:
	devip = p_reqmsg->IPAddr;
	MACToStr(p_reqmsg->RRU_Mac, macbuf);
	memcpy((char *)&devip, (char *)&selfip, 3);//分配在同一网段IP
	newip4 = DEFAULT_STARTIP;//从2查询IP是否有相同IP,如有相同的,IP重新分配
	if ((devip == selfip) || (((char *)&devip+3) == DEFAULT_STARTIP))//与主单元IP相同or IP=X.X.X.0/1
	{
		memcpy(((char *)&devip+3), &newip4, 1);
	}
	
_RECONFIGDEVIP:
	memcpy(&saddr.sin_addr, &devip, 4);
	inet_ntop(AF_INET, (void *)&saddr.sin_addr, ipbuf, sizeof(ipbuf));
	sprintf(sql, "SELECT * FROM %s WHERE IP=\'%s\';", CLIENT_TBL, ipbuf);
	res = SqliteSelect(sql, &sqlres);
	
	if (res == SQLITE_OK)
	{
		if(sqlres.nRow > 0)//此IP已经存在
		{
			//SqlResultDis(&sqlres);
			//MAC地址不同
			printf("dbResultMAC[%d]=%s\r\n", sqlres.nColumn, sqlres.dbResult[sqlres.nColumn + CLIENTTBL_MAC]);
			if (strcmp(sqlres.dbResult[sqlres.nColumn + CLIENTTBL_MAC], macbuf) != 0)
			{
				//重新分配IP
				printf("重新分配IP\r\n");
				memcpy(((char *)&devip+3), &newip4, 1);
				newip4++;
				if (newip4 == CONNECT_TOTALNUMBER)
				{
					DEBUGOUT("UDP Broadcast IP Record Full!\r\n");
				}
				else
				{
					goto _RECONFIGDEVIP;
				}
			}
		}
	}
	p_reqmsg->IPAddr = devip;
	memcpy(&saddr.sin_addr, &devip, 4);
	inet_ntop(AF_INET, (void *)&saddr.sin_addr, ipbuf, sizeof(ipbuf));
	//新增设备IP信息更新
	sprintf(sql, "UPDATE %s SET IP=\'%s\' WHERE Mac=\'%s\';", CLIENT_TBL, ipbuf, macbuf);
	SqliteUpdate(sql);//更新sqtbl数据表中对应数据

	//DevNo处理
	devip = p_reqmsg->DevNo;
	newip4 = 1;//从1查询DevNo是否有相同DevNo,如有相同的,DevNo重新分配
	if (devip == 0)//与主单元DevNo相同
	{
		devip = newip4;
	}

_RECONFIGDEVNO:
	sprintf(sql, "SELECT * FROM %s WHERE DevNo=%d;", CLIENT_TBL, devip);
	res = SqliteSelect(sql, &sqlres);

	if (res == SQLITE_OK)
	{
		if(sqlres.nRow > 0)//此DevNo存在,重新分配DevNo
		{
			printf("dbResultMAC[%d]=%s\r\n", sqlres.nColumn, sqlres.dbResult[sqlres.nColumn + CLIENTTBL_MAC]);
			if (strcmp(sqlres.dbResult[sqlres.nColumn + CLIENTTBL_MAC], macbuf) != 0)
			{
				printf("重新分配DevNo\r\n");
				devip = newip4;
				newip4++;
				if (newip4 == CONNECT_TOTALNUMBER)
				{
					DEBUGOUT("DevNo Record Full!\r\n");
				}
				else
				{
					goto _RECONFIGDEVNO;
				}
			}
		}
	}
	p_reqmsg->DevNo = devip;
	sprintf(sql, "UPDATE %s SET DevNo=%d WHERE Mac=\'%s\';", CLIENT_TBL, devip, macbuf);
	SqliteUpdate(sql);//更新sqtbl数据表中对应数据
					
	p_ackmsg->BBU_OptInterface = p_reqmsg->BBU_OptInterface;
  	p_ackmsg->RRU_ID = p_reqmsg->RRU_ID;
  	p_ackmsg->BBU_ID = g_DevType;
	memcpy(p_ackmsg->RRU_Mac, p_reqmsg->RRU_Mac, sizeof(p_reqmsg->RRU_Mac));
	p_ackmsg->RRU_IP = p_reqmsg->IPAddr;
	p_ackmsg->BBU_IP = GetSelfIp("eth0");

 	pdevinfo = &DevInfo;
	memset(pdevinfo, 0, sizeof(DevInfo_t));
	DbGetParaValue(pdevinfo, DEVNETMASK_ID, &pdu);
	memcpy(&selfip, &pdu.var, pdu.len);
	p_ackmsg->NetMask = selfip;

	DbGetParaValue(pdevinfo, STATIONNO_ID, &pdu);
	memcpy(&selfip, &pdu.var, pdu.len);
	p_ackmsg->StationNo = selfip;
	p_ackmsg->DevNo = p_reqmsg->DevNo;
	return 1;
}

/*******************************************************************************
*函数名称 : INT32U UDPBroadcastMsnDeal(char *pmac)
*功    能 : UDPBroadcastMsnDeal处理函数
*输入参数 : char *pmac:新连接MAC地址
*输出参数 : IP地址
*******************************************************************************/ 
INT32U UDPBroadcastMsnDeal(char *pmac)
{
int i, j, k, flag;
ClientBuf_t *pcbuf;
INT32U selfip;
char macbuf[MACSIZE], newip4, ip4;

  //查找连接记录,看是否已经存在该记录
printf("\r\nMAC:");
for(i=0;i<6;i++)
printf(":%02X",*pmac++);
printf("\r\n\r\n");
char ipbuf[20];
struct sockaddr_in saddr;

  flag = 0;
  for (i = 0; i < CONNECT_TOTALNUMBER; i++)
  {
    pcbuf = &g_TCPClient[i];
    //如果已经存在连接记录信息,判断是否是来自同一MAC地址
    if (MACCompare(pcbuf->Mac, pmac) == 1)//mac地址相同,保持原有IP地址不变,将设备分配为同一网段设备
    {
      flag = 1;
      memcpy(&ip4, ((char *)&pcbuf->IPAddr+3), 1);
      
memcpy(&saddr.sin_addr, &pcbuf->IPAddr, 4);
 inet_ntop(AF_INET, (void *)&saddr.sin_addr, ipbuf, sizeof(ipbuf));
printf("Saved ip:%s\r\n",ipbuf);

      if (ip4 == 0)//无效的IP
      {
        pcbuf->IPAddr = 0;
        goto _CONFIGNEWIP;
      }
      else
      {
        selfip = GetSelfIp("eth0");
        memcpy((char *)&pcbuf->IPAddr, (char *)&selfip, 3);//分配在与主单元同一网段IP
        if (pcbuf->IPAddr == selfip)//与主单元IP相同
        {

memcpy(&saddr.sin_addr, &pcbuf->IPAddr, 4);
  inet_ntop(AF_INET, (void *)&saddr.sin_addr, ipbuf, sizeof(ipbuf));
printf("pcbuf->IPAddr == selfip:%s\r\n",ipbuf);

          pcbuf->IPAddr = 0;
          goto _CONFIGNEWIP;
        }
        else
        {

memcpy(&saddr.sin_addr, &pcbuf->IPAddr, 4);
 inet_ntop(AF_INET, (void *)&saddr.sin_addr, ipbuf, sizeof(ipbuf));
printf("pcbuf->IPAddr:%s\r\n",ipbuf);

          return pcbuf->IPAddr;
        }
      }
    }
  }

  //如果不存在该记录,将新客户端的加入数组
  if (flag == 0)
  {
    memset(macbuf, 0, sizeof(macbuf));
    for (i = 0; i < CONNECT_TOTALNUMBER; i++)
    {
      pcbuf = &g_TCPClient[i];
      if (MACCompare(pcbuf->Mac, macbuf) == 1)//全0,表示无记录,存新MAC地址,分配新的IP
      {
        memcpy(pcbuf->Mac, pmac, MACSIZE);//
_CONFIGNEWIP:
        //MAC地址,分配新IP
        selfip = GetSelfIp("eth0");
        memcpy((char *)&pcbuf->IPAddr, (char *)&selfip, 3);//分配在同一网段IP

memcpy(&saddr.sin_addr, &pcbuf->IPAddr, 4);
  inet_ntop(AF_INET, (void *)&saddr.sin_addr, ipbuf, sizeof(ipbuf));
printf("分配在同一网段IP:%s\r\n",ipbuf);

        newip4 = DEFAULT_STARTIP;//从2查询IP是否有相同IP,如有相同的,IP增加1
        for (j = 0; j < CONNECT_TOTALNUMBER; j++)
        {
          for (k = 0; k < CONNECT_TOTALNUMBER; k++)//IP按顺序增加
          {
            memcpy(&ip4, ((char *)&selfip+3), 1);
            if (newip4 == ip4)//与主单元IP相同
            {
printf("与主单元IP相同\r\n");
              break;
            }

            memcpy(&ip4, ((char *)&g_TCPClient[k].IPAddr+3), 1);
            if (newip4 == ip4)//IP地址已有
            {
            	memcpy(((char *)&pcbuf->IPAddr+3), &newip4, 1);
            	//memset(((char *)&pcbuf->IPAddr+3), newip4, 1);
printf("IP地址已有,IP:%d\r\n",ip4);
printf("IPAddr:%d", pcbuf->IPAddr);
memcpy(&saddr.sin_addr, &pcbuf->IPAddr, 4);
  inet_ntop(AF_INET, (void *)&saddr.sin_addr, ipbuf, sizeof(ipbuf));
printf("IP地址:%s\r\n",ipbuf);
							return pcbuf->IPAddr;
            }
          }
          if (k == CONNECT_TOTALNUMBER)
          {
printf("newip4:%d", newip4);
memcpy(&pcbuf->IPAddr, &selfip, 3);//分配在同一网段IP
printf("selfip:%d\r\n", pcbuf->IPAddr);
            memcpy(((char *)&pcbuf->IPAddr+3), &newip4, 1);
printf("IPAddr:%d\r\n", pcbuf->IPAddr);
//printf("pcbuf:%d\r\n", pcbuf);
pcbuf=&g_TCPClient[i];
//printf("pcbuf:%d\r\n", pcbuf);
memcpy(&saddr.sin_addr, &pcbuf->IPAddr, 4);
  inet_ntop(AF_INET, (void *)&saddr.sin_addr, ipbuf, sizeof(ipbuf));
printf("newip4:%d;新分配的IP:%s\r\n", newip4, ipbuf);

printf("i=:%d;\r\n", i);
memcpy(&saddr.sin_addr, &g_TCPClient[i].IPAddr, 4);
  inet_ntop(AF_INET, (void *)&saddr.sin_addr, ipbuf, sizeof(ipbuf));
printf("新分配的IP:%s\r\n",ipbuf);
            return pcbuf->IPAddr;
          }
          newip4++;
        }
        //记录已满
        if(j == CONNECT_TOTALNUMBER)
        {
          DEBUGOUT("UDP Broadcast IP Record Full!\r\n");
        }
      }
    }
    //记录已满
    if(i == CONNECT_TOTALNUMBER)
    {
      DEBUGOUT("Not find Matching MAC!\r\n");
    }
  }
  return 0;
}

struct st
{
	char r;
	int a,b;
};

/*******************************************************************************
*函数名称 : void *UDPCom_Thread(void *pvoid)
*功    能 : UDP通讯线程
*输入参数 : none
*输出参数 : none
*******************************************************************************/ 
void *UDPCom_Thread(void *pvoid)
{
int resum;
char ipstr[20];
struct in_addr inaddr;
INT32U broadcast_ip;
ComBuf_t *p_combuf;
Pdu_t pdu;
DevicePara_t *pdevpara;
DevInfo_t DevInfo, *pdevinfo;
time_t starttime;
UDPRequestMsg_t reqmsg;
UDPAckMsg_t ackmsg;

  pthread_detach(pthread_self());
  pthread_setcancelstate(PTHREAD_CANCEL_ENABLE, NULL);
  pthread_setcanceltype(PTHREAD_CANCEL_ASYNCHRONOUS, NULL);//线程设置为立即取消

  UDPComInit();
  
  starttime = time(NULL)-NETCONNETINTERVAL+3;
  printf("UDPCom_Thread Run!\r\n");
  sprintf(ipstr, "192.168.10.255");
  inet_pton(AF_INET, ipstr, (void *)&inaddr);
  broadcast_ip = inaddr.s_addr;
  while(1)
  {
    sleep(1);
    p_combuf = &g_UDPCom;	//modified by kevin 2015-5-5
    p_combuf->Timer = (int)(time(NULL) - starttime);
    pdevinfo = &DevInfo;

    if (p_combuf->Status == NET_DISCONNET)
    {
      if (g_DevType == MAIN_UNIT)
        SocketClientConnet(p_combuf, 0, 0, broadcast_ip, IRUDPRMUPORT, UDP_BROADCAST_MODE);
      else
        SocketClientConnet(p_combuf, 0, 0, broadcast_ip, IRUDPRAUPORT, UDP_BROADCAST_MODE);
    }
    if (p_combuf->Status == NET_CONNET)
    {
      if (g_DevType == MAIN_UNIT)
      {
        resum = UDPSocketReceiveData(p_combuf, broadcast_ip, IRUDPRAUPORT, COMWAITTIME);
        if (resum > 0)
        {
          DEBUGOUT("Fd:%d.UDPCom_Thread ReceiveData:%d.\r\n", p_combuf->Fd, p_combuf->RecvLen);
          ComDataHexDis(p_combuf->Buf, p_combuf->RecvLen);
          
          memcpy(&reqmsg, p_combuf->Buf, sizeof(reqmsg));
          if (UDPMainMsnDeal(&reqmsg, &ackmsg) == 1)
         	{
          	UDPSocketSendData(p_combuf->Fd, (char *)&ackmsg, sizeof(ackmsg), broadcast_ip, IRUDPRAUPORT);
          }
          ClearComBuf(p_combuf);
        }
      }
      else
      {
        resum = UDPSocketReceiveData(p_combuf, broadcast_ip, IRUDPRMUPORT, COMWAITTIME);
        if (resum > 0)
        {
          DEBUGOUT("Fd:%d.UDPCom_Thread ReceiveData:%d.\r\n", p_combuf->Fd, p_combuf->RecvLen);
          ComDataHexDis(p_combuf->Buf, p_combuf->RecvLen);
          
          memcpy(&ackmsg, p_combuf->Buf, sizeof(ackmsg));
          GetSelfMac("eth0", reqmsg.RRU_Mac);
          if (MACCompare(ackmsg.RRU_Mac, reqmsg.RRU_Mac) == 1)//判断是否是本机MAC,设置IP地址
          {
            pdevpara = &g_DevicePara;
            pdevinfo = &DevInfo;

            memset(pdevinfo, 0, sizeof(DevInfo_t));
            memset(pdu.var, 0, sizeof(pdu.var));
						//设备IP
            memcpy(&pdevpara->DeviceIP, &ackmsg.RRU_IP, 4);
            SetDevIPpara("eth0", pdevpara->DeviceIP, SET_IP);
	          pdu.len = 4 ;
            memcpy(pdu.var, &ackmsg.RRU_IP, 4);
            DbSaveParaValue(pdevinfo, DEVICEIP_ID, &pdu);
            //设备默认网关:默认xx.xx.xx.1
            memset(((char *)&ackmsg.RRU_IP+3), 1, 1);
            memcpy(&pdevpara->DeviceGateway, &ackmsg.RRU_IP, 4);
            SetDevIPpara("eth0", pdevpara->DeviceGateway, SET_GATEWAY);
	          pdu.len = 4 ;
            memcpy(pdu.var, &ackmsg.RRU_IP, 4);
            DbSaveParaValue(pdevinfo, DEVDEFAULTGW_ID, &pdu);
						//设备子网掩码
			/*
            memcpy(&pdevpara->DeviceNetmask, &ackmsg.NetMask, 4);
            SetDevIPpara("eth0", pdevpara->DeviceNetmask, SET_NETMASK);
	          pdu.len = 4 ;
            memcpy(pdu.var, &ackmsg.NetMask, 4);
            DbSaveParaValue(pdevinfo, DEVNETMASK_ID, &pdu);//存储子网掩码
            */
            //站点编号StationNo
            pdevpara->StationNo = ackmsg.StationNo;
	          pdu.len = 4 ;
	          memcpy(pdu.var, &ackmsg.StationNo, 4);
            DbSaveParaValue(pdevinfo, STATIONNO_ID, &pdu);
						//设备DeviceNo
						memset(pdu.var, 0, sizeof(pdu.var));
            pdevpara->DeviceNo = ackmsg.DevNo;
	          pdu.len = 1 ;
	          pdu.var[0] = pdevpara->DeviceNo;
            DbSaveParaValue(pdevinfo, DEVICENO_ID, &pdu);
            
			memcpy(&serverip, &ackmsg.BBU_IP, 4);//主单元IP
						
            memset((char *)&ackmsg, 0, sizeof(UDPAckMsg_t));
            SocketClientDisconnet(p_combuf);
            goto _UDPCONFIGNEWIP_EXIT;
          }
          ClearComBuf(p_combuf);
        }
        else if (p_combuf->Timer > IRCONNETINTERVAL)
        {
        	pdevpara = &g_DevicePara;
          starttime = time(NULL);
          reqmsg.BBU_OptInterface = 1;
          reqmsg.RRU_ID = g_DevType;
          GetSelfMac("eth0", reqmsg.RRU_Mac);//RRU MAC地址,6BYTE
          //自定义数据
          reqmsg.DevNo = pdevpara->DeviceNo;
          reqmsg.IPAddr = GetSelfIp("eth0");
          reqmsg.DevType = g_DevType;
          UDPSocketSendData(p_combuf->Fd, (char *)&reqmsg, sizeof(reqmsg), broadcast_ip, IRUDPRMUPORT);
        }
      }
    }
    else
    {
      ClearComBuf(p_combuf);
      p_combuf->Status = NET_DISCONNET;
    }
  }
_UDPCONFIGNEWIP_EXIT:
  g_UDPComThread.ThreadStatus = THREAD_STATUS_EXIT;
  g_UDPComThread.Tid = 0;
  DEBUGOUT("UDPCom_Thread Exit!\r\n");
  pthread_exit(NULL);
}

/*******************************************************************************
*函数名称 : int UDPComInit(void)
*功    能 : UDP通讯配置函数
*输入参数 : None
*输出参数 : Fd
*******************************************************************************/
void UDPComInit(void)
{
  g_UDPCom.Fd = -1;
  g_UDPCom.RecvLen = 0;
  g_UDPCom.Status = NET_DISCONNET;

  memset(g_UDPCom.Buf, 0, COMBUF_SIZE);
  memset(&g_TCPClient, 0 , sizeof(g_TCPClient));
  if (g_DevType == MAIN_UNIT)
  {
  	ClientDbTableInit(CLIENT_TBL);
  }
  UDPComThreadInit();
}

/*******************************************************************************
*函数名称 : void UDPComThreadInit(void)
*功    能 : UDP通讯线程初始化
*输入参数 : none
*输出参数 : none
*******************************************************************************/ 
void UDPComThreadInit(void)
{
  g_UDPComThread.Tid = 0;
  g_UDPComThread.ThreadStatus = THREAD_STATUS_EXIT;
}

/*******************************************************************************
*函数名称 : void UDPComThreadStart(void)
*功    能 : 开始UDP通讯线程
*输入参数 : none
*输出参数 : none
*******************************************************************************/ 
void UDPComThreadStart(void)
{
  if (g_UDPComThread.ThreadStatus != THREAD_STATUS_RUNNING)
  {
    pthread_create(&g_UDPComThread.Tid, NULL, UDPCom_Thread, NULL);
    g_UDPComThread.ThreadStatus = THREAD_STATUS_RUNNING;
    printf("UDPCom_Thread ID: %lu.\n", g_UDPComThread.Tid);
  }
}

/*******************************************************************************
*函数名称 : void UDPComThreadStop(void)
*功    能 : 停止UDP通讯线程
*输入参数 : none
*输出参数 : none
*******************************************************************************/ 
void UDPComThreadStop(void)
{
  if (g_UDPComThread.ThreadStatus != THREAD_STATUS_EXIT)
  {
    pthread_cancel(g_UDPComThread.Tid);
    g_UDPComThread.Tid = 0;
    g_UDPComThread.ThreadStatus = THREAD_STATUS_EXIT;
    printf("UDPCom_Thread Stop!\r\n");
  }
}

/*******************************************************************************
*函数名称 : int DbTableInit(char *tblname)
*功    能 : Client数据表初始化
*输入参数 : char *tblname:数据表名称
*输出参数 : 成功返回1,出错返回<0
*******************************************************************************/
int ClientDbTableInit(char *tblname)
{
int res, result;
INT32U selfip;
SqlResult_t sqlres;
char macbuf[20], ipbuf[20], sql[SQL_CMD_SIZE];
struct sockaddr_in saddr;

  //查找tbl数据表是否存在,如不存在则建立
  sprintf(sql, "SELECT * FROM sqlite_master WHERE name=\'%s\';", tblname);
  res = SqliteSelect(sql, &sqlres);
  if(res == SQLITE_OK)
  {
  	if(sqlres.nRow > 0)//tblname数据表存在
  	{
      sprintf(sql, "DROP TABLE %s;", tblname);
      res = SqliteCreate(sql);//创建sqtbl数据表
      if(res != SQLITE_OK)//创建sqtbl数据表失败
     	{
     		DEBUGOUT("ClientDbTableInit:Drop %s Failure!\r\n", CLIENTTBL_HEAD);
			  sqlite3_free_table(sqlres.dbResult);
			  return res;
     	}
  	}

    sprintf(sql, "CREATE TABLE %s%s;", tblname, CLIENTTBL_HEAD);
    res = SqliteCreate(sql);//创建sqtbl数据表
    if(res != SQLITE_OK)//创建sqtbl数据表失败
    {
      DEBUGOUT("ClientDbTableInit:Sqlite Create %s Failure!\r\n", tblname);
      result = res;
    }
    else
    {
      //增加server信息
      GetSelfMac("eth0", ipbuf);
      MACToStr(ipbuf, macbuf);
      selfip = GetSelfIp("eth0");
			memcpy(&saddr.sin_addr, &selfip, 4);
  		inet_ntop(AF_INET, (void *)&saddr.sin_addr, ipbuf, sizeof(ipbuf));
			//数据表中增加主单元信息,(Fd,DevNo,Mac,IP,DevType,LastComTime)
			sprintf(sql, "INSERT INTO %s(DevNo,Mac,IP,DevType) VALUES (0,\'%s\',\'%s\',1);", CLIENT_TBL, macbuf, ipbuf);
			SqliteInsert(sql);//记录插入数据库
      DEBUGOUT("ClientDbTableInit:Sqlite Create %s Success!\r\n", tblname);
      result = 1;
    }
  }
  else
  {
  	result = res;
  }
  sqlite3_free_table(sqlres.dbResult);//释放掉sqlres.dbResult的内存空间
  return result;
}
/*********************************End Of File*************************************/
