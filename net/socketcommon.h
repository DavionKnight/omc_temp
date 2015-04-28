/*******************************************************************************
********************************************************************************
* 文件名称:  socketcommon.h
* 功能描述: socket common 函数和结构定义
* 使用说明: 
* 文件作者:	H4
* 编写日期: （2012/10/25）
* 修改历史:
* 修改日期    修改人       修改内容
*-------------------------------------------------------------------------------
*******************************************************************************/
#ifndef _SOCKETCOMMON_H
#define _SOCKETCOMMON_H

#define SOCKET_ERROR (-1)

#define _NET_DEBUG 

int socket_receive(int socketfd,unsigned char *data_str,unsigned long max_receive_lenght,	unsigned long max_waite_time,unsigned long *receive_lenght);
int socket_client_init(unsigned long ip_addr,unsigned short port,int *socketfd,unsigned char type);
int socket_server_init(int port,int *socketfd,unsigned char type);
unsigned long get_server_ip(void);


#endif
