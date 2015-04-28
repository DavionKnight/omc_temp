#include "apue.h"

/******************************************************************************/


/******************************************************************************/


/******************************************************************************/ 
/*
** 函数功能：main主函数
** 输入参数：argc=输入参数个数 argv=字符串格式的输入参数
** 输出参数：无
** 返回值：0=成功 1=失败
** 备注：
*/
int main(int argc, char * argv[])
{
	int fp = 0;
	int cnt = 0;
	unsigned int flag = 0;

	fp = open("/dev/gpio_int", O_RDONLY);
	if(fp < 0){
		printf("open /dev/gpio_int error.\n");
		return -1;
	}
	cnt = read(fp, (void *)&flag, 4); 	
	if(cnt < 0){
		printf("read /dev/gpio_int error.\n");
		return -2;
	}
	printf("cnt = %d.\n", cnt);
	printf("interrupt_flag = 0x%08X.\n", flag);

	return 0;
}  
