#ifndef _IR_H
#define _IR_H
#include "../common/type_def.h"
#define OUT_TIME 30 
#define MAX_THREAD	8 
#define RECEIVE_THREAD 0
#define TIMING_THREAD 1
#define TRANSMIT_THREAD 2
#define RRU_THREAD 3
#define BBU_THREAD 4
#define DELAY_THREAD 5
#define DELAY_BACK_THREAD 6
#define CONFIG_THREAD 7
#define CONFIG_BACK_THREAD 8
#define IE_IN_USE 0XAA
#define IE_UN_USE 0X00
#define IE_SYMBOL_NULL 0
#define READ_DELAY_TIME_START 0X11
#define READ_DELAY_TIME_DONE 0X12
#define CONFIG_DELAY_TIME_START 0X21
#define CONFIG_DELAY_TIME_DONE 0X22
#define THREAD_NULL 0X11
#define THREAD_RUNNING 0X22
#define THREAD_EXIT 0X33
#define THREAD_START 0XAA
#define THREAD_DONE 0X55
#define DELAY_TIME_OFFSIZE 0X0
#define DELAY_MEASURE_ENABLE 0X0A
#define DELAY_MEASURE_DISABLE 0X00
#pragma pack(1)

typedef struct thread_struct_t{
	int status;
	int control;
	pthread_t p_id;
}thread_struct;
typedef struct use_ie_t{
	U8 is_used;
	U16 ie_symbol;
}use_ie;
typedef struct ie_head_t{
	U16 symbol;
	U16 length;
}ie_head;
typedef struct u8_struct_t{
	U8 data;
}u8_struct;
typedef struct u16_struct_t{
	U16 data;
}u16_struct;
typedef struct u32_struct_t{
	U32 data;
}u32_struct;

void state_req(void);
void state_req_deal(U8 *inbuf);
void parameter_req_deal(U8 *inbuf);
void transimt_deal(void);
void transimt_rsp_deal(U8 *inbuf);
void timing_rru_deal(U8 *inbuf);
void timing_bbu_deal(U8 *inbuf);
void create_ch_req(void);
void create_ch_req_deal(U8 *inbuf);
void create_ch_cfg_deal(U8 *inbuf);
void create_ch_cfg_rsp_deal(U8 *inbuf);
void measure_dalay_time(void);
void delay_measure_req_deal(U8 *inbuf);
void delay_mearsure_rsp_deal(U8 *inbuf);
void delay_time_config_deal(U8 *inbuf);
void delay_time_config_rsp_deal(U8 *inbuf);
void delay_measure_start(void);
void delay_measure_thread(void);
void time_control_delay(void);

void update_result(unsigned long status);
void timing_rru(void);
void *delay_measure_action(void *vp);
void update_result_deal(U8 *inbuf);
void parameter_config(unsigned int index);
void parameter_config_deal(U8 *inbuf);
void parameter_config_deal_rsp(U8 *inbuf);
void set_system_time(sys_time *st);
void set_system_time(sys_time *st);
int system_time_config(void);
int fact_delay_calculate(void);

#endif
