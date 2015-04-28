#ifndef _SW_IIC_H_
#define _SW_IIC_H_

// 高低电平定义
#define HIGH		(1)
#define LOW     	(0)
// 输入输出定义
#define OUT 		(0)
#define IN  		(1)
// 应答结果
#define ACK 		(0)
#define NACK		(1)

//函数名称：发送 start 信号
//输入参数：无
//输出参数：无
//返回值：无
extern unsigned char iic_start(void);

//函数名称：发送 stop 信号
//输入参数：无
//输出参数：无
//返回值：无
extern void iic_stop(void);

//函数名称：iic 接收多字节数据
//输入参数：buf=接收到的数据缓冲区, len=接收到的数据长度
//输出参数：无
//返回值：0=成功
extern unsigned char iic_read_bytes(unsigned char * buf, unsigned char len);

//函数名称：iic 接收1字节数据
//输入参数：ack=接收到1字节数据后的响应 ACK或NACK
//输出参数：无
//返回值：接收到的1字节数据
extern unsigned char iic_read_byte(unsigned char ack);

//函数名称：iic 发送多字节数据
//输入参数：buf=要发送的数据缓冲区指针， len=数据长度
//输出参数：无
//返回值：0=接收到ACK，成功  1=接收到NOACK，失败
extern unsigned char iic_write_bytes(unsigned char * buf, unsigned char len);
#endif
