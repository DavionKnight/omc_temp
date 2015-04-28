/********************  COPYRIGHT(C) ***************************************
**--------------文件信息--------------------------------------------------------
**文   件   名: druheader.h
**创   建   人: 于宏图
**创 建  日 期: 
**程序开发环境：
**描        述: 软件常用变量设置的头文件
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
#ifndef _DRUHEADER_H
#define _DRUHEADER_H

#include <ctype.h> 		//isprint isupper isspace
#include <errno.h>    //错误符号定义
#include <fcntl.h>    //文件控制定义
#include <stdio.h>		//标准输入输出
#include <stdlib.h>		//标准库函数
#include <string.h>   //字符串功能函数
#include <stropts.h>	//ioctl
#include <signal.h>
#include <termios.h>  //POSIX终端控制定义
#include <time.h>
#include <unistd.h>	  //Unix标准函数定义
#include <math.h>

//#include <sys/queue.h>
//#include <sys/signal.h>
#include <sys/socket.h> //for socket
#include <sys/ioctl.h>
#include <sys/stat.h>
#include <sys/select.h>
#include <sys/time.h>
#include <sys/types.h>  //for socket

#include <net/if.h>
#include <netinet/in.h> //for sockaddr_in
#include <arpa/inet.h>

#include <poll.h>
#include <sys/epoll.h> 

//#include <stdarg.h>
//#include <linux/rtc.h>
#include <linux/sched.h>
#include <semaphore.h>
#include <pthread.h>
#include <sqlite3.h>

#include "usertype.h"
#include "apparadef.h"
#include "drudefstruct.h"

#define PRINTDEBUG    0	//调试信息输出控制

#define DEBUGOUT printf
//#define DEBUGOUT 

#define MAIN_UNIT     1 //主单元
#define EXPAND_UNIT   2 //扩展单元
#define RAU_UNIT      3 //远端单元

#define LOCALCOM      2 //利用RS485接口进行本地通讯
#define MODEMUART     2 //MODEM通讯口

#define CONFIGPATH    "/flashDev/program/config"
#define	LOGSAVEDPATH  "/flashDev/data/log"
#define	LOGFILENUM    10
#define	LOGFILESIZE   0x100000


#define SET_IP                  1   //1:设置IP地址
#define SET_NETMASK             2   //2:设置子网掩码
#define SET_GATEWAY             3   //3:设置默认网关

#define UDP_BROADCAST_MODE      3   //3:使用UDP广播模式

#define NET_NULL                0
#define NET_CONNET              0xC0
#define NET_DISCONNET           0xCD

#define LOGINSUCCESS            0xCF
#define NETCONNETINTERVAL       60  //建立通信链路时间间隔60s
#define LOGININTERVAL           60  //监控中心登录时间间隔60s
#define SOCKETTIMEOUT           3   //监控中心登录时间间隔3s

#define CONNETFAILSUM           3   //连接失败次数
#define LOGINFAILSUM            3   //login失败次数
#define NOHEARTBEATSUM          3   //最大心跳丢包次数

#define IRCONNETINTERVAL        10  //IR协议建立通信链路时间间隔6s
#define IRLOGININTERVAL         10  //IR协议client登录server时间间隔s

#define IRCONNETFAILSUM         3   //IR协议连接失败次数
#define IRLOGINFAILSUM          3   //IR协议login失败次数
#define IRNOHEARTBEATSUM        3   //IR协议最大心跳丢包次数

#define IRTCPRMUPORT            30000 //IR协议TCP连接主单元端口号
#define IRUDPRMUPORT            33333 //IR协议UDP广播主单元端口号
#define IRUDPRAUPORT            33334 //IR协议UDP广播扩展,远端端口号

#define COMWAITTIME             0//500

#define THREAD_STATUS_NEW       0 //线程的状态－新建
#define THREAD_STATUS_RUNNING   1 //线程的状态－正在运行
#define THREAD_STATUS_START		2 //线程的状态－可以运行
#define THREAD_STATUS_EXIT     -1 //线程的状态－运行结束

#define CONNECT_TOTALNUMBER     255 //server链接数
#define DEFAULT_STARTIP         2  //系统默认起始IP地址

#endif//_DRUHEADER_H
/*********************************End Of File*************************************/
