#include "../../driver/omap_epld.h"
#include "../../driver/dru_spi.h"
#include "common.h"
#include "apue.h"
#include "commonfun.h"
#include "druhwinfo.h"
#include "../../sqlite/drudatabase.h"
#include "../../task/omccom/omccom.h"
#include "../../protocol/approtocol.h"


extern int g_OMCSetParaFlag;
extern DevicePara_t g_DevicePara;
extern int g_DevType;
extern time_t g_AlarmDectTime;//告警查询计时

extern int GetVGAVal(unsigned int id, int *pval);
extern int GetDataGainVal(INT16U id, int *pval);
extern int dru_lmx2531_wcdma_config(unsigned int freq);
extern int dru_lmx2531_fdd_lte1_config(unsigned int freq);
extern int dru_lmx2531_fdd_lte2_config(unsigned int freq);

extern unsigned short dru_fpga_write(unsigned int addr,unsigned short data);
extern unsigned short dru_fpga_read(unsigned int addr,unsigned short *data);
/*
** 函数功能：读FPGA接口函数
** 输入参数: addr=FPGA地址
** 输出参数：pdata=从FPGA读到的数据
** 返回值：0=成功 其他=失败
** 备注：
*/
int drv_read_fpga(unsigned int addr, unsigned short * pdata)
{
	unsigned short tmp;
	int ret;

	lock_sem(SEM_DRV);
	tmp = dru_fpga_read(addr, pdata);
	if(tmp == *pdata){
		ret = 0;
	}else{
		ret = -1;
	}
	unlock_sem(SEM_DRV);

	return ret;
}
/*
** 函数功能：写FPGA接口函数
** 输入参数: addr=FPGA地址 data=数据
** 输出参数：无
** 返回值：0=成功 其他=失败
** 备注：
*/
int drv_write_fpga(unsigned int addr, unsigned short data)
{
	unsigned short tmp;
	int ret;

	lock_sem(SEM_DRV);
	tmp = dru_fpga_write(addr, data);
	if(tmp == data){
		ret = 0;
	}else{
		ret = -1;
	}
	unlock_sem(SEM_DRV);

	return ret;
}
/*
** 函数功能：读EPLD接口函数
** 输入参数: addr=EPLD地址
** 输出参数：pdata=从EPLD读到的数据
** 返回值：0=成功 其他=失败
** 备注：
*/
int drv_read_epld(unsigned int addr, unsigned short * pdata)
{
	unsigned short tmp;
	int ret;

	lock_sem(SEM_DRV);
	tmp = dru_epld_read(addr, pdata);
	if(tmp == *pdata){
		ret = 0;
	}else{
		ret = -1;
	}
	unlock_sem(SEM_DRV);

	return ret;
}
/*
** 函数功能：写EPLD接口函数
** 输入参数: addr=EPLD地址 data=数据
** 输出参数：无
** 返回值：0=成功 其他=失败
** 备注：
*/
int drv_write_epld(unsigned int addr, unsigned short data)
{
	unsigned short tmp;
	int ret;

	lock_sem(SEM_DRV);
	tmp = dru_epld_write(addr, data);
	if(tmp == data){
		ret = 0;
	}else{
		ret = -1;
	}
	unlock_sem(SEM_DRV);

	return ret;
}
/*
** 函数功能：写DDC配置接口函数
** 输入参数: addr=信道号 data=数据
** 输出参数：无
** 返回值：0=成功 其他=失败
** 备注：
*/
int drv_write_ddc(unsigned char addr,unsigned short data)
{
	lock_sem(SEM_DRV);
	if (addr < 8)
	{
		emif_fpga_ddc_carrier_f8_write(addr,data);
	}
	else
	{
		emif_fpga_ddc_carrier_m8_write(addr-8,data);
	}
	unlock_sem(SEM_DRV);
	return 0;
}
/*
** 函数功能：写DUC配置接口函数
** 输入参数: addr=信道号 data=数据
** 输出参数：无
** 返回值：0=成功 其他=失败
** 备注：
*/
int drv_write_duc(unsigned char addr,unsigned short data)
{
	lock_sem(SEM_DRV);
	if (addr < 8)
	{
		emif_fpga_duc_carrier_f8_write(addr,data);
	}
	else
	{
		emif_fpga_duc_carrier_m8_write(addr-8,data);
	}
	unlock_sem(SEM_DRV);
	return 0;
}
/*******************************************************************************
*函数名称 : void OMCDevParaDeal(void)
*功    能 : 设备告警处理函数,超过3min后有告警上报
*输入参数 : none
*输出参数 : none
*******************************************************************************/ 
void OMCDevParaDeal(void)
{
int i, val, val2, portsum;
unsigned short para2, temp;
DevInfo_t devinfo;

  //有参数设置
  if (g_OMCSetParaFlag==1)
  {
	  printf("set china tele para\r\n");
	  g_OMCSetParaFlag = 0;
	  update();
  	memset(&devinfo, 0, sizeof(DevInfo_t));
  	LoadDevicePara(&devinfo, &g_DevicePara);

    //GSM VGA设置
    if (GetVGAVal(GSMVGA_ID, &val) == 1)
    {
      OMCWriteFpga(GSMVGA_ADDR, val);
    }
    //3G VGA设置
    if (GetVGAVal(G3VGA_ID, &val) == 1)
    {
      OMCWriteFpga(G3VGA_ADDR, val);
    }
    //LTE1 VGA设置
    if (GetVGAVal(LTE1VGA_ID, &val) == 1)
    {
      OMCWriteFpga(LTE1VGA_ADDR, val);
    }
    //LTE2 VGA设置
    if (GetVGAVal(LTE2VGA_ID, &val) == 1)
    {
      OMCWriteFpga(LTE2VGA_ADDR, val);
    }
    
    if (GetDataGainVal(GSMDATAGAIN_ID, &val) == 1)//GSM 数字增益设置
    {
      val = (INT16U)(pow(10.0, (double)val/200) * 8192 + 0.5);
      OMCWriteFpga(GSMDATAGAIN_ADDR, val);
    }
    if (GetDataGainVal(G3DATAGAIN_ID, &val) == 1)//3G 数字增益设置
    {
      val = (INT16U)(pow(10.0, (double)val/200) * 8192 + 0.5);
      OMCWriteFpga(G3DATAGAIN_ADDR, val);
    }
    if (GetDataGainVal(LTE1DATAGAIN_ID, &val) == 1)//LTE1 数字增益设置
    {
      val = (INT16U)(pow(10.0, (double)val/200) * 8192 + 0.5);
      OMCWriteFpga(LTE1DATAGAIN_ADDR, val);
    }
    if (GetDataGainVal(LTE2DATAGAIN_ID, &val) == 1)//LTE2 数字增益设置
    {
      val = (INT16U)(powf(10.0, (double)val/200) * 8192 + 0.5);
      OMCWriteFpga(LTE2DATAGAIN_ADDR, val);
    }
    if (DbGetThisIntPara(GSMCARRIERMASK_ID, &val) == 1)//载波屏蔽寄存器设置
      OMCWriteFpga(GSMCARRIERMASK_ADDR, val);
    if (DbGetThisIntPara(AGCREFERENCE_ID, &val) == 1)//AGC 参考值设置
      OMCWriteFpga(AGCREFERENCE_ADDR, val);
    /*if (DbGetThisIntPara(AGCSTEP_ID, &val) == 1)//AGC 步进设置
      OMCWriteFpga(AGCSTEP_ADDR, val);*/
    if (DbGetThisIntPara(TESTMODELSWITCH_ID, &val) == 1)//测试模式和正常模式切换
      drv_write_fpga(TESTMODELSWITCH_ADDR, val);  //此参数为只写寄存器
    if (DbGetThisIntPara(INTERTESTEN_ID, &val) == 1)//内部测试源使能
      OMCWriteFpga(INTERTESTEN_ADDR, val);
    if (DbGetThisIntPara(INTEFREQRSRC_ID, &val) == 1)//内部源频率设置
      OMCWriteFpga(INTEFREQRSRC_ADDR, val);
    //载波频点设置
    if (g_DevType == MAIN_UNIT)
    {
    	//2G 频点设置
	   	for(i=0; i<16; i++)
	   	{
	   		if (DbGetThisIntPara((WORKINGCHANNELNO_ID+i), &val) == 1)//WORKINGCHANNELNO_ID信道号
	   		{
	   			//主单元配置:DDC 配置:y=(x-957)/0.04 当y>=0时,y=y;当y<0时,y=768-y;DUC 配置:z=768-y;
	   			if (val < 96)//主频段 china_mobile gsm
	   			{
	   				val = ((93500+20*val)-94400)/4;
	   			}
				else if ((val<125) && (val > 95))//主频段china unicom  gsm
	   			{
	   				val = ((93500+20*val)-95700)/4;
	   			}
	   			else if((1000<=val) && (val<=1023))//1000～1023 china mobile e-gsm
	   			{
	   				val = ((93500+20*(val-1024))-94400)/4;
	   			}
				else if((val >= 512) && (val <= 636))//china mobile DCS
				{
	   				val = ((180500+20*(val-511))-181760)/4;
				}
				else if((val >= 637) && (val <= 736))//china unicom DCS
				{
	   				val = ((180500+20*(val-511))-184000)/4;
				}
	   			if (val < 0)
	   			{
	   				val = val*(-1);
	   				val = 768 - val;
	   			}
	   			//配置DDC
	   			drv_write_ddc(i, val);
	   			//配置DUC
	   			drv_write_duc(i, (768-val));
	   		}
   		}
		//3G 频点设置
		DbGetThisIntPara(CH2WORKCHANNELNO_ID, &val);
		if((10562<=val) && (val<=10838))  //10562～10838 wcdma
		{
			val = 200*val - 153600;//153.6MHz频点偏移
			dru_lmx2531_wcdma_config(val);
		}
		else if(((10050<=val) && (val<=10125)) ||  // TD-SCDMA A频段
				((9400<=val) && (val<=9600))) // TD-SCDMA F频段
		{
			val = 100875;
			val = 20*val - 92160;//92.16MHz频点偏移
			dru_lmx2531_wcdma_config(val);
		}
		//LTE1 频点设置 
		DbGetThisIntPara(CH3WORKCHANNELNO_ID, &val);
		if((1200<=val) && (val<=1949))//1200~1949 工作频段33 fdd
		{
			val = 1805000 + 100*(val-1200) - 153600;//153.6MHz频点偏移
			dru_lmx2531_fdd_lte1_config(val);
		}
		else if((2750<=val) && (val<=3449))//1200~1949 工作频段37 fdd
		{
			val = 2620000 + 100*(val-2750) - 153600;//153.6MHz频点偏移
			dru_lmx2531_fdd_lte1_config(val);
		}
		else if((val >= 37750) && (val<=38249))// 工作频段D tdd
		{
			val = 2570000 + 100*(val-37750) - 92160;//92.16MHz频点偏移
			dru_lmx2531_fdd_lte1_config(val);
		}
		else if((val >= 38250) && (val<=38649))// 工作频段F tdd
		{
			val = 1880000 + 100*(val-38250) - 92160;//92.16MHz频点偏移
			dru_lmx2531_fdd_lte1_config(val);
		}
		else if((val >= 38650) && (val<=39649))// 工作频段E tdd
		{
			val = 2300000 + 100*(val-38650) - 92160;//92.16MHz频点偏移
			dru_lmx2531_fdd_lte1_config(val);
		}
		//LTE2 频点设置 
		DbGetThisIntPara(CH4WORKCHANNELNO_ID, &val);
		if((1200<=val) && (val<=1949))//1200~1949
		{
			val = 1805000 + 100*(val-1200) - 153600;//153.6MHz频点偏移
			dru_lmx2531_fdd_lte2_config(val);
		}
		else if((2750<=val) && (val<=3449))//1200~1949 工作频段37
		{
			val = 2620000 + 100*(val-2750) - 153600;//153.6MHz频点偏移
			dru_lmx2531_fdd_lte2_config(val);
		}
		else if((val >= 37750) && (val<=38249))// 工作频段D tdd
		{
			val = 2570000 + 100*(val-37750) - 92160;//92.16MHz频点偏移
			dru_lmx2531_fdd_lte2_config(val);
		}
		else if((val >= 38250) && (val<=38649))// 工作频段F tdd
		{
			val = 1880000 + 100*(val-38250) - 92160;//92.16MHz频点偏移
			dru_lmx2531_fdd_lte2_config(val);
		}
		else if((val >= 38650) && (val<=39649))// 工作频段E tdd
		{
			val = 2300000 + 100*(val-38650) - 92160;//92.16MHz频点偏移
			dru_lmx2531_fdd_lte2_config(val);
		}
		if (DbGetThisIntPara(LTE_ROMOTE_SW_ID, &val) == 1){ //LTE远程功率开关
			drv_read_fpga(0x5a, &temp);
			if((val&3) != (temp&3)){
				drv_write_fpga(0x5a, val&3);
			}
		}
		if (DbGetThisIntPara(TD_ROMOTE_SW_ID, &val) == 1){ //TD远程功率开关
			drv_read_fpga(0x5b, &temp);
			if((val&3) != (temp&3)){
				drv_write_fpga(0x5b, val&3);
			}
		}

	}
    else if (g_DevType == RAU_UNIT)
   	{
		// TD-SCDMA时隙配置
		if (DbGetThisIntPara(TDS_TIMESLOT_ID, &val) == 1)
			OMCWriteFpga(TDS_TIMESLOT_ADDR, val);
		// TD-LTE时隙配置 时隙在前，特殊子帧在后
		if (DbGetThisIntPara(TDLTE_TIMESLOT_ID, &val) == 1){
			if (DbGetThisIntPara(TDLTE_SUB_ID, &val2) == 1){
				OMCWriteFpga(TDLTE_TIMESLOT_ADDR, (((val&0xf)<<4) | (val2&0xf)));
			}
		}
		// TD-SCDMA同步开启门限
		if (DbGetThisIntPara(TDS_SYN_THR_ID, &val) == 1)
			OMCWriteFpga(TDS_SYN_THR_ADDR, val);
		// TD-LTE同步开启门限
		if (DbGetThisIntPara(TDLTE_SYN_THR_ID, &val) == 1)
			OMCWriteFpga(TDLTE_SYN_THR_ADDR, val);
   		//2G 频点设置
	   	for(i=0; i<16; i++)
	   	{
	   		if (DbGetThisIntPara((WORKINGCHANNELNO_ID+i), &val) == 1)//WORKINGCHANNELNO_ID信道号
	   		{
	   			//主单元配置:DDC 配置:y=(x-957)/0.04 当y>=0时,y=y;当y<0时,y=768-y;DUC 配置:z=768-y;
				if (val<96)// china mobile
				{
					val = (89900-(89000+20*val))/4;
				}
				else if ((val<125) && (val > 95))// china unicom
	   			{
	   				val = (91200-(89000+20*val))/4;
	   			}
	   			else if((1000<=val) && (val<=1023))//1000～1023 china mobile e-gsm
	   			{
	   				val = (89900-(89000+20*(val-1024)))/4;
	   			}
				else if((val >= 512) && (val <= 636))// china mobile DCS
				{
					val = (172260-(171000+20*(val-511)))/4;
				}
				else if((val >= 637) && (val <= 736))// china unicom DCS
				{
					val = (174500-(171000+20*(val-511)))/4;
				}
	   			if (val < 0)
	   			{
	   				val = val*(-1);
	   				val = 768 - val;
	   			}
	   			//配置DDC
	   			drv_write_ddc(i, val);
	   			//配置DUC
	   			drv_write_duc(i, (768-val));
		   	}
   		}
		//3G 频点设置
		DbGetThisIntPara(CH2WORKCHANNELNO_ID, &val);
		if((10562<=val) && (val<=10838))//10562～10838
		{
			val = 200*val - 190000 - 92160;//92.16MHz频点偏移
			dru_lmx2531_wcdma_config(val);
		}
		else if(((10050<=val) && (val<=10125)) || // TD-SCDMA A频段
				((9400<=val) && (val<=9600))) // TD-SCDMA F频段
		{
			val = 100875;
			val = 20*val - 92160;//92.16MHz频点偏移
			dru_lmx2531_wcdma_config(val);
		}
		//LTE1 频点设置 FDD
		DbGetThisIntPara(CH3WORKCHANNELNO_ID, &val);
		if((1200<=val) && (val<=1949))//1200~1949
		{
			val = 1805000 + 100*(val-1200) - 95000 - 92160;//92.16MHz频点偏移
			dru_lmx2531_fdd_lte1_config(val);
		}
		else if((2750<=val) && (val<=3449))//1200~1949
		{
			val = 2620000 + 100*(val-2750) - 95000 - 92160;//92.16MHz频点偏移
			dru_lmx2531_fdd_lte1_config(val);
		}
		else if((val >= 37750) && (val<=38249))// 工作频段D tdd
		{
			val = 2570000 + 100*(val-37750) - 92160;//92.16MHz频点偏移
			dru_lmx2531_fdd_lte1_config(val);
		}
		else if((val >= 38250) && (val<=38649))// 工作频段F tdd
		{
			val = 1880000 + 100*(val-38250) - 92160;//92.16MHz频点偏移
			dru_lmx2531_fdd_lte1_config(val);
		}
		else if((val >= 38650) && (val<=39649))// 工作频段E tdd
		{
			val = 2300000 + 100*(val-38650) - 92160;//92.16MHz频点偏移
			dru_lmx2531_fdd_lte1_config(val);
		}
		//LTE2 频点设置 FDD
		DbGetThisIntPara(CH4WORKCHANNELNO_ID, &val);
		if((1200<=val) && (val<=1949))//1200~1949
		{
			val = 1805000 + 100*(val-1200) - 95000 - 92160;//92.16MHz频点偏移
			dru_lmx2531_fdd_lte2_config(val);
		}
		else if((2750<=val) && (val<=3449))//1200~1949
		{
			val = 2620000 + 100*(val-2750) - 95000 - 92160;//92.16MHz频点偏移
			dru_lmx2531_fdd_lte2_config(val);
		}
		else if((val >= 37750) && (val<=38249))// 工作频段D tdd
		{
			val = 2570000 + 100*(val-37750) - 92160;//92.16MHz频点偏移
			dru_lmx2531_fdd_lte2_config(val);
		}
		else if((val >= 38250) && (val<=38649))// 工作频段F tdd
		{
			val = 1880000 + 100*(val-38250) - 92160;//92.16MHz频点偏移
			dru_lmx2531_fdd_lte2_config(val);
		}
		else if((val >= 38650) && (val<=39649))// 工作频段E tdd
		{
			val = 2300000 + 100*(val-38650) - 92160;//92.16MHz频点偏移
			dru_lmx2531_fdd_lte2_config(val);
		}
		if (DbGetThisIntPara(LTE_ROMOTE_SW_ID, &val) == 1){ //LTE远程功率开关
			drv_read_fpga(0x5a, &temp);
			if((val&3) != (temp&3)){
				drv_write_fpga(0x5a, val&3);
			}
		}
		if (DbGetThisIntPara(TD_ROMOTE_SW_ID, &val) == 1){ //TD远程功率开关
			drv_read_fpga(0x5b, &temp);
			if((val&3) != (temp&3)){
				drv_write_fpga(0x5b, val&3);
			}
		}
	}
  }
  if ((time(NULL)-g_AlarmDectTime) > ALARMDECTINTERVAL)//告警查询计时,间隔3s
  {
    g_AlarmDectTime = time(NULL);
    //LO状态
    drv_read_epld(LOSTATUS_ADDR, &para2);//1:本振正常,0:本振告警
    para2 = ~para2;
    for (i = 0; i < 4; i++ )
    {
      val = para2 & 0x01;
      SaveAlarmVal((LOUNLOCKAL_ID+i), val);
      para2 = (para2 >> 1);
    }
    //时钟状态
    drv_read_epld(PLLSTATUS_ADDR, &para2);//1:时钟正常,0:时钟告警
    para2 = ~para2;
    for (i = 0; i < 2; i++ )
    {
      val = para2 & 0x01;
      SaveAlarmVal((PLLAL_ID+i), val);
      para2 = (para2 >> 1);
    }
    //光模块在位检测
    drv_read_epld(OPTICDECT_ADDR, &para2);//1:不在位,0:在位
    para2 = ~para2;
    if (g_DevType == RAU_UNIT)
    {
      portsum = 2;
    }
    else
    {
      portsum = 8;
    }
    for (i = 0; i < portsum; i++ )
    {
      val = para2 & 0x01;
      DbSaveThisIntPara((OPTICSTATUS_ID+i), val);
      para2 = (para2 >> 1);
    }

    //LTE1 数字功率
    drv_read_fpga(LTE1DATAPOWER_ADDR, &para2);
    DbSaveThisIntPara(LTE1DATAPOWER_ID, para2);
    //LTE2 数字功率
    drv_read_fpga(LTE2DATAPOWER_ADDR, &para2);
    DbSaveThisIntPara(LTE2DATAPOWER_ID, para2);
    //3G 数字功率
    drv_read_fpga(G3DATAPOWER_ADDR, &para2);
    DbSaveThisIntPara(G3DATAPOWER_ID, para2);
    //GSM 数字功率
    drv_read_fpga(GSMDATAPOWER_ADDR, &para2);
    DbSaveThisIntPara(GSMDATAPOWER_ID, para2);
    if (g_DevType == MAIN_UNIT)
    {
	    //GSM下行输入功率电平(系数10),-15dBm+([14'h0054]-[14'h0049]+[14'h004d]-[14'h0045])/2
	    drv_read_fpga(0x0054, &para2);
	    val = para2;
	    drv_read_fpga(0x0049, &para2);
	    val = val - para2;
	    drv_read_fpga(0x004d, &para2);
	    val = val + para2;
	    drv_read_fpga(0x0045, &para2);
	    val = val - para2;
	    para2 = -150 + val * 5;
	    DbGetThisIntPara(GSMDLPOWEROFFSET_ID, &val);
	    para2 = para2 + val;
	    DbGetThisIntPara(GSMDLATT_ID, &val);
		para2 = para2 + val * 10;
	    DbSaveThisIntPara(GSMDLPOWER_ID, para2);
	    //3G下行输入功率电平(系数10),-15dBm+([14'h0053]-[14'h0048]+[14'h004c]-[14'h0045])/2
	    drv_read_fpga(0x0053, &para2);
	    val = para2;
	    drv_read_fpga(0x0048, &para2);
	    val = val - para2;
	    drv_read_fpga(0x004c, &para2);
	    val = val + para2;
	    drv_read_fpga(0x0045, &para2);
	    val = val - para2;
	    para2 = -150 + val * 5;
	    DbGetThisIntPara(G3DLPOWEROFFSET_ID, &val);
	    para2 = para2 + val;
		DbGetThisIntPara(G3DLATT_ID, &val);
		para2 = para2 + val * 10;
	    DbSaveThisIntPara(G3DLPOWER_ID, para2);
	    //LTE1下行输入功率电平(系数10),-15dBm+([14'h0051]-[14'h0046]+[14'h004a]-[14'h0045])/2
	    drv_read_fpga(0x0051, &para2);
	    val = para2;
	    drv_read_fpga(0x0046, &para2);
	    val = val - para2;
	    drv_read_fpga(0x004a, &para2);
	    val = val + para2;
	    drv_read_fpga(0x0045, &para2);
	    val = val - para2;
	    para2 = -150 + val * 5;
	    DbGetThisIntPara(LTE1DLPOWEROFFSET_ID, &val);
	    para2 = para2 + val;
		DbGetThisIntPara(LTE1DLATT_ID, &val);
		para2 = para2 + val * 10;
	    DbSaveThisIntPara(LTE1DLPOWER_ID, para2);
	    //LTE2下行输入功率电平(系数10),-15dBm+([14'h0052]-[14'h0047]+[14'h004b]-[14'h0045])/2
	    drv_read_fpga(0x0052, &para2);
	    val = para2;
	    drv_read_fpga(0x0047, &para2);
	    val = val - para2;
	    drv_read_fpga(0x004b, &para2);
	    val = val + para2;
	    drv_read_fpga(0x0045, &para2);
	    val = val - para2;
	    para2 = -150 + val * 5;
	    DbGetThisIntPara(LTE2DLPOWEROFFSET_ID, &val);
	    para2 = para2 + val;
		DbGetThisIntPara(LTE2DLATT_ID, &val);
		para2 = para2 + val * 10;
	    DbSaveThisIntPara(LTE2DLPOWER_ID, para2);

	  }
	  if (g_DevType == RAU_UNIT)
	  {
	  }
  }
}
