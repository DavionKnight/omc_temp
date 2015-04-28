/********************  COPYRIGHT(C)  ***************************************
**--------------文件信息--------------------------------------------------------
**文   件   名: commonfun.c
**创   建   人: 于宏图
**创 建  日 期: 
**程序开发环境：
**描        述: 采用CCITT标准进行的CRC校验程序
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

#include "commonfun.h"
#include "druhwinfo.h"
#include "../net/netcom.h"
#include "../driver/drv_api.h"

HWInfo_t g_HWInfo;

extern int g_DevType;
extern DevicePara_t g_DevicePara;

const int CCITT_CRC16Table[256] = 
{
  0x0000, 0x1021, 0x2042, 0x3063, 0x4084, 0x50A5, 0x60C6, 0x70E7,
  0x8108, 0x9129, 0xA14A, 0xB16B, 0xC18C, 0xD1AD, 0xE1CE, 0xF1EF,
  0x1231, 0x0210, 0x3273, 0x2252, 0x52B5, 0x4294, 0x72F7, 0x62D6,
  0x9339, 0x8318, 0xB37B, 0xA35A, 0xD3BD, 0xC39C, 0xF3FF, 0xE3DE,
  0x2462, 0x3443, 0x0420, 0x1401, 0x64E6, 0x74C7, 0x44A4, 0x5485,
  0xA56A, 0xB54B, 0x8528, 0x9509, 0xE5EE, 0xF5CF, 0xC5AC, 0xD58D,
  0x3653, 0x2672, 0x1611, 0x0630, 0x76D7, 0x66F6, 0x5695, 0x46B4,
  0xB75B, 0xA77A, 0x9719, 0x8738, 0xF7DF, 0xE7FE, 0xD79D, 0xC7BC,
  0x48C4, 0x58E5, 0x6886, 0x78A7, 0x0840, 0x1861, 0x2802, 0x3823,
  0xC9CC, 0xD9ED, 0xE98E, 0xF9AF, 0x8948, 0x9969, 0xA90A, 0xB92B,
  0x5AF5, 0x4AD4, 0x7AB7, 0x6A96, 0x1A71, 0x0A50, 0x3A33, 0x2A12,
  0xDBFD, 0xCBDC, 0xFBBF, 0xEB9E, 0x9B79, 0x8B58, 0xBB3B, 0xAB1A,
  0x6CA6, 0x7C87, 0x4CE4, 0x5CC5, 0x2C22, 0x3C03, 0x0C60, 0x1C41,
  0xEDAE, 0xFD8F, 0xCDEC, 0xDDCD, 0xAD2A, 0xBD0B, 0x8D68, 0x9D49,
  0x7E97, 0x6EB6, 0x5ED5, 0x4EF4, 0x3E13, 0x2E32, 0x1E51, 0x0E70,
  0xFF9F, 0xEFBE, 0xDFDD, 0xCFFC, 0xBF1B, 0xAF3A, 0x9F59, 0x8F78,
  0x9188, 0x81A9, 0xB1CA, 0xA1EB, 0xD10C, 0xC12D, 0xF14E, 0xE16F,
  0x1080, 0x00A1, 0x30C2, 0x20E3, 0x5004, 0x4025, 0x7046, 0x6067,
  0x83B9, 0x9398, 0xA3FB, 0xB3DA, 0xC33D, 0xD31C, 0xE37F, 0xF35E,
  0x02B1, 0x1290, 0x22F3, 0x32D2, 0x4235, 0x5214, 0x6277, 0x7256,
  0xB5EA, 0xA5CB, 0x95A8, 0x8589, 0xF56E, 0xE54F, 0xD52C, 0xC50D,
  0x34E2, 0x24C3, 0x14A0, 0x0481, 0x7466, 0x6447, 0x5424, 0x4405,
  0xA7DB, 0xB7FA, 0x8799, 0x97B8, 0xE75F, 0xF77E, 0xC71D, 0xD73C,
  0x26D3, 0x36F2, 0x0691, 0x16B0, 0x6657, 0x7676, 0x4615, 0x5634,
  0xD94C, 0xC96D, 0xF90E, 0xE92F, 0x99C8, 0x89E9, 0xB98A, 0xA9AB,
  0x5844, 0x4865, 0x7806, 0x6827, 0x18C0, 0x08E1, 0x3882, 0x28A3,
  0xCB7D, 0xDB5C, 0xEB3F, 0xFB1E, 0x8BF9, 0x9BD8, 0xABBB, 0xBB9A,
  0x4A75, 0x5A54, 0x6A37, 0x7A16, 0x0AF1, 0x1AD0, 0x2AB3, 0x3A92,
  0xFD2E, 0xED0F, 0xDD6C, 0xCD4D, 0xBDAA, 0xAD8B, 0x9DE8, 0x8DC9,
  0x7C26, 0x6C07, 0x5C64, 0x4C45, 0x3CA2, 0x2C83, 0x1CE0, 0x0CC1,
  0xEF1F, 0xFF3E, 0xCF5D, 0xDF7C, 0xAF9B, 0xBFBA, 0x8FD9, 0x9FF8,
  0x6E17, 0x7E36, 0x4E55, 0x5E74, 0x2E93, 0x3EB2, 0x0ED1, 0x1EF0
};

/*******************************************************************************
*函数名称 : INT16U CCITT_CRC16(char *pbuf, INT16U len)
*功    能 : 采用CCITT标准对字符串进行CRC校验
*输入参数 : *pbuf:指向待校验字符串的指针
*           len:字符串长度
*输出参数 : 字符串CRC校验结果
*******************************************************************************/
INT16U CCITT_CRC16(char *pbuf, INT16U len)
{
INT16U  crc;

  crc = 0;
  while(len)
  {
    crc = (crc << 8) ^ CCITT_CRC16Table[((crc >> 8) ^ *pbuf) & 0xFF];
    len--;
    pbuf++;
  }
  return crc;
}

/*******************************************************************************
*函数名称 : void ClearComBuf(ComBuf_t *pcombuf)
*功    能 : 清除通讯缓存为初始状态
*输入参数 : ComBuf_t *pcombuf:讯缓存
*输出参数 : None
*******************************************************************************/
void ClearComBuf(ComBuf_t *pcombuf)
{
  pcombuf->RecvLen = 0;
  memset(pcombuf->Buf, 0, COMBUF_SIZE);
}

/*******************************************************************************
*函数名称 : int ComDataWriteLog(char *pstr, int num)
*功    能 : 接收数据日志记录
*输入参数 : char *pstr：数据指针
*           int num：数据长度
*输出参数 : 
*******************************************************************************/
int ComStrWriteLog(char *pstr, int num)
{
int logfileno;
struct stat fstat;
FILE *fp;
char filename[100];

  logfileno = 0;
  while (logfileno < LOGFILENUM)
  {
    memset(filename, 0, sizeof(filename));
    sprintf(filename, "%s/drulog%d.txt", LOGSAVEDPATH, logfileno);
    stat(filename, &fstat);
    if (fstat.st_size < LOGFILESIZE)
    {
      break;
    }
    else
    {
      logfileno++;
    }
  }
	if (logfileno == LOGFILENUM)
  {
  	logfileno = 0;
    memset(filename, 0, sizeof(filename));
    sprintf(filename, "%s/drulog%d.txt", LOGSAVEDPATH, logfileno);
    fp = fopen(filename, "w");
  }
  else
  {
  	fp = fopen(filename, "a");
  }
        
  if(fp == NULL)
  {
    printf("Cannot open Log file.\n");
    return -1;
  }
 	fprintf(fp, "%s\n", pstr);
  fclose(fp);
  
  stat(filename, &fstat);
  if (fstat.st_size > LOGFILESIZE)
  {
  	logfileno++;
    if ( logfileno == LOGFILENUM)
      logfileno = 0;
    memset(filename, 0, sizeof(filename));
    sprintf(filename, "%s/drulog%d.txt", LOGSAVEDPATH, logfileno);
    fp = fopen(filename, "w");
    fputs("\r\n", fp);//换行
    fclose(fp);
  }
  return 1;
}

/*******************************************************************************
*函数名称 : int ComDataWriteLog(char *buf, int num)
*功    能 : 接收数据日志记录
*输入参数 : char *buf：数据指针
*           int num：数据长度
*输出参数 : 
*******************************************************************************/
int ComDataWriteLog(char *buf, int num)
{
int logfileno;
struct stat fstat;
FILE *fp;
char filename[100];

  logfileno = 0;
  while (logfileno < LOGFILENUM)
  {
    memset(filename, 0, sizeof(filename));
    sprintf(filename, "%s/drulog%d.txt", LOGSAVEDPATH, logfileno);
    stat(filename, &fstat);
    if (fstat.st_size < LOGFILESIZE)
    {
      break;
    }
    else
    {
      logfileno++;
    }
  }
	if (logfileno == LOGFILENUM)
  {
  	logfileno = 0;
    memset(filename, 0, sizeof(filename));
    sprintf(filename, "%s/drulog%d.txt", LOGSAVEDPATH, logfileno);
    fp = fopen(filename, "w");
  }
  else
  {
  	fp = fopen(filename, "a");
  }
  
  if(fp == NULL)
  {
    printf("Cannot open Log file.\n");
    return -1;
  }
  
  while(num--)
    fprintf(fp, "%02X", *buf++);
  fputs("\r\n", fp);//换行
  fclose(fp);
  
  stat(filename, &fstat);
  if (fstat.st_size > LOGFILESIZE)
  {
  	logfileno++;
    if ( logfileno == LOGFILENUM)
      logfileno = 0;
    memset(filename, 0, sizeof(filename));
    sprintf(filename, "%s/drulog%d.txt", LOGSAVEDPATH, logfileno);
    fp = fopen(filename, "w");
    fputs("\r\n", fp);//换行
    fclose(fp);
  }
  return 1;
}

/*******************************************************************************
*函数名称 : void DisplaySysTime(void)
*功    能 : 显示系统时间
*输入参数 : None
*输出参数 : None
*******************************************************************************/
void DisplaySysTime(void)
{
//#if PRINTDEBUG
	system("date");
//#endif
}

/*******************************************************************************
*函数名称 : void ComDataHexDis(char *buf, int sum)
*功    能 : 通讯包数据显示
*输入参数 : char *buf:数据缓存;int sum:数据长度
*输出参数 : None
*******************************************************************************/
void ComDataHexDis(char *buf, int sum)
{
//#if PRINTDEBUG
int i;
  for(i = 0; i < sum; i++)
    printf("%02X", *buf++);
  printf("\r\n");
//#endif
}

/*******************************************************************************
*函数名称 : int GetHWInfo(void)
*功    能 : 获取硬件配置信息
*输入参数 : none
*输出参数 : 正常输出1,否则返回-1
*******************************************************************************/
int GetHWInfo(HWInfo_t *p_hwinfo)
{
int i;
unsigned short para2;

  p_hwinfo->DevType = g_DevType;
  p_hwinfo->DeviceNo = g_DevicePara.DeviceNo; //客户端设备编号,参照移动协议
  GetSelfMac("eth0", p_hwinfo->Mac);//MAC地址
	p_hwinfo->IPAddr = GetSelfIp("eth0"); //客户端地址

  drv_read_fpga((unsigned int)FPGATOPVERSION_ADDR, &para2);
  p_hwinfo->FpgaTopVersion = para2;  //bit15-0:FPGA主版本号,MER
  drv_read_fpga((unsigned int)FPGAVERMARK_ADDR, &para2);
  p_hwinfo->FpgaVerMark = para2;  //bit15-0:版本标示说明寄存器,MER
  for (i = 0; i < INTREG_SIZE; i++)
  {
    drv_read_fpga((unsigned int)(INTMASKREG_ADDR+i), &para2);
    p_hwinfo->INTMaskReg[i] = para2; //中断屏蔽寄存器用于屏蔽相应中断信号产生,MER
  }
  for (i = 0; i < INTREG_SIZE; i++)
  {
    drv_read_fpga((unsigned int)(INTENREG_ADDR+i), &para2);
    p_hwinfo->INTEnReg[3] = para2; //中断使能寄存器,MER
  }
  for (i = 0; i < INTREG_SIZE; i++)
  {
    drv_read_fpga((unsigned int)(INTSOURCEREG_ADDR+i), &para2);
    p_hwinfo->INTSourceReg[3] = para2; //中断源寄存器,MER
  }
  for (i = 0; i < INTREG_SIZE; i++)
  {
    drv_read_fpga((unsigned int)(INTCLEARREG_ADDR+i), &para2);
    p_hwinfo->INTClearReg[3] = para2; //中断清除寄存器,MER
  }
  for (i = 0; i < OPT_SIZE; i++)
  {
    drv_read_fpga((unsigned int)(TX2RXTIMEOPT_ADDR+i), &para2);
    p_hwinfo->Tx2RxTimeOpt[8] = para2; //光口时延测量值,计算各光口光纤时延测量值,MER
  }
  drv_read_fpga((unsigned int)UL2KMADJUST_ADDR, &para2);
  p_hwinfo->UL2kmAdjust = para2; //上行2KM缓存调整寄存器,MER
  drv_read_fpga((unsigned int)DL2KMADJUST_ADDR, &para2);
  p_hwinfo->DL2kmAdjust = para2; //下行2KM缓存调整寄存器,MER
  drv_read_fpga((unsigned int)SPDCFGREG_ADDR, &para2);
  p_hwinfo->SpdCfgReg = (char)para2; //固定值5G模式,MER
  drv_read_fpga((unsigned int)RSTIDREGH_ADDR, &para2);
  p_hwinfo->RstIdReg = (p_hwinfo->RstIdReg & 0x0000FFFF) + ((INT32U)para2<<16); //复位控制字,MER
  drv_read_fpga((unsigned int)RSTIDREGL_ADDR, &para2);
  p_hwinfo->RstIdReg = (p_hwinfo->RstIdReg & 0x0000) + para2; //复位控制字,MER
  drv_read_fpga((unsigned int)OPTIQENREG_ADDR, &para2);
  p_hwinfo->OptIQEnReg = (char)para2; //设备IQ叠加使能,MER
  drv_read_fpga((unsigned int)OPTSYNCREG_ADDR, &para2);
  p_hwinfo->OptSyncReg = (char)para2; //同步标识,MER
  drv_read_fpga((unsigned int)DEVIDREGH_ADDR, &para2);
  p_hwinfo->DevIdReg = (p_hwinfo->DevIdReg & 0x0000FFFF) + ((INT32U)para2<<16); //设备级联编号,MER
  drv_read_fpga((unsigned int)DEVIDREGL_ADDR, &para2);
  p_hwinfo->DevIdReg = (p_hwinfo->DevIdReg & 0x0000) + para2; //设备级联编号,MER
  drv_read_fpga((unsigned int)DEVIDLEVEL_ADDR, &para2);
  p_hwinfo->DevIdLevel = para2; //设备级联级数,MER
  drv_read_fpga((unsigned int)MAINIDREG_ADDR, &para2);
  p_hwinfo->MainIdReg = para2; //网络拓扑的主单元编号,MER
  drv_read_fpga((unsigned int)ULOPTSELREG_ADDR, &para2);
  p_hwinfo->ULOptSelReg = (char)para2; //上联光口选择,MER
  
  return 1;
}

/*********************************End Of File*************************************/
