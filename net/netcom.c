/********************  COPYRIGHT(C)***************************************
**--------------文件信息--------------------------------------------------------
**文   件   名: netcom.c
**创   建   人: 于宏图
**创 建  日 期:
**程序开发环境：
**描        述: 网络处理程序
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
#include "../protocol/approtocol.h"
#include "../protocol/apbprotocol.h"
#include "../protocol/apcprotocol.h"
#include "netcom.h"

struct sockaddr_in Receive_addr;	//接收到的数据ip地址
extern DevicePara_t g_DevicePara;

/**********************************************************************
* 函数名称：int GetNetlinkStatus(const char *if_name) 
* 功能描述：检测网络链接是否断开
* 输入参数： const char *（要检测的网口，如：eth0、eth1）
* 输出参数：正常链接0,断开返回-1
***********************************************************************/
int GetNetlinkStatus(const char *if_name)
{
struct ifreq ifr;
int res, skfd = socket(AF_INET, SOCK_DGRAM, 0);

	strcpy(ifr.ifr_name, if_name);
	res = -1;
	if (ioctl(skfd, SIOCGIFFLAGS, &ifr) < 0)
	{
		res = -1;
	}
	else
	{
		if(ifr.ifr_flags & IFF_RUNNING)
		{
			res = 0;//网卡已插上网线
		}
	 	else
	 	{
	 		res = -1;
	 	}
	}
	close(skfd);
	if (res == 0)
	{
		usleep(10000);
	}
	else
	{
		sleep(1);
	}
	return res;
}

/*******************************************************************************
*函数名称 : int GetSelfMac(char *ethname, char *pbuf)
*功    能 : 获取本机网卡"devname"的MAC地址
*输入参数 : char *devname:要获取的设备名称,eth0/ppp0;char *pbuf:回传指针
*输出参数 : 成功返回1
*******************************************************************************/
int GetSelfMac(char *ethname, char *pbuf)
{
int i, skfd;
char buf[50];
struct ifreq ifr;

  skfd = socket(AF_INET, SOCK_STREAM, 0);
  if (skfd == -1)
  {
    sprintf(buf, "%s socket:", ethname);
    perror(buf);
    return 0;
  }
  strcpy(ifr.ifr_name, ethname);
  if (ioctl(skfd, SIOCGIFHWADDR, &ifr) < 0)
  {
    sprintf(buf, "获取 %s MAC Error:", ethname);
    perror(buf);
    close(skfd);
    return 0;
  }
  close(skfd);
  for (i = 0; i < 6; i++)
  {
    *pbuf++ = (char)ifr.ifr_hwaddr.sa_data[i];
  }
  for (i = 0; i < 6; i++)
  {
    if ((char)ifr.ifr_hwaddr.sa_data[i] != 0)
    {
      DEBUGOUT("%s MAC", ethname);
      for (i = 0; i < 6; i++)
        DEBUGOUT(":%02X", ifr.ifr_hwaddr.sa_data[i]);
      DEBUGOUT("\r\n");
      return 1;
    }
  }
  return 0;
}

/*******************************************************************************
*函数名称 : INT32U GetSelfIp(char *ethname)
*功    能 : 获取本机网卡"devname"的ip地址,有gprs通讯时存在多网卡
*输入参数 : char *devname:要获取的设备名称,eth0/ppp0
*输出参数 : 成功获取网卡ip地址,返回的是网络字节序的IP地址
*******************************************************************************/
INT32U GetSelfIp(char *ethname)
{
int skfd;
char buf[50];
struct ifreq ifr; 
struct sockaddr_in saddr;

  skfd = socket(AF_INET, SOCK_STREAM, 0);
  if (skfd == -1)
  {
    DEBUGOUT(buf, "%s socket:", ethname);
    perror(buf);
    return 0;
  }
  strcpy(ifr.ifr_name, ethname);
  if (ioctl(skfd, SIOCGIFADDR, &ifr) < 0)
  {
    DEBUGOUT(buf, "获取 %s IP Error:", ethname);
    perror(buf);
    close(skfd);
    return 0;
  }
  close(skfd);

  memcpy(&saddr, &ifr.ifr_addr, sizeof(saddr));
  inet_ntop(AF_INET, (void *)&saddr.sin_addr, buf, sizeof(buf));
  DEBUGOUT("%s IP:%s\r\n", ethname, buf);
  return saddr.sin_addr.s_addr;
}

/*******************************************************************************
*函数名称 : int SetDevIPpara(char *ethname, INT32U ip, char settype)
*功    能 : 设置本机网卡"devname"的ip地址,有gprs通讯时存在多网卡
*输入参数 : char *devname:要获取的设备名称,eth0/ppp0;INT32U ip:设备IP地址,4字节长整数型数,char settype:IP地址,子网掩码,网关
*输出参数 : 成功设置网卡ip地址,返回1,否则返回-1
*******************************************************************************/
int SetDevIPpara(char *ethname, INT32U ip, char settype)
{
int skfd, res = 0;
char buf[50], ipbuf[20];
struct ifreq ifr; 
struct sockaddr_in saddr;

  skfd = socket(AF_INET, SOCK_STREAM, 0);
  if (skfd == -1)
  {
    sprintf(buf, "%s socket:", ethname);
    perror(buf);
    return 0;
  }
  strcpy(ifr.ifr_name, ethname);
  saddr.sin_family = AF_INET;
  memcpy(&saddr.sin_addr, &ip, 4);
  memcpy(&ifr.ifr_addr, &saddr, sizeof(saddr));
  
  inet_ntop(AF_INET, (void *)&saddr.sin_addr, ipbuf, sizeof(ipbuf));
  if (settype == SET_IP)
  {
    DEBUGOUT("%s Set IP:%s\r\n", ethname, ipbuf);
    res = ioctl(skfd, SIOCSIFADDR, &ifr);
  }
  else if (settype == SET_NETMASK)
  {
    DEBUGOUT("%s Set NetMask:%s\r\n", ethname, ipbuf);
    res = ioctl(skfd, SIOCSIFNETMASK, &ifr);
  }
  else if (settype == SET_GATEWAY)
  {
    sprintf(buf, "/sbin/route add default gw %s\r", ipbuf);
    system(buf);
    DEBUGOUT("%s Set Gateway:%s.\r\n", ethname, ipbuf);
    res = 1;
  }
  if (res < 0)
  {
    sprintf(buf, "Set %s Error:", ethname);
    perror(buf);
    close(skfd);
    return 0;
  }
  close(skfd);
  return 1;
}

/*******************************************************************************
*函数名称 : int SocketServerInit(int serverport, char iptype)
*功    能 : SocketServerInit连接函数
*输入参数 : int serverport:server IP地址端口号;char iptype:IP连接类型tcp/udp
*输出参数 : fd
*******************************************************************************/
int SocketServerInit(int serverport, char iptype)
{
int er, skfd = -1;
struct sockaddr_in skaddr;

  if (iptype == IP_TCP_MODE)
  {
    skfd = socket(AF_INET, SOCK_STREAM, 0);
    DEBUGOUT("IP+TCP:");
  }
  else if (iptype == IP_UDP_MODE)
  {
    skfd = socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP);
    DEBUGOUT("IP+UDP:");
  }
  if (skfd < 0)
  {
    DEBUGOUT("SocketServerInit:Create Socket Failed!\r\n");
    return skfd;
  }
  er = 1;
  //bind属性设置,默认情况下,一个端口是不允许重复绑定的,通过以下设置来允许端口重复绑定
  setsockopt(skfd, SOL_SOCKET, SO_REUSEADDR, &er, sizeof(er));
  
  //把客户机的socket和客户机的IP地址,端口联系起来
  bzero(&skaddr, sizeof(skaddr));
  skaddr.sin_family = AF_INET;
  skaddr.sin_port = htons(serverport);
  skaddr.sin_addr.s_addr = INADDR_ANY;//本机IP
  if (bind(skfd, (struct sockaddr *)&skaddr, sizeof(skaddr)) < 0)
  {
    DEBUGOUT("SocketServerInit:Bind Port Failed!\r\n"); 
    perror("Socket bind:");
    return -1;
  }
  if (listen(skfd, SOMAXCONN) == -1)//调用listen开始监听
  {
    DEBUGOUT("SocketServerInit:Listen Error!\r\n"); 
    return -1;
  }
  return skfd;
}

/*******************************************************************************
*函数名称 : int SocketClientConnet(struct combuf *pcombuf, char *devname, char *server_ip, int server_port)
*功    能 : SocketClient连接函数
*输入参数 : ComBuf_t *p_combuf:对应设备参数;char *ethname:与网络连接设备名;
            INT32U serverip:server IP地址; int serverport:server IP地址端口号;
            INT32U devip:设备IP地址; int devport:设备IP地址端口号;char iptype:IP连接类型tcp/udp
*输出参数 : fd
*******************************************************************************/
int SocketClientConnet(ComBuf_t *pcombuf, INT32U serverip, int serverport, INT32U devip, int devport, char iptype)
{
int result, er;
socklen_t len;
fd_set fdr, fdw;
struct timeval  tm;
struct in_addr ipadder;
struct sockaddr_in skaddr;

  memcpy(&ipadder, &serverip, 4);
  DEBUGOUT("SocketClientConnet:ServerIp:%s;ServerIPPort:%d.\r\n", inet_ntoa(ipadder), serverport);
  memcpy(&ipadder, &devip, 4);
  DEBUGOUT("SocketClientConnet:DevIp:%s;DevIPPort:%d.\r\n", inet_ntoa(ipadder), devport);

  if (iptype == IP_TCP_MODE)
  {
    pcombuf->Fd = socket(AF_INET, SOCK_STREAM, 0);
    DEBUGOUT("IP+TCP:");
  }
  else if ((iptype == IP_UDP_MODE) || (iptype == UDP_BROADCAST_MODE))
  {
    pcombuf->Fd = socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP);
    DEBUGOUT("IP+UDP:");
  }
  if (pcombuf->Fd < 0)
  {
    DEBUGOUT("Create Socket Failed!\r\n");
    pcombuf->Status = NET_DISCONNET;
    close(pcombuf->Fd);
    return pcombuf->Fd;
  }
  er = 1;
  //bind属性设置,默认情况下,一个端口是不允许重复绑定的,通过以下设置来允许端口重复绑定
  setsockopt(pcombuf->Fd, SOL_SOCKET, SO_REUSEADDR, &er, sizeof(er));
  if (iptype == UDP_BROADCAST_MODE)//允许广播
  {
    er = 1;
    setsockopt(pcombuf->Fd, SOL_SOCKET, SO_BROADCAST, &er, sizeof(er));
  }

  //把客户机的socket和客户机的IP地址,端口联系起来
  bzero(&skaddr, sizeof(skaddr));
  skaddr.sin_family = AF_INET;
  skaddr.sin_port = htons(devport);//设备端口
  skaddr.sin_addr.s_addr = devip;//设备IP
  if (bind(pcombuf->Fd, (struct sockaddr *)&skaddr, sizeof(struct sockaddr)) < 0)
  {
    DEBUGOUT("Client Bind Port Failed!\n"); 
    close(pcombuf->Fd);
    return -1;
  }
  er = 4*1024;//设置为4K
  setsockopt(pcombuf->Fd, SOL_SOCKET, SO_RCVBUF, &er, sizeof(er));//接收缓冲区
  setsockopt(pcombuf->Fd, SOL_SOCKET, SO_SNDBUF, &er, sizeof(er));//发送缓冲区

  if (iptype == IP_TCP_MODE)
  {
    DEBUGOUT("正在与服务器连接......\r\n");
    //获取建立的pcombuf->Fd的当前状态
    er = fcntl(pcombuf->Fd, F_GETFL, 0);
    if (er < 0)
    {
      perror("fcntl(Socket,F_GETFL,0)");
      close(pcombuf->Fd);
      return -1;
    }
    er = er | O_NONBLOCK;//设置为非阻塞模式
    if (fcntl(pcombuf->Fd, F_SETFL, er) < 0)
    {
      perror("fcntl(Socket,F_SETFL,0)");
      close(pcombuf->Fd);
      return -1;
    }
    
    bzero(&skaddr, sizeof(skaddr));
    skaddr.sin_family = AF_INET;
    skaddr.sin_port = htons(serverport);//服务器端口
    skaddr.sin_addr.s_addr = serverip;//服务器IP
  //connect调用后,无论连接是否建立立即返回-1,同时将errno(包含errno.h就可以直接使用)设置为EINPROGRESS
  //表示此时tcp三次握手仍旧进行,如果errno不是EINPROGRESS,则说明连接错误,程序结束。
    if (connect(pcombuf->Fd, (struct sockaddr *)&skaddr, sizeof(skaddr)) < 0)
    {
      //发connect调用,这时返回-1,但是errno被设为EINPROGRESS,意即connect仍旧在进行还没有完成
      if (errno != EINPROGRESS)
      {
        perror("Socket Connect");
        return -1;
      }

      tm.tv_sec = 6;//6s 
      tm.tv_usec = 0; 
      FD_ZERO(&fdr);
      FD_ZERO(&fdw);
      FD_SET(pcombuf->Fd, &fdr);
      FD_SET(pcombuf->Fd, &fdw);

      er = select(pcombuf->Fd + 1, &fdr, &fdw, NULL, &tm);
      if (er > 0)
      {
        if (FD_ISSET(pcombuf->Fd, &fdr) || FD_ISSET(pcombuf->Fd, &fdw))//如果有读或写连接请求
        {
          len = sizeof(er);
          if (getsockopt(pcombuf->Fd, SOL_SOCKET, SO_ERROR, &er, &len) < 0)
            result = 0; 
      
          if (er == 0)//如果连接成功,此调用返回0
            result = 1;
          else
            result = 0; 
        }
      }
      else
      {
        DEBUGOUT("Socket Connect Timeout!\r\n");
        close(pcombuf->Fd);
        result = 0;
      }
    }
    else
    {
      result = 1;
    }

    if (result == 1)
    {
      DEBUGOUT("与服务器(TCP)连接成功!\r\n");
      pcombuf->Status = NET_CONNET;
      return pcombuf->Fd;
    }
    else
    {
      DEBUGOUT("与服务器(TCP)连接失败!\r\n");
      pcombuf->Status = NET_DISCONNET;
      close(pcombuf->Fd);
      return -1;
    }
  }
  else//UDP
  {
    pcombuf->Status = NET_CONNET;
    return pcombuf->Fd;
  }
}

/*******************************************************************************
*函数名称 : void SocketClientDisconnet(struct combuf *pcombuf)
*功    能 : 断开SocketClient连接函数
*输入参数 : struct combuf *pcombuf:对应设备参数
*输出参数 : none
*******************************************************************************/
void SocketClientDisconnet(ComBuf_t *pcombuf)
{
  if (pcombuf->Fd > 0)
  {
    DEBUGOUT("Fd:%d,断开与服务器的连接!\r\n", pcombuf->Fd);
    shutdown(pcombuf->Fd, SHUT_RDWR);//close(pcombuf->Fd);
    pcombuf->Fd = -1;
  }
  pcombuf->Status = NET_DISCONNET;
}

/*******************************************************************************
*函数名称 :	int	TCPSocketReceiveData(ComBuf_t *pcombuf, int waittime)
*功    能 :	接收的数据转存到PackBuf数据缓存区中,并处理
*输入参数 : ComBuf_t *p_combuf:对应设备参数;int waittime:超时时间ms
*输出参数 : 接收到数据长度或错误标识
*******************************************************************************/
int	TCPSocketReceiveData(ComBuf_t *pcombuf, int waittime)
{
int	res, rcsum;
fd_set readfs;
struct timeval tv;

	tv.tv_sec = waittime/1000;
	tv.tv_usec = (waittime%1000)*1000;
	FD_ZERO(&readfs);
	FD_SET(pcombuf->Fd, &readfs);

	res = select(pcombuf->Fd + 1, &readfs, NULL, NULL, &tv);
  if (res > 0)
  {
    rcsum = recv(pcombuf->Fd, &pcombuf->Buf[pcombuf->RecvLen], (COMBUF_SIZE - pcombuf->RecvLen), 0);
    if (rcsum > 0)
    {
  		pcombuf->RecvLen = pcombuf->RecvLen + rcsum;
  		return pcombuf->RecvLen;
  	}
  	else
  	{
  	  perror("TCPSocketReceiveData:recv() error!");
      //由于是非阻塞的模式,所以当errno为EAGAIN时,表示当前缓冲区已无数据可读;在这里表示该事件已做处理
      if (rcsum == -1)
      {
      	if(errno == EINTR || errno == EAGAIN)
        	return pcombuf->RecvLen;
        else
        	goto _SOCKET_SHUTDOWN;
      }
      else
      {
    	  //连接关断
_SOCKET_SHUTDOWN:
        shutdown(pcombuf->Fd, SHUT_RDWR);//close(g_NetCom.Fd);
        pcombuf->Status = NET_DISCONNET;
        DEBUGOUT("TCPSocketReceiveData:recv() Error, SHUT_RDWR!\r\n");
        return -1;
      }
  	}
  }
  else if (res < 0)
  {
    perror("TCPSocketReceiveData:select() Error!");
    return -1;
  }
  return pcombuf->RecvLen;
}

/*******************************************************************************
*函数名称 : int	TCPSocketSendData(int fd, char *sbuf, int len)
*功    能 : TCPSocket->fd 发送数据
*输入参数 : TCPSocket->fd:sbuf:缓存;len:度
*输出参数 : 操作成功返回1，否则返回-1
*******************************************************************************/
int	TCPSocketSendData(int fd, char *sbuf, int len)
{
  //设置为MSG_NOSIGNAL；表示出错后不向系统发信号，否则程序会退出
  if (send(fd, sbuf, len, MSG_NOSIGNAL) == -1)
  {
    perror("can't send message:");
    DEBUGOUT("TCPSocket can't send message!\r\n");//存错误信息
    return -1;
  }
  return 1;
}

/*******************************************************************************
*函数名称 : int TCPSocketSendPack(ComBuf_t *pcombuf, APPack_t *p_packbuf)
*功    能 : 发送协议包,将p_packbuf所指的待发送数据发送给pcombuf所指设备
*输入参数 : ComBuf_t *pcombuf:对应设备参数;APPack_t *p_packbuf:缓存区对应数据包结构指针
*输出参数 : 成功返回1,否则返回-1
*******************************************************************************/
int TCPSocketSendPack(ComBuf_t *pcombuf, APPack_t *p_packbuf)
{
int sdsum;
char sdbuf[COMBUF_SIZE];

	sdsum = 0;
	switch(p_packbuf->APType)
	{
	  case AP_A:
	  case AP_C:
	    sdsum = APCPack(p_packbuf, sdbuf);
    break;
    case AP_B:
      sdsum = APBPack(p_packbuf, sdbuf);
    break;
	}
	if(sdsum > 0)
	{
	  DEBUGOUT("Fd:%d.TCPSocketSendData:%d.\r\n", pcombuf->Fd, sdsum);
		TCPSocketSendData(pcombuf->Fd, sdbuf, sdsum);
   	ComDataWriteLog(sdbuf, sdsum);
   	return 1;
	}
	else
	  return -1;
}

/*******************************************************************************
*函数名称 :	int	UDPSocketReceiveData(ComBuf_t *pcombuf, INT32U serverip, int serverport, int waittime)
*功    能 :	接收的数据转存到PackBuf数据缓存区中,并处理
*输入参数 : ComBuf_t *p_combuf:对应设备参数;
            INT32U serverip:server IP地址; int serverport:server IP地址端口号;int waittime:超时时间ms
*输出参数 : 接收到数据长度或错误标识
*******************************************************************************/
int	UDPSocketReceiveData(ComBuf_t *pcombuf, INT32U serverip, int serverport, int waittime)
{
	int	res, rcsum;
	socklen_t addlen;
	fd_set readfs;
	struct timeval tv;
	struct sockaddr_in skaddr;

	//----2015-03-10----------
	DevicePara_t *p_devpara;
	p_devpara = &g_DevicePara;
	//------------------------

	tv.tv_sec = waittime/1000;		//SECOND
	tv.tv_usec = (waittime%1000)*1000;	//USECOND
	FD_ZERO(&readfs);
	FD_SET(pcombuf->Fd, &readfs);

	res = select(pcombuf->Fd + 1, &readfs, NULL, NULL, &tv);
  	if (res > 0)
  	{
	    bzero(&skaddr, sizeof(skaddr));
	    skaddr.sin_family = AF_INET;
	    skaddr.sin_port = htons(serverport);//服务器端口
	    skaddr.sin_addr.s_addr = htonl(serverip);//服务器IP
	    addlen = sizeof(struct sockaddr);
	    rcsum = recvfrom(pcombuf->Fd, &pcombuf->Buf[pcombuf->RecvLen], (COMBUF_SIZE - pcombuf->RecvLen),
	                     0, (struct sockaddr *)&skaddr, &addlen);
	    if (rcsum>0)
	    {
			//-------2015-03-10---------------
			if((p_devpara->RW1_OmcIP == skaddr.sin_addr.s_addr)||(p_devpara->RW2_OmcIP == skaddr.sin_addr.s_addr)
				||(p_devpara->OmcIP == skaddr.sin_addr.s_addr))
				Receive_addr.sin_addr.s_addr = skaddr.sin_addr.s_addr;
			//printf("================= Receive_IP = %d\n",skaddr.sin_addr.s_addr);
			//--------------------------------
			
			pcombuf->RecvLen = pcombuf->RecvLen + rcsum;
	  		return pcombuf->RecvLen;
	  	}
	  	else
	  	{
	  	  return -1;
	  	}
  	}
  	else if (res < 0)
	{
		perror("Socket select() error!");
		return -1;
	}
//	printf("UDPSocketReceiveData time out!\n");
	return pcombuf->RecvLen;
}
/*
int	UDPSocketReceiveData(ComBuf_t *pcombuf, INT32U serverip, int serverport, int waittime)
{
	int	res, rcsum;
	int ReceiveBufNum;
	socklen_t addlen;
	fd_set readfs;
	struct timeval tv;
	struct sockaddr_in skaddr;

	tv.tv_sec = waittime/1000;		//SECOND
	tv.tv_usec = (waittime%1000)*1000;	//USECOND
	FD_ZERO(&readfs);
	FD_SET(pcombuf->Fd, &readfs);

	res = select(pcombuf->Fd + 1, &readfs, NULL, NULL, &tv);
	if (res > 0)
	{
		bzero(&skaddr, sizeof(skaddr));
		skaddr.sin_family = AF_INET;
		skaddr.sin_port = htons(serverport);		//服务器端口
		skaddr.sin_addr.s_addr = htonl(serverip);	//服务器IP
		addlen = sizeof(struct sockaddr);
		
		ReceiveBufNum = 0;
		while(1)
		{
			rcsum = recvfrom(pcombuf->Fd, &pcombuf->Buf[pcombuf->RecvLen], (COMBUF_SIZE - pcombuf->RecvLen),
							 0, (struct sockaddr *)&skaddr, &addlen);
			printf("====aaaaaaaaaaaaaaaaaaaaa rcsum = %d,ReceiveBufNum = %d\n", rcsum,ReceiveBufNum);
			
			if (rcsum > 0)
			{
				ReceiveBufNum++;
				pcombuf->RecvLen = pcombuf->RecvLen + rcsum;
				ComDataHexDis(pcombuf->Buf, pcombuf->RecvLen);
				printf("==================== ReceiveDataNumber = %d, ReceiveBufNumber %d\n",rcsum,ReceiveBufNum);
				usleep(2*1000);
			}
			else
			{
				if(ReceiveBufNum>0)
				{
					printf("===================== DataNumber = %d, ReceiveBufNumber %d\n",pcombuf->RecvLen,ReceiveBufNum);
					return pcombuf->RecvLen;
				}
				else
				{
					printf("========================== No Data%d\n");
					return -1;
				}
			}
		}
	}
	else if (res < 0)
	{
		perror("Socket select() error!");
		return -1;
	}
	return pcombuf->RecvLen;
}
*/
/*******************************************************************************
*函数名称 : int	UDPSocketSendData(int fd, char *sbuf, int len, INT32U serverip, int serverport)
*功    能 : UDPSocket->fd 发送数据
*输入参数 : UDPSocket->fd:sbuf:缓存;len:度;INT32U serverip:server IP地址; int serverport:server IP地址端口号;
*输出参数 : 操作成功返回1，否则返回-1
*******************************************************************************/
int	UDPSocketSendData(int fd, char *sbuf, int len, INT32U serverip, int serverport)
{
	struct sockaddr_in skaddr;
		
	bzero(&skaddr, sizeof(skaddr));
	skaddr.sin_family = AF_INET;
	skaddr.sin_port = htons(serverport);//服务器端口
	skaddr.sin_addr.s_addr = serverip;//服务器IP	

	//设置为MSG_NOSIGNAL；表示出错后不向系统发信号，否则程序会退出
	if (sendto(fd, sbuf, len, MSG_NOSIGNAL, (struct sockaddr *)&skaddr, sizeof(skaddr)) == -1)
	{
		perror("can't send message:");
		DEBUGOUT("UDPSocket can't send message!\r\n");//存错误信息
		return -1;
	}

	return 1;
}

/*******************************************************************************
*函数名称 : int UDPSocketSendPack(ComBuf_t *pcombuf, APPack_t *p_packbuf, INT32U serverip, int serverport)
*功    能 : 发送协议包,将p_packbuf所指的待发送数据发送给pcombuf所指设备
*输入参数 : ComBuf_t *pcombuf:对应设备参数;APPack_t *p_packbuf:缓存区对应数据包结构指针;
            INT32U serverip:server IP地址; int serverport:server IP地址端口号;
*输出参数 : 成功返回1,否则返回-1
*******************************************************************************/
int UDPSocketSendPack(ComBuf_t *pcombuf, APPack_t *p_packbuf, INT32U serverip, int serverport)
{
int sdsum;
char sdbuf[COMBUF_SIZE];

	sdsum = 0;
	switch(p_packbuf->APType)
	{
	  case AP_A:
	  case AP_C:
	    sdsum = APCPack(p_packbuf, sdbuf);
    break;
    case AP_B:
      sdsum = APBPack(p_packbuf, sdbuf);
    break;
    default:
			DEBUGOUT("APType Error!\r\n");
    	ClearAPPackBuf(p_packbuf);//清除包
   	break;
	}
	if(sdsum > 0)
	{
	  DEBUGOUT("Fd:%d.UDPSocketSendData:%d.\r\n", pcombuf->Fd, sdsum);
		UDPSocketSendData(pcombuf->Fd, sdbuf, sdsum, serverip, serverport);
   	ComDataWriteLog(sdbuf, sdsum);
   	return 1;
	}
	else
	  return -1;
}
/*
返回查询IP地址
*/
INT32U GetReceiveIP(void)
{
	INT32U ReceiveIP;
	DevicePara_t *p_devpara;
	
	p_devpara = &g_DevicePara;
	ReceiveIP = p_devpara->OmcIP;
	
	if((p_devpara->RW1_OmcIP == Receive_addr.sin_addr.s_addr)||(p_devpara->RW2_OmcIP == Receive_addr.sin_addr.s_addr))
	{
		ReceiveIP = Receive_addr.sin_addr.s_addr;
		//Receive_addr.sin_addr.s_addr = 0;
	}
	printf("=============== OmcIPIP_0 = %d\n" ,p_devpara->OmcIP);
	printf("=============== OmcIPIP_1 = %d\n" ,p_devpara->RW1_OmcIP);
	printf("=============== OmcIPIP_2 = %d\n" ,p_devpara->RW2_OmcIP);
	printf("=============== Send_IP = %d\n" ,ReceiveIP);
	
	return ReceiveIP;
}

/*********************************End Of File*************************************/
