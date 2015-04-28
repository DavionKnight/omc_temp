/*************************************************************************
	> File Name: irdeal.h
	> Author:h4 
	> Mail:h4@sina.cn	 
	> Created Time: 2012年12月14日 星期五 14时58分22秒
 ************************************************************************/
#ifndef _IRDEAL_H
#define _IRDEAL_H
#define FPGA_SYNC_ADDR 0X0100
#define FPGA_OPT_SEL_ADDR 0X010d
#define FPGA_IQ_EN_ADDR 0X0103
#define FPGA_PORT_ID_BASE_ADDR 0X0110
#define FPGA_PORT_ID_H_0_ADDR 0X0110
#define FPGA_PORT_ID_L_0_ADDR 0X0111
#define FPGA_PORT_ID_H_1_ADDR 0X0112
#define FPGA_PORT_ID_L_1_ADDR 0X0113
#define FPGA_PORT_ID_H_2_ADDR 0X0114
#define FPGA_PORT_ID_L_2_ADDR 0X0115
#define FPGA_PORT_ID_H_3_ADDR 0X0116
#define FPGA_PORT_ID_L_3_ADDR 0X0117
#define FPGA_PORT_ID_H_4_ADDR 0X0118
#define FPGA_PORT_ID_L_4_ADDR 0X0119
#define FPGA_PORT_ID_H_5_ADDR 0X011a
#define FPGA_PORT_ID_L_5_ADDR 0X011b
#define FPGA_PORT_ID_H_6_ADDR 0X011c
#define FPGA_PORT_ID_L_6_ADDR 0X011d
#define FPGA_PORT_ID_H_7_ADDR 0X011e
#define FPGA_PORT_ID_L_7_ADDR 0X011f
#define FPGA_PORT_ID_2_0_ADDR 0X0150
#define FPGA_PORT_ID_2_1_ADDR 0X0151
#define FPGA_PORT_ID_2_2_ADDR 0X0152
#define FPGA_PORT_ID_2_3_ADDR 0X0153
#define FPGA_PORT_ID_2_4_ADDR 0X0154
#define FPGA_PORT_ID_2_5_ADDR 0X0155
#define FPGA_PORT_ID_2_6_ADDR 0X0156
#define FPGA_PORT_ID_2_7_ADDR 0X0157


#define MAX_REBOOT_TIME 100*60*5  //5 min later reboot
#define DELAY_MEASURE_TIME 100*60*60*1 //10ms*100 *60s*60m*1h
#define CPLD_REBOOT_ADDR 0X24

#define CPLD_REBOOT_DATA 0X55

void hardware_init(void);
void reboot_timing(void);
unsigned long long get_dev_port_num(void);

#endif
