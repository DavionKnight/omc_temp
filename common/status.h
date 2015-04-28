#ifndef _STATUS_H
#define _STATUS_H
#include "type_def.h"
#define DEVICE_TYPE_MAIN 1
#define DEVICE_TYPE_EXPEND 2
#define DEVICE_TYPE_RAU 3
#define DRU_DEBUG 1

#define BUFFER_NULL 0
#define BUFFER_READ 1
#define BUFFER_WRITE 2
#define DEBUG_PRINTF printf
//#define DEBUG_PRINTF \/\/
//#endif
//============================================
//定义本振型号和频点。
#define DEV_LM2531_CMGSM_LTED 0X11
#define DEV_LM2531_CMGSM_LTEE 0X12
#define DEV_LM2531_CMGSM_LTEF 0X13

#define DEV_LM2531_CMDCS_LTED 0X14
#define DEV_LM2531_CMDCS_LTEE 0X15
#define DEV_LM2531_CMDCS_LTEF 0X16

#define DEV_LM2581_CMGSM_LTED 0X21
#define DEV_LM2581_CMGSM_LTEE 0X22
#define DEV_LM2581_CMGSM_LTEF 0X23

#define DEV_LM2581_CMDCS_LTED 0X24
#define DEV_LM2581_CMDCS_LTEE 0X25
#define DEV_LM2581_CMDCS_LTEF 0X26
//定义衰减器型号。
#define DEV_SS1112_TDD 0X10
#define DEV_SS1112_FDD 0X20



typedef struct dev_info{
unsigned char dev_vco;//本振型号和频点
unsigned char dev_adc;//ADC 型号
unsigned char dev_dac;//DAC 型号
unsigned char dev_att;//衰减器型号
}dev_info_t;
extern dev_info_t *dru_dev_p;
void read_dev_info(void);
U8 read_device_type(void);
int read_net_type(void);
int get_net_type(void);
U8 get_device_type(void);
void read_pkg_version(void);
int get_net_group(void);



#endif

