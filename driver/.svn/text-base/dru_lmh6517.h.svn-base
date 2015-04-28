/*******************************************************************************
********************************************************************************
* 文件名称:  dru_lmh6517.h
* 功能描述:  关于lmh6517的函数声明和宏定义
* 使用说明: 	所有于lmh6517直接相关的宏定义都在此文件定义。
* 文件作者:	H4
* 编写日期: （2012/06/18）
* 修改历史:
* 修改日期    修改人       修改内容
*-------------------------------------------------------------------------------

*******************************************************************************/
#ifndef _DRU_LMH6517_H
#define _DRU_LMH6517_H


#define LMH6517_REG_LENGTH 16

int dru_LMH6517_gain_set(unsigned char channel,unsigned char gain,unsigned char chipsel);
unsigned char dru_LMH6517_channel_gain_read(unsigned char channel,unsigned char chipsel);

int dru_lte1_gain_set(unsigned char gain);
int dru_lte2_gain_set(unsigned char gain);
int dru_gsm_gain_set(unsigned char gain);
int dru_td_gain_set(unsigned char gain);

unsigned char dru_lte1_gain_read();
unsigned char dru_lte2_gain_read();
unsigned char dru_gsm_gain_read();
unsigned char dru_td_gain_read();

#endif
