#include "apue.h"
#include "common.h"
#include "../../task/msg/exel_fun.h"
#include "../../task/msg/msg.h"
#include "../../task/log/thread_log.h"
#include "../../task/hdalarm/hdalarm.h"
#include "../../driver/dru_object.h"
#include "../../driver/omap_epld.h"
#include "../../driver/omap.h"
#include "../../driver/dru_mmap.h"
#include "../../sqlite/drudatabase.h"
#include "../../sqlite/sqliteops.h"
#include "../../task/rs485/rs485_modules.h"
#include "status.h"
#include "../../net/net_thread.h"
#include "../../protocol/irdeal.h"
// 弱声明消息队列初始化
static int _msg_init(void)
{
	printf("msg_init pthread not declare.\r\n");
	return 0;
}
int msg_init(void) __attribute__((weak, alias("_msg_init")));

// 弱声明硬件初始化
/*static int _hardware_init(void)
{
	printf("hardware_init pthread not declare.\r\n");
	dru_obj_main(NULL);
	return 0;
}
int hardware_init(void) __attribute__((weak, alias("_hardware_init")));
*/
// 弱声明软件初始化
extern int _Omc_Thread_Init(void);
static int _software_init(void)
{
	printf("software_init pthread not declare.\r\n");
	printf("check app update\n");
	system("/ramDisk/app/cp_app.sh");
	_Omc_Thread_Init();
	if(get_device_type()== DEVICE_TYPE_RAU){
		ru_para_init();
	}
	return 0;
}
int software_init(void) __attribute__((weak, alias("_software_init")));

// 弱声明创建接收消息线程
static int _creat_recv_msg_task(void)
{
	printf("creat_recv_msg_task pthread not declare.\r\n");
	return 0;
}
int creat_recv_msg_task(void) __attribute__((weak, alias("_creat_recv_msg_task")));

// 弱声明rs485通信线程
static int _creat_rs485_task(void)
{
	printf("creat_rs485_task pthread not declare.\r\n");
	return 0;
}
int creat_rs485_task(void) __attribute__((weak, alias("_creat_rs485_task")));

// 弱声明dru硬件测试线程
static int _creat_dru_test_task(void)
{
	printf("creat_dru_test_task pthread not declare.\r\n");
	return 0;
}
int creat_dru_test_task(void) __attribute__((weak, alias("_creat_dru_test_task")));

extern int _Omc_Thread(void);
/*
** 函数功能：dru主函数
** 输入参数：argc=输入参数个数 argv=字符串格式的输入参数
** 输出参数：无
** 返回值：0=成功 1=失败
** 备注：
*/
int main(int argc, char * argv[])
{
	void *str;
	if(read_device_type()!=0){
		exit(0);
	}
	log_thread_init();
	dru_obj_main(str);
	msg_init();             // 进程间通信的消息队列初始化
	_sem_init();			// 信号量初始化
    software_init();        // 软件初始化 
    creat_recv_msg_task();  // 创建接收消息线程
	creat_rs485_task();     // 创建rs485通信线程
	_Omc_Thread();
    creat_net_task();
	create_hardware_alarm_thread();
    while(1)
	{
		sleep(10);
	}
    return 0;
}  
