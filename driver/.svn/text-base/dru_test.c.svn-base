#include "../common/apue.h"
#include "../common/common.h"
#include "dru_object.h"
/*
** 函数功能：创建dru测试线程
** 输入参数：arg=输入参数
** 输出参数：无
** 返回值：无
** 备注：
*/
int creat_dru_test_task(void)
{
    pthread_t dru_test_id;		// dru测试线程ID
    
    printf("creat_dru_test_task declare ok.\r\n");
    if( pthread_create(&dru_test_id, NULL, dru_obj_main, NULL) ){
		printf("pthread_creat dru_obj_main error.\r\n");
		return 1;
	}
    return 0;
}
