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
#include <fcntl.h>
#include "type_def.h"
#include "readprofile.h"
#include "../protocol/ir_def.h"
#include "status.h"
U8 device_type=1;
int g_net_type = 0;

dev_info_t dru_dev_info;
dev_info_t *dru_dev_p;
/*******************************************************************************
* 函数名称: get_device_type
* 功    能: 获取设备类型
* 参    数:设备类型记录在文件中
*程序运行初始由程序读取文件
*获取设备类型。取得后设备类型的输出又本函数实现
* 参数名称         类型                描述
* 返回值:
* 
* 说   明:
* 日   期     版本    作者   修改人      DEBUG
* -----------------------------------------------------------------------------
* 2012/11/18  V1.0     H4     无       ：
*******************************************************************************/
U8 read_device_type(void)
{
	int fd,n;
	char str[10];
	char *rau="RAU";
	char *mai="MAIN";
	char *expend="EXPEND";
	read_pkg_version();
	read_dev_info();
	fd=open("/flashDev/program/config/sys_cfg.txt", O_RDONLY);
	if(fd==-1){
		printf("open sys_cfg.txt file error\n");
		goto _err;
	}
	n=read(fd,str,10);
	if((n==0)||(n<3)){
		printf("read config file error\n");
		goto _err;
	}
	if(strncmp(rau,str,3)==0){
		device_type=DEVICE_TYPE_RAU;
		printf("config for rau\n");
		goto _out;
	}else if(strncmp(mai,str,3)==0){
		device_type=DEVICE_TYPE_MAIN;
		printf("config for main\n");
		goto _out;
	}else if(strncmp(expend,str,3)==0){
		device_type=DEVICE_TYPE_EXPEND;
		printf("config for expend\n");
		goto _out;
	}else{
		printf("config for none\n");
		goto _err;
	}
_err:
	close(fd);
	return 1;
_out:
	close(fd);
	return 0;
}

/*******************************************************************************
* 函数名称: read_dev_info
* 功    能: 获取设备详细信息
* 参    数:
*
* 参数名称         类型                描述
* 返回值:
* 
* 说   明:
* 日   期     版本    作者   修改人      DEBUG
* -----------------------------------------------------------------------------
* 2015/02/26  V1.0     H4     无       ：
*******************************************************************************/
void read_dev_info(void)
{
	char str[150];
	dru_dev_p=&dru_dev_info;
	GetProfileString("/flashDev/program/config/sys_cfg.txt","dev_info","dev_vco",str);
	if(strcmp(str,"LM2531_CMGSM_LTEE")==0){
		dru_dev_p->dev_vco=DEV_LM2531_CMGSM_LTEE;
	}else if(strcmp(str,"LM2531_CMGSM_LTED")==0){
		dru_dev_p->dev_vco=DEV_LM2531_CMGSM_LTED;
	}else if(strcmp(str,"LM2531_CMGSM_LTEF")==0){
		dru_dev_p->dev_vco=DEV_LM2531_CMGSM_LTEF;
	}else if(strcmp(str,"LM2581_CMGSM_LTED")==0){
		dru_dev_p->dev_vco=DEV_LM2581_CMGSM_LTED;
	}else if(strcmp(str,"LM2581_CMGSM_LTEE")==0){
		dru_dev_p->dev_vco=DEV_LM2581_CMGSM_LTEE;
	}else if(strcmp(str,"LM2581_CMGSM_LTEF")==0){
		dru_dev_p->dev_vco=DEV_LM2581_CMGSM_LTEF;
	}else{
		dru_dev_p->dev_vco=0x00;
	}
	printf("dev_vco = %d\r\n",dru_dev_p->dev_vco);
	GetProfileString("/flashDev/program/config/sys_cfg.txt","dev_info","dev_att",str);
	if(strcmp(str,"ATT_SS1112_TDD")==0){
		dru_dev_p->dev_att=DEV_SS1112_TDD;
	}else{
		dru_dev_p->dev_att=0x00;
	}
	printf("dev_att = %d\r\n",dru_dev_p->dev_att);
	//exit(0);
}

// 读取网络制式组合
char g_net_buf[128];
int read_net_type(void)
{
	FILE * fp = NULL;
	char * p = NULL;

	fp = fopen("/flashDev/program/config/dev_name", "r");
	if(fp == NULL){
		printf("open file dev_name error.\r\n");
		return -1;
	}
	p = fgets(g_net_buf, sizeof(g_net_buf), fp);
	if(p == NULL){
		printf("read dev_name error.\n");
		return -2;
	}
	g_net_type = g_net_buf[4]-'0';	
	return g_net_type;
}
// 判断字段意思
int get_net_type(void)
{
	return g_net_type;
}
// 判断字段意思
int get_net_group(void)
{
	int tmp = 0;
	int val1, val2;

	switch(g_net_buf[2]){
		case 'm': // 移动
			tmp = 0x100;
			break;
		case 'u': // 联通
			tmp = 0x200;
			break;
		case 't': // 电信
			tmp = 0x300;
			break;
		default:
			;
	}
	if(g_net_buf[3] == 'x'){
		val1 = 0;
	}else{
		val1 = g_net_buf[3] - '0'; // 网络数量
	}
	if(g_net_buf[4] == 'x'){
		val2 = 0;
	}else{
		val2 = g_net_buf[4] - '0'; // 网络组合方式
	}
	tmp = tmp | (val1<<4) | val2;

	return tmp;
}
// 返回dev_name字符串
U8 get_device_type(void)
{
	return device_type;
}
/***********************************************************************
** Funcation Name : read_pkg_version
** Author         : H4
** Description    :	read the version number of the package 
** Input          : 
** Output         : 
** date           : 2013年05月09日 星期四 11时20分28秒
** Version        : V1.0
** Modify         : 
***********************************************************************/
extern rru_software_info dru_rru_software_info;
void read_pkg_version(void)
{
	int fd,ret;
	fd=open("/ramDisk/pkg_version.txt",O_RDONLY);
	if(fd==-1){
		printf("open pkg_version.txt error\n");
		close(fd);
		exit(1);
	}
	printf("open pkg_version number ok\r\n");
	memset(dru_rru_software_info.software_version,0,40);
	ret=read(fd,dru_rru_software_info.software_version,40);
	if(ret<=0){
		printf("read pkg_version.txt error\n");
		close(fd);
		exit(1);
	}
	close(fd);
}

