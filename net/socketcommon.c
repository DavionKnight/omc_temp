/*******************************************************************************
********************************************************************************
* 文件名称:  socketcommon.c
* 功能描述:  socket 客户端和服务器连接程序
* 使用说明:  
* 文件作者:	H4
* 编写日期: （2012/10/22）
* 修改历史:
* 修改日期    修改人       修改内容
*-------------------------------------------------------------------------------

*******************************************************************************/
#include <stdlib.h>
#include <stdio.h>
#include <errno.h>
#include <string.h>
#include <unistd.h>
#include <netdb.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <sys/types.h>
#include <arpa/inet.h>
#include "../common/status.h"
#include "socketcommon.h"
#define MAXSLEEP 128
unsigned long serverip=0;
/*******************************************************************************
* 函数名称: socket_server_init
* 功    能: socket 服务器建立
* 参    数:
* 参数名称         类型                描述
*	port 					int 								服务器监听的端口号
* socketfd     int * 							服务器编号地址
* type 				 unsigned char      	服务器类型
* 返回值:
* 无
* 说   明:
* 日   期     版本    作者   修改人      DEBUG
* -----------------------------------------------------------------
* 2012/10/22  V1.0     H4     无       ：
*******************************************************************************/
int socket_server_init(int port,int *socketfd,unsigned char type)
{
	struct sockaddr_in server_addr;
	int sfd,revalue;
	if(type==SOCK_DGRAM)
		sfd=socket(AF_INET,SOCK_DGRAM,0);
	else
		sfd=socket(AF_INET,SOCK_STREAM,0);
	if(sfd==SOCKET_ERROR)
	{
		close(sfd);
			return 1;//error
	}
	bzero(&server_addr,sizeof(struct sockaddr_in));
	server_addr.sin_family=AF_INET;
	server_addr.sin_port=htons(port);
	server_addr.sin_addr.s_addr=htonl(INADDR_ANY);
	
	revalue=bind(sfd,(struct sockaddr *)&server_addr,sizeof(struct sockaddr));
	if(revalue==SOCKET_ERROR)
	{
			close(sfd);
			return 1;
	}
	if(type==SOCK_STREAM)
	{
			revalue=listen(sfd,30);
			if(revalue==SOCKET_ERROR)
			{
					close(sfd);
					return 1;
			}
	}
#ifdef _NET_DEBUG
	printf("\n socket init ok\n");
#endif
	*socketfd=sfd;
	return 0;
}
/*******************************************************************************
* 函数名称: socket_client_init
* 功    能: socket 客户端建立
* 参    数:
* 参数名称         类型                描述
*	ip_addr 		 unsigned long 				服务器ip地址
* port     		 unsigned short 			服务器端口号
* socketfd     int *								服务器编号
* type 				 unsigned char      	客户端类型
* 返回值:
* 无
* 说   明:
* 日   期     版本    作者   修改人      DEBUG
* -----------------------------------------------------------------
* 2012/10/22  V1.0     H4     无       ：
*******************************************************************************/
int socket_client_init(unsigned long ip_addr,unsigned short port,int *socketfd,unsigned char type)
{
		struct sockaddr_in client_addr;
		int cfd,revalue;
		
		
		if(type==SOCK_DGRAM)
				cfd=socket(AF_INET,SOCK_DGRAM,0);
		else
				cfd=socket(AF_INET,SOCK_STREAM,0);
		if(cfd==SOCKET_ERROR)
		{
				close(cfd);
				return 1;
		}
		if(type==SOCK_STREAM)
		{
				bzero(&client_addr,sizeof(struct sockaddr_in));
				client_addr.sin_family=AF_INET;
				client_addr.sin_port=htons(port);
				client_addr.sin_addr.s_addr=htonl(ip_addr);
				revalue=connect(cfd,(struct sockaddr *)&client_addr,sizeof(struct sockaddr_in));
				if(revalue==SOCKET_ERROR)
				{
					close(cfd);
						return 1;
				}
				 *socketfd=cfd;
		}
		
		return 0;
	
}

/*******************************************************************************
* 函数名称: socket_client_init
* 功    能: socket 客户端建立
* 参    数:
* 参数名称         类型                描述
*	ip_addr 		 unsigned long 				服务器ip地址
* port     		 unsigned short 			服务器端口号
* socketfd     int *								服务器编号
* type 				 unsigned char      	客户端类型
* 返回值:
* 无
* 说   明:
* 日   期     版本    作者   修改人      DEBUG
* -----------------------------------------------------------------
* 2012/10/31  V1.0     H4     无       ：
*******************************************************************************/
int client_connect_retry(int  sockfd,const struct sockaddr *addr,socklen_t alen)
{
		int nsec;
		for(nsec =1 ;nsec<=MAXSLEEP; nsec<<=1){
				if(connect(sockfd,addr,alen)==0){
					return 0;
				}
				if(nsec<=MAXSLEEP/2)
					sleep(nsec);
		}
		return -1;

}
/*******************************************************************************
* 函数名称: socket_receive
* 功    能: socket 服务器接收客户端信息
* 参    数:
* 参数名称         				类型                		描述
*	socket_client 					int 								客户端描述符
* data_str     						unsigned char * 		接收buffer的地址
* max_receive_lenght     	unsigned long 			接收buffer最大长度
* max_waite_time 				 	unsigned long      	超时时间设定
	*	receive_lenght				unsigned long *     实际接收到的长度
	* 返回值:
* 无
* 说   明:
* 日   期     版本    作者   修改人      DEBUG
* -----------------------------------------------------------------
* 2012/10/22  V1.0     H4     无       ：
*******************************************************************************/
int socket_receive(int socketfd,
													unsigned char *data_str,
													unsigned long max_receive_lenght,
													unsigned long max_waite_time,
													unsigned long *receive_lenght)
{
		struct timeval wait_time;
		int revalue;
		unsigned long rec_len;
		fd_set socket_set;
		wait_time.tv_sec=0;
		wait_time.tv_usec=max_waite_time;
		FD_ZERO(&socket_set);
		FD_SET(socketfd,&socket_set);
		revalue=select(socketfd+1,&socket_set,NULL,NULL,&wait_time);
		if(revalue==0){
				return 2;
		}else if(revalue==SOCKET_ERROR){
				return 1;
		}else{
				FD_ZERO(&socket_set);
				rec_len=recv(socketfd,data_str,max_receive_lenght,0);
				if(rec_len>0){
						*receive_lenght=rec_len;
						return 0;
				}else{
						return 1;
				}
		}
}
/*******************************************************************************
* 函数名称: get_ip_addr
* 功    能:	获取ip地址
* 参    数:
* 参数名称         			类型                				描述
*	str 					unsigned char *				ip地址字符串所在地址
* 返回值:
*                                        unsigned long 					ip地址网络字节序
* 说   明:
* 日   期     版本    作者   修改人      DEBUG
* -----------------------------------------------------------------
* 2012/10/29  V1.0     H4     无       ：
*******************************************************************************/
unsigned long get_ip_addr(const unsigned long ip)
{
	unsigned long ipaddr;
	ipaddr=htonl(ip);
	return ipaddr;
}
/*******************************************************************************
* 函数名称: get_server_default_ip
* 功    能:	获取默认的主机ip地址
* 参    数:
* 参数名称         			类型                				描述
*	
* 返回值:
*                                        unsigned long 					ip地址网络字节序
* 说   明:
* 日   期     版本    作者   修改人      DEBUG
* -----------------------------------------------------------------
* 2012/10/29  V1.0     H4     无       ：
*******************************************************************************/
unsigned long get_server_ip(void)
{
	unsigned long ipaddr;
	ipaddr=htonl(serverip);
	return ipaddr;
}

