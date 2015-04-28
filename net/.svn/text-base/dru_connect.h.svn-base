
/*******************************************************************************
********************************************************************************
* 文件名称:  dru_connect.h
* 功能描述:  网络连接部分结构和宏定义
* 使用说明:  
* 文件作者:	H4
* 编写日期: （2012/10/26）
* 修改历史:
* 修改日期    修改人       修改内容
*-------------------------------------------------------------------------------

*******************************************************************************/
#ifndef DRU_CONNECT_H
#define DRU_CONNECT_H
#include "../protocol/ir_def.h"

#define TCP_SEND_FLAGS (MSG_NOSIGNAL)

#define TCP_SERVER_PORT 30000
#define MAX_CONNECT_NUMBER 252
#define MAX_SEND_LENGTH 1500
#define MAX_BUFFER_NUMBER 100
#define MAX_WAITE_US 0
#define CONNECT_TIME_OUT 200

#define CLIENT_CONNECTED 0xaa 
#define CLIENT_UNCONNECTED 0x55 
#define CLIENT_PAUSE	0x22 
#define FLAG_TRUE 1
#define FLAG_FALSE 0
#define RETURN_OK 0
/*define the deviece type*/ 
#define TYPE_MAIN 1
#define TYPE_EXPEND 2
#define TYPE_RAU 3
#pragma pack(1)
//连接记录结构定义。记录客户端socket 描述符
// 和连接状态 以及客户端ip和端口
typedef struct {
		int socket;
		int dev;
		unsigned long long dev_id;
		unsigned char dev_type;
		unsigned char connect_status;
		unsigned long client_ip;
		unsigned char mac[6];
		unsigned char time;
		delay_control *p_dc;
		rru_info *p_info;
}connect_record_t;
typedef struct {
		unsigned char is_ready;
		unsigned short s_length;
		unsigned char s_buffer[MAX_SEND_LENGTH];
}send_message_t;
typedef struct dru_send_list {
	send_message_t *send_message_p;
	struct dru_send_list *next;
}send_list_t;


extern void software_downloard_deal(void);
extern void *listen_connect_thread(void *arg);
extern int dru_tcp_server_init(void);
extern void *receive_server_thread(void *arg);
extern void *receive_client_thread(void *arg);
extern int dru_tcp_client_init(void);
int socket_send(unsigned int index,unsigned char *pbuffer,unsigned int blenght,unsigned char type);
void * time_send_thread(void *arg);
void * time_transimt(void *arg);
int get_index_bydev(int dev);
int get_current_dev(void);
void close_client(void);
void check_connect(void);
int get_dev_byindex(unsigned int index);
int get_index_byid(unsigned long long id);
void *updata_software_thread(void *arg);

extern int get_ip_by_dnum(unsigned int dev_num, unsigned int* ip);
extern int get_dnum_by_ip(unsigned int ip, unsigned long * dev_num);
unsigned long  get_ip_by_topologic(unsigned long long topologic_num, unsigned char port_num);
unsigned int  get_child_port(unsigned long long topologic_num, unsigned char * buf);
#endif
