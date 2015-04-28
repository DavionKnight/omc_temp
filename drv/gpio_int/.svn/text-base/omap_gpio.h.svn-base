/*******************************************************************************
********************************************************************************
* 文件名称: omap_gpio.h
* 功能描述:  gpio的寄存器定义，和对gpio操作的宏定义
* 使用说明: 
* 文件作者:	H4
* 编写日期: （2012/06/15）
* 修改历史:
* 修改日期    修改人       修改内容
*-------------------------------------------------------------------------------

*******************************************************************************/
#ifndef _OMAP_GPIO_H
#define _OMAP_GPIO_H

typedef struct {
	unsigned int pinmux0;
	unsigned int pinmux1;
	unsigned int pinmux2;
	unsigned int pinmux3;
	unsigned int pinmux4;
	unsigned int pinmux5;
	unsigned int pinmux6;
	unsigned int pinmux7;
	unsigned int pinmux8;
	unsigned int pinmux9;
	unsigned int pinmux10;
	unsigned int pinmux11;
	unsigned int pinmux12;
	unsigned int pinmux13;
	unsigned int pinmux14;
	unsigned int pinmux15;
	unsigned int pinmux16;
	unsigned int pinmux17;
	unsigned int pinmux18;
	unsigned int pinmux19;
}pinmux_t;

typedef struct {
	  unsigned int revid0;
	  unsigned int revid1;
	  unsigned int binten;
	  unsigned int rev0;
	  unsigned int         dir01;
	  unsigned int    out_data01;
	  unsigned int    set_data01;
	  unsigned int    clr_data01;
	  unsigned int     in_data01;
	  unsigned int set_ris_tri01;
	  unsigned int clr_ris_tri01;
	  unsigned int set_fal_tri01;
	  unsigned int clr_fal_tri01;
	  unsigned int     intstat01;
	  unsigned int         dir23;
	  unsigned int    out_data23;
	  unsigned int    set_data23;
	  unsigned int    clr_data23;
	  unsigned int     in_data23;
	  unsigned int set_ris_tri23;
	  unsigned int clr_ris_tri23;
	  unsigned int set_fal_tri23;
	  unsigned int clr_fal_tri23;
	  unsigned int     intstat23;
	  unsigned int         dir45;
	  unsigned int    out_data45;
	  unsigned int    set_data45;
	  unsigned int    clr_data45;
	  unsigned int     in_data45;
	  unsigned int set_ris_tri45;
	  unsigned int clr_ris_tri45;
	  unsigned int set_fal_tri45;
	  unsigned int clr_fal_tri45;
	  unsigned int     intstat45;
	  unsigned int         dir67;
	  unsigned int    out_data67;
	  unsigned int    set_data67;
	  unsigned int    clr_data67;
	  unsigned int     in_data67;
	  unsigned int set_ris_tri67;
	  unsigned int clr_ris_tri67;
	  unsigned int set_fal_tri67;
	  unsigned int clr_fal_tri67;
	  unsigned int     intstat67;
	  unsigned int         dir8;
	  unsigned int    out_data8;
	  unsigned int    set_data8;
	  unsigned int    clr_data8;
	  unsigned int     in_data8;
	  unsigned int set_ris_tri8;
	  unsigned int clr_ris_tri8;
	  unsigned int set_fal_tri8;
	  unsigned int clr_fal_tri8;
	  unsigned int     intstat8;
	
}gpio_t;

#define REVID		0
#define CR			0x0004
#define GER			0x0010
#define GNLR		0x001C
#define SISR		0x0020
#define SICR		0x0024
#define EISR		0x0028
#define EICR		0x002C
#define HIEISR		0x0034
#define HIEICR		0x0038
#define VBR			0x0050
#define VSR			0x0054
#define VNR			0x0058
#define GPIR		0x0080
#define GPVR		0x0084
#define SRSR_BASE	0x0200
#define SECR_BASE	0x0280
#define ESR_BASE	0x0300
#define ECR_BASE	0x0380
#define CMR_BASE	0x0400
#define HIPIR1		0x0900
#define HIPIR2		0x0904
#define HINLR1		0x1100
#define HINLR2		0x1104
#define HIER		0x1500
#define HIPVR1		0x1600
#define HIPVR2		0x1604


#define OFFSET(type, member) (unsigned int)(&(((type *)(0))->member))
#define ADDR(addr, type, reg) (addr + OFFSET(type, reg))

#endif 
