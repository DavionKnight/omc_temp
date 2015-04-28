/********************  COPYRIGHT(C) ***************************************
**--------------文件信息--------------------------------------------------------
**文   件   名: apbprotocol.c
**创   建   人: 于宏图
**创 建  日 期: 
**程序开发环境：
**描        述: APB协议处理程序
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
#include "apbprotocol.h"

/*******************************************************************************
*函数名称 : void ByteToAscii(char hex_data, char *pbuf)
*功    能 : 按照APB协议,将hex的数据hex_data转换为2个Ascii数据放置到pbuf所指位置
*输入参数 : hex_data：要转换的hex数据，*pbuf：存储2个Ascii数据首地址位置
*输出参数 : None
*******************************************************************************/
void ByteToAscii(char hex_data, char *pbuf)
{
char  temp;

  temp = hex_data >> 4;//高4位
  if(temp < 0x0A )
  {
    *pbuf = '0' + temp;
  }
  else
  {
    *pbuf = 'A' + (temp - 0x0A);
  }

  temp = hex_data & 0x0F;//低4位
  if(temp < 0x0A )
  {
    *(pbuf + 1) = '0' + temp;
  }
  else
  {
    *(pbuf + 1) = 'A' + (temp - 0x0A);
  }
}

/*******************************************************************************
*函数名称 : int APBByteToAscii(char *pbuf, int sum)
*功    能 : 根据中国移动APB协议，处理数据缓冲区中的HEX字符为ASCII字符
*输入参数 : char *pbuf：数据缓存数据指针
*           int sum:检验缓冲区数据字节长度
*输出参数 : 缓冲区中经过转义的字符数量
*******************************************************************************/
int APBByteToAscii(char *pbuf, int sum)
{
int   j;
char  *wr, *rd;

  wr = pbuf + 2*sum - 3;
  rd = pbuf + sum - 1;
  *wr = *rd;
  rd--;
  wr = wr - 2;
  j = sum - 2;
  while(j--)
  {
    ByteToAscii(*rd, wr);
    rd--;
    wr = wr - 2;
  }
  return  (2*sum - 2);
}

/*******************************************************************************
*函数名称 : char AsciiToByte(char *pbuf)
*功    能 : 按照APB协议,将pbuf所指位置的2个Ascii数据转换为hex的数据
*输入参数 : *pbuf：存储2个Ascii数据首地址位置
*输出参数 : 转换生成的hex数据，
*******************************************************************************/
char AsciiToByte(char *pbuf)
{
char i, outdata;

  outdata = 0;
  
  for(i = 0; i < 2; i++ )
  {
    outdata = outdata << 4;  // 数据左移4位
    if(('0' <= *pbuf) && (*pbuf <= '9'))  // 0~9数据
    {
      outdata += (*pbuf - '0');
    }
    else  if(('a' <= *pbuf) && (*pbuf <= 'f'))  // a~f数据
    {
      outdata += (*pbuf - 'a' + 0x0A);
    }
    else  if(('A' <= *pbuf) && (*pbuf <= 'F'))  // A~F数据
    {
      outdata += (*pbuf - 'A' + 0x0A);
    }
    pbuf++;
  }
  return outdata;
}

/*******************************************************************************
*函数名称 : int APBAsciiToByte(char *pbuf, int sum)
*功    能 : 根据中国移动APB协议，处理数据缓冲区中的ASCII字符为HEX字符
*输入参数 : char *pbuf：数据缓存数据指针
*           int sum:检验缓冲区数据字节长度
*输出参数 : 缓冲区中经过转义的字符数量
*******************************************************************************/
int APBAsciiToByte(char *pbuf, int sum)
{
int   j;
char  *wr;

  wr = pbuf;
  j = sum /2;
  while(j--)
  {
    *wr = AsciiToByte(pbuf);
    wr++;
    pbuf = pbuf + 2;
  }
  return  (sum /2);
}

/*******************************************************************************
*函数名称 : int APBPack(APPack_t *p_packbuf, char *sdbuf)
*功    能 : 按照中国移动APB协议，对协议数据包进行打包的函数
*输入参数 : APPack_t *p_packbuf：协议数据包结构指针
*           char  *sdbuf:打包后数据缓存
*输出参数 : 打包后需要发送的数据包长度
*******************************************************************************/
int APBPack(APPack_t *p_packbuf, char *sdbuf)
{
int sd;

  if (p_packbuf->APType == AP_B)
  {
    sd = APPack(p_packbuf, sdbuf);
    sd = APBByteToAscii(sdbuf, sd);
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
*函数名称 : int	APBUnpack(char *rcbuf, int rcnum, APPack_t *p_packbuf)
*功    能 : 当完成一次数据接收时，调用此函数，按照中国移动AP-B协议解包
*输入参数 : char  *rcbuf:接收数据缓冲
*						int   rcnum：接收数据数
*						APPack_t *p_packbuf：按照包格式解析接收数据
*输出参数 : 正确的数据包返回p_packbuf->PackLen>0,错误返回负数错误信息
*******************************************************************************/
int	APBUnpack(char *rcbuf, int rcnum, APPack_t *p_packbuf)
{
int sum, i, j, packstart;//sum:接收APB数据包有效数据数,j:接收数据计数,packstart:包头在rcbuf中的位置
char buf[COMBUF_SIZE];

  sum = 0;
  j = 0;
  packstart = 0;
  buf[0] = 0;
  while (j < rcnum)
  {
    buf[sum] = rcbuf[j];
    sum++;
    if (buf[0] != APB_STARTFLAG)//找包头
    {
      packstart = j;
      sum = 0;
    }
    else
    {
      if (sum > 1)
      {
        if (buf[sum-1] == APB_ENDFLAG)//找到包尾,判断刚读入的数据是否为包尾
        {
          if (sum < (AP_MSG_HEAD_TAIL_LEN*2)) //数据包中有多个包头、包尾标识，将距离较近中得最后一个作为包头标识
          {
            packstart = j;
            buf[0] = APB_STARTFLAG;	//找包头
            sum = 1;
          }
          else
          {
        		//APB协议处理
        		i = APBAsciiToByte(&buf[1], (sum - 2));
        		buf[i+1] = buf[sum - 1];
        		sum = i + 2;
        		if(APUnpack(buf, sum, p_packbuf) == MSG_CRC_ERR)//CRC校验错误
        		{
              packstart = j;
              buf[0] = APB_STARTFLAG;	//找包头
              sum = 1;
              if ((j+1) < rcnum)//接收的数据包中有误码
              {
                DEBUGOUT("APB Receive Data Error!\r\n");
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

/********************  COPYRIGHT(C) ***************************************/
