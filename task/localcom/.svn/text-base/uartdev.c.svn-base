/********************  COPYRIGHT(C)  ***************************************
**--------------文件信息--------------------------------------------------------
**文   件   名: uartdev.c
**创   建   人: 于宏图
**创 建  日 期: 
**程序开发环境：
**描        述: 用于串口操作
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
#include "../../protocol/approtocol.h"
#include "../../protocol/apbprotocol.h"
#include "../../protocol/apcprotocol.h"
#include "uartdev.h"

//int  g_UartRecIsrFlag = 0;  //UART接收中断标识,收到1,未收到为0
/*******************************************************************************
*函数名称 : void  Uart_ISR(int  signo, siginfo_t *info,void *context)
*功    能 : 通讯中断信号处理程序,有UART中断g_UartIsrFlag置1
*输入参数 : None
*输出参数 : None
*******************************************************************************/
/*
void Uart_ISR(int signo, siginfo_t *info, void *context)
{
#if PRINTDEBUG
  printf("UART signo:%04x;si_code:%04x.\r\n", signo,info->si_code);
  printf("UARTSIGPOLL:%04x;%04x;%04x;%04x;%04x;%04x.\r\n", POLL_IN,POLL_OUT,POLL_MSG,POLL_ERR,POLL_PRI,POLL_HUP);
#endif
  g_UartRecIsrFlag = 1;
}
*/

/*******************************************************************************
*函数名称 : int OpenCommPortISR(int fd, int comport, struct sigaction saio) 
*功    能 : 中断方式打开串口comport,安装信号处理saio
*输入参数 : 串口号,信号
*输出参数 : fd
*******************************************************************************/
int OpenCommPortISR(int fd, int comport, struct sigaction saio) 
{
char *dev[5]={"/dev/ttyS0","/dev/ttyS1","/dev/ttyS2","/dev/ttyS3","/dev/ttyS4"};

  printf("Open Serial Port%d ISR:\n", comport);
  //开启装置为 non-blocking (读取功能会马上结束返回)
  fd = open(dev[comport], O_RDWR|O_NOCTTY|O_NONBLOCK); 
  if (fd == -1)
  { 
    perror("Failed"); 
    return -1; 
  }
  //在使装置非同步化前, 安装信号处理程序
  //sigaction(SIGIO, &saio, NULL); //SIGPOLL是SISTEM V的信号,BSD系统用SIGIO;在SVR4中,SIGIO与SIGPOLL相同
  sigaction(SIGPOLL, &saio, NULL);
  
  //ioctl(fd1 , I_SETSIG , S_INPUT | S_HANGUP ); //设置当有东西读时产生 SIGPOLL信号
  ioctl(fd, I_SETSIG, POLL_IN); //设置当有东西读时产生 SIGPOLL信号
  //ioctl(fd, I_SETSIG, S_INPUT);

  //允许进程去接收 SIGIO 信号
  fcntl(fd, F_SETOWN, getpid());
  //设置串口的文件描述符为异步,man上说,只有 O_APPEND 和 O_NONBLOCK 才能使用F_SETFL...
  fcntl(fd, F_SETFL, O_ASYNC|O_NONBLOCK);//FASYNC);
  return fd;
}

/*******************************************************************************
*函数名称 : int OpenCommPort(int fd, int comport)
*功    能 : 打开串口comport
*输入参数 : 串口号
*输出参数 : fd
*******************************************************************************/
int OpenCommPort(int fd, int comport)
{
int flags;
char *dev[5]={"/dev/ttyS0","/dev/ttyS1","/dev/ttyS2","/dev/ttyS3","/dev/ttyS4"};

  printf("Open Serial Port%d:\r\n", comport);
  fd = open(dev[comport], O_RDWR|O_NOCTTY|O_NDELAY|O_NONBLOCK);  //非阻塞接收串口数据
  if(fd == -1)
  { 
    perror("Failed"); 
    return(-1); 
  }
  //在打开串口成功后,使用fcntl(fd, F_SETFL, FNDELAY)语句,可以使read函数立即返回而不阻塞。FNDELAY选项使read函数在串口无字符时立即返回且为0。

  //判断串口的状态是否为阻塞状态,
  //设置串口的状态为阻塞状态,用于等待串口数据的读入
  flags = fcntl(fd, F_GETFL, 0);
  flags = flags|O_NONBLOCK;//|FNDELAY;
  if(fcntl(fd, F_SETFL, flags) < 0) 
    printf("OpenCommPort:fcntl failed!\n"); 
  else 
    printf("fcntl=%d\n", fcntl(fd, F_SETFL, flags));
/*
  if(fcntl(fd, F_SETFL, 0) < 0) 
    printf("fcntl failed!\n"); 
  else 
    printf("fcntl=%d\n",fcntl(fd, F_SETFL,0));
*/
  //测试是否为终端设备
  if(isatty(STDIN_FILENO) == 0) 
    printf("Standard Input is not a Terminal device\n"); 
  else 
    printf("isatty success!\n"); 

  printf("fd-open=%d\n",fd); 
  return fd;
}

/*******************************************************************************
*函数名称 : int SetCommState(int fd, int nSpeed, int nBits, char nEvent, int nStop)
*功    能 : 配置串口函数
*输入参数 : (int fd, int nSpeed, int nBits, char nEvent, int nStop)
*输出参数 : 成功返回1,出错返回-1
*******************************************************************************/
int SetCommState(int fd, int nSpeed, int nBits, char nEvent, int nStop)
{ 
struct termios newtio, oldtio; 
//termios:c_iflag:输入模式;c_oflag:输出模式;c_cflag:控制模式;c_lflag:本地模式;c_cc[NCCS]:控制字符

  //保存测试现有串口参数设置,在这里如果串口号等出错,会有相关的出错信息
  if(tcgetattr(fd, &oldtio)!=0)
  {  
    perror("SetupSerial:"); 
    return -1; 
  } 

  bzero(&newtio, sizeof(newtio));     //清除结构体以放入新的序列埠设定值
  newtio.c_cflag  |=  CLOCAL | CREAD; //CLOCAL:本地连线, 不具数据机控制功能;CREAD:打开接受者
  newtio.c_cflag &= ~CSIZE;           //CSIZE:字符长度掩码
  //设置停止位
  switch(nBits)//数据位取值为7或者8 
  { 
    case 7: 
      newtio.c_cflag |= CS7; 
    break; 
    case 8: 
      newtio.c_cflag |= CS8; 
    break;
    default:
      newtio.c_cflag |= CS8; 
    break;
  } 
  //设置奇偶校验位
  switch( nEvent ) //效验类型 取值为O,E,N
  { 
    case 'O': //奇数
    case 'o':
      newtio.c_iflag |= (INPCK | ISTRIP); //INPCK:启用输入奇偶检测,ISTRIP:除去奇偶校验位,去掉第八位
      newtio.c_cflag |= (PARENB | PARODD);//设置为奇效验,PARENB:校验位使能;PARODD:输入和输出是奇校验
    break; 
    case 'E'://偶数
    case 'e':  
      newtio.c_iflag |= (INPCK | ISTRIP); //INPCK:启用输入奇偶检测,ISTRIP:除去奇偶校验位,去掉第八位
      newtio.c_cflag |= PARENB;           //PARENB:校验位使能
      newtio.c_cflag &= ~PARODD;          //PARODD:使用奇校验而不使用偶校验->转换为偶效验
    break; 
    case 'N'://无奇偶校验位
    case 'n':
      newtio.c_cflag &= ~PARENB;  //PARENB:校验位使能
    break;
    default:
      newtio.c_cflag &= ~PARENB;  //PARENB:校验位使能
    break;
  } 
  //设置串口通信速率
  switch(nSpeed)
  { 
    case 2400: 
      cfsetispeed(&newtio, B2400); 
      cfsetospeed(&newtio, B2400); 
    break; 
    case 4800: 
      cfsetispeed(&newtio, B4800); 
      cfsetospeed(&newtio, B4800); 
    break; 
    case 9600: 
      cfsetispeed(&newtio, B9600); 
      cfsetospeed(&newtio, B9600); 
    break; 
    case 19200: 
      cfsetispeed(&newtio, B19200); 
      cfsetospeed(&newtio, B19200); 
    break;
    case 38400: 
      cfsetispeed(&newtio, B38400); 
      cfsetospeed(&newtio, B38400); 
    break;
    case 57600: 
      cfsetispeed(&newtio, B57600); 
      cfsetospeed(&newtio, B57600); 
    break; 
    case 115200: 
      cfsetispeed(&newtio, B115200); 
      cfsetospeed(&newtio, B115200); 
    break; 
    default: 
      cfsetispeed(&newtio, B9600); 
      cfsetospeed(&newtio, B9600); 
    break; 
  }
  //设置停止位
  if( nStop == 1 ) //停止位 取值为 1 或者2
    newtio.c_cflag &=  ~CSTOPB; 
  else if ( nStop == 2 ) 
    newtio.c_cflag |=  CSTOPB; //设置两个停止位
    
  //如果不是开发终端之类的,只是串口传输数据,而不需要串口来处理,那么使用原始模式(Raw Mode)方式来通讯,设置方式如下
  newtio.c_lflag  &= ~(ICANON | ECHO | ECHOE | ISIG);  //c_lflag:本地模式
  newtio.c_oflag  &= ~OPOST;   //Output,c_oflag:Output options
  
  //设置等待时间和最小接收字符
  newtio.c_cc[VTIME]  = 0;  //0.1s,VTIME:Time to wait for data (1/10 seconds)
  newtio.c_cc[VMIN] = 1;   //VMIN:Minimum number of characters to read
  //清除串口缓冲,处理未接收字符
  tcflush(fd, TCIFLUSH);   //Update the options and do it NOW
  //激活新配置
  if((tcsetattr(fd, TCSANOW, &newtio))!=0) 
  { 
    perror("Set Serial Failure!\n"); 
    return -1; 
  } 
  printf("Set Serial Success!\n"); 
  return 1; 
}

/*******************************************************************************
*函数名称 : int UartInit(ComBuf_t *p_combuf, int ttyid)
*功    能 : UART口配置函数
*输入参数 : ComBuf_t *p_combuf:对应设备参数; int ttyid:UART口号
*输出参数 : 成功p_combuf->Fd;失败返回-1
*******************************************************************************/
int UartInit(ComBuf_t *p_combuf, int ttyid)
{
  p_combuf->Fd = -1;
  p_combuf->Timer = 0;
  p_combuf->Status = 0;
  p_combuf->RecvLen = 0;
  memset(p_combuf->Buf, 0, COMBUF_SIZE);
  
  //串口设置
  p_combuf->Fd = OpenCommPort(p_combuf->Fd, ttyid);
  if (p_combuf->Fd < 0)  //打开设备失败
    return -1;
  SetCommState(p_combuf->Fd, 115200, 8, 'N', 1); //设置串口设备
  return p_combuf->Fd;
}

// 外部引用omc向rs485转发数据缓冲区结构
extern swap_t omc_rs485_swap;

// 外部引用rs485向omc转发数据缓冲区结构
extern swap_t rs485_omc_swap;
/*******************************************************************************
*函数名称 : int UartReceiveData(ComBuf_t *pcombuf, int waittime)
*功    能 : 接收的数据转存到pcombuf.Buf数据缓存区中,并处理,超时时间waittime
*输入参数 : ComBuf_t *pcombuf:对应设备参数;int waittime:超时时间ms
*输出参数 : 接收到数据长度或错误标识
*******************************************************************************/
int UartReceiveData(ComBuf_t *pcombuf, int waittime)
{
	if(0 != rs485_omc_swap.len){
		memcpy((char *)&pcombuf->Buf[0], (char *)rs485_omc_swap.buf, rs485_omc_swap.len);
		pcombuf->RecvLen = rs485_omc_swap.len;
		rs485_omc_swap.len = 0;
		return pcombuf->RecvLen;
	}
	return 0;
	/*
int res, rcsum;
fd_set readfs;
struct timeval tv;

_RERECVFLAG:
  tv.tv_sec = waittime/1000;//SECOND
  tv.tv_usec = (waittime%1000)*1000;//USECOND
  FD_ZERO(&readfs);
  FD_SET(pcombuf->Fd, &readfs);

  res = select(pcombuf->Fd + 1, &readfs, NULL, NULL, &tv);
  if (res > 0)
  {
    rcsum = read(pcombuf->Fd, &pcombuf->Buf[pcombuf->RecvLen], (COMBUF_SIZE - pcombuf->RecvLen));
    if (rcsum > 0)
    {
      pcombuf->RecvLen = pcombuf->RecvLen + rcsum;
      waittime = 10;
      goto _RERECVFLAG;//重新接收等待结束
    }
    else if (rcsum < 0)
    {
      perror("UartReceiveData:read() error!");
      return -1;
    }
  }
  else if (res < 0)
  {
    perror("UartReceiveData:select() error!");
    return -1;
  }
  return pcombuf->RecvLen;
  */
}

/*******************************************************************************
*函数名称 : int UartSendData(int fd, char *sbuf, int len)
*功    能 : 串口fd 发送数据
*输入参数 : 串口fd,sbuf,长度
*输出参数 : 读操作成功返回1,否则返回-1
*******************************************************************************/
int UartSendData(int fd, char *sbuf, int len)
{
//int sr;

  ComDataHexDis(sbuf, len);
	if(0 == omc_rs485_swap.len){
		memcpy((char *)omc_rs485_swap.buf, sbuf, len);
		omc_rs485_swap.len = len;
	}else{
		printf("omc_rs485_swap error!!!\r\n");
	}
	return 1;
  /*
  sr = write(fd, sbuf, len);
  if(sr == -1)
  {
    printf("Write sbuf error!\r\n");
    return -1;
  }
  return 1;
  */
}

/*******************************************************************************
*函数名称 : int UartSendPack(ComBuf_t *pcombuf, APPack_t *p_packbuf)
*功    能 : 发送协议包,将p_packbuf所指的待发送数据发送给pcombuf所指设备
*输入参数 : ComBuf_t *pcombuf:对应设备参数;APPack_t *p_packbuf:缓存区对应数据包结构指针
*输出参数 : 成功返回1,否则返回-1
*******************************************************************************/
int UartSendPack(ComBuf_t *pcombuf, APPack_t *p_packbuf)
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
	  DEBUGOUT("Fd:%d.UartSendData:%d.\r\n", pcombuf->Fd, sdsum);
		UartSendData(pcombuf->Fd, sdbuf, sdsum);
   	ComDataWriteLog(sdbuf, sdsum);
   	return 1;
	}
	else
	  return -1;
}

/*********************************End Of File*************************************/
