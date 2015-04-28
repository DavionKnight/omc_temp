/* 
 * pppd dial
 * author: Huyubin
 * date:2013-08-08
 * 该文件为使用
 */
#include "../../common/apue.h"
#include "../../common/common.h"
#include "../../sqlite/drudatabase.h"

// check pppd status
int check_pppd(void)
{
	FILE * gprs_file;
	char * one_line;
	int buff_size = 512;

	printf("ifconfig ppp0\n");
	system("ifconfig ppp0 > /var/gprs_status");
	printf("ifconfig ppp0 end\n");
	if((gprs_file = fopen("/var/gprs_status", "r")) == NULL){
		printf("can't open /var/gprs_status\n");
		return -1;
	}
	one_line = (char *)malloc(buff_size);
	if(fgets(one_line, buff_size, gprs_file) == NULL){
		printf("no ppp0\n");
		fclose(gprs_file);
		free(one_line);
		return 0;	
	}
	if(fgets(one_line, buff_size, gprs_file) != NULL){
		if(NULL == strstr(one_line, "inet addr:")){
			printf("no ppp0\n");
			fclose(gprs_file);
			free(one_line);
			return 0;
		}
	}else{
		printf("no ppp0\n");
		fclose(gprs_file);
		free(one_line);
		return 0;	
	}
	fclose(gprs_file);
	free(one_line);
	printf("ppp0 ok\n");	

	return 1;
}

// dial on
int dial_on(void)
{
	int cnt = 0;

	printf("killall pppd\n");
	system("killall pppd");
	sleep(5);
	printf("start dialing ...\n");
	system("pppd file /flashDev/program/peers/gprs-gsm &");
	while(cnt < 15){
		if(check_pppd() == 1){
			system("route add default dev ppp0");
			printf("dial ok and add default route\n");
			return 1;
		}
		cnt++;
		sleep(5);
	}
	printf("dial error\n");
	return 0;
}
// dial off
void dial_off(void)
{
	system("killall pppd");
}
/*
** 函数功能：gprs dialing
** 输入参数：arg
** 输出参数：无
** 返回值：无
** 备注：
*/
void * gprs_pthread(void * agr)
{
	int ret = 0;
	int data = 0;

	while(1){
		ret = DbGetThisIntPara(0x0000000B, &data);
		if(ret == 1){
			if(data != 5){
				if(1 != check_pppd()){
					dial_on();
				}
			}else{
				if(1 == check_pppd()){
					dial_off();
				}
			}
		}else{
			printf("read 0x000B error !\r\n");
		}
		sleep(5);
	}
}
