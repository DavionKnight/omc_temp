/*******************************************************************************
********************************************************************************
* 文件名称:  omap_emif.h
* 功能描述:  cpu并口寄存器控制
* 使用说明: 	
* 文件作者:	H4
* 编写日期: （2012/06/18）
* 修改历史:
* 修改日期    修改人       修改内容
*-------------------------------------------------------------------------------

*******************************************************************************/
#ifndef _OMAP_EMIF_H
#define _OMAP_EMIF_H

#define EMIF_PHY_ADDR_BASE 0x68000000
typedef struct {
	volatile unsigned int midr;
	volatile unsigned int awcc;
	volatile unsigned int sdcr;
	volatile unsigned int sdrcr;
	volatile unsigned int ce2cfg;
	volatile unsigned int ce3cfg;
	volatile unsigned int ce4cfg;
	volatile unsigned int ce5cfg;
}emif_reg;
extern emif_reg *emif_t;
#define WRITE_EMIF(reg,value) ((emif_t->reg)=value)
#define READ_EMIF(reg,value) (value=(emif_t->reg))

#endif
