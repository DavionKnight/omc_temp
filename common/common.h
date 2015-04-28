#ifndef COMMON_H
#define COMMON_H
#include "apue.h"
// 模块序号
enum{
	FUN_MODULE=0,
	TASK1_MODULE=1,
	TASK2_MODULE=2,
	TASK3_MODULE=3,
};
// 消息类型
enum {
	MSG_FUN = 1,
	MSG_FUN_RESULT = 2,
	MSG_CGI = 3,
	MSG_CGI_RESULT = 4,
};
// 信号量定义
enum{
	SEM_RS485_SEND = 0,
	SEM_OMC_RS485 = 1,
	SEM_OMC_IR = 2,
	SEM_DRV = 3,
	SEM_DBSAVE = 4,
};
/*********************** 缓冲区队列列操作 **************************/
// 循环队列缓冲区结构
struct queue_buf{
	int len;		// 申请的循环队列缓冲区长度
	int head;		// 缓冲区头指针
	int tail;		// 缓冲区尾指针
	char * buf;     // 缓冲区
};
/*
** 函数功能：初始化循环队列缓冲区
** 输入参数：queue_buf=欲初始化的循环队列结构体指针 len=循环队列长度
** 输出参数：无
** 返回值：0=成功 -1=失败
** 备注：len必须为128/256/512/1024/2048/4096
*/
extern int queue_buf_init(struct queue_buf * queue_buf, unsigned int len);

/*
** 函数功能：删除循环队列缓冲区
** 输入参数：queue_buf=欲删除的循环队列结构体指针 
** 输出参数：无
** 返回值：0=成功 -1=失败
*/
extern void * queue_buf_exit(void * arg);

/*
** 函数功能：获取循环队列缓冲区有效数据长度
** 输入参数: queue_buf=循环队列结构体指针
** 输出参数：无
** 返回值：有效数据长度
*/
extern int get_queue_buf_len(struct queue_buf * queue_buf);

/*
** 函数功能：循环队列缓冲区入桟
** 输入参数: queue_buf=循环队列结构体指针 buf=待入桟缓冲区指针 len=待入桟数据长度
** 输出参数：无
** 返回值：0=成功 -1=失败
*/
extern int push_queue_buf(struct queue_buf * queue_buf, char * buf, unsigned int len);

/*
** 函数功能：循环队列缓冲区出桟
** 输入参数: queue_buf=循环队列结构体指针 buf=待出桟缓冲区指针 len=待出桟数据长度
** 输出参数：无
** 返回值：0=成功 -1=失败
*/
int pop_queue_buf(struct queue_buf * queue_buf, char * buf, unsigned int len);

/******************************************************************/
#define MSG_BUF_LEN (8192-4)
struct mymesg
{
	long mtype;
	char mtext[MSG_BUF_LEN];
};
extern struct mymesg msg_tmp;

typedef int (*operation)(char * buf, int len);

extern void test_lib(void);
extern int printk(char * buf, int len);
extern int msg_init(void);
extern int msg_send(int index, char * buf, int len);
extern int msg_recv(int index, operation handle);
extern int msg_recv_ex(int index, operation handle);
/*
** 函数功能：将HEX转为unsigned short int 型，倒序排列
** 输入参数: p=hex数据指针
** 输出参数：无
** 返回值：转换值
*/
extern unsigned short int hex2uint16(unsigned char * p);
/*
** 函数功能：将HEX转为ASCII
** 输入参数: p=hex数据 len=数据长度
** 输出参数：out=ascii数据
** 返回值：转换结果长度
*/
extern int hex2ascii(unsigned char * p, int len, unsigned char * out);
/*
** 函数功能：将ASCII转为HEX
** 输入参数: p=ascii数据 len=数据长度
** 输出参数：out=hex数据
** 返回值：转换结果长度
*/
int ascii2hex(unsigned char * p, int len, unsigned char * out);
/*
** 函数功能：将单字节有符号数转为4字节有符号数
** 输入参数: val=单字节有符号数
** 输出参数：无
** 返回值：4字节有符号数
*/
int signed_1to4(char val);
int signed_2to4(short val);
/*
** 函数功能：初始化信号量
** 输入参数：无
** 输出参数：无
** 返回值：0=成功 其他=失败
*/
extern int _sem_init(void);
/*
** 函数功能：信号量加锁
** 输入参数：信号量标识(创建时的序号，由宏实现)
** 输出参数：无
** 返回值：无
*/
extern void lock_sem(int idx);
/*
** 函数功能：信号量解锁
** 输入参数：信号量标识(创建时的序号，由宏实现)
** 输出参数：无
** 返回值：无
*/
extern void unlock_sem(int idx);
/*
** 函数功能：通过ftp下载升级包
** 输入参数: usr=用户名 pw=密码 ip=服务器IP地址 port=端口 file_name=文件名
** 输出参数：无
** 返回值：0
*/
extern int updata_code(char * usr, char * pw, char * ip, char * port, char * file_name);
/*
** 函数功能：解压bz2的压缩包
** 输入参数: file_name=文件名
** 输出参数：无
** 返回值：0
*/
extern int uncompress(char * file_name);
#endif
