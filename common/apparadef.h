/********************  COPYRIGHT(C) ***************************************
**--------------�ļ���Ϣ--------------------------------------------------------
**��   ��   ��: apparadef.h
**��   ��   ��: �ں�ͼ
**�� ��  �� ��: 
**���򿪷�����:
**��        ��: APxЭ��ʹ�ò����궨��ͷ�ļ�
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
#ifndef _APPARADEF_H
#define _APPARADEF_H

#include "../common/druheader.h"

// �����(AP:Access Protocol)Э������ 
#define AP_A    0x01
#define AP_B    0x02
#define AP_C    0x03

//���ʲ�(VP:Visiting Protocol)����Э������ 
#define VP_A    0x01

// VP�㽻����־ (VP interact flag)
#define VP_INTERACT_NORMAL    0x00  //ͨ�������ִ�н��,��ʾִ������
#define VP_INTERACT_BUSY      0x01  //ͨ�������ִ�н��,��ʾ�豸æ,�޷�������������
#define VP_INTERACT_REQUEST   0x80  //��������������

//��ؿ��Ʋ�(MCP:Monitoring Control Protocol)
#define MCP_A    0x01//Э��������Ԫ�����ݵ�Ԫ���
#define MCP_B    0x02
#define MCP_C    0x03

// �����ʶ(command identifier)
#define COMMAND_REPORT          0x01  //�豸�����ϱ�(�澯����վ����¼��������)
#define COMMAND_QUERY           0x02  //��ѯ����
#define COMMAND_SET             0x03  //��������
#define COMMAND_SW_UPDATE_MOD   0x10  //ת������������ģʽ
#define COMMAND_SWVERISONSWITCH 0x11  //�л���������汾
#define COMMAND_NULL            0xFF  //������(�Զ���)

//Ӧ���ʶ(response flag)
#define RESPONSE_SUCCESS            0x00  //�ɹ�
#define RESPONSE_CONDITIOND_EXCUTE  0x01  //���������ִ��
#define RESPONSE_COMMANDNO_ERR      0x02  //�����Ŵ�
#define RESPONSE_LENGTH_ERR         0x03  //���ȴ�
#define RESPONSE_CRC_ERR            0x04  //CRCУ���
#define RESPONSE_MCPAC_ERR          0x05  //MCPC��MCPЭ��Ӧ��
#define RESPONSE_OTHERS_ERR         0xFE  //��������
#define RESPONSE_COMMAND            0xFF  //����,��ʾ�˰�Ϊ����������,���������Ӧ���
#define RESPONSE_SWVERISONSWITCH_ERR   0xFE  //�����汾�л�ִ��ʧ��

//�豸�ϱ���Ϣ����(Notification)
#define REPORT_NULL             0  //���ϱ�
#define REPORT_ALARM            1  //�澯�ϱ�
#define REPORT_CONFIG_CREATE    2  //��վ�ϱ�
#define REPORT_CHECK            3  //Ѳ���ϱ�
#define REPORT_RESTORE          4  //�����޸��ϱ�
#define REPORT_CONFIG_CHANGE    5  //���ñ���ϱ�
#define REPORT_LOGIN_OMC        6  //��¼����������ϱ�
#define REPORT_HEART_BEAT       7  //�����ϱ�
#define REPORT_SW_UPDATE        8  //�豸������������ϱ�
#define REPORT_PSLOGIN_FAIL     9  //PS���¼ʧ���ϱ�
#define REPORT_SAMPLEJOB_END    10 //���ɽ����ϱ�

//�����Э��A,APA��ʶ����
#define APA_STARTFLAG         '~'   // APAЭ����ʼ��־0x7E
#define APA_ENDFLAG           '~'   // APAЭ�������־0x7E
//�����Э��B,APB��ʶ����
#define APB_STARTFLAG         '!'   // APBЭ����ʼ��־ 
#define APB_ENDFLAG           '!'   // APBЭ�������־ 
//�����Э��C,APC��ʶ����
#define APC_STARTFLAG         '~'   //APCЭ����ʼ��־0x7E
#define APC_ENDFLAG           '~'   //APCЭ�������־0x7E

//APAЭ��ת���ַ���־
#define APA_ESCFLAG           '^'   //APAЭ��ת���ַ���־0x5E
#define APA_ESC0X5E           0x5E  //APAЭ��ת���ַ�0x5E
#define APA_ESC0X7E           0x7E  //APAЭ��ת���ַ�0x7E
//APCЭ��ת���ַ���־
#define APC_ESCFLAG           '^'   //APCЭ��ת���ַ���־0x5E
#define APC_ESC0X5E           0x5E  //APCЭ��ת���ַ�0x5E
#define APC_ESC0X7E           0x7E  //APCЭ��ת���ַ�0x7E

#define MSG_RECEIVE_ERR       0x00  //�����ݽ��մ���,���ݳ����û�н��յ�������־ 
#define MSG_RECEIVE_NOEND     0x01  //���ڽ��հ�����,һ�����ݻ�û�д��� 
#define MSG_RECEIVE_END       0x02  //���һ�θ�ʽ��ȷ�İ����ݽ��� 

#define MSG_REPORT_OK         0x01  //�ϱ��������ȷ����
#define MSG_QUERY_OK          0x02  //��ѯ�������ȷ����
#define MSG_SET_OK            0x03  //�����������ȷ����
#define MSG_SW_UPDATE_MOD_OK  0x10  //ת������������ģʽOK
#define MSG_SW_UPDATE_VER_OK  0x11  //�л���������汾OK

#define MSG_REPORTBACK_OK       0x81  //�ϱ������������ȷ����
#define MSG_QUERYBACK_OK        0x82  //��ѯ�����������ȷ����
#define MSG_SETBACK_OK          0x83  //���÷����������ȷ����
#define MSG_SW_UPDATE_QUERY_OK  0x90  //��ѯ����������OK
#define MSG_SW_UPDATE_OK        0x91  //����������OK

#define MCPVAR_NOID_ERR   			0x10  //1:������ݱ�ʶ�޷�ʶ��
#define MCPVAR_OVER_ERR    			0x20  //2:������ݵ�����ֵ������Χ
#define MCPVAR_CODE_ERR      		0x30  //3:������ݱ�ʶ�������ݵ�ֵ������Ҫ��,��Ҫ���ASCII�뷶Χ
#define MCPVAR_LEN_ERR  				0x40  //4:������ݱ�ʶ�������ݳ��Ȳ�ƥ��
#define MCPVAR_UNDER_ERR            0x50  // 5: �������� ��ʾ--
#define MCPVAR_UP_ERR             0x60  // 6: �������� ��ʾ++
#define MCPVAR_DATA_ERR             0x70  // 7: ��������޷����

//�����й��ƶ�APЭ��:ͨ�Ŵ��������ȼ�
#define MSG_AP_ERR            -1   //APЭ�����ʹ���
#define MSG_CRC_ERR           -2   //CRCУ�����
#define MSG_VP_ERR            -3   //VP���ز����
#define MSG_STATION_ERR       -4   //VPվ���Ŵ���
#define MSG_DEV_ERR           -4   //VP�豸��Ŵ���
#define MSG_VP_INTERACT_ERR   -5   //VP������־����
#define MSG_MCP_ERR           -6   //VP��ļ�ؿ��Ʋ�Э���ʶ
#define MSG_AUTHORIZATION_FAILURE -7   //ͨ�Ű�ȫ����(�绰�����Ȩ)
#define MSG_MCP_RESPONSE_ERR      -8   //MCP�����Ԫ�е�Ӧ���־,����Ϊ0xFFʱӦ�����ð�
#define MSG_MCP_PDU_LEN_ERR       -8   //MCP�����ݵ�Ԫ�еļ�������ܳ���
#define MSG_MCP_COMMAND_ERR       -9   //MCP�����Ԫ�е������ʶ
#define MSG_MCP_DATA RANGE_ERR    -10  //MCP�����ݵ�Ԫ�еļ�������Ƿ��ں��ʷ�Χ��
#define MSG_ESC_ERR    						-11  //ת���д���

#define MSG_REPORT_ERR          -31  //�ϱ��������ȷ��������
#define MSG_QUERY_ERR           -32  //��ѯ�������ȷ��������
#define MSG_SET_ERR             -33  //�����������ȷ��������

#define MSG_PACK_ERR            -100  //���ݰ�����
//ͨѶЭ����,����Ĳ���
//0x000B:Զ������ͨ��ģ��
#define GSM_MODEM               0
#define TD_MODEM                1  
#define TD_GSM_MODEM            2
#define WCDMA_MODEM             3
#define WCDMA_GSM_MODEM         4
#define NET8023                 5
//MCP_A:0x0010,MCP_B:0x0201:�豸�����������ģʽ
#define SW_MONITOR_MODE         0   //0:���ģʽ
#define SW_UPDATE_MODE          1   //1:��������ģʽ
#define SW_VERISONSWITCH_MODE   251 //251:�����汾�л�ģʽ

//0x0011:APCЭ����󳤶�
#define APC_MSG_MAX_LEN         0x500  //APCЭ����󳤶�1280Bytes

//0x0020:�豸ʹ�õ�Զ��������ʽ
#define SW_UPDATE_NO          0   //0:��ʾ�豸�޷�����Զ����
#define SW_UPDATE_MCPB        1   //1:��ʾ����MCP:B��ʽ
#define SW_UPDATE_FTP         2   //2:��ʾ����FTP��ʽ
//0x0138:PS����Э�鶨��
#define IP_UDP_MODE           1   //1:ʹ��IP+UDPЭ��
#define IP_TCP_MODE           2   //2:ʹ��IP+TCPЭ��
//0x0142:ͨ�ŷ�ʽ����
#define SMS_MODE              1   //1:���ŷ�ʽ,
#define PS_MODE               2   //2:PS��ʽ

#define AP_MSG_HEAD_TAIL_LEN  17  //���ݰ�����Э���ֽ���(�����ݵ�Ԫ����������,��ͷ+CRC+��β)
#define APA_MSG_HEAD_LEN      13  //���ݰ���ͷ�ֽ���(��������ʼ��־)
#define APB_MSG_HEAD_LEN      13  //���ݰ���ͷ�ֽ���(��������ʼ��־)
#define APC_MSG_HEAD_LEN      13  //���ݰ���ͷ�ֽ���(��������ʼ��־)
#define AP_MSG_UNCRC_LEN      4   //���ݰ��в���CRC���ݳ���
#define AP_PDU_HEAD_LEN       5   //���ݵ�Ԫͷ����,���ݵ�Ԫ����:1�ֽ�,ID:2�ֽ�

#define BROADCAST_STATION     0x00000000  //�㲥վ����
#define BROADCAST_DEV         0xFF        //�㲥�豸���

#define APA_IDPACK_MAX_LEN    230 //APA����б���ѯ���ݰ����̶�
#define APB_IDPACK_MAX_LEN    40 //APB����б���ѯ���ݰ����̶�
#define APC_IDPACK_MAX_LEN    230 //APC����б���ѯ���ݰ����̶�

#define PACK_VALUE_SIZE  APC_MSG_MAX_LEN

#define ALARMREPORTINTERVAL   180   //�澯�ϱ���ʱ���180s
#define ALARMDECTINTERVAL     5     //�澯��ѯ��ʱ���3s
#define ALARMSUCCESSFLAG      0x80  //�澯�ϱ��ɹ���־

//MCP_BЭ�鶨�����
//0203,���ݿ鳤��
#define UPDATEBLOCK_LEN       0x300  //0203,���ݿ鳤��1024Bytes
//1����ʾ�ļ����俪ʼ��
//2����ʾ�ļ����������
//3����ʾ�������ȡ������������
//4����ʾ����������������

//APЭ�鶨�����ID
#define IDLIST_ID             0x00000009  //�豸����б�ID
#define SWRUNVER_ID           0x0000000A  //��ǰ��������汾 str��,��󳤶�20���ֽ�
#define COMMODEMTYPE_ID       0x0000000B  //Զ������ͨ��ģ��
#define SWRUNMODE_ID          0x00000010  //�豸�����������ģʽ uint1�� 0:���ģʽ,1:��������ģʽ,����ֵΪϵͳ���� 
#define APCMAXLEN_ID          0x00000011  //AP:CЭ����󳤶� uint2��
#define SWUPDATERESULT_ID     0x00000018  //�豸ִ�����������Ľ�� uint1��
#define FPGAVER_ID            0x00000021  //FPGA�汾��Ϣ str��,��󳤶�20���ֽ�
#define QUERYLOG_ID           0x000000A0  //��־���ݲ�ѯ

#define STATIONNO_ID          0x00000101  //վ���� uint4��,��������վ����ʱʹ��
#define DEVICENO_ID           0x00000102  //�豸��� uint1��,���������豸���ʱʹ�á�����ϵͳ:�̶�Ϊ255 ��վ:�̶�Ϊ0 ��վ:1��254
#define SMSCTEL_ID            0x00000110  //���ŷ������ĺ��� str��,��󳤶�20���ֽ�
#define QUERYTEL_ID           0x00000111  //0x0111~0x0115 ��ѯ/���õ绰����1~5 str��,��󳤶�20���ֽ�
#define NOTIFYTEL_ID          0x00000120  //�ϱ����� str��,��󳤶�20���ֽ�
#define OMCIP_ID              0x00000130  //�������IP��ַ ���ִ�,��4��uint1���
#define OMCIPPORT_ID          0x00000131  //�������IP��ַ�˿ں� uint2��
#define GPRSAPN_ID            0x00000133  //GPRS���������(��:APN)str��,��󳤶�20���ֽ�
#define HEARTBEATTIME_ID      0x00000134  //�豸�����������ʱ�� uint2��,��λΪ���� ��ֵΪ0ʱ,��ʾ��������
#define GPRSUSER_ID           0x00000136  //GPRS����:�û���ʶ str��,��󳤶�20���ֽ�
#define GPRSPASSWORD_ID       0x00000137  //GPRS����:���� str��,��󳤶�20���ֽ�
#define PSTRANPROTOCOL_ID     0x00000138  //PS����Э�� uint1��,1:ʹ��IP+UDPЭ��,2:ʹ��IP+TCPЭ��
#define DEVICEIPPORT_ID       0x00000139  //�豸IP��ַ�˿ں�(UDPЭ��) uint2��
#define DEVICECOMMTYPE_ID     0x00000142  //�豸ͨ�ŷ�ʽ uint1��,��ȷ�豸��OMC��ͨ�ŷ�ʽ1:���ŷ�ʽ��ѯ������,���ŷ�ʽ�ϱ���2:GPRS��ʽ��ѯ������,GPRS��ʽ�ϱ�������ֵΪϵͳ����
#define REPORTTYPE_ID         0x00000141  //0x0141 �ϱ����� uint1��,��ȷ�ϱ�������(���ϱ�ʱ,�������ݵ�Ԫ����ǰ��)
#define RW1_OMCIP_ID		  0x00000143  //��ѯ����ip��ַ1
#define RW2_OMCIP_ID		  0x00000144  //��ѯ����ip��ַ2
#define DEVICETIME_ID         0x00000150  //�豸��ǰʱ�� ���ִ�,��7���ֽ����,��BCD���ʾ(����4λ,ʱ����24Сʱ��ʽ)
#define DEVICEIP_ID           0x00000151  //�豸IP��ַ ���ִ�,��4��uint1���
#define DEVNETMASK_ID         0x00000152  //�豸�������� ���ִ�,��4��uint1���
#define DEVDEFAULTGW_ID       0x00000153  //�豸Ĭ������ ���ִ�,��4��uint1���
#define WORKINGCHANNELNO_ID   0x00000410  //�����ŵ���1 uint2
#define SOURCECI_ID           0x00000452  //��ԴС��ʶ���� uint2
#define WORKINGCHANNEL_SW_ID  0x00000860  //�����ŵ�����1 bit 
#define AUTO_CHANNEL_SW_ID    0x00000185  // �Զ��ز����ٿ���
#define GSMULATT_ID           0x08001005  //GSM����˥��ֵ(ͨ��1(GSM)) uint1
#define CDMAULATT_ID          0x08003005  //GSM����˥��ֵ(ͨ��1(GSM)) uint1
#define GSMDLATT_ID           0x08001006  //GSM����˥��ֵ(ͨ��1(GSM)) uint1
#define CDMADLATT_ID          0x08003006  //GSM����˥��ֵ(ͨ��1(GSM)) uint1
#define G3ULATT_ID            0x08004005  //3G����˥��ֵ(ͨ��2(3G)) uint1
#define G3DLATT_ID            0x08004006  //3G����˥��ֵ(ͨ��2(3G)) uint1
#define LTE1ULATT_ID          0x08007005  //LTE1����˥��ֵ(ͨ��3(LTE1)) uint1
#define FDD1ULATT_ID          0x08005005  //LTE1����˥��ֵ(ͨ��3(LTE1)) uint1
#define LTE1DLATT_ID          0x08007006  //LTE1����˥��ֵ(ͨ��3(LTE1)) uint1
#define FDD1DLATT_ID          0x08005006  //LTE1����˥��ֵ(ͨ��3(LTE1)) uint1
#define LTE2ULATT_ID          0x08008005  //LTE2����˥��ֵ(ͨ��4(LTE2)) uint1
#define FDD2ULATT_ID          0x08006005  //LTE2����˥��ֵ(ͨ��4(LTE2)) uint1
#define LTE2DLATT_ID          0x08008006  //LTE2����˥��ֵ(ͨ��4(LTE2)) uint1
#define FDD2DLATT_ID          0x08006006  //LTE2����˥��ֵ(ͨ��4(LTE2)) uint1
#define GSMDLPOWER_ID         0x08001001  //�������빦�ʵ�ƽ(ͨ��1(GSM))sint2
#define CDMADLPOWER_ID        0x08003001  //�������빦�ʵ�ƽ(ͨ��1(GSM))sint2
#define G3DLPOWER_ID          0x08004001  //�������빦�ʵ�ƽ(ͨ��2(3G))sint2
#define LTE1DLPOWER_ID        0x08007001  //�������빦�ʵ�ƽ(ͨ��3(LTE1))sint2
#define FDD1DLPOWER_ID        0x08005001  //�������빦�ʵ�ƽ(ͨ��3(LTE1))sint2
#define LTE2DLPOWER_ID        0x08008001  //�������빦�ʵ�ƽ(ͨ��4(LTE2))sint2
#define FDD2DLPOWER_ID        0x08006001  //�������빦�ʵ�ƽ(ͨ��4(LTE2))sint2
#define FTPSERVERIP_ID        0x00000160  //FTP������IP��ַ ���ִ�,��4��uint1���
#define FTPSERVERIPPORT_ID    0x00000161  //FTP������IP��ַ�˿ں� uint2��
#define SAMPLESTARTTIME_ID    0x00000601  //���ɿ�ʼʱ�� ���ִ�,��7���ֽ����,��BCD���ʾ(����4λ,ʱ����24Сʱ��ʽ)
#define QUERYSAMPLEDATA_ID    0x00000606  //�������ݲ�ѯ
#define RU_ACCESS_STATE_ID    0x08000080  // Զ�˽���״̬
#define DEVROUTE_ID           0x00000873  //�豸·�ɵǼǵ�ַ ���ִ�,��4���ֽ����
#define DEVTEMP_ID            0x0000087C  // �豸�¶�
#define	LASER_RXPW_ID         0x080007E0  // ���չ���1
#define	LASER_TXPW_ID         0x080007E8  // �ⷢ����1
//�澯
#define PW_DOWN_EN_ID         0x00000201  // ��Դ����澯ʹ��
#define PW_DOWN_ALARM_ID      0x00000301  // ��Դ����澯
#define PW_ERROR_EN_ID		  0x00000202  // ��Դ����
#define PW_ERROR_ALARM_ID     0x00000302
#define BAT_ERROR_EN_ID       0x00000204  // ��ع���
#define BAT_ERROR_ALARM_ID    0x00000304
#define SHIFT_ERROR_EN_ID     0x00000205  // λ�ø澯
#define SHIFT_ERROR_ALARM_ID  0x00000305 
#define OTHER_ERROR_EN_ID     0x00000208  // ����ģ��澯
#define OTHER_ERROR_ALARM_ID  0x00000308  
#define LOUNLOCKALEN_ID       0x00000209  //����澯ʹ��
#define LOUNLOCKAL_ID         0x00000309  //����澯
#define MAIN_LINE_EN_ID       0x0000020F  // ���Ӽ����·�澯
#define MAIN_LINE_ALARM_ID    0x0000030F 
#define OUTSIDE_EN_ID         0x00000220   // �ⲿ�澯1~6
#define OUTSIDE_ALARM_ID      0x00000320  
#define TD_ASYNC_EN_ID        0x00000272   // TD-SCDMAʧ���澯
#define TD_ASYNC_ALARM_ID     0x00000372
#define LTE_ASYNC_EN_ID       0x00000273   // LTEʧ���澯
#define LTE_ASYNC_ALARM_ID    0x00000373
#define LASER_MODULE_EN_ID    0x00000030   // ���շ����ϸ澯1~8
#define LASER_MODULE_ALARM_ID 0x0000005C
#define DEV_TEMP_EN_ID        0x00000049   // �豸���¸澯 
#define DEV_TEMP_ALARM_ID     0x00000075
#define GSM_DIN_UNDER_EN_ID   0x0800100C    // GSM��������Ƿ���ʸ澯ʹ��
#define TD_DIN_UNDER_EN_ID    0x0800400C    // TD��������Ƿ���ʸ澯ʹ��
#define LTE1_DIN_UNDER_EN_ID  0x0800700C    // lte1��������Ƿ���ʸ澯ʹ��
#define LTE2_DIN_UNDER_EN_ID  0x0800800C    // lte2��������Ƿ���ʸ澯ʹ��
#define GSM_DIN_OVER_EN_ID    0x0800100D    // GSM������������ʸ澯ʹ��
#define TD_DIN_OVER_EN_ID     0x0800400D    // TD������������ʸ澯ʹ��
#define LTE1_DIN_OVER_EN_ID   0x0800700D    // lte1������������ʸ澯ʹ��
#define LTE2_DIN_OVER_EN_ID   0x0800800D    // lte2������������ʸ澯ʹ��
#define GSM_DIN_UNDER_ID      0x0800100E    // GSM��������Ƿ���ʸ澯
#define TD_DIN_UNDER_ID       0x0800400E    // TD��������Ƿ���ʸ澯
#define LTE1_DIN_UNDER_ID     0x0800700E    // lte1��������Ƿ���ʸ澯
#define LTE2_DIN_UNDER_ID     0x0800800E    // lte2��������Ƿ���ʸ澯
#define GSM_DIN_OVER_ID       0x0800100F    // GSM������������ʸ澯
#define TD_DIN_OVER_ID        0x0800400F    // TD������������ʸ澯
#define LTE1_DIN_OVER_ID      0x0800700F    // lte1������������ʸ澯
#define LTE2_DIN_OVER_ID      0x0800800F    // lte2������������ʸ澯
#define RU_DEV_TEMP_ALARM_ID  0x08000600    // Զ���豸���¸澯1
#define RU_DEV_TEMP_EN_ID     0x08000580    // Զ���豸���¸澯ʹ��1
#define RU_DEV_TEMP_ID		  0x08000500    // Զ���豸�¶�
#define RU_DOUT_UNDER_ALARM_ID 0x08001480   // Զ���������Ƿ���ʸ澯
#define RU_DOUT_OVER_ALARM_ID 0x08001580   // Զ��������������ʸ澯
#define RU_DOUT_UNDER_EN_ID   0x08001400   // Զ���������Ƿ����ʹ�� 
#define RU_DOUT_OVER_EN_ID    0x08001500   // Զ���������������ʹ��
#define RU_LASER_ALARM_ID     0x08000060   // Զ�˹��շ�ģ����ϸ澯
#define RU_LASER_EN_ID        0x08000040   // ʹ��
#define RU_RF_ALARM_ID        0x08000380   // Զ����Ƶ��·���ϸ澯
#define RU_RF_EN_ID           0x08000300   // ʹ��
#define RU_LINK_ALARM_ID      0x08000480   // Զ����·���ϸ澯
#define RU_LINK_EN_ID         0x08000400   // ʹ��
#define RU_RF_SW_ID           0x08001280   // gsm1 ��Ƶ����
#define GSM_MAX_DELAY_ID      0x08001010   // gsm���ʱ��
#define CDMA_MAX_DELAY_ID     0x08003010   // gsm���ʱ��
#define TD_MAX_DELAY_ID       0x08004010   // td���ʱ��
#define LTE1_MAX_DELAY_ID     0x08007010   // lte1���ʱ��
#define FDD1_MAX_DELAY_ID     0x08005010   // lte1���ʱ��
#define LTE2_MAX_DELAY_ID     0x08008010   // lte2���ʱ��
#define FDD2_MAX_DELAY_ID     0x08006010   // lte2���ʱ��
#define GSM_AUTO_DELAY_SW_ID  0x08001011   // gsm ʱ�ӵ�������
#define CDMA_AUTO_DELAY_SW_ID 0x08003011   // gsm ʱ�ӵ�������
#define TD_AUTO_DELAY_SW_ID   0x08004011   // td ʱ�ӵ�������
#define LTE1_AUTO_DELAY_SW_ID 0x08007011   // lte1 ʱ�ӵ�������
// �澯����
#define DEV_TEMP_THD_ID       0x00000172    // �豸��������
#define GSM_DIN_UNDER_THD     0x08001007    // gsm��������Ƿ��������
#define TD_DIN_UNDER_THD      0x08004007    // td��������Ƿ��������
#define LTE1_DIN_UNDER_THD    0x08007007    // lte1��������Ƿ��������
#define LTE2_DIN_UNDER_THD    0x08008007    // lte2��������Ƿ��������
#define GSM_DIN_OVER_THD      0x08001008    // gsm�����������������
#define TD_DIN_OVER_THD       0x08004008    // td�����������������
#define LTE1_DIN_OVER_THD     0x08007008    // lte1�����������������
#define LTE2_DIN_OVER_THD     0x08008008    // lte2�����������������
#define RU_DOUT_UNDER_THD     0x08001009    // Զ���������Ƿ��������gsm
#define RU_DOUT_OVER_THD      0x0800100A    // Զ�������������������gsm
// Զ�˲���
#define RU_DOUT_ID            0x08001100    //  Զ������������� gsm 1

//�Զ��������ѯ
#define CPLDVER_ID            0x008001A0  //CPLD�汾��Ϣ str��,��󳤶�20���ֽ�
#define DEVMAC_ID             0x008001A1  //�豸MAC��ַ str��,��󳤶�20���ֽ�
#define PLLAL_ID              0x008003A6  //ʱ�Ӹ澯
//#define OPTICALEN_ID          0x02A8  //���1�澯ʹ��,0x02A8~0x02AF
//#define OPTICAL_ID            0x03A8  //���1�澯,0x03A8~0x03AF

#define DEVICETYPE_ID         0x06D2  //��ѯ�豸��ģ��
#define OPTICSTATUS_ID        0x0A00  //P1�ڹ�ģ��״̬,��ģ���ڲ���λ���,0x0A10~0x0A17 uint1 
#define LTE1DATAPOWER_ID      0x00800A08  //LTE1 ���ֹ��� uint2
#define LTE2DATAPOWER_ID      0x00800A09  //LTE2 ���ֹ��� uint2
#define G3DATAPOWER_ID        0x00800A0A  //3G ���ֹ��� uint2
#define GSMDATAPOWER_ID       0x00800A0B  //GSM ���ֹ��� uint2
#define LTE1VGA_ID            0x00800B00  //LTE1 VGA uint1
#define LTE2VGA_ID            0x00800B01  //LTE2 VGA uint1
#define G3VGA_ID              0x00800B02  //3G VGA uint1
#define GSMVGA_ID             0x00800B03  //GSM VGA uint1
#define LTE1DATAGAIN_ID       0x00800B04  //LTE1 �������� uint2
#define LTE2DATAGAIN_ID       0x00800B05  //LTE2 �������� uint2
#define G3DATAGAIN_ID         0x00800B06  //3G �������� uint2
#define GSMDATAGAIN_ID        0x00800B07  //GSM �������� uint2
#define TESTMODELSWITCH_ID    0x00800B09  //����ģʽ������ģʽ�л� uint1
#define INTERTESTEN_ID        0x00800B0A  //�ڲ�����Դʹ�� uint1
#define INTEFREQRSRC_ID       0x00800B0B  //�ڲ�ԴƵ������ uint2
#define AGCREFERENCE_ID       0x00800B0C  //AGC �ο�ֵ�趨 uint2
#define AGCSTEP_ID            0x00800B0D  //AGC �������� uint1
#define GSMULGAINOFFSET_ID    0x00800B10  //GSM��������ƫ�� sint1
#define GSMDLGAINOFFSET_ID    0x00800B11  //GSM��������ƫ�� sint1
#define G3ULGAINOFFSET_ID     0x00800B12  //3G��������ƫ�� sint1
#define G3DLGAINOFFSET_ID     0x00800B13  //3G��������ƫ�� sint1
#define LTE1ULGAINOFFSET_ID   0x00800B14  //LTE1��������ƫ�� sint1
#define LTE1DLGAINOFFSET_ID   0x00800B15  //LTE1��������ƫ�� sint1
#define LTE2ULGAINOFFSET_ID   0x00800B16  //LTE2��������ƫ�� sint1
#define LTE2DLGAINOFFSET_ID   0x00800B17  //LTE2��������ƫ�� sint1
#define GSMDLPOWEROFFSET_ID   0x00800B18  //CH1 GSM�������빦�ʵ�ƽУ׼ sint2
#define G3DLPOWEROFFSET_ID    0x00800B19  //CH2 3G�������빦�ʵ�ƽУ׼ sint2
#define LTE1DLPOWEROFFSET_ID  0x00800B1A  //CH3 LTE1�������빦�ʵ�ƽУ׼ sint2
#define LTE2DLPOWEROFFSET_ID  0x00800B1B  //CH4 LTE2�������빦�ʵ�ƽУ׼ sint2
#define CH2WORKCHANNELNO_ID   0x08004004  //�����ŵ���ͨ��2(3G) uint2
#define CH3WORKCHANNELNO_ID   0x08007004  //�����ŵ���ͨ��3(LTE1) uint2
#define FDD1WORKCHANNELNO_ID  0x08005004  //�����ŵ���ͨ��3(LTE1) uint2
#define CH4WORKCHANNELNO_ID   0x08008004  //�����ŵ���ͨ��4(LTE2) uint2
#define FDD2WORKCHANNELNO_ID  0x08006004  //�����ŵ���ͨ��4(LTE2) uint2
#define LTE_ROMOTE_SW_ID	  0x00800B20  // LTEԶ�̹��ʿ���
#define TD_ROMOTE_SW_ID		  0x00800B21  // TDԶ�̹��ʿ���
#define TDS_TIMESLOT_ID		  0x00000610  // TD-SCDMAʱ϶����, �ڶ�ʱ϶�л���
#define TDLTE_TIMESLOT_ID	  0x00000609  // TD-LTEʱ϶����
#define TDS_SYN_THR_ID	      0x00800B24  // TD-SCDMAͬ����������
#define TDLTE_SYN_THR_ID	  0x00800B25  // TD-LTEͬ����������
#define TDLTE_SUB_ID		  0x00000608  // TD-LTE������֡����
#define BINDWIDTH_MODEL_ID    0x00800B28  //����ѡƵģʽ�л�
#define LTE_AUTO_CONFIG_ID    0x00800B36  // LTE ʱ϶�Զ�����
#define TD_FIRTRP_FINE_ID	  0x008004C5  //TD��һת����΢�� sint1
#define TD_SECTRP_FINE_ID	  0x008004C6  //TD�ڶ�ת����΢�� sint1
#define LTE_FIRTRP_FINE_ID	  0x00800B37  //LTE��һת����΢�� sint1
#define LTE_SECTRP_FINE_ID	  0x00800B38  //LTE�ڶ�ת����΢�� sint1
#define LTE_CHNCHS_ID		  0x00800B39  //LTE����ͨ��ѡ�� sint1
#define POWER_SW1_ID		  0x008006A8  // Զ���翪��1
#define POWER_SW2_ID		  0x008006A9  // Զ���翪��2
#define POWER_SW3_ID		  0x008006AA  // Զ���翪��3
#define POWER_SW4_ID		  0x008006AB  // Զ���翪��4
#define POWER_SW5_ID		  0x008006AC  // Զ���翪��5
#define POWER_SW6_ID		  0x008006AD  // Զ���翪��6
#define POWER_SW7_ID		  0x008006AE  // Զ���翪��7
#define POWER_SW8_ID		  0x008006AF  // Զ���翪��8

#define AUTHOR_ID			  0x00800FFF  // ����Ȩ��
#endif  //_APPARADEF_H

/*********************************End Of File*************************************/