/********************  COPYRIGHT(C)  ***************************************
**--------------文件信息--------------------------------------------------------
**文   件   名: apcprotocol.c
**创   建   人: 于宏图
**创 建  日 期: 2012.10.29
**程序开发环境：
**描        述: APC协议处理程序
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

#include "approtocol.h"
#include "apcprotocol.h"

/*******************************************************************************
*函数名称 : int APCEscSum(char *pbuf, INT16U sum)
*功    能 : 根据中国移动APC协议，计算数据缓冲区中的需要转义的字符数量，返回转义字符的个数 
*输入参数 : char  *pbuf：数据缓存数据指针
*           INT16U sum:检验缓冲区数据字节长度
*输出参数 : 缓冲区中需要转义的字符数量 
*******************************************************************************/
int APCEscSum(char *pbuf, INT16U sum)
{
char data;
INT16U j;

  j = 0;//转义字符数
  while(sum--)
  {
    data = *pbuf++;
    if(( data == APC_ESC0X5E )//转义处理0x5E 
     ||( data == APC_ESC0X7E ))
    {
      j++;//转义字符数
    }
  }
  return  j;
}

/*******************************************************************************
*函数名称 : int	APCEscProcess(char *pbuf, int sum)
*功    能 : 根据中国移动APC协议，处理数据缓冲区中的经过转义的字符，并计算转义字符数量，返回转义字符数量
*输入参数 : char *pbuf：数据缓存数据指针
*           INT16U sum:检验缓冲区数据字节长度
*输出参数 : 缓冲区中经过转义的字符数量,转义有错误,返回-1
*版本修订 : 2007.10.8,增加有转义标识但后续字符错误的处理,2014.4.23修改
*******************************************************************************/
int	APCEscProcess(char *pbuf, int sum)
{
int j;
char data, *wr;

  j = 0;//转义字符数
  wr = pbuf;
  while(sum)
  {
    data = *pbuf++;
    sum--;
    if (data == APC_ESCFLAG)//转义处理 0x5E
    {
      data = *pbuf++;
      sum--;
      if(data == 0x5D)//0x5E 
      {
        *wr++ = 0x5E;
        j++;//转义字符数
      }
      else if(data == 0x7D)//0x7E 
      {
        *wr++ = 0x7E;
        j++;//转义字符数
      }
      else//转义有错误,有APC_ESCFLAG但后续字符错误
      {
      	return MSG_ESC_ERR;
      }
    }
    else
    {
      *wr++ = data;
    }
  }
  return j;
}

/*******************************************************************************
*函数名称 : int APCPack(APPack_t *p_packbuf, char *sdbuf)
*功    能 : 按照中国移动APC协议，对协议数据包进行打包的函数
*输入参数 : APPack_t *p_packbuf：协议数据包结构指针
*           char  *sdbuf:打包后数据缓存
*输出参数 : 打包后需要发送的数据包长度
*******************************************************************************/
int APCPack(APPack_t *p_packbuf, char *sdbuf)
{
int rd, wr, sum, sd;

  if ((p_packbuf->APType == AP_A)
   || (p_packbuf->APType == AP_C))
  {
    sd = APPack(p_packbuf, sdbuf);
    sum = APCEscSum(&sdbuf[1], (p_packbuf->PackLen - 2));//检查转义字符的数量,不包括头尾
    if(sum > 0)
    {
      rd = sd - 1;//数据缓冲中数据读地址
      wr = rd + sum;//缓冲中数据写地址
      sd = sd + sum;
      sdbuf[wr--] = sdbuf[rd--];
      //数据缓冲从后向前处理字符
      while(rd > 0)//从后向前在数据存储区域中放置待发送的数据,不包括包头7E
      {
        if((sdbuf[rd] == APC_ESC0X5E)  //转义处理 5E->5E5D
        || (sdbuf[rd] == APC_ESC0X7E)) //转义处理 7E->5E7D
        {
          sdbuf[wr--] = (sdbuf[rd] & 0xF0) + 0x0D;
          sdbuf[wr] = APC_ESCFLAG;
          sum--;
          if(sum == 0)
          {
            break;
          }
        }
        else
        {
          sdbuf[wr] = sdbuf[rd];
        }
        rd--;
        wr--;
      }
    }
    ClearAPPackBuf(p_packbuf);
    return sd;
  }
  else
  {
    ClearAPPackBuf(p_packbuf);
    return MSG_AP_ERR;
  }
}

/*******************************************************************************
*函数名称 : int	APCUnpack(char *rcbuf, int rcnum, APPack_t *p_packbuf)
*功    能 : 当完成一次数据接收时，调用此函数，按照中国移动AP-C协议解包
*输入参数 : char  *rcbuf:接收数据缓冲
*						int   rcnum：接收数据数
*						APPack_t *p_packbuf：按照包格式解析接收数据
*输出参数 : 正确的数据包返回p_packbuf->PackLen>0,错误返回负数错误信息
*******************************************************************************/
int	APCUnpack(char *rcbuf, int rcnum, APPack_t *p_packbuf)
{
	int sum, escsum, j, packstart;//sum:接收APC数据包有效数据数,j:接收数据计数,packstart:包头在rcbuf中的位置
	char buf[COMBUF_SIZE];

	sum = 0;
	j = 0;
	packstart = 0;
	buf[0] = 0;
	while (j < rcnum)
	{
    	buf[sum] = rcbuf[j];
    	sum++;
    	if (buf[0] != APC_STARTFLAG)//找包头
    	{
	      	packstart = j;
	      	sum = 0;
    	}
   	 	else
    	{
      		if (sum > 1)
      		{
        		if (buf[sum-1] == APC_ENDFLAG)//找到包尾,判断刚读入的数据是否为包尾
        		{
          			if (sum < AP_MSG_HEAD_TAIL_LEN) //数据包中有多个包头、包尾标识，将距离较近中得最后一个作为包头标识
          			{
            			packstart = j;
           			 	buf[0] = APC_STARTFLAG;	//找包头
            			sum = 1;
          			}
         	 		else
          			{
        				//转义处理
        				escsum = APCEscProcess(buf, sum);//sum为转义处理完数据总数
        				if (escsum == MSG_ESC_ERR)//2014.4.23
        				{
        					return MSG_ESC_ERR;
        				}
        				else
        				{
        					sum = sum - escsum;//转义处理完数据总数
        				}
        				if(APUnpack(buf, sum, p_packbuf)==MSG_CRC_ERR)//CRC校验错误
        				{
							packstart = j;
							buf[0] = APC_STARTFLAG;	//找包头
							sum = 1;
							if ((j+1) < rcnum)//接收的数据包中有误码,搜索剩余数据
							{
								DEBUGOUT("APC Receive Data Error!\r\n");
							}
							else
							{
								return MSG_CRC_ERR;
							}
        				}
        				else
        				{
        		 			 return	p_packbuf->PackLen;
        				}
         			}
        		}
      		}
    	}
    	j++;
  	}
  	return MSG_AP_ERR;//非正常的数据包:AP协议类型错误
}

/*********************************End Of File*************************************/