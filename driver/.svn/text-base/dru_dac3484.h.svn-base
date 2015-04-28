/*******************************************************************************
********************************************************************************
* 文件名称:  dru_dac3484.h
* 功能描述:  dac3484的头文件
* 使用说明: 	所有与dac3484相关的宏定义都在此定义
* 文件作者:	H4
* 编写日期: （2012/06/18）
* 修改历史:
* 修改日期    修改人       修改内容
*-------------------------------------------------------------------------------

*******************************************************************************/
#ifndef _DRU_DAC3484_H
#define _DRU_DAC3484_H
#define DAC3484_REG_LENGTH 24
#define SELECT_DAC3481_1  5
#define SELECT_DAC3481_2 6
int dru_dac3484_init(void);

void dac3484_lte1_qmc_offset_a(unsigned short offset);
void dac3484_lte1_qmc_offset_b(unsigned short offset);
void dac3484_lte2_qmc_offset_c(unsigned short offset);
void dac3484_lte2_qmc_offset_d(unsigned short offset);

void dac3484_gsm_qmc_offset_a(unsigned short offset);
void dac3484_gsm_qmc_offset_b(unsigned short offset);
void dac3484_td_qmc_offset_c(unsigned short offset);
void dac3484_td_qmc_offset_d(unsigned short offset);

void dac3484_lte1_qmc_gain_a(unsigned short offset);
void dac3484_lte1_qmc_gain_b(unsigned short offset);
void dac3484_lte2_qmc_gain_c(unsigned short offset);
void dac3484_lte2_qmc_gain_d(unsigned short offset);

void dac3484_gsm_qmc_gain_a(unsigned short offset);
void dac3484_gsm_qmc_gain_b(unsigned short offset);
void dac3484_td_qmc_gain_c(unsigned short offset);
void dac3484_td_qmc_gain_d(unsigned short offset);

void dac3484_lte1_qmc_phase_ab(signed short offset);
void dac3484_lte2_qmc_phase_cd(signed short offset);
void dac3484_gsm_qmc_phase_ab(signed short offset);
void dac3484_td_qmc_phase_cd(signed short offset);

void dac1_reset_n(unsigned char level);
void dac1_txen(unsigned char level);
void dac2_reset_n(unsigned char level);
void dac2_txen(unsigned char level);

#endif
