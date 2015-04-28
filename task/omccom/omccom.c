/********************  COPYRIGHT(C) ***************************************
**--------------�ļ���Ϣ--------------------------------------------------------
**��   ��   ��: omccom.c
**��   ��   ��: 
**�� ��  �� ��:  
**���򿪷�������
**��        ��: ��OMCͨѶϵͳ���Ƴ���
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
#include "../localcom/localcom.h"
#include "../driver/drv_api.h"
#include "../../common/commonfun.h"
#include "../../common/druhwinfo.h"
#include "../../net/netcom.h"
#include "../../protocol/approtocol.h"
#include "../../protocol/apbprotocol.h"
#include "../../protocol/apcprotocol.h"
#include "../../protocol/mcpb_protocol.h"
#include "../../sqlite/drudatabase.h"
#include "omccom.h"
#include "modem.h"
#include "common.h"

int g_OMCLoginFlag, g_OMCHeartBeatFlag, g_OMCPackNo, g_OMCSetParaFlag, g_MCPFlag;
time_t g_AlarmDectTime;//�澯��ѯ��ʱ
int m_report_state;  // �����ϸ�״̬ 0���������ϸ� 2:��վ�ϱ� 3��Ѳ���ϱ� 4�������޸��ϱ� 5�����ñ���ϱ�
volatile unsigned int g_alarm_report_time = 0x0;
volatile unsigned int g_alarm_report_cnt = 0;
unsigned int m_alarm_report_cnt = 0;

ComBuf_t g_OMCCom;
SelfThread_t g_OMCComThread;
ComBuf_t g_OMC_IrTranCom, g_Ir_OMCTranCom;

extern int g_DevType;
extern DevicePara_t g_DevicePara;

extern ComBuf_t omc_rs485_combuf, rs485_omc_combuf;
extern ComBuf_t g_Sms_Net_Buf, g_Net_Sms_Buf;
//extern const char * g_ver;
extern const char * g_type_ver; 
extern const char * g_drv_ver;
extern const char * g_test_ver;
extern const char * g_comm_test_ver;
extern const char * g_comm_ver;
extern MCPBPara_t SWUpdateData; //�����������̲�������
extern int dru_lmx2531_wcdma_config(unsigned int freq);
extern int dru_lmx2531_fdd_lte1_config(unsigned int freq);
extern int dru_lmx2531_fdd_lte2_config(unsigned int freq);
extern int GetClientIP(char devno);
extern int ModemSmsReceiveData(ComBuf_t *p_combuf, char *tel);
extern int LoadUpdatePara(void);
extern void UpdateModeApp(void);
extern ComBuf_t g_SmsCom;
extern int g_OMCSetParaRs485Flag;

/*******************************************************************************
*�������� : void ApPackTransimtPack(APPack_t *p_packbuf, ComBuf_t *p_combuf)
*��    �� : ApPack͸�����ݰ����
*������� : none
*������� : none
*******************************************************************************/ 
void ApPackTransimtPack(APPack_t *p_packbuf, ComBuf_t *p_combuf)
{
int i;
DevInfo_t DevInfo, *p_devinfo;

  for(i = 0; i < 20; i++)//���ȴ�3s,�����һ����������
  {
    if (p_combuf->RecvLen == 0)
      break;
    sleep(1);
  }
  if (i == 20)
  {
    p_combuf->RecvLen = 0;
    DEBUGOUT("ApPackTransimtPack:Clear Last SendBuf!\r\n");
  }
  p_devinfo = &DevInfo;
  GetDevInfo(p_devinfo, p_packbuf);
  memset(&p_combuf->Buf, 0, COMBUF_SIZE);
  memcpy(&p_combuf->Buf, &p_packbuf->PackLen, 2);
  memcpy(&p_combuf->Buf[2], &p_packbuf->StartFlag, p_packbuf->PackLen);
  memcpy(&p_combuf->Buf[p_packbuf->PackLen-1], &p_packbuf->CRCData, 3);
  //p_combuf->Fd = p_devinfo->DeviceNo+DEFAULT_STARTIP-1;
  if (g_DevType == MAIN_UNIT)
  	p_combuf->Fd = (unsigned char)GetClientIP(p_devinfo->DeviceNo);
  else
  	p_combuf->Fd = p_devinfo->DeviceNo;
  p_combuf->RecvLen = p_packbuf->PackLen+2;
  ComDataHexDis(p_combuf->Buf, p_combuf->RecvLen);
}

/*******************************************************************************
*�������� : void ApPackTransimtUnPack(APPack_t *p_packbuf, ComBuf_t *p_combuf)
*��    �� : ApPack͸�����ݰ����
*������� : none
*������� : none
*******************************************************************************/ 
void ApPackTransimtUnPack(APPack_t *p_packbuf, ComBuf_t *p_combuf)
{
  if (p_combuf->RecvLen == COMBUF_SIZE)
  {
    memcpy(&p_combuf->RecvLen, &p_combuf->Buf, 2);
    p_combuf->RecvLen = p_combuf->RecvLen+2;
    g_OMCHeartBeatFlag = 0;
    g_OMCCom.Timer = 0;
  }
  //ComDataHexDis(p_combuf->Buf, p_combuf->RecvLen);
  DEBUGOUT("ApPackTransimtUnPack.\r\n");
  ComDataWriteLog(&p_combuf->Buf[2], p_combuf->RecvLen);//����־,��ʼ2�ֽ�
  memset((char *)p_packbuf, 0, sizeof(APPack_t));
  memcpy((char *)p_packbuf, &p_combuf->Buf[2], (p_combuf->RecvLen-5));//��ʼ2�ֽ�+CRC2�ֽ�+��β
  memcpy((char *)&p_packbuf->CRCData, &p_combuf->Buf[p_combuf->RecvLen-3], 3);
  p_packbuf->PackLen = p_combuf->RecvLen-2;
  p_combuf->RecvLen = 0;
}

/*******************************************************************************
*�������� : void *OMCCom_Thread(void *pvoid)
*��    �� : ����ԪOMCͨѶ�߳�
*������� : none
*������� : none
*******************************************************************************/ 
void *OMCCom_Thread(void *pvoid)
{
	DevicePara_t *p_devpara;
	ComBuf_t *p_combuf;
	APPack_t PackBuf, *p_packbuf;
	DevInfo_t DevInfo, *p_devinfo;
	int connetsum, loginsum, resum, res, sms_resum, sms_res, hearttime;
	int i_xd,APC_StartEnd_FlagNum, dd;
	char *Clearbuf;
	time_t starttime, alarmreporttime;
	time_t current_time;
	unsigned int next_time;
	time_t start_delay_time;
	char smstel[20];
	int HeartBeatFlag = 0;

	pthread_detach(pthread_self());
	pthread_setcancelstate(PTHREAD_CANCEL_ENABLE, NULL);
	pthread_setcanceltype(PTHREAD_CANCEL_ASYNCHRONOUS, NULL);//�߳�����Ϊ����ȡ��

	OMCComInit();

	starttime = time(NULL)-NETCONNETINTERVAL+3;
	alarmreporttime = time(NULL);
	printf("OMCCom_Thread Run!\r\n");
	g_OMCPackNo = 0x8000;
	g_MCPFlag = MCP_C;
	LoadUpdatePara();//�豸�����������ģʽ,����ʱ��һ���ļ����ݿ����
	connetsum = 0;//��������
	loginsum = 0;
	start_delay_time = time(NULL);
	dd = 0;

	while(1)
	{
N_APCUnpack:
		usleep(10000);
		OMCDevParaDeal();
		UpdateModeApp();//MCP_B�������Ƴ���
		p_devpara = &g_DevicePara;
		p_combuf = &g_OMCCom;
		p_packbuf = &PackBuf;
		p_devinfo = &DevInfo;
		p_combuf->Timer = (int)(time(NULL) - starttime);

		if (m_report_state != 0)
		{
			starttime = time(NULL);
			//��Ϊ����ĸ����ϱ�,�翪վ\�ָ���
			memset(p_devinfo, 0, sizeof(DevInfo_t));
			p_devinfo->StationNo = p_devpara->StationNo;
			p_devinfo->DeviceNo = p_devpara->DeviceNo;
			if (p_devpara->ComModemType == GSM_MODEM)	//ͨѶģ��ΪMODEM
			{
				if (p_devpara->DeviceCommType == SMS_MODE)	//���ŷ�ʽ
				{
_SMS_REPORT_OMC:
					ReportParaPack(AP_B, p_devinfo, m_report_state, g_OMCPackNo++, p_packbuf);
					m_report_state = 0;
					ApPackTransimtPack(p_packbuf, &g_Net_Sms_Buf);
					ClearAPPackBuf(p_packbuf);
				}
				else
				{
					if (p_combuf->Status == LOGINSUCCESS)
					{
						ReportParaPack(AP_C, p_devinfo, m_report_state, g_OMCPackNo++, p_packbuf);
						m_report_state = 0;
						goto _SEND_AP_PACK_OMC;
					}
					else
					{
						goto _SMS_REPORT_OMC;
					}
				}
			}
			else if(p_devpara->ComModemType == NET8023)
			{
				ReportParaPack(AP_C, p_devinfo, m_report_state, g_OMCPackNo++, p_packbuf);
				m_report_state = 0;
				goto _SEND_AP_PACK_OMC;
			}
		}
		//if((p_combuf->Status == LOGINSUCCESS) || ((time(NULL) - start_delay_time) > 180))
		if((p_combuf->Status == LOGINSUCCESS) 
				|| ((p_devpara->ComModemType == GSM_MODEM)&&(p_devpara->DeviceCommType == SMS_MODE)) 
				|| (g_SmsCom.Status == GPRS_SMS))
		{
			//if((p_combuf->Status == LOGINSUCCESS)){ 
			if ((time(NULL)-alarmreporttime) > 2)// ÿ3��ִ��һ��
			{
				alarmreporttime = time(NULL);
				m_alarm_report_cnt++;
				//printf("\n\n>>>>>>>>>>>>>>>\n");
				//printf("m_alarm_report_cnt=%d, g_alarm_report_time=%d\n", m_alarm_report_cnt, g_alarm_report_time);
				if((m_alarm_report_cnt > g_alarm_report_time) 
						|| ((alarmreporttime-current_time) > next_time))
				{
					m_alarm_report_cnt = 0;
					memset(p_devinfo, 0, sizeof(DevInfo_t));
					p_devinfo->StationNo = p_devpara->StationNo;//���豸վ����
					p_devinfo->DeviceNo = p_devpara->DeviceNo;//���豸�豸���
					if (ReportParaPack(AP_C, p_devinfo, REPORT_ALARM, g_OMCPackNo++, p_packbuf) > 0)
					{
						g_alarm_report_cnt++;
						printf("send alarm!!!\n");
						if((g_alarm_report_cnt == 3)||(g_alarm_report_cnt == 6))
						{
							g_alarm_report_time = 3600;
							current_time = time(NULL);
							next_time = 3*60*60;
						}
						else if(g_alarm_report_cnt == 9)
						{
							g_alarm_report_time = 0xffffffff;
							current_time = time(NULL);
							next_time = 0xffffffff;
						}
						else
						{
							g_alarm_report_time = 60;
							current_time = time(NULL);
							next_time = 0xffffffff;
						}
						if (p_devpara->ComModemType == GSM_MODEM)
						{
							if (p_devpara->DeviceCommType == SMS_MODE)
							{
_SMS_ALM_REPORT:
								ReportParaPack(AP_B, p_devinfo, REPORT_ALARM, (g_OMCPackNo-1), p_packbuf);
								ApPackTransimtPack(p_packbuf, &g_Net_Sms_Buf);
								ClearAPPackBuf(p_packbuf);
							}
							else
							{
								if (p_combuf->Status == LOGINSUCCESS)
								{

									goto _SEND_AP_PACK_OMC;
								}
								else
								{
									goto _SMS_ALM_REPORT;
								}
							}
						}
						else if(p_devpara->ComModemType == NET8023)
						{
							if (p_combuf->Status == LOGINSUCCESS)
							{
								goto _SEND_AP_PACK_OMC;
							}
						}
					}
				}
			}
		}

		if (g_Ir_OMCTranCom.RecvLen > 0)//IRЭ��ת��͸��
		{
			DEBUGOUT("Ir->OMC TransimtPack!\r\n");
			ApPackTransimtUnPack(p_packbuf, &g_Ir_OMCTranCom);
			if (p_devpara->ComModemType == GSM_MODEM)
			{
				if (p_devpara->DeviceCommType == SMS_MODE)
				{
_SMS_SEND:
					ApPackTransimtPack(p_packbuf, &g_Net_Sms_Buf);
					ClearAPPackBuf(p_packbuf);
				}
				else
				{
					if (p_combuf->Status == LOGINSUCCESS)
					{
						goto _SEND_AP_PACK_OMC;
					}
					else
					{
						goto _SMS_SEND;
					}
				}
			}
			else if(p_devpara->ComModemType == NET8023)
			{
				if (p_combuf->Status == LOGINSUCCESS)
				{
					goto _SEND_AP_PACK_OMC;
				}
			}
		}

		//sleep(3);
		//DEBUGOUT("p_combuf->Status:%d;       %d     %d.zzzzzz\r\n",p_combuf->Status, connetsum, loginsum);
		if (p_combuf->Status == NET_DISCONNET)
		{
			if (p_combuf->Timer > NETCONNETINTERVAL)//����ͨ����·ʱ����
			{
				//SocketClientDisconnet(p_combuf);
				starttime = time(NULL);
				//��OMC����ͨ����·,����CONNETFAILSUMʧ�ܶ϶�Ϊδ����
				//if (connetsum < (CONNETFAILSUM+2))
				connetsum++;
				if (connetsum == CONNETFAILSUM)//��PS���¼ʧ�ܡ��ϱ���һ���Ե��ϱ������ϱ�ʧ�ܺ�,�豸����Ҫ�Զ��ط�
				{
_PSLOGIN_FAIL:
					//���ŷ���,PS������ʧ��
					memset(p_devinfo, 0, sizeof(DevInfo_t));
					p_devinfo->StationNo = p_devpara->StationNo;
					p_devinfo->DeviceNo = p_devpara->DeviceNo;
					ReportParaPack(AP_B, p_devinfo, REPORT_PSLOGIN_FAIL, g_OMCPackNo++, p_packbuf);
					ApPackTransimtPack(p_packbuf, &g_Net_Sms_Buf);
					DEBUGOUT("SocketClientConnet:Fail,ReportParaPack.\r\n");
					ClearAPPackBuf(p_packbuf);
					SocketClientDisconnet(p_combuf);
					g_SmsCom.Status = GPRS_SMS;
				}
				else if (connetsum == (CONNETFAILSUM+2))
				{
					connetsum = CONNETFAILSUM; 
					g_SmsCom.Status = GPRS_SMS;
				}
				else
				{
					memset(p_devinfo, 0, sizeof(DevInfo_t));
					LoadDevicePara(p_devinfo, p_devpara);//��ȡ����
					if (p_devpara->ComModemType == NET8023) //0x000B:802.3����
					{
						p_devpara->DeviceIP = GetSelfIp("eth0:1");//�豸Զ��ͨ��ģ�������̫��������ADSL MODEMʱ��ֻ֧��PS��ʽ������UDPЭ��
						SocketClientConnet(p_combuf, p_devpara->OmcIP, p_devpara->OmcIPPort, p_devpara->DeviceIP, p_devpara->DeviceIPPort, IP_UDP_MODE);
						//SocketClientConnet(p_combuf, p_devpara->OmcIP, p_devpara->OmcIPPort, p_devpara->DeviceIP, p_devpara->DeviceIPPort, p_devpara->PSTranProtocol);
					}
					else if ((p_devpara->ComModemType == GSM_MODEM) && (p_devpara->DeviceCommType == PS_MODE))
					{
						DEBUGOUT("GSM_MODEM CONNET................................................................\r\n");
						SocketClientConnet(p_combuf, p_devpara->OmcIP, p_devpara->OmcIPPort, GetSelfIp("ppp0"), p_devpara->DeviceIPPort, p_devpara->PSTranProtocol);
					}
					if (p_combuf->Status == NET_CONNET)
					{
						connetsum = CONNETFAILSUM;
						g_OMCLoginFlag = 0;
						starttime = time(NULL) - LOGININTERVAL + 3;
						if (p_devpara->ComModemType == NET8023) //0x000B:802.3����
							p_combuf->Status = LOGINSUCCESS;

					}
				}
			}
		}
		else if ((p_combuf->Status == NET_CONNET)||(p_combuf->Status == LOGINSUCCESS))
		{
			//��OMC���ӻ�ɹ���¼��Socket��������
			resum = 0;
			if (p_devpara->PSTranProtocol == IP_TCP_MODE)
			{
				resum = TCPSocketReceiveData(p_combuf, COMWAITTIME);
			}
			else if (p_devpara->PSTranProtocol == IP_UDP_MODE)
			{
				resum = UDPSocketReceiveData(p_combuf, p_devpara->OmcIP, p_devpara->OmcIPPort, COMWAITTIME);
				/*
				while(-1!= UDPSocketReceiveData(p_combuf, p_devpara->OmcIP, p_devpara->OmcIPPort, COMWAITTIME))
				{
					resum ++;
					usleep(1*1000);
				}
				
				if(resum>0)
				{
					printf("==================== resum = %d\n", resum);
					ComDataHexDis(p_combuf->Buf, p_combuf->RecvLen);
				}
				*/
			}

			if ((resum > 0) ||(dd == 1))
			{
//N_APCUnpack:
				g_OMCHeartBeatFlag = 0;
				p_combuf->Timer = 0;
				starttime = time(NULL);
				DEBUGOUT("OMC_Socket_Fd:%d.ReceiveData:%d.\r\n", p_combuf->Fd, p_combuf->RecvLen);
				DisplaySysTime();		//��ʾϵͳʱ��
				//ComDataHexDis(p_combuf->Buf, p_combuf->RecvLen);
				res = APCUnpack(p_combuf->Buf, p_combuf->RecvLen, p_packbuf);
				DEBUGOUT("OMCCom:SocketReceiveData WriteLogBook.\r\n");
				ComDataWriteLog(p_combuf->Buf, p_combuf->RecvLen);//����־
				if(res > 0)
				{
					GetDevInfo(p_devinfo, p_packbuf);
					if (p_devinfo->DeviceNo == p_devpara->DeviceNo)//���豸����
					{
						if (p_devinfo->ModuleType != 0x00)//͸��,תRS485ͨѶ
						{
							DEBUGOUT("OMC->RS485 TransimtPack!\r\n");
							ApPackTransimtPack(p_packbuf, &omc_rs485_combuf);
						}
						else
						{
							if (APProcess(p_packbuf, p_devpara) > 0)
							{
								printf("PackLen: %d\n", p_packbuf->PackLen);
								if (p_packbuf->PackLen > 0)
								{
									p_combuf->Timer = 0;
									starttime = time(NULL);
									printf("1111mcp_b time return :%d\n", p_devpara->PSTranProtocol);
									DisplaySysTime();
									printf("2222mcp_b time return :%d\n", p_devpara->PSTranProtocol);
									if (p_devpara->PSTranProtocol == IP_TCP_MODE)
									{
										printf("tcp !!!!!!!!\n");
										TCPSocketSendPack(p_combuf, p_packbuf);
									}
									else if (p_devpara->PSTranProtocol == IP_UDP_MODE)
									{
										printf("return len: %d\n", p_packbuf->PackLen);
										//UDPSocketSendPack(p_combuf, p_packbuf, p_devpara->OmcIP, p_devpara->OmcIPPort);
										UDPSocketSendPack(p_combuf, p_packbuf, GetReceiveIP(), p_devpara->OmcIPPort);
									}else
									{
										printf("p_devpara->PSTranProtocol error :%d\n", p_devpara->PSTranProtocol);
									}
								}
							}
						}
					}
					else//ͨ��IRЭ��ת��
					{
						if (g_DevType == MAIN_UNIT)
						{
							DEBUGOUT("OMC->Ir TransimtPack!\r\n");
							ApPackTransimtPack(p_packbuf, &g_OMC_IrTranCom);
						}
					}
					
					APC_StartEnd_FlagNum = 0;
					Clearbuf = p_combuf->Buf;
					for(i_xd=0;i_xd<p_combuf->RecvLen;i_xd++)
					{
						if(Clearbuf[i_xd]==APC_STARTFLAG)
						{
							APC_StartEnd_FlagNum++;
							if(APC_StartEnd_FlagNum<=2)
							{
								Clearbuf[i_xd] = 0;
							}
							else
							{
							    dd = 1;
								usleep(600*1000);
								goto N_APCUnpack;
							}
						}
					}
					dd = 0;
					ClearComBuf(p_combuf);
				}
				else
				{
					DEBUGOUT("OMCCom APC Unpacked Error!\r\n");//��������ݰ���ʶ
					ClearComBuf(p_combuf);
				}
			}
			//����������ӡ���¼����
			if (p_combuf->Status == NET_CONNET)
			{
				if (g_OMCLoginFlag == 1)
				{
					g_OMCLoginFlag = 0;
					p_combuf->Status = LOGINSUCCESS;
					DEBUGOUT("Login ok!!!!!!!!!!!!\r\n");
				}
				//��¼�������
				else if (p_combuf->Timer > LOGININTERVAL)
				{
					starttime = time(NULL);
					//if (loginsum < (LOGINFAILSUM+1))
					loginsum++;
					if (loginsum == LOGINFAILSUM)//��PS���¼ʧ�ܡ��ϱ���һ���Ե��ϱ������ϱ�ʧ�ܺ�,�豸����Ҫ�Զ��ط�
					{
						SocketClientDisconnet(p_combuf);
						//          	if ((p_devpara->ComModemType == NET8023) && (p_devpara->PSTranProtocol == IP_UDP_MODE))//0x000B:802.3����
						//            	goto _PSLOGIN_FAIL;
						if ((p_devpara->ComModemType == GSM_MODEM) && (p_devpara->PSTranProtocol == IP_UDP_MODE))//modem,ģʽUDP
							//else if (p_devpara->PSTranProtocol == IP_TCP_MODE)//modem��ģʽtcp
						{
							goto _PSLOGIN_FAIL;
						}
					}
					else if (loginsum == (LOGINFAILSUM+2))
					{
						loginsum = LOGINFAILSUM; 
						g_SmsCom.Status = GPRS_SMS;
					}
					else
					{
						//��¼����������ϱ�
						memset(p_devinfo, 0, sizeof(DevInfo_t));
						p_devinfo->StationNo = p_devpara->StationNo;
						p_devinfo->DeviceNo = p_devpara->DeviceNo;
						ReportParaPack(AP_C, p_devinfo, REPORT_LOGIN_OMC, g_OMCPackNo++, p_packbuf);
						goto _SEND_AP_PACK_OMC;
					}
				}
			}
			//��¼������ĳɹ�
			else if (p_combuf->Status == LOGINSUCCESS)
			{
				//loginsum = 0;
				//        if (g_Ir_OMCTranCom.RecvLen > 0)//IRЭ��ת��͸��
				//        {
				//          DEBUGOUT("Ir->OMC TransimtPack!\r\n");
				//          ApPackTransimtUnPack(p_packbuf, &g_Ir_OMCTranCom);
				//          goto _SEND_AP_PACK_OMC;
				//        }
				if (rs485_omc_combuf.RecvLen > 0)//RS485ͨѶ͸��
				{
					DEBUGOUT("RS485->OMC TransimtPack:%d\r\n", rs485_omc_combuf.RecvLen);
					ComDataHexDis(rs485_omc_combuf.Buf, rs485_omc_combuf.RecvLen);
					ApPackTransimtUnPack(p_packbuf, &rs485_omc_combuf);
					goto _SEND_AP_PACK_OMC;
				}
				//����������ж�,������Ĳ����豸����Ӧ��ʱ���豸Ĭ��Ϊ30s
				if (g_OMCHeartBeatFlag >= 1)
				{
					if (p_combuf->Timer > hearttime)
						hearttime = 30;
				}
				else
				{
					hearttime = p_devpara->HeartBeatTime;
				}

				if (p_combuf->Timer > hearttime)
				{
					if (p_devpara->HeartBeatTime != 0)//����Ϊ0ʱ,������
					{
						g_OMCHeartBeatFlag++;//������ʶ
						starttime = time(NULL);
						//3�������޷�Ӧ,�Ͽ�����
						if (g_OMCHeartBeatFlag > NOHEARTBEATSUM)
						{
							//SHUT_RDWR:�൱�ڵ���shutdown���Σ���������SHUT_RD,Ȼ����SHUT_WR
							shutdown(p_combuf->Fd, SHUT_RDWR);//close(p_combuf->Fd);
							g_OMCHeartBeatFlag = 0;
							//connetsum = 0;//��������
							SocketClientDisconnet(p_combuf);
							DEBUGOUT("�����޷���,����������ӶϿ�!\r\n");
						}
						else
						{
							//�������ϱ�
							memset(p_devinfo, 0, sizeof(DevInfo_t));
							p_devinfo->StationNo = p_devpara->StationNo;
							p_devinfo->DeviceNo = p_devpara->DeviceNo;
							ReportParaPack(AP_C, p_devinfo, REPORT_HEART_BEAT, g_OMCPackNo++, p_packbuf);
							HeartBeatFlag = 1;
_SEND_AP_PACK_OMC:
							p_combuf->Timer = 0;
							starttime = time(NULL);

							if (p_devpara->PSTranProtocol == IP_TCP_MODE)
							{
								DEBUGOUT("TCPSocketSendPack................................................................\r\n");
								TCPSocketSendPack(p_combuf, p_packbuf);
							}
							else if (p_devpara->PSTranProtocol == IP_UDP_MODE)
							{
								if(HeartBeatFlag==1)
								{
									HeartBeatFlag = 0;
									DEBUGOUT("UDPSocketSendPack................................................................\r\n");
									UDPSocketSendPack(p_combuf, p_packbuf, p_devpara->OmcIP, p_devpara->OmcIPPort);
								}
								else
								{
									DEBUGOUT("UDPSocketSendPack................................................................\r\n");
									UDPSocketSendPack(p_combuf, p_packbuf, GetReceiveIP(), p_devpara->OmcIPPort);
								}
							}
						}
					}
					else
					{
						g_OMCHeartBeatFlag = 0;
					}
				}
				if (g_MCPFlag == MCP_C)//MCP_B�������ϱ�
				{
					if (SWUpdateData.SWRunMode == SW_UPDATE_MODE)
					{
						DEBUGOUT("���������ϱ�.................!\r\n");
						SWUpdateData.SWRunMode = SW_MONITOR_MODE;
						SaveUpdatePara();
						memset(p_devinfo, 0, sizeof(DevInfo_t));
						p_devinfo->StationNo = p_devpara->StationNo;
						p_devinfo->DeviceNo = p_devpara->DeviceNo;
						ReportParaPack(AP_C, p_devinfo, REPORT_SW_UPDATE, g_OMCPackNo++, p_packbuf);
						goto _SEND_AP_PACK_OMC;
					}
				}
			}
		}
		else if (p_combuf->Status == NET_NULL)
		{
			if (p_devpara->ComModemType == NET8023) //0x000B:802.3����
			{
				SocketClientDisconnet(p_combuf);
			}
			//gprs��modem�����в�����ɺ�����Ӧ��״̬
		}
		else
		{
			//connetsum = 0;//��������
			ClearComBuf(p_combuf);
			starttime = time(NULL)-NETCONNETINTERVAL+3;
			SocketClientDisconnet(p_combuf);
		}

		}
		g_OMCComThread.ThreadStatus = THREAD_STATUS_EXIT;
		g_OMCComThread.Tid = 0;
		pthread_exit(NULL);
}

/*******************************************************************************
*�������� : void *SlaveOMCCom_Thread(void *pvoid)
*��    �� : ��չ��Զ��OMCͨѶ�߳�
*������� : none
*������� : none
*******************************************************************************/ 
extern int g_stop_alarm;
void *SlaveOMCCom_Thread(void *pvoid)
{
	DevicePara_t *p_devpara;
	APPack_t PackBuf, *p_packbuf;
	DevInfo_t DevInfo, *p_devinfo;
	time_t alarmreporttime;
	time_t current_time;
	unsigned int next_time;

	pthread_detach(pthread_self());
	pthread_setcancelstate(PTHREAD_CANCEL_ENABLE, NULL);
	pthread_setcanceltype(PTHREAD_CANCEL_ASYNCHRONOUS, NULL);//�߳�����Ϊ����ȡ��
	OMCComInit();

	alarmreporttime = time(NULL);
	printf("SlaveOMCCom_Thread Run!\r\n");
	g_OMCPackNo = 0x8000;
	g_MCPFlag = MCP_C;
	LoadUpdatePara();	//�豸�����������ģʽ,����ʱ��һ���ļ����ݿ����
	sleep(20); 			//�ȴ�����Ԫ��ʼ����� IR
	
	while(1)
	{
	    usleep(10000);
	    OMCDevParaDeal();
	    UpdateModeApp();//MCP_B�������Ƴ���
	    p_devpara = &g_DevicePara;
	    p_packbuf = &PackBuf;
	    p_devinfo = &DevInfo;

		if (g_Ir_OMCTranCom.RecvLen > 0)	//IRЭ��ת��͸��
		{
			ApPackTransimtUnPack(p_packbuf, &g_Ir_OMCTranCom);
			//����,�з���
			GetDevInfo(p_devinfo, p_packbuf);
			printf("buf deviceNO=%d, devpara deviceNO=%d\n", p_devinfo->DeviceNo, p_devpara->DeviceNo);
			if (p_devinfo->DeviceNo == p_devpara->DeviceNo)//�Ǳ���Ԫ����
			{
				if (p_devinfo->ModuleType != 0x00)//͸��,תRS485ͨѶ
				{
					ApPackTransimtPack(p_packbuf, &omc_rs485_combuf);
				}
				else
				{
					if (APProcess(p_packbuf, p_devpara) > 0)
					{
						if (p_packbuf->PackLen > 0)
						{
							ApPackTransimtPack(p_packbuf, &g_OMC_IrTranCom);
						}
					}
				}
			}
		}
		
		if(rs485_omc_combuf.RecvLen > 0)//RS485ͨѶ͸��
		{
			ApPackTransimtUnPack(p_packbuf, &rs485_omc_combuf);
			ApPackTransimtPack(p_packbuf, &g_OMC_IrTranCom);
		}
		
		if ((time(NULL)-alarmreporttime) > 2)// ÿ3��ִ��һ��
		{
			alarmreporttime = time(NULL);
			m_alarm_report_cnt++;
			if((m_alarm_report_cnt > g_alarm_report_time)||((alarmreporttime-current_time) > next_time))
			{
				m_alarm_report_cnt = 0;
				memset(p_devinfo, 0, sizeof(DevInfo_t));
				p_devinfo->StationNo = p_devpara->StationNo;//���豸վ����
				p_devinfo->DeviceNo = p_devpara->DeviceNo;//���豸�豸���
				if (ReportParaPack(AP_C, p_devinfo, REPORT_ALARM, g_OMCPackNo++, p_packbuf) > 0)
				{
					g_alarm_report_cnt++;
					printf("send alarm!!!\n\r\n\r\n\r\n\r\n");
					if((g_alarm_report_cnt == 3)||(g_alarm_report_cnt == 6))
					{
						g_alarm_report_time = 3600;
						current_time = time(NULL);
						next_time = 3*60*60;
					}
					else if(g_alarm_report_cnt == 9)
					{
						g_alarm_report_time = 0xffffffff;
						current_time = time(NULL);
						next_time = 0xffffffff;
					}
					else
					{
						g_alarm_report_time = 60;
						current_time = time(NULL);
						next_time = 0xffffffff;
					}
					ApPackTransimtPack(p_packbuf, &g_OMC_IrTranCom);
				}
			}
		}
		if (m_report_state != 0)
		{
			//��Ϊ����ĸ����ϱ�,�翪վ\�ָ���
			memset(p_devinfo, 0, sizeof(DevInfo_t));
			p_devinfo->StationNo = p_devpara->StationNo;
			p_devinfo->DeviceNo = p_devpara->DeviceNo;
			ReportParaPack(AP_C, p_devinfo, m_report_state, g_OMCPackNo++, p_packbuf);
	        ApPackTransimtPack(p_packbuf, &g_OMC_IrTranCom);
			m_report_state = 0;
		}
		if (g_MCPFlag == MCP_C)//MCP_B�������ϱ�
		{
			if (SWUpdateData.SWRunMode == SW_UPDATE_MODE)
			{
				DEBUGOUT("���������ϱ�.................!\r\n");
				sleep(10);//�ȴ�Ir���ӳɹ�
				SWUpdateData.SWRunMode = SW_MONITOR_MODE;
				SaveUpdatePara();
				memset(p_devinfo, 0, sizeof(DevInfo_t));
				p_devinfo->StationNo = p_devpara->StationNo;
				p_devinfo->DeviceNo = p_devpara->DeviceNo;
				ReportParaPack(AP_C, p_devinfo, REPORT_SW_UPDATE, g_OMCPackNo++, p_packbuf);
				ApPackTransimtPack(p_packbuf, &g_OMC_IrTranCom);
			}
		}
	}
	
	g_OMCComThread.ThreadStatus = THREAD_STATUS_EXIT;
	g_OMCComThread.Tid = 0;
	pthread_exit(NULL);
}
// ��ȡ�豸·�ɵǼǵ�ַ,������Ҫ��ѯ�˲���
void get_dev_route_addr(void)
{
	char buf[30];
	unsigned short para1;
	unsigned char tmp;
	unsigned char tmp1, tmp2;

	memset(buf, 0, sizeof(buf));
	drv_read_fpga((unsigned int)DEVIDREGL_ADDR, &para1);//�豸������Ÿ��ֽ�
	tmp = (para1&0xff);
	if(tmp&0x08){
		tmp1 = (tmp&0x7)+0x01;
	}else{
		tmp1 = 0;
	}
	if(tmp&0x80){
		tmp2 = (tmp&0x70)+0x10;
	}else{
		tmp2 = 0;
	}
	tmp = tmp1|tmp2;
	buf[0] = ((tmp>>4)|(tmp<<4));
	tmp = ((para1>>8)&0xff);
	if(tmp&0x08){
		tmp1 = (tmp&0x7)+0x01;
	}else{
		tmp1 = 0;
	}
	if(tmp&0x80){
		tmp2 = (tmp&0x70)+0x10;
	}else{
		tmp2 = 0;
	}
	tmp = tmp1|tmp2;
	buf[1] = ((tmp>>4)|(tmp<<4));
	drv_read_fpga((unsigned int)(DEVIDREGH_ADDR), &para1);//�豸������ŵ��ֽ�
	tmp = para1&0xff;
	if(tmp&0x08){
		tmp1 = (tmp&0x7)+0x01;
	}else{
		tmp1 = 0;
	}
	if(tmp&0x80){
		tmp2 = (tmp&0x70)+0x10;
	}else{
		tmp2 = 0;
	}
	tmp = tmp1|tmp2;
	buf[2] = ((tmp>>4)|(tmp<<4));
	tmp = ((para1>>8)&0xff);
	if(tmp&0x08){
		tmp1 = (tmp&0x7)+0x01;
	}else{
		tmp1 = 0;
	}
	if(tmp&0x80){
		tmp2 = (tmp&0x70)+0x10;
	}else{
		tmp2 = 0;
	}
	tmp = tmp1|tmp2;
	buf[3] = ((tmp>>4)|(tmp<<4));
	printk(buf, 4);
	DbSaveThisStrPara(DEVROUTE_ID, buf);
}
/*******************************************************************************
*�������� : void OMCReadHWInfo(void)
*��    �� : OMC��ȡӲ���汾��ϢͨѶ���ú���
*������� : None
*������� : None
*******************************************************************************/
void OMCReadHWInfo(void)
{
char *pbuf, buf[200];
unsigned short para1, para2;
FILE  *fp = NULL;
unsigned char tmp;

  //�����汾��Ϣ
  memset(buf, 0, sizeof(buf));
  if (g_DevType == MAIN_UNIT)
  {
    sprintf(buf, "MU-%s-%s.%s.%s%s", g_type_ver, g_comm_ver, g_drv_ver, g_comm_test_ver, g_test_ver);
    fp = fopen(SOFT_VERSION, "r");
  	if(fp != NULL) //��Ҫ�򿪵��ļ�������,���½���
  	{
			rewind(fp);//ָ���ļ�ͷ
  		fgets(buf, sizeof(buf), fp);//���ж�ȡ�ļ�
  		fgets(buf, sizeof(buf), fp);//���ж�ȡ�ļ�
  		memset(buf, 0, sizeof(buf));
  		fgets(buf, sizeof(buf), fp);//������Ϊ�汾
			DEBUGOUT("����Ԫ:�����汾:%s...................\r\n", buf);
  	}
  }
  else if (g_DevType == EXPAND_UNIT)
  {
    sprintf(buf, "EU-%s-%s.%s.%s%s", g_type_ver, g_comm_ver, g_drv_ver, g_comm_test_ver, g_test_ver);
    fp = fopen(SOFT_VERSION, "r");
  	if(fp != NULL) //��Ҫ�򿪵��ļ�������,���½���
  	{
			rewind(fp);//ָ���ļ�ͷ
  		fgets(buf, sizeof(buf), fp);//���ж�ȡ�ļ�
  		fgets(buf, sizeof(buf), fp);//���ж�ȡ�ļ�
  		memset(buf, 0, sizeof(buf));
  		fgets(buf, sizeof(buf), fp);//������Ϊ�汾
  		DEBUGOUT("��չ��Ԫ:�����汾:%s...................\r\n", buf);
  	}
  }
  else
  {
    sprintf(buf, "RU-%s-%s.%s.%s%s", g_type_ver, g_comm_ver, g_drv_ver, g_comm_test_ver, g_test_ver);
  }
  DbSaveThisStrPara(SWRUNVER_ID, buf);

  //CPLD�汾��Ϣ
  memset(buf, 0, sizeof(buf));
  drv_read_epld((unsigned int)CPLDVERSION_ADDR, &para1);//CPLD�汾��
  sprintf(buf, "CPLD V%04X", para1);
  DbSaveThisStrPara(CPLDVER_ID, buf);
  
  //FPGA�汾��Ϣ
  memset(buf, 0, sizeof(buf));
  drv_read_fpga((unsigned int)FPGATOPVERSION_ADDR, &para1);//FPGA���汾��
  drv_read_fpga((unsigned int)FPGAVERMARK_ADDR, &para2);//�汾��ʾ˵��
  sprintf(buf, "FPGA V%04X.%04X", para1, para2);
  DbSaveThisStrPara(FPGAVER_ID, buf);
 
  //�豸·�ɵǼǵ�ַ
  /*
  memset(buf, 0, sizeof(buf));
  drv_read_fpga((unsigned int)DEVIDREGL_ADDR, &para1);//�豸������Ÿ��ֽ�
  tmp = (para1&0x77);
  buf[0] = ((tmp>>4)|(tmp<<4))+0x11;
  tmp = ((para1>>8)&0x77);
  buf[1] = ((tmp>>4)|(tmp<<4))+0x11;
  drv_read_fpga((unsigned int)(DEVIDREGH_ADDR), &para1);//�豸������ŵ��ֽ�
  tmp = para1&0x77;
  buf[2] = ((tmp>>4)|(tmp<<4))+0x11;
  tmp = ((para1>>8)&0x77);
  buf[3] = ((tmp>>4)|(tmp<<4))+0x11;
  DbSaveThisStrPara(DEVROUTE_ID, buf);
 */ 
  //MAC��ַ
  memset(buf, 0, sizeof(buf));
  GetSelfMac("eth0", &buf[21]);//MAC��ַ
  for (para1 = 0; para1 < 6; para1++)
  {
    ByteToAscii(buf[21+para1], &buf[para1*3]);
    if (para1 < 5)
     buf[para1*3+2] = ':';
  }
  DbSaveThisStrPara(DEVMAC_ID, buf);
  //��ȡFPGAĬ�ϲ���
  /*drv_read_fpga((unsigned int)LTE1VGA_ADDR, &para1);
  DbSaveThisIntPara(LTE1VGA_ID, para1);//LTE1 VGA����
  drv_read_fpga((unsigned int)LTE2VGA_ADDR, &para1);
  DbSaveThisIntPara(LTE2VGA_ID, para1);//LTE2 VGA����
  drv_read_fpga((unsigned int)G3VGA_ADDR, &para1);
  DbSaveThisIntPara(G3VGA_ID, para1);//3G VGA����
  drv_read_fpga((unsigned int)GSMVGA_ADDR, &para1);
  DbSaveThisIntPara(GSMVGA_ID, para1);//GSM VGA����
  drv_read_fpga((unsigned int)LTE1DATAGAIN_ADDR, &para1);
  DbSaveThisIntPara(LTE1DATAGAIN_ID, para1);//LTE1 ������������
  drv_read_fpga((unsigned int)LTE2DATAGAIN_ADDR, &para1);
  DbSaveThisIntPara(LTE2DATAGAIN_ID, para1);//LTE2 ������������
  drv_read_fpga((unsigned int)G3DATAGAIN_ADDR, &para1);
  DbSaveThisIntPara(G3DATAGAIN_ID, para1);//3G ������������
  drv_read_fpga((unsigned int)GSMDATAGAIN_ADDR, &para1);
  DbSaveThisIntPara(GSMDATAGAIN_ID, para1);//GSM ������������
  drv_read_fpga((unsigned int)GSMCARRIERMASK_ADDR, &para1);
  DbSaveThisIntPara(GSMCARRIERMASK_ID, para1);//�ز����μĴ�������
  drv_read_fpga((unsigned int)TESTMODELSWITCH_ADDR, &para1);
  DbSaveThisIntPara(TESTMODELSWITCH_ID, para1);//����ģʽ������ģʽ�л�
  drv_read_fpga((unsigned int)INTERTESTEN_ADDR, &para1);
  DbSaveThisIntPara(INTERTESTEN_ID, para1);//�ڲ�����Դʹ��
  drv_read_fpga((unsigned int)INTEFREQRSRC_ADDR, &para1);
  DbSaveThisIntPara(INTEFREQRSRC_ID, para1);//�ڲ�ԴƵ������
  drv_read_fpga((unsigned int)AGCREFERENCE_ADDR, &para1);
  para1 = para1 & 0x1FF;
  DbSaveThisIntPara(AGCREFERENCE_ID, para1);//AGC �ο�ֵ����
  */
  drv_read_fpga((unsigned int)AGCSTEP_ADDR, &para1);
  para1 = para1 & 0x03;
  DbSaveThisIntPara(AGCSTEP_ID, para1);//AGC ��������
}

/*******************************************************************************
*�������� : int OMCComInit(void)
*��    �� : OMCͨѶ���ú���
*������� : None
*������� : Fd
*******************************************************************************/
void OMCComInit(void)
{
  g_OMCCom.Fd = -1;
  g_OMCCom.RecvLen = 0;
  g_OMCCom.Status = NET_NULL;
  memset(g_OMCCom.Buf, 0, COMBUF_SIZE);
  g_OMCHeartBeatFlag = 0;
  g_OMCSetParaFlag = 1;
  g_OMCSetParaRs485Flag = 1;
  m_report_state = 0;
  g_AlarmDectTime = time(NULL);
  OMCReadHWInfo();
  SocketClientDisconnet(&g_OMCCom);
  OMCComThreadInit();
}

/*******************************************************************************
*�������� : void OMCComThreadInit(void)
*��    �� : OMCͨѶ�̳߳�ʼ��
*������� : none
*������� : none
*******************************************************************************/ 
void OMCComThreadInit(void)
{
  g_OMCComThread.Tid = 0;
  g_OMCComThread.ThreadStatus = THREAD_STATUS_EXIT;
}

/*******************************************************************************
*�������� : void OMCComThreadStart(void)
*��    �� : ��ʼOMCͨѶ�߳�
*������� : none
*������� : none
*******************************************************************************/ 
void OMCComThreadStart(void)
{
  if (g_OMCComThread.ThreadStatus != THREAD_STATUS_RUNNING)
  {
    if (g_DevType == MAIN_UNIT)
      pthread_create(&g_OMCComThread.Tid, NULL, OMCCom_Thread, NULL);
    else
      //pthread_create(&g_OMCComThread.Tid, NULL, OMCCom_Thread, NULL);
      pthread_create(&g_OMCComThread.Tid, NULL, SlaveOMCCom_Thread, NULL);
    g_OMCComThread.ThreadStatus = THREAD_STATUS_RUNNING;
    printf("OMCCom_Thread ID: %lu.\n", g_OMCComThread.Tid);
  }
}

/*******************************************************************************
*�������� : void OMCComThreadStop(void)
*��    �� : ֹͣOMCͨѶ�߳�
*������� : none
*������� : none
*******************************************************************************/ 
void OMCComThreadStop(void)
{
  if (g_OMCComThread.ThreadStatus != THREAD_STATUS_EXIT)
  {
    pthread_cancel(g_OMCComThread.Tid);
    g_OMCComThread.Tid = 0;
    g_OMCComThread.ThreadStatus = THREAD_STATUS_EXIT;
    printf("OMCCom_Thread Stop!\r\n");
  }
}

/*******************************************************************************
*�������� : void SaveAlarmVal(INT16U objectid, int val)
*��    �� : �洢�澯ֵ
*������� : INT16U objectid:�澯ID;int val:�ź�
*������� : none
*******************************************************************************/ 
void SaveAlarmVal(unsigned int objectid, int val)
{
int alen, al;

  //���澯ʹ��״̬
  DbGetThisIntPara((objectid-0x0100), &alen);
  if (alen == 0x01)
  {
  	if((val & 0x01) == 0x01)
  	{
	    DbGetThisIntPara(objectid, &al);
	    if((al & 0x01) == 0)//���γ��ָ澯
	    {
        al = al | val;
        DbSaveThisIntPara(objectid, al);
     	}
	  }
	  else
	  {
	    DbGetThisIntPara(objectid, &al);
	    if ((al & 0x01) == 0x01)
	    {
	      al = al & (~0x01);
	      DbSaveThisIntPara(objectid, al);
	    }
	  }
	}
	else if(alen == 0)
	{
		DbGetThisIntPara(objectid, &al);
		if (al != 0)
		{
			DbSaveThisIntPara(objectid, 0);
		}
	}
}

/*******************************************************************************
*�������� : int OMCWriteFpga(unsigned int addr, INT16U data)
*��    �� : OMC���ò���д��FPGA
*������� : unsigned int addr:��ַ,INT16U data:����
*������� : none
*******************************************************************************/ 
int OMCWriteFpga(unsigned int addr, int data)
{
INT16U para1;
  
  drv_read_fpga(addr, &para1);
  if (para1 != (INT16U)data)
  {
    para1 = (INT16U)data;
    drv_write_fpga(addr, para1);
    //�����ش���VGA��������(д010)
    if ((addr == LTE1VGA_ADDR) || (addr == LTE2VGA_ADDR)
      ||(addr == G3VGA_ADDR) || (addr == GSMVGA_ADDR))
    {
      drv_write_fpga(AGCEN_ADDR, 0);
      usleep(1);
      drv_write_fpga(AGCEN_ADDR, 1);
      usleep(1);
      drv_write_fpga(AGCEN_ADDR, 0);
    }
    return 1;
  }
  return -2;
}

/*******************************************************************************
*�������� : int PATCGain(INT16U id)
*��    �� : �����¶Ȳ���
*������� : INT16U id:����ID��
*������� : ���ز�������(ϵ��10)
*******************************************************************************/ 
int PATCGain(INT16U id)
{
  return 0;
}

/*******************************************************************************
*�������� : int GetVGAVal(INT16U id, int *pval)
*��    �� : ��ȡVGA����
*������� : INT16U id:����ID��;int *pval:��������ֵָ��
*������� : ���³ɹ�����1,δ���³ɹ�����-1
*******************************************************************************/ 
int GetVGAVal(unsigned int id, int *pval)
{
unsigned int gainid,attid;
int val, data;

  if (g_DevType == MAIN_UNIT)
  {
    switch(id)
    {
      case GSMVGA_ID://GSM
        gainid = GSMDLGAINOFFSET_ID;
		if(get_net_group() == 0x331){ // ����
			attid = CDMADLATT_ID;
		}else{
			attid = GSMDLATT_ID;
		}
      break;
      case G3VGA_ID://3G
        gainid = G3DLGAINOFFSET_ID;
        attid = G3DLATT_ID;
      break;
      case LTE1VGA_ID://LTE1
        gainid = LTE1DLGAINOFFSET_ID;
		if(get_net_group() == 0x331){ // ����
			attid = FDD1DLATT_ID;
		}else{
			attid = LTE1DLATT_ID;
		}
      break;
      case LTE2VGA_ID://LTE2
        gainid = LTE2DLGAINOFFSET_ID;
		if(get_net_group() == 0x331){ // ����
			attid = FDD2DLATT_ID;
		}else{
			attid = LTE2DLATT_ID;
		}
      break;
      default:
        return -1;
      break;
    }
    if (DbGetThisIntPara(id, &val) == 1)
    {
      DbGetThisIntPara(gainid, &data);
      val = val+data;
      DbGetThisIntPara(attid, &data);//ϵ��2
      val = val+data*2;
      if (val > VGAMAXSETVAL)//6bit(63)
        val = VGAMAXSETVAL;
      *pval = val;
      return 1;
    }
    else
    {
      return -1;
    }
  }
  else if (g_DevType == EXPAND_UNIT)
  {
    if (DbGetThisIntPara(id, &val) == 1)
    {
      *pval = val;
      if (val > VGAMAXSETVAL)//6bit(63)
        val = VGAMAXSETVAL;
      return 1;
    }
    else
    {
      return -1;
    }
  }
  else if (g_DevType == RAU_UNIT)
  {
    switch(id)
    {
      case GSMVGA_ID://GSM
        gainid = GSMULGAINOFFSET_ID;
        attid = GSMULATT_ID;
      break;
      case G3VGA_ID://3G
        gainid = G3ULGAINOFFSET_ID;
        attid = G3ULATT_ID;
      break;
      case LTE1VGA_ID://LTE1
        gainid = LTE1ULGAINOFFSET_ID;
        attid = LTE1ULATT_ID;
      break;
      case LTE2VGA_ID://LTE2
        gainid = LTE2ULGAINOFFSET_ID;
        attid = LTE2ULATT_ID;
      break;
      default:
        return -1;
      break;
    }
    if (DbGetThisIntPara(id, &val) == 1)
    {
      DbGetThisIntPara(gainid, &data);
      val = val+data;
      DbGetThisIntPara(attid, &data);//ϵ��2
      val = val+data*2;
      if (val > VGAMAXSETVAL)//6bit(63)
        val = VGAMAXSETVAL;
      *pval = val;
      return 1;
    }
    else
    {
      return -1;
    }
  }
  else
  {
    return -1;
  }
}

/*******************************************************************************
*�������� : int GetDataGainVal(INT16U id, int *pval)
*��    �� : ��ȡ�����������
*������� : INT16U id:����ID��;int *pval:��������ֵָ��
*������� : ���³ɹ�����1,δ���³ɹ�����-1
*******************************************************************************/ 
int GetDataGainVal(unsigned int id, int *pval)
{
unsigned int gainid,attid;
int val, data;

  if (g_DevType == MAIN_UNIT)
  {
    switch(id)
    {
      case GSMDATAGAIN_ID://GSM
        gainid = GSMULGAINOFFSET_ID;
		if(get_net_group() == 0x331){ // ����
			attid = CDMAULATT_ID;
		}else{
			attid = GSMULATT_ID;
		}
      break;
      case G3DATAGAIN_ID://3G
        gainid = G3ULGAINOFFSET_ID;
        attid = G3ULATT_ID;
      break;
      case LTE1DATAGAIN_ID://LTE1
        gainid = LTE1ULGAINOFFSET_ID;
		if(get_net_group() == 0x331){ // ����
			attid = FDD1ULATT_ID;
		}else{
			attid = LTE1ULATT_ID;
		}
      break;
      case LTE2DATAGAIN_ID://LTE2
        gainid = LTE2ULGAINOFFSET_ID;
		if(get_net_group() == 0x331){ // ����
			attid = FDD2ULATT_ID;
		}else{
			attid = LTE2ULATT_ID;
		}
		break;
	  default:
		return -1;
      break;
    }
    if (DbGetThisIntPara(id, &val) == 1)
    {
      DbGetThisIntPara(gainid, &data);//ϵ��2
      val = val-data*5;
      DbGetThisIntPara(attid, &data);//ϵ��1
      val = val-data*10;
      *pval = val;
      return 1;
    }
    else
    {
      return -1;
    }
  }
  else if (g_DevType == EXPAND_UNIT)
  {
    if (DbGetThisIntPara(id, &val) == 1)
    {
      *pval = val;
      return 1;
    }
    else
    {
      return -1;
    }
  }
  else if (g_DevType == RAU_UNIT)
  {
    switch(id)
    {
      case GSMDATAGAIN_ID://GSM
        gainid = GSMDLGAINOFFSET_ID;
        attid = GSMDLATT_ID;
      break;
      case G3DATAGAIN_ID://3G
        gainid = G3DLGAINOFFSET_ID;
        attid = G3DLATT_ID;
      break;
      case LTE1DATAGAIN_ID://LTE1
        gainid = LTE1DLGAINOFFSET_ID;
        attid = LTE1DLATT_ID;
      break;
      case LTE2DATAGAIN_ID://LTE2
        gainid = LTE2DLGAINOFFSET_ID;
        attid = LTE2DLATT_ID;
      break;
      default:
        return -1;
      break;
    }
    if (DbGetThisIntPara(id, &val) == 1)
    {
      DbGetThisIntPara(gainid, &data);//ϵ��2
      val = val-data*5;
      DbGetThisIntPara(attid, &data);//ϵ��1
      val = val-data*10;
      val = val-PATCGain(id);
      *pval = val;
      return 1;
    }
    else
    {
      return -1;
    }
  }
  else
  {
    return -1;
  }
}

/*******************************************************************************
*�������� : void OMCDevParaDeal(void)
*��    �� : �豸�澯��������,����3min���и澯�ϱ�
*������� : none
*������� : none
*******************************************************************************/ 
void _OMCDevParaDeal(void)
{
int i, val, portsum;
unsigned short para2;
DevInfo_t devinfo;

  //�в�������
  if (g_OMCSetParaFlag==1)
  {
    g_OMCSetParaFlag = 0;
  	memset(&devinfo, 0, sizeof(DevInfo_t));
  	LoadDevicePara(&devinfo, &g_DevicePara);

    //GSM VGA����
    if (GetVGAVal(GSMVGA_ID, &val) == 1)
    {
      OMCWriteFpga(GSMVGA_ADDR, val);
    }
    //3G VGA����
    if (GetVGAVal(G3VGA_ID, &val) == 1)
    {
      OMCWriteFpga(G3VGA_ADDR, val);
    }
    //LTE1 VGA����
    if (GetVGAVal(LTE1VGA_ID, &val) == 1)
    {
      OMCWriteFpga(LTE1VGA_ADDR, val);
    }
    //LTE2 VGA����
    if (GetVGAVal(LTE2VGA_ID, &val) == 1)
    {
      OMCWriteFpga(LTE2VGA_ADDR, val);
    }
    
    if (GetDataGainVal(GSMDATAGAIN_ID, &val) == 1)//GSM ������������
    {
      val = (INT16U)(pow(10.0, (double)val/200) * 8192 + 0.5);
      OMCWriteFpga(GSMDATAGAIN_ADDR, val);
    }
    if (GetDataGainVal(G3DATAGAIN_ID, &val) == 1)//3G ������������
    {
      val = (INT16U)(pow(10.0, (double)val/200) * 8192 + 0.5);
      OMCWriteFpga(G3DATAGAIN_ADDR, val);
    }
    if (GetDataGainVal(LTE1DATAGAIN_ID, &val) == 1)//LTE1 ������������
    {
      val = (INT16U)(pow(10.0, (double)val/200) * 8192 + 0.5);
      OMCWriteFpga(LTE1DATAGAIN_ADDR, val);
    }
    if (GetDataGainVal(LTE2DATAGAIN_ID, &val) == 1)//LTE2 ������������
    {
      val = (INT16U)(powf(10.0, (double)val/200) * 8192 + 0.5);
      OMCWriteFpga(LTE2DATAGAIN_ADDR, val);
    }
    //if (DbGetThisIntPara(GSMCARRIERMASK_ID, &val) == 1)//�ز����μĴ�������
     // OMCWriteFpga(GSMCARRIERMASK_ADDR, val);
    if (DbGetThisIntPara(AGCREFERENCE_ID, &val) == 1)//AGC �ο�ֵ����
      OMCWriteFpga(AGCREFERENCE_ADDR, val);
    /*if (DbGetThisIntPara(AGCSTEP_ID, &val) == 1)//AGC ��������
      OMCWriteFpga(AGCSTEP_ADDR, val);*/
    if (DbGetThisIntPara(TESTMODELSWITCH_ID, &val) == 1)//����ģʽ������ģʽ�л�
      drv_write_fpga(TESTMODELSWITCH_ADDR, val);  //�˲���Ϊֻд�Ĵ���
    if (DbGetThisIntPara(INTERTESTEN_ID, &val) == 1)//�ڲ�����Դʹ��
      OMCWriteFpga(INTERTESTEN_ADDR, val);
    if (DbGetThisIntPara(INTEFREQRSRC_ID, &val) == 1)//�ڲ�ԴƵ������
      OMCWriteFpga(INTEFREQRSRC_ADDR, val);
    //�ز�Ƶ������
    if (g_DevType == MAIN_UNIT)
    {
    	//2G Ƶ������
	   	for(i=0; i<16; i++)
	   	{
	   		if (DbGetThisIntPara((WORKINGCHANNELNO_ID+i), &val) == 1)//WORKINGCHANNELNO_ID�ŵ���
	   		{
	   			//����Ԫ����:DDC ����:y=(x-957)/0.04 ��y>=0ʱ,y=y;��y<0ʱ,y=768-y;DUC ����:z=768-y;
	   			if (val < 96)//��Ƶ�� china_mobile gsm
	   			{
	   				val = ((93500+20*val)-94400)/4;
	   			}
				else if ((val<125) && (val > 95))//��Ƶ��china unicom  gsm
	   			{
	   				val = ((93500+20*val)-95700)/4;
	   			}
	   			else if((1000<=val) && (val<=1023))//1000��1023 china mobile e-gsm
	   			{
	   				val = ((93500+20*(val-1024))-94400)/4;
	   			}
				else if((val >= 512) && (val <= 661))//china mobile DCS
				{
	   				val = ((180500+20*(val-511))-181760)/4;
				}
				else if((val >= 662) && (val <= 736))//china unicom DCS
				{
	   				val = ((180500+20*(val-511))-184000)/4;
				}
	   			if (val < 0)
	   			{
	   				val = val*(-1);
	   				val = 768 - val;
	   			}
	   			//����DDC
	   			drv_write_ddc(i, val);
	   			//����DUC
	   			//drv_write_duc(i, (768-val));
				drv_write_duc(i, val);
	   		}
   		}
		//3G Ƶ������
		DbGetThisIntPara(CH2WORKCHANNELNO_ID, &val);
		if((10562<=val) && (val<=10838))  //10562��10838 wcdma
		{
			val = 200*val - 153600;//153.6MHzƵ��ƫ��
			dru_lmx2531_wcdma_config(val);
		}
		else if(((10050<=val) && (val<=10125)) ||  // TD-SCDMA AƵ��
				((9400<=val) && (val<=9600))) // TD-SCDMA FƵ��
		{
			val = 100875;
			val = 20*val - 92160;//92.16MHzƵ��ƫ��
			dru_lmx2531_wcdma_config(val);
		}
		//LTE1 Ƶ������ 
		DbGetThisIntPara(CH3WORKCHANNELNO_ID, &val);
		if((1200<=val) && (val<=1949))//1200~1949 ����Ƶ��33 fdd
		{
			val = 1805000 + 100*(val-1200) - 153600;//153.6MHzƵ��ƫ��
			dru_lmx2531_lte1_config(val);
		}
		else if((2750<=val) && (val<=3449))//1200~1949 ����Ƶ��37 fdd
		{
			val = 2620000 + 100*(val-2750) - 153600;//153.6MHzƵ��ƫ��
			dru_lmx2531_lte1_config(val);
		}
		else if((val >= 37750) && (val<=38249))// ����Ƶ��D tdd
		{
			val = 2570000 + 100*(val-37750) - 92160;//92.16MHzƵ��ƫ��
			dru_lmx2531_lte1_config(val);
		}
		else if((val >= 38250) && (val<=38649))// ����Ƶ��F tdd
		{
			val = 1880000 + 100*(val-38250) - 92160;//92.16MHzƵ��ƫ��
			dru_lmx2531_lte1_config(val);
		}
		else if((val >= 38650) && (val<=39649))// ����Ƶ��E tdd
		{
			val = 2300000 + 100*(val-38650) - 92160;//92.16MHzƵ��ƫ��
			dru_lmx2531_lte1_config(val);
		}
		//LTE2 Ƶ������ 
		DbGetThisIntPara(CH4WORKCHANNELNO_ID, &val);
		if((1200<=val) && (val<=1949))//1200~1949
		{
			val = 1805000 + 100*(val-1200) - 153600;//153.6MHzƵ��ƫ��
			dru_lmx2531_fdd_lte2_config(val);
		}
		else if((2750<=val) && (val<=3449))//1200~1949 ����Ƶ��37
		{
			val = 2620000 + 100*(val-2750) - 153600;//153.6MHzƵ��ƫ��
			dru_lmx2531_fdd_lte2_config(val);
		}
		else if((val >= 37750) && (val<=38249))// ����Ƶ��D tdd
		{
			val = 2570000 + 100*(val-37750) - 92160;//92.16MHzƵ��ƫ��
			dru_lmx2531_fdd_lte2_config(val);
		}
		else if((val >= 38250) && (val<=38649))// ����Ƶ��F tdd
		{
			val = 1880000 + 100*(val-38250) - 92160;//92.16MHzƵ��ƫ��
			dru_lmx2531_fdd_lte2_config(val);
		}
		else if((val >= 38650) && (val<=39649))// ����Ƶ��E tdd
		{
			val = 2300000 + 100*(val-38650) - 92160;//92.16MHzƵ��ƫ��
			dru_lmx2531_fdd_lte2_config(val);
		}
	}
    else if (g_DevType == RAU_UNIT)
   	{
   		//2G Ƶ������
	   	for(i=0; i<16; i++)
	   	{
	   		if (DbGetThisIntPara((WORKINGCHANNELNO_ID+i), &val) == 1)//WORKINGCHANNELNO_ID�ŵ���
	   		{
	   			//����Ԫ����:DDC ����:y=(x-957)/0.04 ��y>=0ʱ,y=y;��y<0ʱ,y=768-y;DUC ����:z=768-y;
				if (val<96)// china mobile
				{
					val = (89900-(89000+20*val))/4;
				}
				else if ((val<125) && (val > 95))// china unicom
	   			{
	   				val = (91200-(89000+20*val))/4;
	   			}
	   			else if((1000<=val) && (val<=1023))//1000��1023 china mobile e-gsm
	   			{
	   				val = (89900-(89000+20*(val-1024)))/4;
	   			}
				else if((val >= 512) && (val <= 661))// china mobile DCS
				{
					val = (172260-(171000+20*(val-511)))/4;
				}
				else if((val >= 662) && (val <= 885))// china unicom DCS
				{
					val = (174500-(171000+20*(val-511)))/4;
				}
	   			if (val < 0)
	   			{
	   				val = val*(-1);
	   				val = 768 - val;
	   			}
	   			//����DDC
	   			drv_write_ddc(i, val);
	   			//����DUC
	   			//drv_write_duc(i, (768-val));
				drv_write_duc(i,val);
		   	}
   		}
		//3G Ƶ������
		DbGetThisIntPara(CH2WORKCHANNELNO_ID, &val);
		if((10562<=val) && (val<=10838))//10562��10838
		{
			val = 200*val - 190000 - 92160;//92.16MHzƵ��ƫ��
			dru_lmx2531_wcdma_config(val);
		}
		else if(((10050<=val) && (val<=10125)) || // TD-SCDMA AƵ��
				((9400<=val) && (val<=9600))) // TD-SCDMA FƵ��
		{
			val = 100875;
			val = 20*val - 92160;//92.16MHzƵ��ƫ��
			dru_lmx2531_wcdma_config(val);
		}
		//LTE1 Ƶ������ FDD
		DbGetThisIntPara(CH3WORKCHANNELNO_ID, &val);
		if((1200<=val) && (val<=1949))//1200~1949
		{
			val = 1805000 + 100*(val-1200) - 95000 - 92160;//92.16MHzƵ��ƫ��
			dru_lmx2531_lte1_config(val);
		}
		else if((2750<=val) && (val<=3449))//1200~1949
		{
			val = 2620000 + 100*(val-2750) - 95000 - 92160;//92.16MHzƵ��ƫ��
			dru_lmx2531_lte1_config(val);
		}
		else if((val >= 37750) && (val<=38249))// ����Ƶ��D tdd
		{
			val = 2570000 + 100*(val-37750) - 92160;//92.16MHzƵ��ƫ��
			dru_lmx2531_lte1_config(val);
		}
		else if((val >= 38250) && (val<=38649))// ����Ƶ��F tdd
		{
			val = 1880000 + 100*(val-38250) - 92160;//92.16MHzƵ��ƫ��
			dru_lmx2531_lte1_config(val);
		}
		else if((val >= 38650) && (val<=39649))// ����Ƶ��E tdd
		{
			val = 2300000 + 100*(val-38650) - 92160;//92.16MHzƵ��ƫ��
			dru_lmx2531_lte1_config(val);
		}
		//LTE2 Ƶ������ FDD
		DbGetThisIntPara(CH4WORKCHANNELNO_ID, &val);
		if((1200<=val) && (val<=1949))//1200~1949
		{
			val = 1805000 + 100*(val-1200) - 95000 - 92160;//92.16MHzƵ��ƫ��
			dru_lmx2531_fdd_lte2_config(val);
		}
		else if((2750<=val) && (val<=3449))//1200~1949
		{
			val = 2620000 + 100*(val-2750) - 95000 - 92160;//92.16MHzƵ��ƫ��
			dru_lmx2531_fdd_lte2_config(val);
		}
		else if((val >= 37750) && (val<=38249))// ����Ƶ��D tdd
		{
			val = 2570000 + 100*(val-37750) - 92160;//92.16MHzƵ��ƫ��
			dru_lmx2531_fdd_lte2_config(val);
		}
		else if((val >= 38250) && (val<=38649))// ����Ƶ��F tdd
		{
			val = 1880000 + 100*(val-38250) - 92160;//92.16MHzƵ��ƫ��
			dru_lmx2531_fdd_lte2_config(val);
		}
		else if((val >= 38650) && (val<=39649))// ����Ƶ��E tdd
		{
			val = 2300000 + 100*(val-38650) - 92160;//92.16MHzƵ��ƫ��
			dru_lmx2531_fdd_lte2_config(val);
		}
   	}
  }
  if ((time(NULL)-g_AlarmDectTime) > ALARMDECTINTERVAL)//�澯��ѯ��ʱ,���3s
  {
    g_AlarmDectTime = time(NULL);
    //LO״̬
    drv_read_epld(LOSTATUS_ADDR, &para2);//1:��������,0:����澯
    para2 = ~para2;
    for (i = 0; i < 4; i++ )
    {
      val = para2 & 0x01;
      SaveAlarmVal((LOUNLOCKAL_ID+i), val);
      para2 = (para2 >> 1);
    }
    //ʱ��״̬
    drv_read_epld(PLLSTATUS_ADDR, &para2);//1:ʱ������,0:ʱ�Ӹ澯
    para2 = ~para2;
    for (i = 0; i < 2; i++ )
    {
      val = para2 & 0x01;
      SaveAlarmVal((PLLAL_ID+i), val);
      para2 = (para2 >> 1);
    }
    //��ģ����λ���
    drv_read_epld(OPTICDECT_ADDR, &para2);//1:����λ,0:��λ
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

    //LTE1 ���ֹ���
    drv_read_fpga(LTE1DATAPOWER_ADDR, &para2);
    DbSaveThisIntPara(LTE1DATAPOWER_ID, para2);
    //LTE2 ���ֹ���
    drv_read_fpga(LTE2DATAPOWER_ADDR, &para2);
    DbSaveThisIntPara(LTE2DATAPOWER_ID, para2);
    //3G ���ֹ���
    drv_read_fpga(G3DATAPOWER_ADDR, &para2);
    DbSaveThisIntPara(G3DATAPOWER_ID, para2);
    //GSM ���ֹ���
    drv_read_fpga(GSMDATAPOWER_ADDR, &para2);
    DbSaveThisIntPara(GSMDATAPOWER_ID, para2);
    if (g_DevType == MAIN_UNIT)
    {
	    //GSM�������빦�ʵ�ƽ(ϵ��10),-15dBm+([14'h0054]-[14'h0049]+[14'h004d]-[14'h0045])/2
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
	    //3G�������빦�ʵ�ƽ(ϵ��10),-15dBm+([14'h0053]-[14'h0048]+[14'h004c]-[14'h0045])/2
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
	    //LTE1�������빦�ʵ�ƽ(ϵ��10),-15dBm+([14'h0051]-[14'h0046]+[14'h004a]-[14'h0045])/2
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
	    //LTE2�������빦�ʵ�ƽ(ϵ��10),-15dBm+([14'h0052]-[14'h0047]+[14'h004b]-[14'h0045])/2
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
void OMCDevParaDeal(void) __attribute__((weak, alias("_OMCDevParaDeal")));

int report_change(int argc, char * argv[])
{
	unsigned int para1;

	msg_tmp.mtype = MSG_FUN_RESULT;	
	if(argc != 2){
		printf("input para cnt is not 2.\r\n");
		sprintf(msg_tmp.mtext, "input para cnt is not 2.\r\n");
		msg_send(TASK2_MODULE, (char *)&msg_tmp, strlen(msg_tmp.mtext));
		return 1;
	}
	para1 = strtol(argv[1], NULL, 10);
	m_report_state = para1;
	sprintf(msg_tmp.mtext, "m_report_state=%d.\r\n", m_report_state);
	msg_send(TASK2_MODULE, (char *)&msg_tmp, strlen(msg_tmp.mtext));
	return 0;
}

/*********************************End Of File*************************************/