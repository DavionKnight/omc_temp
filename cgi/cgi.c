#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include "../common/common.h"

// 没有使用
char * id_list = "&0x0120=站点编号&0x0130=服务器IP地址&0x0131=设备编号&0x0234=设备IP&0x0301=输出功率1&0x0501=输出功率2&0x0201=输出功率3&0x0401=输出功率4&0x0601=输出功率5&0x0701=输出功率6&0x0801=输出功率7";
// 没有使用
char * GetParaList(void)
{
	return id_list;	
}
// 没有使用
int GetDataById(int id, char * data)
{
	switch(id){
		case 0x0004:
			strcpy(data, "DRU-RAU");// 设备型号
			break;
		case 0x000a: // DRU版本号
			strcpy(data, "DRU20130225.0");
			break;
		case 0x01a0: // CPLD版本号
			strcpy(data, "V8");
			break;
		case 0x0021: // FPGA版本号
			strcpy(data, "V17");
			break;
		default:
			strcpy(data, "hello default");
	}
	return 0;
}
// 没有使用
int SetDataById(int id, char * data)
{
	switch(id){
		case 0x0004:
			strcpy(data, "DRU-RAU");// 设备型号
			break;
		case 0x000a: // DRU版本号
			strcpy(data, "DRU20130225.0");
			break;
		case 0x01a0: // CPLD版本号
			strcpy(data, "V8");
			break;
		case 0x0021: // FPGA版本号
			strcpy(data, "V17");
			break;
		default:
			strcpy(data, "not find");
	}
	return 0;
}
// 没有使用
int checkType(char * data)
{
	char * p = NULL;
	int flag = 0;
	char buf[128];
	int id;
	char * ps = NULL;
	
	p = strsep(&data, "&");
	if(p == NULL){
		printf("url get error!");
		return -1;
	}
	if((strstr(p, "read") != NULL)){
		flag = 1;
		printf("cmd=read");
		for(p = strsep(&data, "&"); p != NULL;p = strsep(&data, "&")){
			id = 0;
			sscanf(p, "0x%04X=?", &id);
			memset(buf, 0, 128);
			GetDataById(id, buf);
			printf("&0x%04X=%s", id, buf);
		}
	}else if((strstr(p, "write") != NULL)){
		printf("cmd=write");
		flag = 2;
		for(p = strsep(&data, "&"); p != NULL;p = strsep(&data, "&")){
			memset(buf, 0, 128);
			sscanf(p, "0x%04X=%s", &id, buf);
			printf("&0x%04X=%s", id, buf);
		}
	}else if((strstr(p, "para_list") != NULL)){
		printf("cmd=para_list");
		ps = GetParaList();
		if(ps != NULL){
			printf(ps);
		}
	}else{
		printf("cmd error");
		return 0;
	}
	return flag;
}
// 组织消息队列数据包
struct mymesg msg_buf;
int packet_msg(int type, char * argv)
{
	msg_buf.mtype = type;
	sprintf(msg_buf.mtext, "%s", argv);
	return 0;
}
// 处理DRU进程应答的消息队列，发送给WEB前台
int deal_cgi(char * buf, int len)
{
	struct mymesg * p_msg_buf;

	p_msg_buf = (struct mymesg *)buf;
	switch(p_msg_buf->mtype&(~0x80)){
		case MSG_CGI_RESULT:
			printf("%s", p_msg_buf->mtext);
			break;
		default:
			printf("mytype=0x%02X\r\n", (unsigned int)p_msg_buf->mtype);
			printf("消息类型错误.\r\n");
	}
	return 0;
}
// CGI主程序，重定向到BOA
int main(void)
{
    time_t current;
    struct tm *timeinfo;
	char * pRequestMethod;
	char * p = NULL;
	int len = 0;
	msg_init(); // 初始化消息队列
    time(&current); // 获取当前时间
    timeinfo = localtime(&current);
	//这一句一定要加，否则异步访问会出现页面异常
	printf("Content-type:text/html;charset=UTF-8;\n\n");
    //printf("%s", asctime(timeinfo));
	// 获取http请求类型
	pRequestMethod = getenv("REQUEST_METHOD");
	if(pRequestMethod == NULL){
		printf("&request=null");
	}else if(strstr(pRequestMethod, "POST") != NULL){
		// POST类型, 获取请求字符串的长度
		p = getenv("CONTENT_LENGTH");
		if(p != NULL){
			len = atoi(p);
		}else{
			len = 0;
		}
		p = malloc(len+1);
		if(p == NULL){
			printf("&malloc error");
			return 0;
		}
		// 将WEB请求的字符串，以消息队列的形式发送给DRU进程
		memset(p, 0, len+1);
		fgets(p, len+1, stdin);
		packet_msg(MSG_CGI, p);
		// 发送消息队列给DRU进程
		msg_send(TASK1_MODULE, (char *)&msg_buf, strlen(msg_buf.mtext));
		// 接收DRU进程的应答消息队列
		msg_recv_ex(TASK3_MODULE, deal_cgi);
		//printf(p);
		free(p);
	}else if(strstr(pRequestMethod, "GET") != NULL){
		// GET类型，程序没有使用
		printf("request=get\r\n");
		p = getenv("QUERY_STRING");
		if(p != NULL){
			packet_msg(MSG_CGI, p);
			msg_send(TASK1_MODULE, (char *)&msg_buf, strlen(msg_buf.mtext));
			msg_recv_ex(TASK3_MODULE, deal_cgi);
		}
	}else{
		printf("method is error");
	}
	
	fflush(stdout);
	return 0;
}
