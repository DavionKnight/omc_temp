/*******************************************************************************
********************************************************************************
* �ļ�����:  ftp_deal.c
* ��������:  ftp����
* ʹ��˵��:  
* �ļ�����:	H4
* ��д����: ��2013/04/13��
* �޸���ʷ:
* �޸�����    �޸���       �޸�����
*-------------------------------------------------------------------------------
*******************************************************************************/
#ifndef _FTP_DEAL_H
#define _FTP_DEAL_H
extern int FtpGet(unsigned long host,char *user,char *pass,char *filename,char *pcSaveFile);
extern int FtpPut(char *host,char *user,char *pass,char *filename,char *pcSaveFile);
#endif