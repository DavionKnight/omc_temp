#include "../../common/apue.h"
#include "../../common/common.h"
#include "../../common/crc.h"
#include "../../common/status.h"
#include "../localcom/localcom.h"
#include "../omccom/omccom.h"
#include "../../common/drudefstruct.h"
#include "../../protocol/approtocol.h"
#include "../../protocol/apcprotocol.h"
#include "../../protocol/apbprotocol.h"
#include "uart_conf.h"
#include "rs485_modules.h"
#include "rs485.h"
#include "../../driver/drv_api.h"
#include "../../sqlite/drudatabase.h"


#define START 0x00000001
#define STOP  0x80000005
#define SET				0x80000001
#define CLR				0x80000002
#define READ_ST			0x80000003
#define DEBUG			0x80000004
#define SET_ADDR		0x80000005
// SDA和SCL引脚定义
#define SDA (1<<20)
#define SCL (1<<21)
int uart_fd;
struct queue_buf uart_recv;
DevInfo_t dev_ap;
APPack_t recv_pack;
ComBuf_t omc_rs485_combuf;
ComBuf_t rs485_omc_combuf;
swap_t omc_rs485_swap;
swap_t rs485_omc_swap;
volatile int rs485_recv_flag = 0;
volatile int omc_rs485_flag = 0;
// led灯状态
struct led_data led_sta_power[16];	//远供电指示灯
struct led_data led_sta_led[16];	//指示灯板
extern unsigned short int g_pack_no;

int g_OMCSetParaRs485Flag;

void read_power_tw_sta(void);
/*
** 函数功能：打印接收到的数据包
** 输入参数：pack=数据包结构体
** 输出参数：无
** 返回值：0=成功 -1=失败
** 备注：
*/
int print_pack(APPack_t * pack)
{
	unsigned int len = pack->PackLen-AP_MSG_HEAD_TAIL_LEN;
	unsigned char * p = (unsigned char *)&(pack->PackValue[0]);
	unsigned char l;
	unsigned short int t;
	unsigned char * v = NULL;

	t = hex2uint16(p+1);
	if(t != 0x06d2){
		printf("recv omc_local len=%d.\n", pack->PackLen);
		memcpy((char *)rs485_omc_swap.buf, (char *)pack, pack->PackLen-3);
		memcpy((char *)(rs485_omc_swap.buf+pack->PackLen-3), (char *)&pack->CRCData, 3);
		rs485_omc_swap.len = pack->PackLen;
	}else{
		while(len){
			l = *(unsigned char *)p;	
			t = hex2uint16(p+1);
			v = p+3;
			p += l;
			if(t == 0x06d2){// 模块类型
				if(*v == 0x63){// 灯板操作，不存储
					//printf("set led exit.\r\n");
					break;
				}
				if(*v == 0x64){ // 远供电源模块，不存储
					break;
				}
				if(0 != check_module_type(v[0], hex2uint16(v+1))){
					printf("模块地址或类型错。\r\n");
					break;
				}
			}else if(t == 0x0009){  // 参量列表
				write_id_list(l, t, v);
			}else{  // 其他参数
				//write_para(l, t, v);
				if((t >= 0x0300) && (t < 0x0400)){// 功放模块告警，写入数据库
					write_alarm(t, v);
				}
			}
			len -= l;
		}
		rs485_recv_flag = 0;
	}
	
	return 0;
}
void * rs485_recv_pthread(void * agr)
{
	char tbuf[256];
	int cnt = 0;
	int ret = 0;
	int loop_cnt = 0;
	int exit_pro = 1;

	while(1)
	{
		cnt = 0;
		loop_cnt = 0;
		exit_pro = 1;
		cnt = recv_data(uart_fd, tbuf, 200, 5, 200);
		if(cnt > 0)
		{
			while(exit_pro)
			{
				ret = push_queue_buf(&uart_recv, tbuf, cnt);		
				if(ret == -1)
				{
					printf("push error.\r\n");
					sleep(1);
					loop_cnt++;
					if(loop_cnt > 5)
					{
						exit_pro = 0;
					}
				}
				else
				{
					//printf("push ok.\r\n");
					exit_pro = 0;
				}
			}
		}
	}
	return (void *)0;
}
/*
** 函数功能：RS485轮询任务线程函数
** 输入参数：arg
** 输出参数：无
** 返回值：0=成功 -1=失败
** 备注：
*/
void * rs485_poll_pthread(void * agr)
{

	read_id_list();
	while(1){
		read_para_all();
		sleep(3);
	}
	return (void *)0;
}
/*
** 函数功能：RS485处理OMC查询设置函数
** 输入参数：arg
** 输出参数：无
** 返回值：0=成功 -1=失败
** 备注：
*/
void * rs485_omc_pthread(void * agr)
{
	int dly_cnt = 0;

	while(1){
		if(0 != omc_rs485_combuf.RecvLen){
			printf("recv omc->rs485 len is %d.\r\n", omc_rs485_combuf.RecvLen);
			//my_printf(omc_rs485_combuf.Buf, omc_rs485_combuf.RecvLen);
			lock_sem(SEM_RS485_SEND);	
			omc_rs485_flag = 1;
			omc_rs485_combuf.RecvLen = data_change((unsigned char *)omc_rs485_combuf.Buf+2, (int)omc_rs485_combuf.RecvLen-2);
			send_data(uart_fd, omc_rs485_combuf.Buf+2, omc_rs485_combuf.RecvLen);
			dly_cnt = 0;
			while(omc_rs485_flag){
				usleep(100000);
				dly_cnt++;
				if(dly_cnt > 30){
					printf("omc to rs485 not recv.\r\n");
					omc_rs485_flag = 0;
					break;
				}
			}
			omc_rs485_combuf.RecvLen = 0;
			unlock_sem(SEM_RS485_SEND);
		}
		if(0 != omc_rs485_swap.len){
			printf("send local_omc omc_rs485_swap len=%d.\n", omc_rs485_swap.len);
			lock_sem(SEM_RS485_SEND);
			send_data(uart_fd, (char *)omc_rs485_swap.buf, omc_rs485_swap.len);
			omc_rs485_swap.len = 0;
			unlock_sem(SEM_RS485_SEND);
		}
		usleep(10000);
	}
	return (void *)0;
}
/*
** 函数功能: 灯板设置组包
** 输入参数：无
** 输出参数：无
** 返回值：0=成功 其他=失败
** 备注：
*/
int led_para_pack_power(DevInfo_t * p_devinfo, int packno, APPack_t * p_packbuf)
{
	int pdustart, pvlen, i;

	// 按照协议组包头
	pdustart = APHeadPack(p_devinfo,AP_C, COMMAND_SET, packno, p_packbuf);
	if(pdustart < 0)
	{
		printf("led_para_pack error.\r\n");
		return -1;
	}
	memset(&p_packbuf->PackValue[pdustart], 0, (sizeof(p_packbuf->PackValue)-pdustart));
	pvlen = pdustart;
	for(i = 0; i < 16; i++)
	{
		p_packbuf->PackValue[pvlen++] = led_sta_power[i].len;
		p_packbuf->PackValue[pvlen++] = (unsigned char)led_sta_power[i].id;
		p_packbuf->PackValue[pvlen++] = (unsigned char)(led_sta_power[i].id>>8);
		p_packbuf->PackValue[pvlen++] = led_sta_power[i].sta;
		if(pvlen > (APC_MSG_MAX_LEN-pdustart-AP_MSG_HEAD_TAIL_LEN))
		{
			printf("led_para_pack len more than APC_MSG_MAX_LEN!\r\n");
			break;
		}
	}
	// 参数列表数据包长度
	p_packbuf->PackLen = pvlen + AP_MSG_HEAD_TAIL_LEN;
	return p_packbuf->PackLen;
}

int led_para_pack_led(DevInfo_t * p_devinfo, int packno, APPack_t * p_packbuf)
{
	int pdustart, pvlen, i;

	// 按照协议组包头
	pdustart = APHeadPack(p_devinfo,AP_C, COMMAND_SET, packno, p_packbuf);
	if(pdustart < 0)
	{
		printf("led_para_pack error.\r\n");
		return -1;
	}
	memset(&p_packbuf->PackValue[pdustart], 0, (sizeof(p_packbuf->PackValue)-pdustart));
	pvlen = pdustart;
	for(i = 0; i < 16; i++){
		p_packbuf->PackValue[pvlen++] = led_sta_power[i].len;
		p_packbuf->PackValue[pvlen++] = (unsigned char)led_sta_led[i].id;
		p_packbuf->PackValue[pvlen++] = (unsigned char)(led_sta_led[i].id>>8);
		p_packbuf->PackValue[pvlen++] = led_sta_led[i].sta;
		if(pvlen > (APC_MSG_MAX_LEN-pdustart-AP_MSG_HEAD_TAIL_LEN))
		{
			printf("led_para_pack len more than APC_MSG_MAX_LEN!\r\n");
			break;
		}
	}
	// 参数列表数据包长度
	p_packbuf->PackLen = pvlen + AP_MSG_HEAD_TAIL_LEN;
	return p_packbuf->PackLen;
}
/*
** 函数功能：灯板初始化
** 输入参数：无
** 输出参数：无
** 返回值：0=成功 其他=失败
** 备注：
*/

int init_power(unsigned char sta)
{
	int i = 0; 

	for(i=0;i<8;i++)
	{
		led_sta_power[i].len = 4;
		led_sta_power[i].id = 0x0801+i;
		led_sta_power[i].sta = sta;
	}

	for(i=0;i<8;i++)
	{
		led_sta_power[i+8].len = 4;
		led_sta_power[i+8].id = 0x06A8+i;
		led_sta_power[i+8].sta = sta;
	}
	return 0;
}
int init_led(unsigned char sta)
{
	int i = 0; 

	for(i = 0; i < 16; i++)
	{
		led_sta_led[i].len = 4;
		led_sta_led[i].id = 0x0801+i;
		led_sta_led[i].sta = sta;
	}
	return 0;
}
/*
** 函数功能：LM75初始化
** 输入参数：无
** 输出参数：无
** 返回值：0=成功 其他=失败
** 备注：
*/
#define LM75 0x90
#define LM75_WRITE (LM75)
#define LM75_READ (LM75|0x1)
#define TEMP_REG 0x0
#define CONF_REG 0x1
#define HYST_REG 0x2
#define OS_REG 0x3
int read_lm75(int fd, int reg, unsigned char * buf)
{
	unsigned char tbuf[8];
	int cnt = 0;

	cnt = 0;
	tbuf[cnt++] = reg;
	if(write(fd, tbuf, cnt)){
		printf("write 1  error.\n");
		return 0;
	}
	if(reg == CONF_REG){
		read(fd, buf, 1); 
	}else{
		read(fd, buf, 2); 
		
	}
	
	return 0;
}
int write_lm75(int fd, int reg, char temp)
{
	unsigned char tbuf[128];
	int cnt = 0;

	cnt = 0;
	tbuf[cnt++] = reg;
	tbuf[cnt++] = temp;
	if(reg != CONF_REG){
		tbuf[cnt++] = 0x0;
	}
	if(write(fd, tbuf, cnt)){
		printf("write 1  error.\n");
		return 0;
	}
	return 0;
}

int lm75_conf = 0x4;
int os_temp = 0;
int hyst_temp = 0;

int set_lm75_change(int argc, char * argv[])
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
	os_temp = para1;
	hyst_temp = para1-10;
	sprintf(msg_tmp.mtext, "lm75(%d, %d).\r\n", hyst_temp, os_temp);
	msg_send(TASK2_MODULE, (char *)&msg_tmp, strlen(msg_tmp.mtext));
	return 0;
}
int init_lm75(void)
{
	printf("init lm75\n");
	lm75_conf = 0x0;
	os_temp = 40;
	hyst_temp = 35;
	return 0;
}
int get_lm75(void)
{
	int fd = 0; 
	unsigned char tbuf[8];

	//printf("get lm75\n");
	fd = open("/dev/iic_laser", O_RDWR);
	if(fd < 0){
		printf("open /dev/iic_laser error.\n");
		return -1;
	}
	drv_write_epld(0x22, 8); // 选择CPLD的IIC接口，0～7 laser0~7 8=lm75 9=eeprom
	ioctl(fd, SET_ADDR, LM75);
	read_lm75(fd, TEMP_REG, tbuf);
	//printf("real temp is %d oC.\n", tbuf[0]);
	DbSaveThisIntPara(DEVTEMP_ID,tbuf[0]);
	read_lm75(fd, CONF_REG, tbuf);
	if(lm75_conf != (int)tbuf[0]){
		printf("set lm75_conf is 0x%02x.\n", (char)lm75_conf);
		write_lm75(fd, CONF_REG, (char)lm75_conf);
	}
	read_lm75(fd, OS_REG, tbuf);
	//printf("os temp is %d oC.\n", tbuf[0]);
	if(os_temp != (int)tbuf[0]){
		printf("set lm75_os is %d oC.\n", os_temp);
		write_lm75(fd, OS_REG, (char)os_temp);
	}
	read_lm75(fd, HYST_REG, tbuf);
	//printf("hyst temp is %d oC.\n", tbuf[0]);
	if(hyst_temp != (int)tbuf[0]){
		printf("set lm75_hyst is %d oC.\n", hyst_temp);
		write_lm75(fd, HYST_REG, (char)hyst_temp);
	}
	close(fd);
	return 0;
}
#define VENDOR_ADDR 0xa0
#define VENDOR_READ (VENDOR_ADDR|0x1)
#define VENDOR_WRITE (VENDOR_ADDR)
#define PARA_ADDR 0xa2
#define PARA_READ (PARA_ADDR|0x1)
#define PARA_WRITE (PARA_ADDR)

int laser_read_a0(int fd)
{
	char tbuf[128];
	int cnt = 0;
	
	printf("laser a0:\n");
	cnt = 0;
	tbuf[cnt++] = VENDOR_WRITE;
	tbuf[cnt++] = 0;
	if(write(fd, tbuf, cnt)){
		printf("laser write a0 error.\n");
		return -2;
	}
	cnt = 0;
	tbuf[cnt++] = VENDOR_READ;
	if(write(fd, tbuf, cnt)){
		printf("laser write 2 a0 error.\n");
		return -1;
	}
	read(fd, tbuf, 128);
	printk(tbuf, 128);
	return 0;
}
int laser_read_a2(int fd)
{
	char tbuf[128];
	int cnt = 0;
	
	printf("laser a2:\n");
	cnt = 0;
	tbuf[cnt++] = PARA_WRITE;
	tbuf[cnt++] = 0;
	if(write(fd, tbuf, cnt)){
		printf("laser write a2 error.\n");
		return -2;
	}
	cnt = 0;
	tbuf[cnt++] = PARA_READ;
	if(write(fd, tbuf, cnt)){
		printf("laser write 2 a2 error.\n");
		return -1;
	}
	read(fd, tbuf, 128);
	printk(tbuf, 128);
	return 0;
}
char change_temp(unsigned char * val)
{
	char temp = 0;
	temp = (int)*val;
	//printf("temperature=%dC.\n", temp);
	return temp;
}
float change_vcc(unsigned char * val)
{
	unsigned short tmp = 0;
	float vcc = 0.0;

	tmp = ((*val)<<8) | (*(val+1));
	vcc = ((float)tmp)/10;
	//printf("Vcc=%fmV.\n", vcc);
	return vcc;
}
float change_bias(unsigned char * val)
{
	unsigned short tmp = 0;
	float bias = 0.0;

	tmp = ((*val)<<8) | (*(val+1));
	bias = ((float)tmp)*2/1000;
	//printf("bias=%fmA.\n", bias);
	return bias;
}
float change_tx(unsigned char * val)
{
	unsigned short tmp = 0;
	float tx = 0.0;
	float dbm = 0.0;

	//printf("tx:\n");
	//printk(val, 2);
	tmp = ((*val)<<8) | (*(val+1));
	if(tmp == 0){
		tmp = 1;
	}
	//printf("tmp = %d.\n", tmp);
	tx = ((float)tmp)/10000;
	dbm = 10*log10(tx);
	//printf("tx=%fmW : %fdbm.\n", tx, dbm);
	return dbm;
}
float change_rx(unsigned char * val)
{
	unsigned short tmp = 0;
	float rx = 0.0;
	float dbm = 0.0;

	//printf("rx:\n");
	//printk(val, 2);
	tmp = ((*val)<<8) | (*(val+1));
	if(tmp == 0){
		tmp = 1;
	}
	//printf("tmp = %d.\n", tmp);
	rx = ((float)tmp)/10000;
	dbm = 10*log10(rx);
	//printf("rx=%fmW : %fdbm.\n", rx, dbm);
	return dbm;
}
int read_rx(int fd, int idx, float * rx_pw, float * tx_pw)
{
	unsigned char tbuf[128];
	int cnt = 0;

	drv_write_epld(0x22, idx&0x7); // 选择CPLD的IIC接口，0～7 laser0~7 8=lm75 9=eeprom
	ioctl(fd, SET_ADDR, PARA_ADDR);
	cnt = 0;
	tbuf[cnt++] = 96;
	if(write(fd, tbuf, cnt)){
		printf("write 1  error.\n");
		return -30.0;
	}
	read(fd, tbuf, 10); 
	//printf("laser %d:\n", idx);
	//printk((char *)tbuf, 10);
	change_temp(tbuf);
	change_vcc(tbuf+2);
	change_bias(tbuf+4);
	*tx_pw = change_tx(tbuf+6);
	*rx_pw = change_rx(tbuf+8);

	return 0;
}
extern int g_DevType;
int read_laser(unsigned short line, unsigned short * link)
{
	int iic_fd = 0;
	int i = 0;
	float rx_power = 0.0;
	float tx_power = 0.0;
	int cnt;
	
	if(g_DevType == MAIN_UNIT){
		cnt = 6;
	}else{
		cnt = 8;
	}

	*link = 0xffff;	
	iic_fd = open("/dev/iic_laser", O_RDWR);
	if(iic_fd < 0){
		printf("open /dev/iic_laser error.\n");
		return -1;
	}
	for(i = 0; i < cnt; i++){
		if(((line>>i)&0x1) == 0x0){ // 光模块在位
			read_rx(iic_fd, i, &rx_power, &tx_power);
			if(rx_power > -20.0){
				*link &= ~(1<<i);
			}
			DbSaveThisIntPara_MCP_C(LASER_RXPW_ID+i, (int)rx_power, 1);
			DbSaveThisIntPara_MCP_C(LASER_TXPW_ID+i, (int)tx_power, 1);
		}else{
			DbSaveThisIntPara_MCP_C(LASER_RXPW_ID+i, 0, 1);
			DbSaveThisIntPara_MCP_C(LASER_TXPW_ID+i, 0, 1);
		}
	}
	close(iic_fd);

	return 0;
}

/*
** 函数功能：读取链路状态，从FPGA读取
** 输入参数：无
** 输出参数：无
** 返回值：0=成功 -1=失败
** 备注：
*/
#define LINE_ADDR 0x20
// fpga同步指示寄存器
#define SYNC_ADDR 0x100
// fpga光纤链路使能寄存器
#define LINK_ENABLE 0x103
volatile unsigned short line_old = 0xffff;
volatile unsigned short link_old = 0xffff;
volatile unsigned short recv_old = 0xffff;
volatile unsigned short laser_num_old = 0;
volatile unsigned short laser_num_new = 0;
volatile unsigned short laser_cnt_old = 0;
volatile unsigned short laser_cnt_new = 0;
int read_cpld_sta(void)
{
	unsigned short line_data;
	unsigned short link_data;
	unsigned short recv_data;
	unsigned short i ;
	int ret = 0;

	drv_read_epld(LINE_ADDR, &line_data);// 是否有光模块 bit0~bit7 1=无光模块 0=有光模块
	if(line_data != line_old){
		line_old = line_data;
		ret = 1;
	}
	read_laser(line_data, &link_data);//	光模块收光是否正常 bit0~bit7 1=收光异常 0=收光正常
	//link_data = line_data;	
	if(link_data != link_old){
		link_old = link_data;
		ret = 1;
	}
	drv_read_fpga(SYNC_ADDR, &recv_data);//	光模块接收数据是否正常 bit0~bit7 1=同步 0=不同步
	if(recv_data != recv_old){
		recv_old = recv_data;
		ret = 1;
		// 当设备为远端时，如果出现不同步则关闭链路，否则打开链路, 只有同步状态发生变化时执行
		/*
		if(DEVICE_TYPE_RAU == get_device_type()){ // 判断设备类型
			if((recv_data & 0x3) != 0x3){ // 判断有否有不同步的端口0和1
				drv_write_fpga(LINK_ENABLE, recv_data&0x3);
			}
		}
		*/
	}
	
	/*
	printf("line_data=0x%04x\n", line_data);
	printf("link_data=0x%04x\n", link_data);
	printf("recv_data=0x%04x\n", recv_data);
	*/
	// 光模块不在位：灭
	// 光模块在位，收光异常：单闪
	// 光模块在位，收光正常,未同步：双闪
	// 同步：常亮
	if(get_device_type() == DEVICE_TYPE_MAIN)	//主单元
	{
		for(i = 0; i < 6; i++){
			if((line_data>>i)&0x1){  // 
				// 光模块不在位
				drv_write_epld(i+0x30, 0);
			}else if((link_data>>i)&0x1){
				// 光模块在位，收无光
				drv_write_epld(i+0x30, 1);
			}else if(((recv_data>>i)&0x1) == 0x0){
				// 光模块在位，收有光，不同步
				drv_write_epld(i+0x30, 2);
			}else{
				drv_write_epld(i+0x30, 3);
			}
		}
	}
	else	//扩展单元
	{
		for(i = 0; i < 8; i++)
		{
			if((line_data>>i)&0x1)		// 0=灭 1=亮 2=闪	
			{  // 
				led_sta_power[i].sta = 0; // 光模块不在位
				led_sta_led[i<<1].sta = 0; // 光模块不在位			
				led_sta_led[(i<<1)+1].sta = 0;			
			}
			else if((link_data>>i)&0x1)
			{
				led_sta_power[i].sta = 2; // 光模块在位，收无光
				led_sta_led[i<<1].sta = 1;  // 光模块在位，收无光			
				led_sta_led[(i<<1)+1].sta = 0;	
			}
			else if(((recv_data>>i)&0x1) == 0x0)
			{
				led_sta_power[i].sta = 3; // 光模块在位，收有光，不同步
				led_sta_led[i<<1].sta = 1; // 光模块在位，收有光，不同步			
				led_sta_led[(i<<1)+1].sta = 2;	
			}
			else
			{
				led_sta_power[i].sta = 1; // 同步
				led_sta_led[i<<1].sta = 1; // 同步			
				led_sta_led[(i<<1)+1].sta = 1;	
			}
		}
	}
	return ret;
}

/*
** 函数功能：RS485控制LED
** 输入参数：arg
** 输出参数：无
** 返回值：0=成功 -1=失败
** 备注：
*/
void * led_pthread(void * agr)
{
	int i = 0;
	int j = 0;
	int cnt = 0;

	init_led(0);
	init_power(0);
	init_lm75();
	drv_write_epld(0x21, 0x00); 				// 选择CPLD使能发光
	if(get_device_type() != DEVICE_TYPE_RAU)	// 记录上电时光模块数量，做光链路故障告警使用
	{ 
		read_cpld_sta();
		for(i = 0; i < 6; i++)
		{
			if(((line_old>>i)&0x1) == 0){
				laser_num_old++;
			}
		}
		laser_cnt_old = line_old;
	}
	while(1)
	{
		get_lm75();// 风扇控制
		if(get_device_type() == DEVICE_TYPE_EXPEND)	//判断为扩展
		{
			if((1 == read_cpld_sta())||(g_OMCSetParaRs485Flag==1))
			{	
				read_power_tw_sta();
				set_led_power();
				set_led_led();
				g_OMCSetParaRs485Flag = 0;
				cnt = 0;
			}
			else
			{
				cnt++;
				if(cnt > 60)
				{
					read_power_tw_sta();
					set_led_power();
					set_led_led();
					cnt = 0;
				}
			}
		}
		else	//主单元
		{
			read_cpld_sta();
		}
		
		laser_cnt_new = line_old;
		j = 0;
		
		for(i = 0; i < 6; i++)
		{
			if(((line_old>>i)&0x1) == 0)
			{
				j++;
			}
		}
		
		laser_num_new = j;
		sleep(5);
	}
}
/*
** 函数功能：run指示
** 输入参数：arg
** 输出参数：无
** 返回值：0=成功 -1=失败
** 备注：
*/
#define SET 0x80000001
#define CLR 0x80000002
void * run_pthread(void * agr)
{
	int fd = 0;
	fd = open("/dev/led", O_RDWR);
	while(1){
		ioctl(fd, SET, NULL);
		usleep(1000000);
		ioctl(fd, CLR, NULL);
		usleep(1000000);
	}
}
/*
** 函数功能：从数据缓冲区中读取符合中国移动协议的数据包
** 输入参数：queue_buf=串口缓冲区结构
** 输出参数：buf=符合移动协议的数据包 len=数据包长度
** 返回值：0=成功 -1=失败
** 备注：
*/
unsigned char pack_buf[1024];
unsigned int pack_buf_len;
unsigned char pack_status;
int check_cm_pack(struct queue_buf * queue_buf)
{
	int cnt, ret;
	char tmp_c = 0;
	unsigned short int tmp_crc;

	if((cnt = get_queue_buf_len(queue_buf)) != 0){
		ret = pop_queue_buf(queue_buf, &tmp_c, 1);	
		if(ret == 0){
			switch(pack_status){
				case 0:
					if(tmp_c == 0x7e){
						pack_buf[pack_buf_len++] = tmp_c;
						pack_status = 1;
						//printf("recv head.\r\n");
					}
					break;
				case 1:
					if(tmp_c == 0x7e){
						pack_buf[pack_buf_len++] = tmp_c;
						//printf("recv tail. len=%d.\r\n", pack_buf_len);
						//my_printf((char *)pack_buf, (int)pack_buf_len);
						if(pack_buf_len > 15){
							pack_buf_len -= (unsigned int)APCEscProcess((char *)pack_buf, (int)pack_buf_len);
							tmp_crc = CCITT_CRC16(pack_buf+1, pack_buf_len-4);
							//printf("crc=0x%04x.\r\n", tmp_crc);
							if(((tmp_crc&0xff)==pack_buf[pack_buf_len-3]) 
									&& (((tmp_crc>>8)&0xff)==pack_buf[pack_buf_len-2])){
								if(omc_rs485_flag){
									if(rs485_omc_combuf.RecvLen == 0){
										pack_buf_len += 2;
										memcpy(rs485_omc_combuf.Buf, (char *)&pack_buf_len, 2);
										memcpy(rs485_omc_combuf.Buf+2, pack_buf, pack_buf_len-2);
										rs485_omc_combuf.RecvLen = pack_buf_len;
										//printf("send rs485->omc len=%d.\r\n", rs485_omc_combuf.RecvLen);
										//my_printf(rs485_omc_combuf.Buf, rs485_omc_combuf.RecvLen);
									}
									omc_rs485_flag = 0;
								}else{
									ret = APUnpack((char *)pack_buf, (int)pack_buf_len, &recv_pack); 
									//printf("ret=%d.\r\n", ret);
									if(ret > 0){
										print_pack(&recv_pack);
									}
								}
								pack_status = 0;
								pack_buf_len = 0;
								memset((char *)pack_buf, 0, 1024);
							}else{
								printf("crc error.\r\n");
								pack_status = 1;
								pack_buf_len = 1;
								memset((char *)pack_buf, 0, 1024);
								pack_buf[0] = 0x7e;
							}
						}else{
							printf("len error.\r\n");
							pack_status = 1;
							pack_buf_len = 1;
							memset((char *)pack_buf, 0, 1024);
							pack_buf[0] = 0x7e;
						}
					}else{
						pack_buf[pack_buf_len++] = tmp_c;
						if(pack_buf_len > 512){
							printf("pack is too long. error.\r\n");
							pack_status = 0;
							pack_buf_len = 0;
							memset((char *)pack_buf, 0, 1024);
						}
					}
					break;
				default:
					;
			}
		}
	}

	return 0;
}

// 解析接收到的485数据
void * unprocess_pthread(void * agr)
{
	memset(pack_buf, 0, 1024);
	pack_buf_len = 0;
	pack_status = 0;
	while(1){
		check_cm_pack(&uart_recv);
		usleep(1000);
	}
	return 0;
}
extern void * gprs_pthread(void * agr);
/*
** 函数功能：创建RS485线程
** 输入参数：arg=输入参数
** 输出参数：无
** 返回值：无
** 备注：
*/
int creat_rs485_task(void)
{
	pthread_t led_id;				// RS485_LED线程ID
	pthread_t run_id;				// 运行指示线程ID
	pthread_t rs485_recv_id;
	pthread_t unprocess_id;

	if(get_device_type() == DEVICE_TYPE_EXPEND)	
	{ 
		//初始化串口，床将串口接收线程
		uart_fd = init_uart("/dev/ttyS2", 115200, 1, 8, 0);
		if(uart_fd == 0)
		{
			printf("init_uart error\n");
			return (void *)NULL;
		}

		//初始化循环队列缓冲区
		if(0 != queue_buf_init(&uart_recv, 1024))	
		{
			printf("creat queue_buf_init error.\n");
			return (void *)NULL;
		}

		//创建线程
		if(pthread_create(&rs485_recv_id, NULL, rs485_recv_pthread, NULL))
		{
			printf("pthread_creat rs485_pthread error.\n");
			return NULL;
		}
	}
	// led灯板控制
	printf("create led_pthread.\r\n");
	if( pthread_create(&led_id, NULL, led_pthread, NULL))
	{
		printf("pthread_create led_pthread error.\r\n");
		return -1;
	}
	// run指示灯
	printf("create run_pthread.\r\n");
	if( pthread_create(&run_id, NULL, run_pthread, NULL)){
		printf("pthread_create run_pthread error.\r\n");
		return -1;
	}
	if(get_device_type() == DEVICE_TYPE_EXPEND){ // 扩展485接收数据解析
		printf("create unprocess_pthread.\r\n");
		if( pthread_create(&unprocess_id, NULL, unprocess_pthread, NULL)){
			printf("pthread_create unprocess_pthread error.\r\n");
			return -1;
		}
	}
    return 0;
}
/*
读取远供电源开关状态
*/
void read_power_tw_sta(void)
{
	int i,val;
	
	for(i=0;i<8;i++)
	{
		if(1==DbGetThisIntPara(POWER_SW1_ID+i, &val))
			led_sta_power[i+8].sta = val;
	}
}