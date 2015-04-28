#include "unp.h"
#include "apue.h"
#include "errno.h"
#include "common.h"
#include "udp_relay.h"
#include "../net/netcom.h"
#include "../net/dru_connect.h"
#include "../protocol/approtocol.h"
#include "../protocol/approtocol.h"
#include "../sqlite/drudatabase.h"

extern int g_DevType;
extern unsigned long serverip;
extern unsigned long long get_dev_port_num(void);
extern int g_OMCSetParaFlag;
int g_OmcBroadcastFlag = 0;
int g_RuSetParaFlag = 0;
extern int g_OMCSetParaRs485Flag;

#define AU_PORT 30000
#define EU_RU_PORT 30001
#define AU_BOARDCAST_PORT 30002
#define EU_RU_BOARDCAST_PORT 30003

#define UPDATE_TOPOLOGIC 1
#define QUERY_CMD 2
#define SET_CMD   3

#define BUFLEN 0x400

unsigned char g_din_pw[4];  // 下行输入功率
unsigned char g_uatt[4];    // 上行衰减值
unsigned char g_din_under_alarm[4]; // 下行输入欠功率告警
unsigned char g_lo_alarm;   // 本振失锁告警
unsigned char g_power_down_alarm; // 接入单元，电源掉电告警
unsigned char g_power_error_alarm; // 接入单元，电源故障告警

int sockfd;
unsigned char recv_buf[BUFLEN]; // 接收缓冲区
unsigned char send_buf[BUFLEN]; // 发送缓冲区
volatile unsigned char m_state = IDLE; // 扩展单元数据转发状态
volatile unsigned char m_relay_state = 0;
int m_port_cnt = 0;
unsigned char m_port_buf[16];

#pragma pack (1)
struct ru_para{
	unsigned int id;
	unsigned char len;
};
#pragma pack()
struct ru_para * p_read_para_st;  // 查询参数
/*
struct ru_para read_para_st[] =  // 查询参数
{
	{0x08000060, 1}, // 远端光收发模块告警
	{0x08000220, 1}, // WLAN/固网宽带接入状态
	{0x08000500, 1}, // 远端设备温度
	{0x08000000, 1}, // 远端光收功率
	{0x08000020, 1}, // 远端光发功率
	{0x08001080, 4}, //GSM远端下行链路时延
	{0x08004080, 4}, 
	{0x08007080, 4}, 
	{0x08008080, 4}, 
	{0x08001100, 1}, //GSM远端下行输出导频功率
	{0x08004100, 1}, 
	{0x08007100, 1}, 
	{0x08008100, 1}, 
	{0x08001180, 1}, // 远端上行理论增益
	{0x08004180, 1}, 
	{0x08007180, 1}, 
	{0x08008180, 1}, 
	{0x08001200, 1},  // 远端下行实际增益
	{0x08004200, 1}, 
	{0x08007200, 1}, 
	{0x08008200, 1}, 

};
*/
struct ru_para * p_write_para_st;  // 设置参数
/*
struct ru_para write_para_st[] =  // 设置参数
{
	{0x08001280, 1},  // 远端射频信号开关
	{0x08004280, 1}, 
	{0x08007280, 1}, 
	{0x08008280, 1}, 
	{0x08001300, 1},  // 远端上行衰减值
	{0x08004300, 1}, 
	{0x08007300, 1}, 
	{0x08008300, 1}, 
	{0x08001380, 1},  // 远端下行衰减值
	{0x08004380, 1}, 
	{0x08007380, 1}, 
	{0x08008380, 1}, 
};
*/
unsigned int m_read_para_cnt;
unsigned int m_write_para_cnt;

// 自定义数据转发协议
#pragma pack (1)
struct relay_pack{
	unsigned short int len;  // 数据包长度，包括len
	unsigned long long topologic_num;  // 网络拓扑数
	unsigned char port_num;  // 端口号
	unsigned char cmd;       // 命令  02:查询  03:设置
	unsigned char buf[0];    // 零长数组，指向变长数据，由多个TLV组成
};
struct mcp_c_ltv{         // mcp_c数据格式
	unsigned char len;  // 长度
	unsigned int tag;   // 命令表示
	unsigned char v[0]; // 数据内容指针
};
#pragma pack()

#define PACK_HEAD_LEN (sizeof(struct relay_pack))
unsigned int m_pack_head_len;
// 获取转发状态
unsigned char get_relay_state(void)
{
	return m_state;
}
// 设置转发状态
void set_relay_state(unsigned char sta)
{
	m_state = sta;
}
// 组织转发包的包头
void packed_relay_head(unsigned char * tbuf, unsigned char cmd, unsigned char idx)
{
	struct relay_pack * p;
	
	p = (struct relay_pack *)tbuf;
	p->len = m_pack_head_len;  // 初始化包头长度
	p->topologic_num = get_dev_port_num();    // 填写本机拓扑数
	p->port_num = idx;       // 端口号
	p->cmd = cmd;            // 数据包功能
	return;
}
// 向转发数据包中添加LTV
void add_ltv(unsigned char * tbuf, unsigned char * ltv)
{
	struct relay_pack * p;
	struct mcp_c_ltv * from, * to; 

	p = (struct relay_pack *)tbuf;
	to = (struct mcp_c_ltv *)&(p->buf[p->len-m_pack_head_len]); // 指向缓冲区末尾，添加新的LTV
	from = (struct mcp_c_ltv *)ltv;
	to->len = from->len;              // 添加len字段
	to->tag = from->tag;              // 添加tag字段
	memcpy(to->v, from->v, from->len); // 添加value字段
	p->len += from->len;              // 修改数据包长度
}
// EU保存参数
volatile int ru_laser_error = 0; // 远端光收发模块故障
volatile int ru_rf_error = 0; // 远端射频链路故障
extern int ru_dout_pw[8][4]; // 下行输出功率
void save_mcpc_eu(unsigned char * tbuf, unsigned int len, unsigned char port_num)
{
	DevInfo_t devinfo;
	struct mcp_c_ltv * t; 
	unsigned char cnt = 0;

	memset((char *)&devinfo, 0, sizeof(devinfo));
	printf("save_mcpc_eu() \n");
	while(len > 5){
		t = (struct mcp_c_ltv *)tbuf;
		cnt = t->len;
		if(t->tag == 0x08000060){
			if(t->v[0] == 0x1){
				ru_laser_error |= (1<<port_num);
			}else{
				ru_laser_error &= ~(1<<port_num);
			}
			printf("ru_laser_error = 0x%04x\n", ru_laser_error);
		}else if(t->tag == 0x08000380){
			if(t->v[0] == 0x1){
				ru_rf_error |= (1<<port_num);
			}else{
				ru_rf_error &= ~(1<<port_num);
			}
			printf("ru_rf_error = 0x%04x\n", ru_rf_error);
		}else{
			switch(t->tag){
				case 0x08001100:
				case 0x08003100:
					ru_dout_pw[port_num][0] = t->v[0];
					break;
				case 0x08004100:
					ru_dout_pw[port_num][1] = t->v[0];
					break;
				case 0x08007100:
				case 0x08005100:
					ru_dout_pw[port_num][2] = t->v[0];
					break;
				case 0x08008100:
				case 0x08006100:
					ru_dout_pw[port_num][3] = t->v[0];
					break;
			}
			/*
			if(t->tag == 0x08001100){ // gsm下行输出功率电平
				ru_dout_pw[port_num][0] = t->v[0];
			}
			if(t->tag == 0x08004100){ // td下行输出功率电平
				ru_dout_pw[port_num][1] = t->v[0];
			}
			if(t->tag == 0x08007100){ // lte1下行输出功率电平
				ru_dout_pw[port_num][2] = t->v[0];
			}
			if(t->tag == 0x08008100){ // lte2下行输出功率电平
				ru_dout_pw[port_num][3] = t->v[0];
			}
			*/
			SetParaValue_MCP_C(&devinfo, t->tag + port_num, (char *)tbuf, 1); 
		}
		len -= cnt;
		tbuf += cnt;
	}
}
// EU读取参数
void read_mcpc_eu(unsigned char * tbuf, unsigned int len, unsigned char port_num)
{
	DevInfo_t devinfo;
	struct mcp_c_ltv * t; 
	unsigned char cnt = 0;

	memset((char *)&devinfo, 0, sizeof(devinfo));
	while(len > 0){
		t = (struct mcp_c_ltv *)tbuf;
		cnt = t->len;
		if((t->tag & 0xf) == 0){
			QueryParaValue_MCP_C(&devinfo, t->tag + port_num, (char *)tbuf); 
		}else{
			QueryParaValue_MCP_C(&devinfo, t->tag, (char *)tbuf); 
		}
		len -= cnt;
		tbuf += cnt;
	}
}
// 查询设置参数初始化
void para_init(void)
{
	DevInfo_t devinfo;

	printf("para_init start...\r\n");
	memset((char *)&devinfo, 0, sizeof(devinfo));
	m_read_para_cnt = SqliteGetReadCnt(&devinfo);
	m_write_para_cnt = SqliteGetWriteCnt(&devinfo);
	printf("m_read_para_cnt=%d.\n", m_read_para_cnt);
	printf("m_write_para_cnt=%d.\n", m_write_para_cnt);
	p_read_para_st = (struct ru_para *)malloc(m_read_para_cnt*(sizeof(struct ru_para)));
	if(p_read_para_st == NULL){
		exit(1);
	}
	p_write_para_st = (struct ru_para *)malloc(m_write_para_cnt*(sizeof(struct ru_para)));
	if(p_write_para_st == NULL){
		exit(1);
	}
	SqliteGetInitPara(&devinfo, (void *)p_read_para_st, 2);
	//printk(p_read_para_st, m_read_para_cnt*sizeof(struct ru_para));
	SqliteGetInitPara(&devinfo, (void *)p_write_para_st, 3);
	//printk(p_write_para_st, m_write_para_cnt*sizeof(struct ru_para));
	printf("para_init end.\r\n");
}
// 发送查询远端数据
void pack_relay_send(unsigned char cmd, unsigned char port_num)
{
	struct mcp_c_ltv * p;
	struct relay_pack * m;
	struct sockaddr_in to_addr;
	unsigned char tbuf[128];
	unsigned int i = 0;

	m = (struct relay_pack *)send_buf;
	memset(tbuf, 0, 128);
	packed_relay_head(send_buf, cmd, port_num); // 初始化包头
	if(cmd == QUERY_CMD){
		for(i = 0; i < m_read_para_cnt; i++){
			p = (struct mcp_c_ltv *)tbuf;
			p->len = p_read_para_st[i].len+5;
			p->tag = p_read_para_st[i].id;
			add_ltv(send_buf, tbuf);
		}
		printf("read ru ,send\n");
		//printk(send_buf, m->len);
	}else{
		printf("set ru para :%d.\n", port_num);
		for(i = 0; i < m_write_para_cnt; i++){
			p = (struct mcp_c_ltv *)tbuf;
			p->len = p_write_para_st[i].len+5;
			p->tag = p_write_para_st[i].id;
			add_ltv(send_buf, tbuf);
		}
		// 读取数据库，更新数据
		read_mcpc_eu(m->buf, m->len - m_pack_head_len, port_num); 
		//printk(send_buf, m->len);
	}
	bzero(&to_addr, sizeof(to_addr));
	to_addr.sin_family = AF_INET;
	to_addr.sin_addr.s_addr = serverip; // 主单元IP地址和端口
	to_addr.sin_port = htons(AU_PORT);
	sendto(sockfd, send_buf, m->len, 0, (struct sockaddr *)&to_addr, sizeof(to_addr));
	set_relay_state(BUSY); // 设置转发状态为BUSY
}
// 更新远端拓扑图
void read_topologic(void)
{
	struct relay_pack * m;
	struct sockaddr_in to_addr;
	int ret = 0;

	m = (struct relay_pack *)send_buf;
	packed_relay_head(send_buf, UPDATE_TOPOLOGIC, 0);
	m->len += 17;
	bzero(&to_addr, sizeof(to_addr));
	to_addr.sin_family = AF_INET;
	to_addr.sin_addr.s_addr = serverip; // 主单元IP地址和端口
	to_addr.sin_port = htons(AU_PORT);
	ret = sendto(sockfd, send_buf, m->len, 0, (struct sockaddr *)&to_addr, sizeof(to_addr));
	set_relay_state(BUSY); // 设置转发状态为BUSY
	//printf("fd:%d, ip16=0x%lx, port=%d. \r\n", sockfd, serverip, AU_PORT);
	if(ret < 0){
		printf("send_to error!!!!!!!!!!!!!!\r\n");
	}else{
		//printf("send_to ok. len = %d\r\n", ret);
		//printk((char *)send_buf, m->len);
	}
}
// 获取接收到的数据
// len: 输出参数，返回有效数据的长度
unsigned char * get_recv_relay(unsigned int * len)
{
	struct relay_pack * p;

	p = (struct relay_pack *)recv_buf;
	*len = p->len - m_pack_head_len;

	return p->buf;
}

//
// 远端应答
void response_relay_ru(void)
{
	struct relay_pack * sp, * rp;
	struct mcp_c_ltv * t; 
	unsigned char * p_tmp;
	DevInfo_t devinfo;
	unsigned int len = 0;
	struct sockaddr_in to_addr;
	char cbuf[128];

	memset((char *)&devinfo, 0, sizeof(devinfo));
	sp = (struct relay_pack *)send_buf;
	rp = (struct relay_pack *)recv_buf;
	memcpy(send_buf, recv_buf, m_pack_head_len);
	memcpy(sp->buf, rp->buf, rp->len - m_pack_head_len);
	sp->topologic_num = get_dev_port_num();    // 填写本机拓扑数
	if(sp->cmd == QUERY_CMD){ // 查询命令
		len = sp->len - m_pack_head_len;
		p_tmp = sp->buf;
		while(len > 0){
			t = (struct mcp_c_ltv *)p_tmp;
			QueryParaValue_MCP_C(&devinfo, t->tag, (char *)p_tmp);
			len -= t->len;
			p_tmp += t->len;
		}
	}else if(sp->cmd == SET_CMD){  // 设置命令
		len = sp->len - m_pack_head_len;
		p_tmp = sp->buf;
		printf("response_relay_ru()\n");
		while(len > 5){
			printf("len=%d\n", len);
			t = (struct mcp_c_ltv *)p_tmp;
			QueryParaValue_MCP_C(&devinfo, t->tag, cbuf);
			if(memcmp(&cbuf[5], t->v, t->len) != 0){ 
				SetParaValue_MCP_C(&devinfo, t->tag, (char *)p_tmp, 0); 
			}
			len -= t->len;
			p_tmp += t->len;
		}
	}
	bzero(&to_addr, sizeof(to_addr));
	to_addr.sin_family = AF_INET;
	to_addr.sin_addr.s_addr = serverip; // 主单元IP地址和端口
	to_addr.sin_port = htons(AU_PORT);
	sendto(sockfd, send_buf, sp->len, 0, (struct sockaddr *)&to_addr, sizeof(to_addr));
}
// EU接收到的数据处理
void eu_deal(void)
{
	struct relay_pack * rp;
	DevInfo_t devinfo;
	int i, j;
	unsigned char cbuf[32];
	struct mcp_c_ltv * p = cbuf;
	//char tmp = 0;
	int flag = 0;	
	int cnt = 0;
	//int objectid = 0;

	memset((char *)&devinfo, 0, sizeof(devinfo));
	rp = (struct relay_pack *)recv_buf;
	if(rp->cmd == QUERY_CMD){ // 查询命令
		printf("eu_deal save_mcpc_eu\n");
		//printk(recv_buf, rp->len);
		save_mcpc_eu(rp->buf, rp->len - m_pack_head_len, rp->port_num);
	}else if(rp->cmd == SET_CMD){  // 设置命令
		
	}else if(rp->cmd == UPDATE_TOPOLOGIC){
		m_port_cnt = rp->buf[0];
		memset(m_port_buf, 0, 16);
		memcpy(m_port_buf, rp->buf+1, rp->buf[0]);
		for(i = 0; i < 8; i++){
			p->len = 6;
			p->tag = RU_ACCESS_STATE_ID+i; 
			QueryParaValue_MCP_C(&devinfo, p->tag, cbuf); 
			printf("\n\nid=0x%08x, ru_access_state[%d]=%d\n", p->tag, i, cbuf[5]);
			//printk(cbuf,6);
			if(cbuf[5] != 0){ // 0:表示未使用，其他自动设置
				flag = 0;
				for(j = 0; j < m_port_cnt; j++){	
					if(m_port_buf[j] == i){ // 在位
						flag = 1;
						//printf("port[%d] is online\n", i);
						break;
					}
				}
				if(flag == 1){ 
					cnt = SqliteGetAlarmCnt(i);
					//printf("alarm_cnt=%d\n", cnt);
					if(cnt != 0){
						//printf("port[%d] alarm\n", i);
						p->v[0] = 2; //2: 在位异常
						SetParaValue_MCP_C(&devinfo, p->tag, (char *)cbuf, 1); 
					}else{
						//printf("port[%d] ok\n", i);
						p->v[0] = 1; // 1:正常
						SetParaValue_MCP_C(&devinfo, p->tag, (char *)cbuf, 1); 
					}
				}else{ // 3:不在位
					//printf("port[%d] is outline\n", i);
					p->v[0] = 3;
					SetParaValue_MCP_C(&devinfo, p->tag, (char *)cbuf, 1); 
				}
			}
		}
	}
}
// 服务程序处理
void server(int sockfd)
{
	int n;
	struct sockaddr_in from_addr, to_addr;
	socklen_t len = sizeof(struct sockaddr_in);
	unsigned int ip;
	struct relay_pack * p;
	//int i = 0;
	
	while(1){
		len = sizeof(struct sockaddr_in);
		if (( n = recvfrom(sockfd, recv_buf, BUFLEN, 0, (struct sockaddr * )&from_addr, &len)) < 0){
			printf(" recv udprelay error: %s\r\n", strerror(errno));
		}
		//printf("recv_buf %d.\n", n);
		//printk((char *)recv_buf, n);
		if(g_DevType == MAIN_UNIT){ // 主单元转发数据 
			memcpy((unsigned char *)&to_addr, (unsigned char *)&from_addr, len);
			p = (struct relay_pack *)recv_buf;
			if(p->cmd == UPDATE_TOPOLOGIC){
				p->buf[0] = get_child_port(p->topologic_num, &(p->buf[1]));
				//printf("query topologic: count=%d\n", p->buf[0]);
				//printf("port is ");
			//	for(i = 0; i < p->buf[0]; i++){
			//		printf("%d ", p->buf[i+1]);
			//	}
				//printf("\n");
				sendto(sockfd, recv_buf, n, 0, (struct sockaddr *)&from_addr, len);
			}else{
				ip = get_ip_by_topologic(p->topologic_num, p->port_num);
				if(ip != 0){
					bzero(&to_addr, sizeof(to_addr));
					to_addr.sin_family = AF_INET;
					to_addr.sin_addr.s_addr = ip;
					to_addr.sin_port = htons(EU_RU_PORT);
					sendto(sockfd, recv_buf, n, 0, (struct sockaddr *)&to_addr, len);
				}else{
					printf("get_ip_by_topologic error\n");
				}
			}
		}else if (g_DevType == EXPAND_UNIT){ // 扩展接收到远端的应答
			eu_deal();      // 写入数据库
		}else if (g_DevType == RAU_UNIT){ // 远端接收到查询信息
			response_relay_ru();
		}
	}
}
// server init
int initserver(int type, const struct sockaddr * addr, socklen_t alen, int qlen, int udp_type)
{
	int fd, err;
	int reuse = 1;

	if ((fd = socket(addr->sa_family, type, 0)) < 0)
		return (-1);
	if (setsockopt(fd, SOL_SOCKET, SO_REUSEADDR, &reuse, sizeof(int)) < 0){
		err = errno;
		goto errout;
	}
	if(udp_type == 1){ // udp 广播方式
		if( setsockopt(fd, SOL_SOCKET, SO_BROADCAST, &reuse, sizeof(int)) < 0 ){
			err = errno;
			goto errout;
		}
	}
	if (bind(fd, addr, alen) < 0) {
		err = errno;
		goto errout;
	}
	if (type == SOCK_STREAM || type == SOCK_SEQPACKET) {
		if (listen(fd, qlen) < 0) {
			err = errno;
			goto errout;
		}
	}
	return (fd);

errout:
	close(fd);
	errno = err;
	return (-1);
}
/*
** 函数功能：udp转发
** 输入参数：无
** 输出参数：无
** 返回值：无
** 备注：
*/
void * udp_relay_pthread(void * arg)
{
	struct sockaddr_in servaddr;
	unsigned int ip;
	int port = 0;

	if(g_DevType == MAIN_UNIT){
		port = AU_PORT;
	}else{
		port = EU_RU_PORT;
	}
	bzero(&servaddr, sizeof(servaddr));
	servaddr.sin_family = AF_INET;
	servaddr.sin_port = htons(port);
	ip = GetSelfIp("eth0");
	if(ip == 0 ){
		printf("get eth0:1 ip error \r\n");
	}
	servaddr.sin_addr.s_addr = ip;
	if ((sockfd = initserver(SOCK_DGRAM, (struct sockaddr *)&servaddr, sizeof(struct sockaddr), 0, 0)) >= 0) {
		printf("udp relay socket fd:%d, ip=%d.%d.%d.%d port=%d.\r\n", sockfd, ip&0xff, (ip>>8)&0xff, (ip>>16)&0xff, (ip>>24)&0xff, port);
		server(sockfd);
	}
	return (void *)0;
}
void * udp_read_pthread(void * arg)
{
	int read_idx = 0;
	int write_idx = 0;
	
	while(1){
		if (g_RuSetParaFlag == 1){
			g_RuSetParaFlag = 0;
			m_relay_state = SET_CMD;
		}
		switch (m_relay_state) {
			case UPDATE_TOPOLOGIC:  //  更新远端拓扑图
				//printf("read topologic...\r\n");
				read_topologic();	
				m_relay_state = QUERY_CMD;
				break;
			case QUERY_CMD:  // 轮寻远端设备参数
				if((m_port_cnt > 0)&&(read_idx < m_port_cnt)){
					pack_relay_send(QUERY_CMD, m_port_buf[read_idx]);	
					read_idx++;
				}else{
					m_relay_state = UPDATE_TOPOLOGIC;
					read_idx = 0;
				}
				break;
			case SET_CMD:  // 设置参数
				if((m_port_cnt > 0)&&(write_idx < m_port_cnt)){
					pack_relay_send(SET_CMD, m_port_buf[write_idx]);	
					write_idx++;
				}else{
					m_relay_state = UPDATE_TOPOLOGIC;
					write_idx = 0;
				}
				break;
			default:
				m_relay_state = UPDATE_TOPOLOGIC;

		}
		sleep(3);
	}
	return (void *)0;
}
extern void delay_measure_start(void);
unsigned char b_sendbuf[BUFLEN];
unsigned char b_recvbuf[BUFLEN];
void * udp_broadcast_cli_pthread(void * arg)
{
	struct sockaddr_in servaddr;
	int fd;
	int ret;
	char ipstr[20];
	int er = 1;
	DevInfo_t devinfo;
	int cnt;
	//unsigned short int len;

	bzero(&servaddr, sizeof(servaddr));
	servaddr.sin_family = AF_INET;
	servaddr.sin_port = htons(AU_BOARDCAST_PORT);
	memset(ipstr, 0, 20);
	strcpy(ipstr, "192.168.10.255"); 
	ret = inet_pton(AF_INET, ipstr, &servaddr.sin_addr); 
	if(ret < 0){
		printf("inet_pton error for %s\n", ipstr);
	}	
	fd = socket(AF_INET, SOCK_DGRAM, 0);
	if(fd < 0){
		printf("socket error\n");
	}
	setsockopt(fd, SOL_SOCKET, SO_REUSEADDR, &er, sizeof(er));
	setsockopt(fd, SOL_SOCKET, SO_BROADCAST, &er, sizeof(er));
	while(1){
		if(g_OmcBroadcastFlag == 1){
			g_OmcBroadcastFlag = 0;
			printf("send_broadcast\n");
			memset((void *)&devinfo, 0, sizeof(devinfo));
			cnt = SqliteGetBPara(&devinfo, b_sendbuf+3);
			printf("sqlitegetbpara_cnt=%d\n", cnt);
			cnt += 3;
			memcpy(b_sendbuf, &cnt, 2);
			//printf("cnt=%d\n", cnt);
			b_sendbuf[2] = 0x01; // 广播设置参数
			ret = sendto(fd, b_sendbuf, cnt, 0, (struct sockaddr *)&servaddr, sizeof(servaddr));
			if(ret != cnt){
				printf("sendto error\n");
			}
			//printf("\n\n>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>\n");
			printf("send_broadcast: 1, %d\n", cnt);
			//printk(b_sendbuf, cnt);
		}else{
			memset((void *)&devinfo, 0, sizeof(devinfo));
			cnt = SqliteGetStPara(&devinfo, b_sendbuf+3);
			cnt += 3;
			memcpy(b_sendbuf, &cnt, 2);
			b_sendbuf[2] = 0x04; // 广播状态参数,不需要设置
			ret = sendto(fd, b_sendbuf, cnt, 0, (struct sockaddr *)&servaddr, sizeof(servaddr));
			if(ret != cnt){
				printf("sendto error\n");
			}
			//printf("\n\nstate>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>\n");
			//printf("send_broadcast: 4, %d\n", cnt);
			//printk(b_sendbuf, cnt);
		}
	  //delay_measure_start();	
		sleep(5);
	}

	return (void *)0;
}
void * udp_broadcast_serv_pthread(void * arg)
{
	struct sockaddr_in cliaddr;
	int fd;
	int ret;
	socklen_t len;
	struct sockaddr recvaddr;
	int n = 0;
	int er = 1;
	unsigned short cnt = 0;
	unsigned char * p_tmp;
	struct mcp_c_ltv * t;
	DevInfo_t devinfo;
	char cbuf[128];

	fd = socket(AF_INET, SOCK_DGRAM, 0);
	if(fd < 0){
		printf("socket error\n");
	}
	bzero(&cliaddr, sizeof(cliaddr));
	cliaddr.sin_family = AF_INET;
	cliaddr.sin_port = htons(AU_BOARDCAST_PORT);
	cliaddr.sin_addr.s_addr = htonl(INADDR_ANY);
	setsockopt(fd, SOL_SOCKET, SO_REUSEADDR, &er, sizeof(er));
	ret = bind(fd, (struct sockaddr *)&cliaddr, sizeof(cliaddr));
	if(ret < 0){
		printf("bind error\n");
	}
	for(;;){
		len = sizeof(recvaddr);
		n = recvfrom(fd, b_recvbuf, 0x400, 0, &recvaddr, &len);
		if( n < 0 ){
			printf("recvfrom error\n");
		}
		//printf("b_recvbuf:%d\n", n);
		//printk(b_recvbuf, n);
		cnt = *(unsigned short *)b_recvbuf;
		p_tmp = b_recvbuf+3;
		if(b_recvbuf[2] == 0x01){ // 广播设置参数
			while(cnt > 5){
				//printf("cnt=%d\n", cnt);
				t = (struct mcp_c_ltv *)p_tmp;
				QueryParaValue_MCP_C(&devinfo, t->tag, cbuf);
				if(memcmp(&cbuf[5], t->v, t->len) != 0){ 
					SetParaValue_MCP_C(&devinfo, t->tag, (char *)p_tmp, 0); 
				}
				cnt -= t->len;
				p_tmp += t->len;
			}
			g_OMCSetParaFlag = 1;
			g_OMCSetParaRs485Flag = 1;
		}else if(b_recvbuf[2] == 0x04){ // 广播接入单元状态参数
			while(cnt > 5){
				t = (struct mcp_c_ltv *)p_tmp;
				switch(t->tag){
					case 0x08001001:
					case 0x08003001:
						g_din_pw[0] = t->v[0];
						break;
					case 0x08004001:
						g_din_pw[1] = t->v[0];
						break;
					case 0x08007001:
					case 0x08005001:
						g_din_pw[2] = t->v[0];
						break;
					case 0x08008001:
					case 0x08006001:
						g_din_pw[3] = t->v[0];
						break;
					case 0x08001005:
					case 0x08003005:
						g_uatt[0] = t->v[0];
						break;
					case 0x08004005:
						g_uatt[1] = t->v[0];
						break;
					case 0x08007005:
					case 0x08005005:
						g_uatt[2] = t->v[0];
						break;
					case 0x08008005:
					case 0x08006005:
						g_uatt[3] = t->v[0];
						break;
					case 0x0800100e:
					case 0x0800300e:
						g_din_under_alarm[0] = t->v[0];
						break;
					case 0x0800400e:
						g_din_under_alarm[1] = t->v[0];
						break;
					case 0x0800700e:
					case 0x0800500e:
						g_din_under_alarm[2] = t->v[0];
						break;
					case 0x0800800e:
					case 0x0800600e:
						g_din_under_alarm[3] = t->v[0];
						break;
					case 0x00000309:
						g_lo_alarm =  t->v[0];
						break;
					case 0x00000301:
						g_power_down_alarm = t->v[0];
						break;
					case 0x00000302:
						g_power_error_alarm = t->v[0];
						break;
				}
				cnt -= t->len;
				p_tmp += t->len;
			}
		}
	}

	return (void *)0;
}

extern void get_dev_route_addr(void);
/*
** 函数功能：创建创建udp转发线程
** 输入参数：无
** 输出参数：无
** 返回值：无
** 备注：
*/
int creat_udp_relay(void)
{
	pthread_t udp_relay_id;				// udp转发线程ID
	pthread_t udp_read_id;	
	pthread_t udp_broadcast_serv_id;   
	pthread_t udp_broadcast_cli_id;

	sleep(20);
	// 查询设备路由登记地址
	get_dev_route_addr();
	m_pack_head_len = PACK_HEAD_LEN;
	para_init();
	// udp转发线程 
	printf("create udp_relay_pthread.\r\n");
	if( pthread_create(&udp_relay_id, NULL, udp_relay_pthread, NULL)){
		printf("pthread_create udp_relay_pthread error.\r\n");
		return -1;
	}
	if (g_DevType == EXPAND_UNIT){ // 扩展接收到远端的应答
		// udp轮寻线程
		printf("create udp_read_pthread.\r\n");
		if( pthread_create(&udp_read_id, NULL, udp_read_pthread, NULL)){
			printf("pthread_create udp_read_pthread error.\r\n");
			return -1;
		}
	}
	if(g_DevType == MAIN_UNIT){ // 主单元广播参数线程
		printf("create udp_broadcast_cli_pthread.\r\n");
		if( pthread_create(&udp_broadcast_cli_id, NULL, udp_broadcast_cli_pthread, NULL)){
			printf("pthread_create udp_broadcast_cli_pthread error.\r\n");
			return -1;
		}
	}else{ // 远端\扩展接收广播参数
		printf("create udp_broadcaset_serv_pthread.\r\n");
		if( pthread_create(&udp_broadcast_serv_id, NULL, udp_broadcast_serv_pthread, NULL)){
			printf("pthread_create udp_broast_serv_pthread error.\r\n");
			return -1;
		}
	}
	return 0;
}
