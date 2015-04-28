#ifndef _UART_CONF_
#define _UART_CONF_

//#define RS485_CONFIG
#ifdef RS485_CONFIG
#define TIOCGRS485 0x542E
#define TIOCSRS485 0x542F
#endif
/* stopbit 2:2位停止位  1:1位停止位 其他：1位停止位
 * databit 7:7位数据位  8:8位数据位 其他: 8位数据位
 * parity  0:不校验     1:奇校验    2:偶校验 其他: 不校验
 */
extern int init_uart(char * s_dev, unsigned long int baud, int stopbit, int databit, int parity);
extern int send_data(int fd, char *data, int data_len);
extern int recv_data(int fd, char *data, int data_len, int s, int us);
extern void delay(int sec, int usec);
extern void my_printf(char * buf, int len);

#endif
