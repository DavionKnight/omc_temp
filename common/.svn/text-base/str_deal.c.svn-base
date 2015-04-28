/*******************************************************************************
********************************************************************************
* 文件名称:  str_deal.c
* 功能描述:  字符串高级操作
* 使用说明:  
* 文件作者:	H4
* 编写日期: （2013/04/11）
* 修改历史:
* 修改日期    修改人       修改内容
*-------------------------------------------------------------------------------

*******************************************************************************/
#include <stdio.h>
#include <string.h>
#define MAX_STRING_LENGTH 200
/*******************************************************************************
* 函数名称: strdelnell
* 功    能: 去除字符串中不可见字符（ASC||）
* 参    数: oarg sarg
* 参数名称         类型                描述
*	darg					char *						目的串地址
* sarg					const char * 			源串地址
* length				unsigned int      源串长度，应小于或等于目的串长度
* 返回值:	
* 0:成功   -1：输入长度错
* 说   明:
* 日   期     版本    作者   修改人      DEBUG
* -----------------------------------------------------------------
* 2013/04/11  V1.0     H4     无       ：
*******************************************************************************/
int strdelnull(const char* sarg,char *darg,unsigned int length)
{
	int i,j=0;
	for(i=0;i<length;i++){
		if((sarg[i]>='!')&&(sarg[i]<='~')){
			darg[j]=sarg[i];
			j++;
		}
	}
	return 0;
}
/*******************************************************************************
* 函数名称: strcmpasc
* 功    能: 去除串中所有不可见字符后比较串
* 参    数: str1 str2
* 参数名称         类型                描述
*	str1					const char *						目的串地址
* str2					const char * 			源串地址
* 返回值:	
* 0:相同   -1，1，2，：都为不相同
* 说   明:
* 日   期     版本    作者   修改人      DEBUG
* -----------------------------------------------------------------
* 2013/04/11  V1.0     H4     无       ：
*******************************************************************************/
int strcmpasc(const char *str1,const char *str2,unsigned int length)
{
	int ret;
	char str1tmp[MAX_STRING_LENGTH];
	char str2tmp[MAX_STRING_LENGTH];
	if((length>MAX_STRING_LENGTH)&&(length=0)){
		return 2;
	}
	memset(str1tmp,0,MAX_STRING_LENGTH);
	memset(str2tmp,0,MAX_STRING_LENGTH);
	strdelnull(str1,str1tmp,length);
	strdelnull(str2,str2tmp,length);
	ret=strcmp(str1tmp,str2tmp);
	//printf("str1 = %s\r\nstr2 = %s\r\n",str1tmp,str2tmp);
	//printf("ret = %d\r\n",ret);
	return ret;
}

/*int main(void)
{
	char mstr[40]="dru201301 00.33 ";
	char rstr[40]="dru20 1301 00. 33";
		int i;
		i=strcmpasc(mstr,rstr,40);
		
		printf("ret = %d\r\n",i);
}*/
