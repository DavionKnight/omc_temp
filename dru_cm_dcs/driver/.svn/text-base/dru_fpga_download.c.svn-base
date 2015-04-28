/*******************************************************************************
********************************************************************************
* 文件名称:  dru_fpga_download.c
* 功能描述:  fpga 下载程序
*						
* 使用说明: 
* 文件作者:	H4
* 编写日期: （2012/06/26）
* 修改历史:
* 修改日期    修改人       修改内容
*-------------------------------------------------------------------------------

*******************************************************************************/
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <sys/mman.h>
#include <string.h>
#include <time.h>
#include "../../driver/omap_epld.h"
#include "../../driver/dru_fpga_download.h"
#include "../../driver/dru_reg.h"
/*******************************************************************************
* 函数名称: task_delay
* 功    能: 用于fpga下载延时
* 参    数:
* 参数名称         类型                描述
* delayTime      unsigned char      延时时长
* 返回值:
* 无
* 说   明:
* 日   期     版本    作者   修改人      DEBUG
* -----------------------------------------------------------------
* 2012/06/15  V1.0     H4     无       ：
*******************************************************************************/
void task_delay(unsigned char delayTime)                                                   
{                                                                                        
    unsigned int index = 0;                                                                    
    for (index = 0; index < delayTime * 10; index++)   
    {                                                                                    
        asm(" nop");                                                                     
    }                                                                                    
    return;                                                                              
}          
/*******************************************************************************
* 函数名称: dru_fpga_download
* 功    能: dac3484初始化配置
* 参    数:
* 参数名称         类型                描述
* 无
* 返回值:
* 无
* 说   明:
* 日   期     版本    作者   修改人      DEBUG
* -----------------------------------------------------------------
* 2012/06/18  V1.0     H4     无       ：
*******************************************************************************/
void dru_fpga_download(unsigned int filelength,char *filename)
{
	unsigned char readstatus;
	unsigned short regdata;
	unsigned int index,i,flength,leftlength;
	int fd,status;
	char tmpbuffer[BUFFER_SIZE];
	printf("fd ok\n");
	fd=open(filename,O_RDONLY);
	if(fd==-1)
	{
		printf("open error.\r\n");
		return;
	}
	//WRITE_EPLD(DOWNLOAD_FPGA_CS,FPGA_L);
	*DRU_REGADDR(epld_base_addr, EPLD_REGOFFSET_DOWNLOAD_FPGA_CS)=FPGA_L;
	usleep(10);
	//WRITE_EPLD(DOWNLOAD_FPGA_PROG,FPGA_L);
	*DRU_REGADDR(epld_base_addr, EPLD_REGOFFSET_DOWNLOAD_FPGA_PROG)=FPGA_L;
		usleep(600);
	//WRITE_EPLD(DOWNLOAD_FPGA_PROG,FPGA_H);
	*DRU_REGADDR(epld_base_addr, EPLD_REGOFFSET_DOWNLOAD_FPGA_PROG)=FPGA_H;
	for(index=0;index<WAIT_TIME;index++)
	{
		usleep(1);
		//READ_EPLD(DOWNLOAD_FPGA_INIT, readstatus);
		readstatus=*DRU_REGADDR(epld_base_addr, EPLD_REGOFFSET_DOWNLOAD_FPGA_INIT);
		if(readstatus){
			break;
		}
		if(index==(WAIT_TIME-1)){
			printf("init error\n");
			return;
		}
	}
	status=read(fd,tmpbuffer,BUFFER_SIZE);
	if(status==-1){
		//WRITE_EPLD(DOWNLOAD_FPGA_CS, FPGA_H);
		*DRU_REGADDR(epld_base_addr, EPLD_REGOFFSET_DOWNLOAD_FPGA_CS)=FPGA_H;
		close(fd);
		printf("read error\n");
		return;
	}
	//WRITE_EPLD(DOWNLOAD_FPGA_CLK,FPGA_L);
	*DRU_REGADDR(epld_base_addr, EPLD_REGOFFSET_DOWNLOAD_FPGA_CLK)=FPGA_L;
	//printf("tmpbuffer is %x\n",tmpbuffer[32]);
	i=BUFFER_SIZE;
	for(index=0;index<i;index++)
	{
		regdata=(unsigned short)tmpbuffer[index];
		*DRU_REGADDR(epld_base_addr, EPLD_REGOFFSET_DOWNLOAD_FPGA_DATA)=regdata;
		*DRU_REGADDR(epld_base_addr, EPLD_REGOFFSET_DOWNLOAD_FPGA_CLK)=FPGA_L;
		*DRU_REGADDR(epld_base_addr, EPLD_REGOFFSET_DOWNLOAD_FPGA_CLK)=FPGA_H;
		*DRU_REGADDR(epld_base_addr, EPLD_REGOFFSET_DOWNLOAD_FPGA_CLK)=FPGA_L;
		//WRITE_EPLD(DOWNLOAD_FPGA_DATA,regdata);
		//WRITE_EPLD(DOWNLOAD_FPGA_CLK,FPGA_L);
		//WRITE_EPLD(DOWNLOAD_FPGA_CLK,FPGA_H);
		//WRITE_EPLD(DOWNLOAD_FPGA_CLK,FPGA_L);
	}
      	for(i=15000;i>0;i--)
      	{
      		*DRU_REGADDR(epld_base_addr, EPLD_REGOFFSET_DOWNLOAD_FPGA_CLK)=FPGA_L;
		*DRU_REGADDR(epld_base_addr, EPLD_REGOFFSET_DOWNLOAD_FPGA_CLK)=FPGA_H;
		*DRU_REGADDR(epld_base_addr, EPLD_REGOFFSET_DOWNLOAD_FPGA_CLK)=FPGA_L;
		*DRU_REGADDR(epld_base_addr, EPLD_REGOFFSET_DOWNLOAD_FPGA_CLK)=FPGA_H;
		//WRITE_EPLD(DOWNLOAD_FPGA_CLK,FPGA_L);
		//WRITE_EPLD(DOWNLOAD_FPGA_CLK,FPGA_H);
		//WRITE_EPLD(DOWNLOAD_FPGA_CLK,FPGA_L);
		//WRITE_EPLD(DOWNLOAD_FPGA_CLK,FPGA_H);		
      	}
	for(index=0;index<WAIT_TIME;index++)
	{
		//READ_EPLD(DOWNLOAD_FPGA_DONE, readstatus);
		readstatus=*DRU_REGADDR(epld_base_addr, EPLD_REGOFFSET_DOWNLOAD_FPGA_DONE);
		if(readstatus){
		printf("done ok\n");

			break;
		}
		if(index==(WAIT_TIME-1)){
			printf("done kerror\n");
			close(fd);
			return;
		}
	}
	//WRITE_EPLD(DOWNLOAD_FPGA_CS, FPGA_L);
	*DRU_REGADDR(epld_base_addr, EPLD_REGOFFSET_DOWNLOAD_FPGA_CS)=FPGA_L;
	close(fd);
	printf("close ok\n");
}
/*void dru_fpga_download(unsigned int filelength,char *filename)
{
	unsigned char readstatus;
	unsigned short regdata;
	unsigned int index,i,flength,leftlength;
	int fd,status;
	char tmpbuffer[BUFFER_SIZE];
	WRITE_EPLD(DOWNLOAD_FPGA_CS,FPGA_L);
	usleep(10);
	WRITE_EPLD(DOWNLOAD_FPGA_PROG,FPGA_L);
		usleep(600);
	WRITE_EPLD(DOWNLOAD_FPGA_PROG,FPGA_H);
	for(index=0;index<WAIT_TIME;index++)
	{
		usleep(1);
		READ_EPLD(DOWNLOAD_FPGA_INIT, readstatus);
		if(readstatus){
			break;
		}
		if(index==(WAIT_TIME-1)){
			printf("init error\n");
			return;
		}
	}
	printf("fd ok\n");
	fd=open(filename,O_RDONLY);
	if(fd==-1)
	{
		printf("open error\n");
		return;
	}
	printf("open ok\n");
	WRITE_EPLD(DOWNLOAD_FPGA_CLK,FPGA_L);
	i=0;
	leftlength=filelength;
	for(;;){
		status=read(fd,tmpbuffer,BUFFER_SIZE);
		if(status==-1){
			WRITE_EPLD(DOWNLOAD_FPGA_CS, FPGA_H);
			close(fd);
			printf("read error\n");
			return;
		}
		
		if(leftlength<BUFFER_SIZE){
			i=leftlength;
			leftlength=0;
		}
		else{
			i=BUFFER_SIZE;
			leftlength-=BUFFER_SIZE;
		}
		for(index=0;index<i;index++)
		{
			regdata=(unsigned short)tmpbuffer[index];
			WRITE_EPLD(DOWNLOAD_FPGA_DATA,regdata);
			WRITE_EPLD(DOWNLOAD_FPGA_CLK,FPGA_L);
			WRITE_EPLD(DOWNLOAD_FPGA_CLK,FPGA_H);
			WRITE_EPLD(DOWNLOAD_FPGA_CLK,FPGA_L);
		}
		if(leftlength==0){
			break;
		}
	}
      	for(i=25000;i>0;i--)
      	{
      		WRITE_EPLD(DOWNLOAD_FPGA_CLK,FPGA_L);
		WRITE_EPLD(DOWNLOAD_FPGA_CLK,FPGA_H);
		WRITE_EPLD(DOWNLOAD_FPGA_CLK,FPGA_L);
		WRITE_EPLD(DOWNLOAD_FPGA_CLK,FPGA_H);
      	}
	for(index=0;index<WAIT_TIME;index++)
	{
		READ_EPLD(DOWNLOAD_FPGA_DONE, readstatus);
		if(readstatus){
		printf("done ok\n");

			break;
		}
		if(index==(WAIT_TIME-1)){
			close(fd);
			printf("done error\n");
			return;
		}
	}
	WRITE_EPLD(DOWNLOAD_FPGA_CS, FPGA_H);
	close(fd);
	printf("close ok\n");
}*/
