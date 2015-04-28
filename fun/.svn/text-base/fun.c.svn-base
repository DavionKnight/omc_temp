#include "../common/apue.h"
#include "../common/common.h"

/******************************************************************************/


/******************************************************************************/


/******************************************************************************/ 
struct mymesg msg_buf;
/*
** 函数功能：组织发送消息的数据包
** 输入参数：type=消息类型 argc=消息体参数个数 argv=消息体中参数内容 
** 输出参数：无
** 返回值：0=成功
** 备注：无
*/
int packet_msg(int type, int argc, char * argv[])
{
	char tbuf[256];
	int i = 0;

	memset(tbuf, 0, 256);
	for(i = 0; i < argc-1; i++){
		strcat(tbuf, argv[i+1]);
		strcat(tbuf, " ");
	}
	msg_buf.mtype = type;
	sprintf(msg_buf.mtext, "%s", tbuf);
	return 0;
}
/*
** 函数功能：处理接收到的消息，将消息打印出来
** 输入参数：buf=消息缓冲区 len=数据长度
** 输出参数：无
** 返回值：0=成功
** 备注：无
*/
int echo_fun(char * buf, int len)
{
	struct mymesg * p_msg_buf;

	p_msg_buf = (struct mymesg *)buf;
	switch(p_msg_buf->mtype){// 根据不同的消息类型，做不同的处理
		case MSG_FUN_RESULT:
			printf("%s", p_msg_buf->mtext);
			break;
		default:
			printf("消息类型错误.\r\n");
	}
	return 0;
}
/*
** 函数功能：fun主函数
** 输入参数：argc=输入参数个数 argv=字符串格式的输入参数
** 输出参数：无
** 返回值：0=成功 1=失败
** 备注：
*/
int main(int argc, char * argv[])
{
	// 判断输入的参数个数是否正确
	if(argc < 2){
		printf("please input the function name:\r\n");
		return 1;
	}
	// 将输入的参数及参数个数组成消息队列数据包
	packet_msg(MSG_FUN, argc, argv);
	// 向TASK1_MODULE模块发送消息
	msg_send(TASK1_MODULE, (char *)&msg_buf, strlen(msg_buf.mtext));
	// 等待返回消息，然后退出程序
	msg_recv_ex(TASK2_MODULE, echo_fun);
    return 0;
}  
