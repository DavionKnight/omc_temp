#define _SW_IIC_C_

#define uint8 unsigned char

#include <linux/kernel.h>
#include "sw_iic.h"

#define WRITE_LOG(format,args...) printk(format,##args)

//引用模块外部变量

//模块内定义的全局变量

//模块内部变量


//函数名称：SDA数据线输出数据
//输入参数：无
//输出参数：无
//返回值：无
void _tx_sda(uint8 a) 	
{
	;
}
void tx_sda(uint8 a) __attribute__((weak, alias("_tx_sda")));

//函数名称：读取SDA数据线电平
//输入参数：无
//输出参数：无
//返回值：SDA电平 0=低电平 1=高电平
uint8 _rx_sda(void)
{
	return 0;
}
uint8 rx_sda(void) __attribute__((weak, alias("_rx_sda")));

//函数名称：SCL数据线输出数据
//输入参数：无
//输出参数：无
//返回值：无
void _tx_scl(uint8 a) 	
{
	;
}
void tx_scl(uint8 a) __attribute__((weak, alias("_tx_scl")));

//函数名称：读取SCL数据线电平
//输入参数：无
//输出参数：无
//返回值：SCL电平 0=低电平 1=高电平
uint8 _rx_scl(void)
{
	return 0;
}
uint8 rx_scl(void) __attribute__((weak, alias("_rx_scl")));
// iic总线启动标志
unsigned char g_start = 0;
 
//函数名称：5us延时函数
//输入参数：无
//输出参数：无
//返回值：无
void _delay5us(void)
{
	;
}
void delay5us(void) __attribute__((weak, alias("_delay5us")));

//函数名称：6us延时函数
//输入参数：无
//输出参数：无
//返回值：无
void _delay6us(void)
{
	;
}
void delay6us(void) __attribute__((weak, alias("_delay6us")));

//函数名称：发送 start 信号
//输入参数：无
//输出参数：无
//返回值：无
uint8 iic_start(void)
{
	if(g_start == 1) // 当iic已经开始，发送重新开始 
	{		
		rx_sda();		// 释放SDA、SCL总线 	
		rx_scl();		
		delay5us(); 	
		if(rx_scl() == 0)	// 从设备是否延长低电平时钟 	
		{			
			WRITE_LOG("IIC restart error!\r\n"); // 添加超时处理
			return 0 ;		
		}
		delay5us(); // Repeated start setup time, minimum 4.7us 
	} 
	rx_sda();
	rx_scl();
	delay6us();
	if((rx_sda() == 0)||(rx_scl() == 0))	
	{
		WRITE_LOG("IIC start error!\r\n");	// 添加总线忙，错误处理 	
		return 0;	
	} 
	delay5us();
	delay5us(); 	
	tx_sda(0);	
	delay5us(); 
	tx_scl(0);	
	g_start = 1;
	
	return 1;//added by zxb
}
//函数名称：发送 stop 信号
//输入参数：无
//输出参数：无
//返回值：无
void iic_stop(void)
{
	tx_sda(0);
	delay5us();
	rx_scl();
   	delay6us();
	if(rx_scl() == 0)	// 从设备是否延长低电平时钟
	{
		WRITE_LOG("IIC stop error!\r\n");
	}
	delay5us();
	tx_sda(1);
	delay5us();
	g_start = 0;
}
//函数名称：iic 写1个bit数据
//输入参数：value=要写入的1bit数据
//输出参数：无
//返回值：无
void iic_write_bit(uint8 value)
{
	delay5us();
	tx_sda(value);
	delay5us();
	rx_scl();
	delay6us();
	if(rx_scl() == 0)		// SCL 置高
	{
		WRITE_LOG("IIC SCL error!\r\n");
	}
	delay5us();
	tx_scl(0);
}
//函数名称：iic 读1个bit数据
//输入参数：无
//输出参数：无
//返回值：读取到的1bit数据
uint8 iic_read_bit(void)
{
	uint8 value = 0;
	
	tx_scl(0);
	delay5us();
	rx_sda();					// 释放SDA数据线
	delay5us();
	rx_scl();
	delay6us();
	if(rx_scl() == 0)		// SCL 置高
	{
		WRITE_LOG("IIC SCL error!\r\n");;
	}
	delay5us();
	value = rx_sda();
	delay5us();
	tx_scl(0);
	delay5us();
	return value;
}
//函数名称：iic 发送1字节数据
//输入参数：dat=要发送的1字节数据
//输出参数：无
//返回值：0=接收到ACK，成功  1=接收到NOACK，失败
uint8 iic_write_byte(uint8 dat)
{
	uint8 i = 0;
	uint8 value =0;
	
	for(i = 0; i < 8; i++)
	{
		iic_write_bit((dat>>(7-i))&0x1);
	}
	value = iic_read_bit();
	return (value);
}
//函数名称：iic 发送多字节数据
//输入参数：buf=要发送的数据缓冲区指针， len=数据长度
//输出参数：无
//返回值：0=接收到ACK，成功  1=接收到NOACK，失败
unsigned char iic_write_bytes(unsigned char * buf, unsigned char len)
{
	uint8 i = 0;
	uint8 value =0;
	
	for(i = 0; i < len; i++)
	{
		value = iic_write_byte(buf[i]);
		if(value == 0)
		{// 写入成功
			//WRITE_LOG("Send ADDR , Recv ACK!\n");
		}
		else if(value == 1)
		{// 没有收到ACK应答
			iic_stop();
			WRITE_LOG("Send ADDR , Recv NACK!\n");
			return 1;
		}
	}
	return (0);
}
//函数名称：iic 接收1字节数据
//输入参数：ack=接收到1字节数据后的响应 ACK或NACK
//输出参数：无
//返回值：接收到的1字节数据
uint8 iic_read_byte(uint8 ack)
{
	unsigned char i = 0;
	unsigned char cc = 0;
	
	for(i = 0; i < 8; i++)
	{
		cc |= (iic_read_bit()<<(7-i));
	}
	iic_write_bit(ack);
	return cc;
}
//函数名称：iic 接收多字节数据
//输入参数：buf=接收到的数据缓冲区, len=接收到的数据长度
//输出参数：无
//返回值：0=成功
uint8 iic_read_bytes(uint8 * buf, uint8 len)
{
	uint8 i = 0;
	
	for(i = 0; i < len-1; i++)
	{
		buf[i] = iic_read_byte(ACK);
	}
	buf[len-1] = iic_read_byte(NACK);
	return (0);
}

