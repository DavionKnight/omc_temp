/*******************************************************************************
********************************************************************************
* 文件名称:  dru_iic.h
* 功能描述:  模拟iic头文件
*						
* 使用说明: 
* 文件作者:	H4
* 编写日期: （2012/07/03）
* 修改历史:
* 修改日期    修改人       修改内容
*-------------------------------------------------------------------------------

*******************************************************************************/
#ifndef _DRU_IIC_H
#define _DRU_IIC_H

#define IIC_DELAY_TIME 50

#define IIC_8873_WRITE_ADDR 0xBE
#define IIC_8873_READ_ADDR 0xBF

#define DRU_ICC_8873_SCL_DIR_REG dir01
#define DRU_ICC_8873_SDA_DIR_REG dir01
#define DRU_ICC_8873_SCL_OUT_SET_REG set_data01
#define DRU_ICC_8873_SDA_OUT_SET_REG set_data01
#define DRU_ICC_8873_SCL_OUT_CLR_REG clr_data01
#define DRU_ICC_8873_SDA_OUT_CLR_REG clr_data01
#define DRU_ICC_8873_SDA_IN_DATA_REG in_data01

#define DRU_ICC_8873_SCL_PIN 0x00800000
#define DRU_ICC_8873_SDA_PIN 0x00400000
/*
#define DRU_ICC_8873_SCL_DIR_REG dir67
#define DRU_ICC_8873_SDA_DIR_REG dir67
#define DRU_ICC_8873_SCL_OUT_SET_REG set_data67
#define DRU_ICC_8873_SDA_OUT_SET_REG set_data67
#define DRU_ICC_8873_SCL_OUT_CLR_REG clr_data67
#define DRU_ICC_8873_SDA_OUT_CLR_REG clr_data67
#define DRU_ICC_8873_SDA_IN_DATA_REG in_data67

#define DRU_ICC_8873_SCL_PIN 0x00400000
#define DRU_ICC_8873_SDA_PIN 0x00200000
*/
#define DRU_IIC_8873_SAVE_DIRREG()  iic_gpio_save=gpio_t->dir01
#define DRU_ICC_8873_SCL_DIR_OUT()  dru_iic_dir(DRU_ICC_8873_SCL_PIN,1)
#define DRU_ICC_8873_SDA_DIR_OUT()  dru_iic_dir(DRU_ICC_8873_SDA_PIN,1)
//#define DRU_ICC_8873_SCL_DIR_IN()  GPIO_DIR_IN(DRU_ICC_8873_SCL_DIR_REG,DRU_ICC_8873_SCL_PIN)
#define DRU_ICC_8873_SDA_DIR_IN()  dru_iic_dir(DRU_ICC_8873_SDA_PIN,0)

#define DRU_ICC_8873_SCL_OUT_HIGH()  GPIO_OUT(DRU_ICC_8873_SCL_OUT_SET_REG,DRU_ICC_8873_SCL_PIN)
#define DRU_ICC_8873_SDA_OUT_HIGH()  GPIO_OUT(DRU_ICC_8873_SDA_OUT_SET_REG,DRU_ICC_8873_SDA_PIN)
#define DRU_ICC_8873_SCL_OUT_LOW()  GPIO_OUT(DRU_ICC_8873_SCL_OUT_CLR_REG,DRU_ICC_8873_SCL_PIN)
#define DRU_ICC_8873_SDA_OUT_LOW()  GPIO_OUT(DRU_ICC_8873_SDA_OUT_CLR_REG,DRU_ICC_8873_SDA_PIN)


#define DRU_ICC_8873_SDA_IN(level)  level=dru_iic_gpio_in(DRU_ICC_8873_SDA_PIN)

int dru_8873_read(unsigned char addr,unsigned char *data);
int dru_8873_write(unsigned char addr,unsigned char data);
void dru_iic_8873_write_byte(unsigned char data);
void iic_delay(unsigned char delayTime);
#endif
