#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/msg.h>
#include <stdio.h>
#include <errno.h>
#include <string.h>
#include <stdlib.h>
#include "common.h"

const char * g_comm_ver = "20";
const char * g_comm_test_ver = "0"; // 0:正式版 奇数：待测试版本 偶数：已测试版本
void test_lib(void)
{
	printf("call lib is ok!\r\n");
}
// 模块内部变量
struct mymesg m_recv_buf;
struct mymesg m_send_buf;

// 计算key的文件路径
char * msg_ftok_arr[] = {
	"/",
	"/etc",
	"/usr",
	"/tmp",
	"/bin",
	"/sbin",
	"/lib",
	"/home",
};

/*
** 函数功能：计算key值
** 输入参数：index=进程序号
** 输出参数：无
** 返回值：key值
*/
int msg_get_key(int index)
{
	int num = sizeof(msg_ftok_arr)/sizeof(char *);
	key_t key = 0;

	// 判断index是否超出范围
	if(index > num){
		printf("线程序号错误!\r\n");
		return -1;
	}
	key = ftok(msg_ftok_arr[index], index+1);
	if(key == -1){
		printf("ftok error.\r\n");
		return -1;
	}
	//printf("index=%d, path=%s, key=%d\r\n", index, msg_ftok_arr[index], key);
	return key;
}
/*
** 函数功能：初始化消息队列
** 输入参数：无
** 输出参数：无
** 返回值：0=成功 其他=失败
*/
int msg_init(void)
{
	int num = sizeof(msg_ftok_arr)/sizeof(char *);
	int i = 0;
	key_t key_id = 0;
	int msg_id = 0;
	struct msqid_ds msg_info;
	int tmp = 0;
	
	printf("msg_init is declare ok.\r\n");
	for(i = 0; i < num; i++){
		key_id = msg_get_key(i);
		if(key_id == -1){
			printf("init_msg error.\r\n");
			return -1;
		}
		msg_id = msgget(key_id, IPC_CREAT | 0666); 
		if(-1 == msg_id){	
			printf("msgget error.");
			return -1;
		}
		tmp = msgctl(msg_id, IPC_STAT, &msg_info);
		if(tmp == -1){
			printf("msgctl read error.\r\n");
			return -1;
		}
		printf("msg_qnum=%d.\r\n", (int)msg_info.msg_qnum);
		while(msg_info.msg_qnum){
			msgrcv(msg_id, (char *)&m_recv_buf, MSG_BUF_LEN, 0,  MSG_NOERROR);
			tmp = msgctl(msg_id, IPC_STAT, &msg_info);
			if(tmp == -1){
				printf("msgctl read error.\r\n");
				return -1;
			}
			printf("msg_qnum=%d.\r\n", (int)msg_info.msg_qnum);
		}
		msg_info.msg_perm.uid = 8;
		msg_info.msg_perm.gid = 8;
		msg_info.msg_qbytes = 4096;
		tmp = msgctl(msg_id, IPC_SET, &msg_info);
		if(tmp == -1){
			printf("msgctl set error.\r\n");
			return -1;
		}
	}
	return 0;
}
/*
** 函数功能：将数据发送到消息队列中
** 输入参数：index=进程序号,buf=数据缓冲区,len=缓冲区数据长度
** 输出参数：无
** 返回值：0=成功 -1=失败
*/
int msg_send(int index, char * buf, int len)
{
	key_t key_id = 0;
	int msg_id = 0;
	int ret = 0;
	
	key_id = msg_get_key(index);
	if(key_id == -1){
		printf("msg_send msg_get_key error.\r\n");
		return -1;
	}
	// 获取消息队列的ID
	msg_id = msgget(key_id, 0); 
	if(msg_id == -1){
		printf("msg %d is not exist.\r\n", index);
		return -1;
	}
	//printf("msg_id=%d\r\n", msg_id);
	while(1){
		ret = msgsnd(msg_id, buf, len, MSG_NOERROR);
		if(ret == -1){
			if((errno == EINTR) || (errno == EAGAIN)){
				errno = 0;
				printf("msgsnd continue.\r\n");
				continue;
			}else{
				if(errno == EACCES){
					printf("EACCES\r\n");
				}else if(errno == EFAULT){
					printf("EFAULT\r\n");
				}else if(errno == EIDRM){
					printf("EIDRM\r\n");
				}else if(errno == EINVAL){
					printf("EINVAL\r\n");
				}
				printf("msgsnd error\r\n");
				exit(0);
			}
		}
		break;
	}
	return 0;
}
/*
** 函数功能：从消息队列中读数据
** 输入参数：index=进程序号, handle=数据处理函数
** 输出参数：无
** 返回值：0=成功 -1=失败
*/
int msg_recv(int index, operation handle)
{
	key_t key_id = 0;
	int msg_id = 0;
	int ret = 0;
	
	key_id = msg_get_key(index);
	if(key_id == -1){
		printf("msg_recv msg_get_key error.\r\n");
		return -1;
	}
	// 获取消息队列的ID
	msg_id = msgget(key_id, 0); 
	if(msg_id == -1){
		printf("msg %d is not exist.\r\n", index);
		return -1;
	}
	while(1){
		memset(m_recv_buf.mtext, 0, MSG_BUF_LEN);
		ret = msgrcv(msg_id, (char *)&m_recv_buf, MSG_BUF_LEN, 0,  MSG_NOERROR);
		if(ret == -1){
			if((errno == EINTR) || (errno == EAGAIN)){
				errno = 0;
				continue;
			}else{
				printf("msgrcv error\r\n");
				return -1;
			}
		}
		if(handle){
			(*handle)((char *)&m_recv_buf, ret);
		}
	}
	return 0;
}
/*
** 函数功能：从消息队列中读数据扩展
** 输入参数：index=进程序号, handle=数据处理函数
** 输出参数：无
** 返回值：0=成功 -1=失败
*/
int msg_recv_ex(int index, operation handle)
{
	key_t key_id = 0;
	int msg_id = 0;
	int ret = 0;
	
	key_id = msg_get_key(index);
	if(key_id == -1){
		printf("msg_recv msg_get_key error.\r\n");
		return -1;
	}
	// 获取消息队列的ID
	msg_id = msgget(key_id, 0); 
	if(msg_id == -1){
		printf("msg %d is not exist.\r\n", index);
		return -1;
	}
	while(1){
		memset(m_recv_buf.mtext, 0, MSG_BUF_LEN);
		ret = msgrcv(msg_id, (char *)&m_recv_buf, MSG_BUF_LEN, 0,  MSG_NOERROR);
		if(ret == -1){
			if((errno == EINTR) || (errno == EAGAIN)){
				errno = 0;
				continue;
			}else{
				printf("msgrcv error\r\n");
				return -1;
			}
		}
		if(handle){
			(*handle)((char *)&m_recv_buf, ret);
		}
		if(m_recv_buf.mtype&0x80){
			//printf("continue mtype=0x%02x\r\n", (unsigned int)m_recv_buf.mtype);
		}else{
			//printf("break mtype=0x%02x\r\n", (unsigned int)m_recv_buf.mtype);
			break;
		}
	}
	return 0;
}

// 计算key的文件路径,信号量使用
char * sem_ftok_arr[] = {
	"/dev",
	"/proc",
	"/mnt",
	"/sys",
	"/var",
};

/*
** 函数功能：计算key值
** 输入参数：index=进程序号
** 输出参数：无
** 返回值：key值
*/
int sem_get_key(int index)
{
	int num = sizeof(sem_ftok_arr)/sizeof(char *);
	key_t key = 0;

	// 判断index是否超出范围
	if(index > num){
		printf("线程序号错误!\r\n");
		return -1;
	}
	key = ftok(sem_ftok_arr[index], index+1);
	if(key == -1){
		printf("ftok error.\r\n");
		return -1;
	}
	return key;
}
/*
** 函数功能：初始化信号量
** 输入参数：无
** 输出参数：无
** 返回值：0=成功 其他=失败
*/
int _sem_init(void)
{
	int num = sizeof(sem_ftok_arr)/sizeof(char *);
	int i = 0;
	key_t key_id = 0;
	int sem_id = 0;
	int tmp = 0;
	
	for(i = 0; i < num; i++)
	{
		key_id = sem_get_key(i);
		if(key_id == -1)
		{
			printf("init_sem error.\r\n");
			return -1;
		}
		
		sem_id = semget(key_id, 1, IPC_CREAT | 0600); 
		if(-1 == sem_id)
		{	
			printf("semget error.");
			return -1;
		}
		
		tmp = semctl(sem_id, 0, SETVAL, 1);
		if(tmp == -1)
		{
			printf("semctl set error.\r\n");
			return -1;
		}
		
		tmp = semctl(sem_id, 0, GETVAL, 0);
		if(tmp == -1)
		{
			if(errno == EACCES)
			{
				printf("semctl get val errno: %s.\r\n", "EACCES");
			}
			else if(errno == EFAULT)
			{
				printf("semctl get val errno: %s.\r\n", "EFAULT");
			}
			else if(errno == EIDRM)
			{
				printf("semctl get val errno: %s.\r\n", "EIDRM");
			}
			else if(errno == EINVAL)
			{
				printf("semctl get val errno: %s.\r\n", "EINVAL");
			}
			else if(errno == EPERM)
			{
				printf("semctl get val errno: %s.\r\n", "EPERM");
			}
			else if(errno == ERANGE)
			{
				printf("semctl get val errno: %s.\r\n", "ERANGE");
			}
		}
		printf("semctl get val: %d.\r\n", tmp);
	}
	return 0;
}
/*
** 函数功能：信号量加锁
** 输入参数：信号量标识(创建时的序号，由宏实现)
** 输出参数：无
** 返回值：无
*/
void lock_sem(int idx)
{
	int ret, semid, keyid;
	static struct sembuf op = {
		.sem_num = 0,
		.sem_op = -1,
		.sem_flg = SEM_UNDO,
	};

	keyid = sem_get_key(idx);
	semid = semget(keyid, 1, 0);
	while(1){
		ret = semop(semid, &op, 1);
		if(ret == -1){
			if(errno == EINTR){
				continue;
			}else{
				printf("semop error.\r\n");
				return;
			}
		}
		break;
	}
}
/*
** 函数功能：信号量解锁
** 输入参数：信号量标识(创建时的序号，由宏实现)
** 输出参数：无
** 返回值：无
*/
void unlock_sem(int idx)
{
	int ret, semid, keyid;
	static struct sembuf op = {
		.sem_num = 0,
		.sem_op = 1,
		.sem_flg = SEM_UNDO,
	};

	keyid = sem_get_key(idx);
	semid = semget(keyid, 1, 0);
	while(1){
		ret = semop(semid, &op, 1);
		if(ret == -1){
			if(errno == EINTR){
				continue;
			}else{
				printf("semop error.\r\n");
				return;
			}
		}
		break;
	}
}
int printk(char * buf, int len)
{
	int i =  0;
	unsigned char * p = (unsigned char *)buf;

	while(len){
		printf("0x%02X ", *p);
		p++;
		i++;
		if(i == 8){
			printf("\r\n");
			i = 0;
		}
		len -= 1;
	}
	if(i != 0){
		printf("\r\n");
	}
	return 0;
}
/*
** 函数功能：初始化循环队列缓冲区
** 输入参数：queue_buf=欲初始化的循环队列结构体指针 len=循环队列长度
** 输出参数：无
** 返回值：0=成功 -1=失败
** 备注：len必须为128/256/512/1024/2048/4096
*/
int queue_buf_init(struct queue_buf * queue_buf, unsigned int len)
{
	char * buf = NULL;
	int i = 0;
	int flag = 0;

	// 判断缓冲区大小
	for(i = 32; i > 0 ; i--)
	{
		if(flag == 0)
		{
			if( len&(1<<(i-1)) )
			{
				flag = 1;
			}
		}
		else
		{
			if( len&(1<<(i-1)) )
			{
				printf("struct queue_buf init len is error.%x\r\n", len);
				return -1;
			}
		}
	}
	// 判断缓冲区是否为0
	if(len == 0)
	{
		printf("struct queue_buf init error. len = 0.\r\n");
		return -1;
	}
	// 申请缓冲区
	buf = (char *)malloc(len);
	if(buf == NULL)
	{
		printf("malloc error.\r\n");
		return -1;
	}
	else
	{ 
		// 初始化缓冲区
		queue_buf->len = len;
		queue_buf->head = 0;
		queue_buf->tail = 0;
		queue_buf->buf = buf;
		return 0;
	}
}
/*
** 函数功能：删除循环队列缓冲区
** 输入参数：queue_buf=欲删除的循环队列结构体指针 
** 输出参数：无
** 返回值：0=成功 -1=失败
*/
void * queue_buf_exit(void * arg)
{
	struct queue_buf * queue_buf = (struct queue_buf *)arg;
	// 判断循环队列和缓冲区是否为空
	if((queue_buf != NULL)&&(queue_buf->buf != NULL)){
		// 释放缓冲区资源
		free(queue_buf->buf);	
		queue_buf->len = 0;
		queue_buf->head = 0;
		queue_buf->tail = 0;
		queue_buf->buf = NULL;
		printf("free queue_buf ok.\r\n");
		return (void *)0;
	}
	printf("free error.\r\n");
	return (void *)-1;
}
/*
** 函数功能：获取循环队列缓冲区有效数据长度
** 输入参数: queue_buf=循环队列结构体指针
** 输出参数：无
** 返回值：有效数据长度
*/
int get_queue_buf_len(struct queue_buf * queue_buf)
{
	return (((queue_buf->head-queue_buf->tail)+queue_buf->len)&(queue_buf->len-1));
}
/*
** 函数功能：循环队列缓冲区入桟
** 输入参数: queue_buf=循环队列结构体指针 buf=待入桟缓冲区指针 len=待入桟数据长度
** 输出参数：无
** 返回值：0=成功 -1=失败
*/
int push_queue_buf(struct queue_buf * queue_buf, char * buf, unsigned int len)
{
	unsigned int i = 0;
	unsigned int cnt = 0;

	// 检查缓冲区是否有足够的空间
	cnt = get_queue_buf_len(queue_buf);
	if((len+cnt) > (queue_buf->len-1)){
		printf("缓冲区空间不足，入桟失败.\r\n");
		return -1;
	}
	// 将数据存入缓冲区，并更改下标
	for(i = 0; i < len; i++){
		queue_buf->buf[queue_buf->head++] = buf[i];
		queue_buf->head &= (queue_buf->len - 1);
	}
	return 0;
}
/*
** 函数功能：循环队列缓冲区出桟
** 输入参数: queue_buf=循环队列结构体指针 buf=待出桟缓冲区指针 len=待出桟数据长度
** 输出参数：无
** 返回值：0=成功 -1=失败
*/
int pop_queue_buf(struct queue_buf * queue_buf, char * buf, unsigned int len)
{
	unsigned int i = 0;
	unsigned int cnt = 0;

	// 检查缓冲区有效数据长度
	cnt = get_queue_buf_len(queue_buf);
	if(len > cnt){
		printf("缓冲区有效数据太少，len=%d, cnt=%d.\r\n", len, cnt);
		return -1;
	}
	// 将缓冲区中的有效数据取出，存入buf中
	for(i = 0; i < len; i++){
		buf[i] = queue_buf->buf[queue_buf->tail++];
		queue_buf->tail &= (queue_buf->len-1);
	}
	return 0;
}
/*
** 函数功能：将HEX转为unsigned short int 型，倒序排列
** 输入参数: p=hex数据指针
** 输出参数：无
** 返回值：转换值
*/
unsigned short int hex2uint16(unsigned char * p)
{
	return ((unsigned short int)p[0] | (unsigned short int)(p[1]<<8));
}
/*
** 函数功能：将HEX转为ASCII
** 输入参数: p=hex数据 len=数据长度
** 输出参数：out=ascii数据
** 返回值：转换结果长度
*/
const char ascii_tb[] = "0123456789ABCDEF";
int hex2ascii(unsigned char * p, int len, unsigned char * out)
{
	int i = 0;
	int cnt = 0;
	
	for( i = 0; i < len; i++){
		out[cnt++] = ascii_tb[((p[i]>>4)&0x0f)];
		out[cnt++] = ascii_tb[((p[i])&0x0f)];
	}
	return cnt;
}
// ascii转为HEX
unsigned char _ascii2hex(unsigned char p)
{
	unsigned char tmp = 0;

	if((p >= '0')&&(p <= '9')){
		tmp = p - '0';
	}else if((p >= 'a')&&(p <= 'f')){
		tmp = p - 'a';
	}else if((p >= 'A')&&(p <= 'F')){
		tmp = p - 'A';
	}
	return tmp;
}
/*
** 函数功能：将ASCII转为HEX
** 输入参数: p=ascii数据 len=数据长度
** 输出参数：out=hex数据
** 返回值：转换结果长度
*/
int ascii2hex(unsigned char * p, int len, unsigned char * out)
{
	int i = 0;
	int cnt = 0;
	
	for( i = 0; i < len; i += 2){
		out[cnt++] = (_ascii2hex(p[i])<<4) | _ascii2hex(p[i+1]);
	}
	return cnt;
}
/*
** 函数功能：将单字节有符号数转为4字节有符号数
** 输入参数: val=单字节有符号数
** 输出参数：无
** 返回值：4字节有符号数
*/
int signed_1to4(char val)
{
	int cnt = val;
	
	if(val&0x80){
		val = ~val;
		val = val+1;
		cnt = 0-val;
	}
	return cnt;
}
/*
** 函数功能：将2字节有符号数转为4字节有符号数
** 输入参数: val=单字节有符号数
** 输出参数：无
** 返回值：4字节有符号数
*/
int signed_2to4(short val)
{
	int cnt = val;
	
	if(val&0x8000){
		val = ~val;
		val = val+1;
		cnt = 0-val;
	}
	return cnt;
}
/*
** 函数功能：通过ftp下载升级包
** 输入参数: usr=用户名 pw=密码 ip=服务器IP地址 port=端口 file_name=文件名
** 输出参数：无
** 返回值：0  没有使用
*/
int updata_code(char * usr, char * pw, char * ip, char * port, char * file_name)
{
	char cmd[1024];

	memset(cmd, 0, 1024);
	if(usr == NULL){
		sprintf(cmd, "ftpget -P %s %s /flashDev/data/%s %s", port, ip, file_name, file_name);
	}else if((usr != NULL)&&(pw == NULL)){
		sprintf(cmd, "ftpget -u %s -P %s %s /flashDev/data/%s %s", usr, port, ip, file_name, file_name);
	}else{
		sprintf(cmd, "ftpget -u %s -p %s -P %s %s /flashDev/data/%s %s", usr, pw, port, ip, file_name, file_name);
	}
	printf("%s\n", cmd);
	system(cmd);
    return 0;
}  
/*
** 函数功能：解压bz2的压缩包
** 输入参数: file_name=文件名
** 输出参数：无
** 返回值：0
*/
int uncompress(char * file_name)
{
	char cmd[1024];
	memset(cmd, 0, 1024);
	sprintf(cmd, "chmod 777 %s", file_name);
	printf("%s\n", cmd);
	system(cmd);

	memset(cmd, 0, 1024);
	sprintf(cmd, "tar xjvf %s", file_name);
	printf("%s\n", cmd);
	system(cmd);
	return 0;
}
