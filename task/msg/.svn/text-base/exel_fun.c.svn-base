#include "../../common/apue.h"
#include "../../common/common.h"
#include "../../driver/omap.h"

/******************************************************************************/


/******************************************************************************/

struct mymesg msg_tmp;

/******************************************************************************/ 
/*
** 函数功能：计算2个整数的和
** 输入参数：a=加数, b=加数
** 输出参数：无
** 返回值：和
*/
int sum(int a, int b)
{
	return (a+b);
}
// 测试代码，没有使用
int sum_change(int argc, char * argv[])
{
	int tmp = 0;

	msg_tmp.mtype = MSG_FUN_RESULT;	
	if(argc != 3){
		printf("input para cnt is not 3.\r\n");
		sprintf(msg_tmp.mtext, "intput para cnt is not 3.\r\n");
		msg_send(TASK2_MODULE, (char *)&msg_tmp, strlen(msg_tmp.mtext));	
		return 1;
	}
	tmp = sum(atoi(argv[1]), atoi(argv[2]));
	printf("sum(%s, %s)=%d\r\n", argv[1], argv[2], tmp);
	sprintf(msg_tmp.mtext, "sum(%s, %s)=%d\r\n", argv[1], argv[2], tmp);
	msg_send(TASK2_MODULE, (char *)&msg_tmp, strlen(msg_tmp.mtext));	

	return 0;
}
// linux代码升级, 没有使用
int updata_code_change(int argc, char * argv[])
{
	msg_tmp.mtype = MSG_FUN_RESULT;	
	if(argc != 6){
		printf("input para cnt is not 6.\r\n");
		sprintf(msg_tmp.mtext, "intput para cnt is not 6. usr,pw,ip,port,file_name\r\n");
		msg_send(TASK2_MODULE, (char *)&msg_tmp, strlen(msg_tmp.mtext));	
		return 1;
	}
	updata_code(argv[1], argv[2], argv[3], argv[4], argv[5]);
	printf("updata(%s, %s, %s, %s, %s);", argv[1], argv[2], argv[3], argv[4], argv[5]);
	sprintf(msg_tmp.mtext, "updata(%s, %s, %s, %s, %s);", argv[1], argv[2], argv[3], argv[4], argv[5]);
	msg_send(TASK2_MODULE, (char *)&msg_tmp, strlen(msg_tmp.mtext));	

	return 0;
}
/*
** 函数功能：计算2个整数的差
** 输入参数：a=减数, b=被减数
** 输出参数：无
** 返回值：差值
*/
int sub(int a, int b)
{
	return (a-b);
}
int sub_change(int argc, char * argv[])
{
	int tmp = 0;

	msg_tmp.mtype = MSG_FUN_RESULT;	
	if(argc != 3){
		printf("input para cnt is not 3.\r\n");
		sprintf(msg_tmp.mtext, "intput para cnt is not 3.\r\n");
		msg_send(TASK2_MODULE, (char *)&msg_tmp, strlen(msg_tmp.mtext));	
		return 1;
	}
	tmp = sub(atoi(argv[1]), atoi(argv[2]));
	printf("sub(%s, %s)=%d\r\n", argv[1], argv[2], tmp);
	sprintf(msg_tmp.mtext, "sub(%s, %s)=%d\r\n", argv[1], argv[2], tmp);
	msg_send(TASK2_MODULE, (char *)&msg_tmp, strlen(msg_tmp.mtext));	
	return 0;
}
// 定义函数指针
typedef int (*PFUN)(int argc, char * argv[]);
// 函数对照表最小元素
struct fun_struct
{
	char * fun_name; // 函数名称字符串
	PFUN fun_change; // 函数指针
};

int dis_fun(int argc, char * argv[]);
extern int display_change(int argc, char * argv[]);
// 函数对照表, 函数名称字符串和函数指针
struct fun_struct fun_table[] = 
	{
		{	"-h",
			dis_fun,
		},
		{
			"sum",
			sum_change,
		},
		{	"sub",
			sub_change,
		},
		{	"dru_cpld_write",
			dru_epld_write_change,
		},
		{	"dru_cpld_read",
			dru_epld_read_change,
		},
		{	"dru_fpga_write",
			dru_fpga_write_change,
		},
		{	"dru_fpga_read",
			dru_fpga_read_change,
		},
		{	"emif_cpld_adc_spi_write",
			emif_epld_adc_spi_write_change,
		},
		{	"dru_8873_write",
			dru_8873_write_change,
		},
		{	"dru_8873_read",
			dru_8873_read_change,
		},
		{	"set_pin_tx",
			set_pin_tx_change,
		},
		{	"dru_init",
			dru_init_change,
		},
		{	"dru_lmx2531_wcdma_config",
			dru_lmx2531_wcdma_config_change,
		},
		{	"dru_lmx2531_lte1_config",
			dru_lmx2531_lte1_config_change,
		},
		{	"dru_lmx2531_lte2_config",
			dru_lmx2531_lte2_config_change,
		},
		{	"dac3484_gsm_qmc_offset_a",
			dac3484_gsm_qmc_offset_a_change,
		},
		{	"dac3484_gsm_qmc_offset_b",
			dac3484_gsm_qmc_offset_b_change,
		},
		{	"emif_fpga_ddc_carrier_f8_write",
			emif_fpga_ddc_carrier_f8_write_change,
		},
		{	"emif_fpga_ddc_carrier_m8_write",
			emif_fpga_ddc_carrier_m8_write_change,
		},
		{	"emif_fpga_duc_carrier_f8_write",
			emif_fpga_duc_carrier_f8_write_change,
		},
		{	"emif_fpga_duc_carrier_m8_write",
			emif_fpga_duc_carrier_m8_write_change,
		},
		{	"dru_get_version",
			dru_get_version_change,
		},
		{	"updata_cpld",
			updata_cpld_change,
		},
		{	"display",
			display_change,
		},
		{	"updata_code",
			updata_code_change,
		},
		{	"download_fpga",
			download_fpga_change,
		},
		{	"delay_measure_action",
			delay_measure_action_change,
		},
		{	"dru_lmx2531_power_down",
			dru_lmx2531_power_down_change,
		},
		{	"dru_lmx2531_power_up",
			dru_lmx2531_power_up_change,
		},
		{	"set_lm75",
			set_lm75_change,
		},
		{	"set_reboot",
			set_reboot_change,
		},
		{	"report",
			report_change,
		},
		{	"set_alarm",
			set_alarm_change,
		},
		{	"gz_test",
			gz_test_change,
		},
		{	"alarm_debug",
			alarm_debug_change,
		},
		{	"set_lo",
			set_lo_change,
		},
		{	"no_alarm",
			no_alarm_change,
		},
	};

// 宏定义计算函数列表中的函数个数
#define FUN_TABLE_CNT (sizeof(fun_table)/sizeof(struct fun_struct))

/*
** 函数功能：显示支持的函数名
** 输入参数：argc=输入的参数个数 argv=函数名和参数
** 输出参数：无
** 返回值：0=成功 1=失败
** 备注：
*/
int dis_fun(int argc, char * argv[])
{
	int i = 0;
	int j = 0;

	// 计算函数列表中支持的函数个数
	i = FUN_TABLE_CNT;			
	// 将函数名称存入消息队列中
	memset(msg_tmp.mtext, 0, MSG_BUF_LEN);
	for(j = 0; j < i; j++){
		strcat(msg_tmp.mtext, fun_table[j].fun_name);
		strcat(msg_tmp.mtext, "\r\n");
	}
	// 发送消息
	msg_tmp.mtype = MSG_FUN_RESULT;	
	msg_send(TASK2_MODULE, (char *)&msg_tmp, strlen(msg_tmp.mtext));	

	return 0;
}
/*
** 函数功能：执行输入的函数
** 输入参数：argc=输入的参数个数 argv=函数名和参数
** 输出参数：无
** 返回值：0=成功 1=失败
** 备注：
*/
int exel_fun(int argc, char * argv[])
{
	int i = 0;
	int j = 0;	
	
	// 计算函数对照表中支持的函数个数
	i = FUN_TABLE_CNT;
	// 在函数对照表中查找是否有对应的函数
	for(j = 0; j < i; j++){
		if(0 == strcmp(fun_table[j].fun_name, argv[0])){
			break;
		}
	}
	if(j < i){
		// 执行对应的函数
		lock_sem(SEM_DRV);
		fun_table[j].fun_change(argc, argv);
		unlock_sem(SEM_DRV);
	}else{
		// 函数对照表中没有该函数，不支持
		printf("not find fun.\r\n");
		msg_tmp.mtype = MSG_FUN_RESULT;
		sprintf(msg_tmp.mtext, "not find %s.\r\n", argv[0]);
		msg_send(TASK2_MODULE, (char *)&msg_tmp, strlen(msg_tmp.mtext));	
		return 1;
	}
    return 0;
}  
