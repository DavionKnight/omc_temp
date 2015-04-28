/********************  COPYRIGHT(C) ***************************************
**--------------�ļ���Ϣ--------------------------------------------------------
**��   ��   ��: drudefstruct.h
**��   ��   ��: �ں�ͼ
**�� ��  �� ��: 
**���򿪷�������
**��        ��: ����ʹ�ýṹ���õ�ͷ�ļ�
**--------------��ʷ�汾��Ϣ----------------------------------------------------
** ������: �ں�ͼ
** ��  ��: v1.0
** �ա���: 
** �衡��: ԭʼ�汾
**--------------��ǰ�汾�޶�----------------------------------------------------
** �޸���:
** ��  ��:
** �ա���:
** �衡��:
**------------------------------------------------------------------------------
**
*******************************************************************************/
#ifndef _DRUDEFSTRUCT_H
#define _DRUDEFSTRUCT_H

#include "usertype.h"
#include <semaphore.h>

#define COMBUF_SIZE  0x600

#pragma pack(1)
typedef struct ComBuf
{
  	int   Fd;           		//ͨѶ�豸��
	char  Buf[COMBUF_SIZE]; 	//�������ݻ���
	int   RecvLen;				//�������ݳ���

	int   Timer;        		//ͨѶ�豸��ʱ
	int   Status;       		//ͨѶ�豸״̬
	sem_t SemFlag;       		//�ź�����ʶ
}ComBuf_t, *ComBuf_tp;
#pragma pack()

typedef struct SelfThread
{
  pthread_t Tid;    	//��ǰ�̵߳��߳�ID    
  int ThreadStatus; 	//�̵߳�״̬
}SelfThread_t, *SelfThread_tp;

typedef struct Pdu
{
  int   id;           //��ʶ
  char  mode[5];      //RO/RWģʽ
  int   len;          //���ݳ���
  char  var_type[10]; //uint1,uint2...��������
  int   coefficient;  //ϵ��
  char  var[48];      //����
  char  name[100];    //��������
  int	min;		  //��Сֵ
  int	max;          //���ֵ
  int	pro;          //��ʶΪ�Ƿ�Ϊ�����������  1:�����������  0���Զ������
}Pdu_t, *Pdu_tp;

//ͨѶ���ݰ�ͷ�Ķ���
#pragma pack(1)
typedef struct APPackHead
{
  char   StartFlag;      //ͨѶ��ʼ��־
  char   APType;         //AP��Э������
  char   VPType;         //VP����Э������
  INT32U  StationNo;     //վ����,200.200.200�㲥��ַ���
  char   DeviceNo;       //�豸���,0xFF��Ϊ�㲥��ַ���
  INT16U  PackNo;        //ͨѶ����ʶ��
  char   VPInteractFlag; //VP�㽻����־
  char   MCPFlag;        //MCP��Э���ʶ
  char   CommandFlag;    //�����ʶ
  char   ResponseFlag;   //Ӧ���־
}APPackHead_t, *APPackHead_tp;

//ͨѶ���ݰ�β�Ķ���
typedef struct APPackTail
{
  INT16U  CRCData;    	//CRCУ����
  char   EndFlag;    	//ͨѶ��ֹͣ��־ 
}APPackTail_t, *APPackTail_tp;

typedef struct APPack
{
  char   StartFlag;      //ͨѶ��ʼ��־
  char   APType;         //AP��Э������
  char   VPType;         //VP����Э������
  INT32U  StationNo;     //վ����,200.200.200�㲥��ַ���
  char   DeviceNo;       //�豸���,0xFF��Ϊ�㲥��ַ���
  INT16U  PackNo;        //ͨѶ����ʶ��
  char   VPInteractFlag; //VP�㽻����־
  char   MCPFlag;        //MCP��Э���ʶ
  char   CommandFlag;    //�����ʶ
  char   ResponseFlag;   //Ӧ���־
  
  char PackValue[PACK_VALUE_SIZE];//���ݵ�Ԫ����
  
  INT16U CRCData;      //CRCУ����
  char   EndFlag;      //ͨѶֹͣ��־ 

  char   ComFlag;      //ͨѶ��־:0����,1���� 
  char   ComBufOver;   //ͨѶ�����������־
  char   *PtrComWr;    //ָ����һ�ַ����������λ��
  char   *PtrComRd;    //ָ����һ�ַ�������ȡ��λ��
  INT16U  ComCounter;   //ͨѶ�������ַ���
  INT16U  PackLen;      //ͨѶ���ݰ�����
}APPack_t, *APPack_tp;
#pragma pack()

// �������ܵ��Ժ�485ģ����ԣ�����RS485����ʱ�Ľ����������ṹ
typedef struct SWAP_ST{
	unsigned char buf[1024];
	int len;
}swap_t, *swap_tp;

//�豸��Ϣ����
typedef struct DevInfo
{
  INT32U  StationNo; 	//վ����
  char   DeviceNo;  	//�豸���,0xFF��Ϊ�㲥��ַ���
  char   ModuleAddr;	//ģ���ַ
  INT16U  ModuleType;	//ģ������
  INT16U  PduStart;
}DevInfo_t, *DevInfo_tp;

typedef struct DevicePara
{
	int  ManufacturerID;            //0x0002  ���̴��� uint1�� 
	int  DeviceClass;               //0x0003  �豸��� uint1�� 
	char DeviceType[20];            //0x0004  �豸�ͺ�,str��,��󳤶�20���ֽ�,�������豸�ͺŽ��м�Ҫ���� 
	char SerialID[20];              //0x0005  �豸�������к�,str��,��󳤶�20���ֽ� 
	int  CarrierNums;               //0x0006  �豸��ʵ���ز����� uint1�� 
	char Longitude[20];             //0x0007  �豸���� str��,��󳤶�20���ֽ�(Ҫ��λ�á�)��:����103.12345678������"E103.12345678"��ʾ
	char Latitude[20];              //0x0008  �豸γ�� str��,��󳤶�20���ֽ�(Ҫ��λ�á�)��:��γ23.12345678������"N23.12345678"��ʾ��
	char SWRunVer[20];              //0x000A  ��ǰ��������汾 str��,��󳤶�20���ֽ� 
	int  ComModemType;              //0x000B  Զ������ͨ��ģ�� uint1��
	int  SWRunMode;                 //0x0010  �豸�����������ģʽ uint1�� 0:���ģʽ,1:��������ģʽ,����ֵΪϵͳ���� 
	int  APCMaxLen;                 //0x0011  AP:CЭ����󳤶� uint2�� 
	int  MCPBExchangeMode;          //0x0012  MCP:B���õĽ������� uint1�� 0:��ʾ����ÿ��ȷ�Ϸ�ʽ����:ÿ�յ�һ����ȷ��һ�Ρ�������NC����ȴ���Ӧ�Ļ���, 1:��ʾ���ö���һ��ʽ����:ÿ����NC��ȷ��һ�Ρ�������NC����ȴ���Ӧ�Ļ���, ����ֵΪϵͳ����
	int  ContinuumNum;              //0x0013  ����ϵ��(NC)uint1�� 
	int  RespondOvertime;           //0x0014  �豸��Ӧ��ʱ(TOT1)uint1��,��λΪS
	int  SendIntervaltime;          //0x0015  ���ͼ��ʱ��(TG)(��������NC��1ʱ��������) uint2��,��λΪmS 
	int  PauseTime;                 //0x0016  ��ͣ����ȴ�ʱ��(TP) uint1��,��λΪS
	int  SWUpdateOMCWaitTime;       //0x0017  ת������������ģʽʱ,OMC��Ҫ�ȴ���ʱ�� uint2��,��λΪS ����ֵΪ0ʱ,��ʾ����ȴ�
	int  SWUpdateResult;            //0x0018  �豸ִ�����������Ľ�� uint1��
	int  SWUpdateMode;              //0x0020  �豸ʹ�õ�Զ��������ʽ uint1�� 0:��ʾ�豸�޷�����Զ��������1:��ʾ����MCP:B��ʽ��2:��ʾ����FTP��ʽ������ֵΪϵͳ������
	char FPGAVer[20];               //0x0021  FPGA�汾��Ϣ str��,��󳤶�20���ֽ�

	int  StationNo;                 //0x0101  վ���� uint4��,��������վ����ʱʹ��
	char DeviceNo;                  //0x0102  �豸��� uint1��,���������豸���ʱʹ�á�����ϵͳ:�̶�Ϊ255 ��վ:�̶�Ϊ0 ��վ:1��254
	char SmscTel[20];               //0x0110  ���ŷ������ĺ��� str��,��󳤶�20���ֽ� 
	char QueryTel[5][20];           //0x0111~0x0115 ��ѯ/���õ绰����1~5 str��,��󳤶�20���ֽ� 
	char NotifyTel[20];             //0x0120  �ϱ����� str��,��󳤶�20���ֽ�
	INT32U OmcIP;                   //0x0130  �������IP��ַ,4�ֽڳ���������
	int  OmcIPPort;                 //0x0131  �������IP��ַ�˿ں� uint2��
	char GprsAPN[20];               //0x0133  GPRS���������(��:APN)str��,��󳤶�20���ֽ�
	int  HeartBeatTime;             //0x0134  �豸�����������ʱ�� uint2��,��λΪ���� ��ֵΪ0ʱ,��ʾ����������
	char GprsUser[20];              //0x0136  GPRS����:�û���ʶ str��,��󳤶�20���ֽ�
	char GprsPassword[20];          //0x0137  GPRS����:���� str��,��󳤶�20���ֽ�
	int  PSTranProtocol;            //0x0138  PS����Э�� uint1��,1:ʹ��IP+UDPЭ��,2:ʹ��IP+TCPЭ��
	int  DeviceIPPort;              //0x0139  �豸���ն˿�(UDPЭ��) uint2��
	int  NotifyType;                //0x0141  �ϱ����� uint1��,��ȷ�ϱ�������(���ϱ�ʱ,�������ݵ�Ԫ����ǰ��)
	int  DeviceCommType;            //0x0142  �豸ͨ�ŷ�ʽ uint1��,��ȷ�豸��OMC��ͨ�ŷ�ʽ1:���ŷ�ʽ��ѯ������,���ŷ�ʽ�ϱ���2:GPRS��ʽ��ѯ������,GPRS��ʽ�ϱ�������ֵΪϵͳ����
	INT32U RW1_OmcIP;				//0x0143  ��ѯ/����IP��ַ1		
	INT32U RW2_OmcIP;				//0x0144  ��ѯ/����IP��ַ1
	char DeviceTime[7];             //0x0150  �豸��ǰʱ�� ���ִ�,��7���ֽ����,��BCD���ʾ(����4λ,ʱ����24Сʱ��ʽ)
	INT32U DeviceIP;                //0x0151  �豸IP��ַ���ִ�,4�ֽڳ���������
	INT32U DeviceNetmask;           //0x0152  �豸��������,4�ֽڳ���������
	INT32U DeviceGateway;           //0x0153  �豸Ĭ������,4�ֽڳ���������

	INT32U FtpServerIP;             //0x0160  FTP������IP��ַ,4�ֽڳ���������
	int  FtpServerIPPort;           //0x0161  FTP������IP��ַ�˿ں� uint2��
	char FtpUser[20];               //0x0162  FTP�����������û��� str��,��󳤶�20���ֽ�
	char FtpPassword[20];           //0x0163  FTP���������Ŀ��� str��,��󳤶�20���ֽ�
	char SWFilePath[40];            //0x0164  ���·�� str��,��󳤶�40���ֽ�
	char SWFileName[40];            //0x0165  �ļ��� str��,��󳤶�40���ֽ�
	int  FtpTranCtrl;               //0x0166  FTP�ļ�������� uint1��

	int  BoxTemp;                   //0x05A6  �����¶� sint1��,��λΪ��
	char SampleJobStartTime[7];     //0x0601  ���ɿ�ʼʱ�� ���ִ�,��7���ֽ����,��BCD���ʾ(����4λ,ʱ����24Сʱ��ʽ)
	int  SampleJobTime;             //0x0602  ����ʱ�� uint2��,��λΪ����
	int  SampleJobID;               //0x0603  ���ɶ��� uint2�� 
	int  SampleJobSwitch;           //0x0604  ���ɿ��� uint1�� 
	int  SampleJobStatus;           //0x0605  ����״̬ uint1�� 
	int  SampleJobDataLen;          //0x0607  ��������ÿ�鳤�� uint1��
	INT32U DevRoute;                //0x0873  �豸·�ɵǼǵ�ַ ���ִ�,��4���ֽ����
}DevicePara_t, *DevicePara_tp;

typedef struct SqlResult
{
	char  **dbResult;   	//����������ݶ�����������,��ʵ�����Ǹ�1ά����(��Ҫ��Ϊ��2ά����),ÿһ��Ԫ�ض���һ�� char * ֵ,��һ���ֶ�����(���ַ�������ʾ,��\0��β)
	int   nRow;         	//��ѯ����������¼(�����������)
	int   nColumn;      	//���ٸ��ֶ�(������)
	char  *errmsg;      	//������Ϣ
}SqlResult_t, *SqlResult_tp;

#define MACSIZE       6 	//MAC��ַ����
typedef struct ClientBuf
{
  	int   Fd;           	//�ͻ���socket������
  	int   DevNo;       		//�豸���,�����ƶ�Э��
	char  Mac[MACSIZE];   	//MAC��ַ
	INT32U IPAddr;        	//IP��ַ
	int   DevType;        	//�豸����,1��,2��,3Զ
	int   Status;       	//�ͻ�������״̬
	time_t LastComTime;   	//�ͻ������һ��ͨѶʱ��  
}ClientBuf_t, *ClientBuf_tp;

//RRU��BBU����UDP�㲥������Ϣ�ĸ�ʽ
#pragma pack(1)
typedef struct UDPRequestMsg
{
	char BBU_OptInterface;
	char RRU_ID;
	char RRU_Mac[MACSIZE];	//MAC��ַ
	//�Զ���
	int DevNo;    		//�豸���,�����ƶ�Э��
	INT32U IPAddr;		//IP��ַ
	int DevType;  		//�豸����,1��,2��,3Զ
}UDPRequestMsg_t, *UDPRequestMsg_tp;

//BBUӦ�����Ϣ��ʽ
typedef struct UDPAckMsg
{
	char BBU_OptInterface;
	char RRU_ID;
	char BBU_ID;
	char RRU_Mac[MACSIZE];
	INT32U RRU_IP;
	INT32U BBU_IP;
	INT32U NetMask;
	//�Զ���
	int StationNo; 		//վ����
	int DevNo;      	//�豸���,�����ƶ�Э��
}UDPAckMsg_t, *UDPAckMsg_tp;
#pragma pack()

#endif//_DRUDEFSTRUCT_H

/*********************************End Of File*************************************/