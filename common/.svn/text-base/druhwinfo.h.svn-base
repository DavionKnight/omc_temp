/********************  COPYRIGHT(C) ***************************************
**--------------文件信息--------------------------------------------------------
**文   件   名: druhwinfo.h
**创   建   人: 于宏图
**创 建  日 期: 
**程序开发环境：
**描        述: DRU数组平台硬件配置信息头文件
**--------------历史版本信息----------------------------------------------------
** 创建人: 于宏图
** 版  本: v1.0
** 日　期: 
** 描　述: 原始版本
**--------------当前版本修订----------------------------------------------------
** 修改人:
** 版  本:
** 日　期:
** 描　述:
**------------------------------------------------------------------------------
**
*******************************************************************************/
#ifndef _DRUHWINFO_H
#define _DRUHWINFO_H

#include "usertype.h"
//CPLD地址定义
#define CPLDVERSION_ADDR      0x0000  //bit15-0:CPLD版本号
#define LOSTATUS_ADDR         0x0012  //本振状态
#define PLLSTATUS_ADDR        0x0017  //PLL时钟状态
#define OPTICDECT_ADDR        0x0020  //光模块在不在位检测
//FPGA地址定义
#define LTE1DATAPOWER_ADDR    0x004A  //LTE1 数字功率 uint2
#define LTE2DATAPOWER_ADDR    0x004B  //LTE2 数字功率 uint2
#define G3DATAPOWER_ADDR      0x004C  //3G 数字功率 uint2
#define GSMDATAPOWER_ADDR     0x004D  //GSM 数字功率 uint2
#define LTE1VGA_ADDR          0x0046  //LTE1 VGA uint1
#define LTE2VGA_ADDR          0x0047  //LTE2 VGA uint1
#define G3VGA_ADDR            0x0048  //3G VGA uint1
#define GSMVGA_ADDR           0x0049  //GSM VGA uint1
#define LTE1DATAGAIN_ADDR     0x0040  //LTE1 数字增益 uint2
#define LTE2DATAGAIN_ADDR     0x0041  //LTE2 数字增益 uint2
#define G3DATAGAIN_ADDR       0x0042  //3G 数字增益 uint2
#define GSMDATAGAIN_ADDR      0x0043  //GSM 数字增益 uint2
#define GSMCARRIERMASK_ADDR   0x0050  //GSM 载波屏蔽寄存器 uint2
#define TESTMODELSWITCH_ADDR  0x003F  //测试模式和正常模式切换 uint1
#define INTERTESTEN_ADDR      0x003E  //内部测试源使能 uint1
#define INTEFREQRSRC_ADDR     0x003C  //内部源频率设置 uint2
#define AGCREFERENCE_ADDR     0x0045  //AGC 参考值设定 uint2
#define AGCSTEP_ADDR          0x004E  //AGC 步进设置 uint1
#define AGCEN_ADDR            0x004F  //AGC 使能寄存器 1 bit 上升沿触发VGA增益设置(写010)

#define FPGATOPVERSION_ADDR   0x0000  //bit15-0:FPGA主版本号,MER
#define FPGAVERMARK_ADDR      0x0001  //bit15-0:版本标示说明寄存器,MER
#define INTMASKREG_ADDR       0x0002  //中断屏蔽寄存器用于屏蔽相应中断信号产生,MER
#define INTENREG_ADDR         0x0005  //中断使能寄存器,MER
#define INTSOURCEREG_ADDR     0x0008  //中断源寄存器,MER
#define INTCLEARREG_ADDR      0x000B  //中断清除寄存器,MER
#define TX2RXTIMEOPT_ADDR     0x000E  //光口时延测量值,计算各光口光纤时延测量值,MER
#define UL2KMADJUST_ADDR      0x0016  //上行2KM缓存调整寄存器,MER
#define DL2KMADJUST_ADDR      0x0017  //下行2KM缓存调整寄存器,MER
#define SPDCFGREG_ADDR        0x0018  //固定值5G模式,MER
#define RSTIDREGH_ADDR        0x0019  //复位控制字高字节,MER
#define RSTIDREGL_ADDR        0x001A  //复位控制字低字节,MER
#define OPTIQENREG_ADDR       0x001B  //设备IQ叠加使能,MER
#define OPTSYNCREG_ADDR       0x0100  //同步标识,MER
#define DEVIDREGH_ADDR        0x0110  //设备级联编号高字节,MER
#define DEVIDREGL_ADDR        0x0111  //设备级联编号低字节,MER
#define DEVIDLEVEL_ADDR       0x010d  //设备级联级数,MER
#define MAINIDREG_ADDR        0x0020  //网络拓扑的主单元编号,MER
#define ULOPTSELREG_ADDR      0x0021  //上联光口选择,MER
#define TDS_TIMESLOT_ADDR     0x005d  //TD-SCDMA时隙配置寄存器
#define TDLTE_TIMESLOT_ADDR   0x0058  //TD-LTE时隙配置寄存器
#define TDS_SYN_THR_ADDR      0x0055  //TD-SCDMA同步开启门限寄存器
#define TDLTE_SYN_THR_ADDR    0x080  //TD-LTE同步开启门限寄存器
#define	TD_DW_POWER_ADDR	  0x05F  // TD 导频功率
#define TD_TS0_POWER_ADDR	  0x060  // TD TS0 时隙功率
#define TD_TS1_POWER_ADDR	  0x061  // TD TS0 时隙功率
#define TD_TS2_POWER_ADDR	  0x062  // TD TS0 时隙功率
#define TD_TS3_POWER_ADDR	  0x063  // TD TS0 时隙功率
#define TD_TS4_POWER_ADDR	  0x064  // TD TS0 时隙功率
#define TD_TS5_POWER_ADDR	  0x065  // TD TS0 时隙功率
#define TD_TS6_POWER_ADDR	  0x066  // TD TS0 时隙功率
#define	LTE_DW_POWER_ADDR	  0x067  // LTE 导频功率
#define	LTE_TS0_POWER_ADDR	  0x068  // LTE TS0 时隙功率
#define	LTE_TS2_POWER_ADDR	  0x069  // LTE TS0 时隙功率
#define	LTE_TS3_POWER_ADDR	  0x06A  // LTE TS0 时隙功率
#define	LTE_TS4_POWER_ADDR	  0x06B  // LTE TS0 时隙功率
#define TD_ASYNC_ADDR         0x06E  // TD-SCDMA失步告警
#define LTE_ASYNC_ADDR        0x06F  // LTE失步告警
#define BINDWIDTH_MODEL_ADDR  0x073  // 2G 宽带选频模式选择
#define LTE_AUTO_CONFIG_ADDR  0x074  // LTE 自动配置
#define TD_FIRTRP_FINE_REG	  0x082	 // TD第一转换点微调
#define TD_SECTRP_FINE_REG	  0x083	 // TD第二转换点微调
#define LTE_FIRTRP_FINE_REG	  0x056	 // LTE第一转换点微调
#define LTE_SECTRP_FINE_REG	  0x057  // LTE第二转换点微调
#define LTE_CHNCHS_REG		  0x076	 // LTE的接入通道选择

#define MACSIZE       6 //MAC地址长度
#define INTREG_SIZE   3 //中断源寄存器数量
#define OPT_SIZE      8 //光口数量

#define VGAMAXSETVAL  63//VGA最大设置值
#define DATAPOWERBITS 0x1FF//数字功率

#pragma pack(1)
typedef struct HWInfo
{
  char DevType;         //设备类型
  int  DeviceNo;        //客户端设备编号,参照移动协议
	char Mac[MACSIZE];    //MAC地址
	INT32U IPAddr;        //客户端地址
  //FPGA版本及标识说明
  INT16U FpgaTopVersion;  //bit15-0:FPGA主版本号,MER
  INT16U FpgaVerMark;     //bit15-0:版本标示说明寄存器,MER
  //中断控制
  INT16U INTMaskReg[INTREG_SIZE];   //中断屏蔽寄存器用于屏蔽相应中断信号产生,MER
  INT16U INTEnReg[INTREG_SIZE];     //中断使能寄存器,MER
  INT16U INTSourceReg[INTREG_SIZE]; //中断源寄存器,MER
  INT16U INTClearReg[INTREG_SIZE];  //中断清除寄存器,MER
  //时延测量
  INT16U Tx2RxTimeOpt[8]; //光口时延测量值,计算各光口光纤时延测量值,MER
  INT16U UL2kmAdjust;     //上行2KM缓存调整寄存器,MER
  INT16U DL2kmAdjust;     //下行2KM缓存调整寄存器,MER
  //物理层控制字
  INT8U SpdCfgReg;        //固定值5G模式,MER
  INT32U RstIdReg;        //复位控制字,MER
  INT8U OptIQEnReg;       //设备IQ叠加使能,MER
  INT8U OptSyncReg;       //同步标识,MER
  INT32U DevIdReg;        //设备级联编号,MER
  INT16U DevIdLevel;      //设备级联级数,MER
  INT16U MainIdReg;       //网络拓扑的主单元编号,MER
  //光口gtx
  INT8U ULOptSelReg;      //上联光口选择,MER
}HWInfo_t, *HWInfo_tp;

#endif//_DRUHWINFO_H

/*********************************End Of File*************************************/
