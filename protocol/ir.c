#include <stdlib.h>
#include <stdio.h>
#include <errno.h>
#include <string.h>
#include <unistd.h>
#include <netdb.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <sys/types.h>
#include <arpa/inet.h>
#include "ir_def.h"
#include "../common/status.h"
#include "../common/str_deal.h"
#include "../common/common.h"
#include "../common/type_def.h"
#include "../net/dru_connect.h"
#include "../net/socketcommon.h"
#include "ir.h"
#include "../common/druheader.h"
#include "../common/drudefstruct.h"
volatile int tim_delay_config_flag=0;//2014.05.05
extern const char * g_ver;
extern ComBuf_t g_OMC_IrTranCom;
extern ComBuf_t g_Ir_OMCTranCom;
extern volatile int current_link_dev;
extern volatile unsigned char client_time;
extern volatile connect_record_t connect_buffer[MAX_CONNECT_NUMBER];
//define the message serial numbe
//用于生成序列号
volatile U32 current_serial_number=0;
U32 create_serial_number(void)
{
	current_serial_number+=1;
	return current_serial_number;
}
/*
**define the used ie 
**if you want to config one or more parameter 
**you must set the ie symbol into this buffer
**then call the deal funcation for the massage number
**ACEWAY H4 2012.11.27
*/

volatile use_ie use_ie_buf[MAX_IE_BUFFER];
//clean the use buffer 
//you may call it before you use the buffer
void clean_use_buffer(void)
{
	U16 i;
	for(i=0;i<MAX_IE_BUFFER;i++)
	{
		use_ie_buf[i].is_used=IE_UN_USE;
		use_ie_buf[i].ie_symbol=IE_SYMBOL_NULL;
	}
}
///////////////////////////////////////////////////////
//ie处理函数用于存储处理完的ie等待最后的读取和发送
//将处理完的ie结构的指针和长度存于ie_buffer当要发送时
//可以由get_ie_buffer函数取得所有的数据和总长度
//为了避免多线程带来的冲突。
//clean_ie_buffer将一个ie buffer清空
//add_ie_buffer 将一个ie的地址和长度加入buffer
//get_ie_buffer 获取整个buffer的地址和长度
volatile ie_addr ie_buf[MAX_THREAD][MAX_IE_BUFFER];
///////////////////////////////////////////////////////
void clean_ie_buffer(U16 index)
{
	U16 i;
	for(i=0;i<MAX_IE_BUFFER;i++){
		ie_buf[index][i].paddr=NULL;
		ie_buf[index][i].length=0;
	}
}
U8 add_ie_buffer(U16 index,U8 *p,U16 length)
{
	U16 i;
	for(i=0;i<MAX_IE_BUFFER;i++){
		if((ie_buf[index][i].paddr==NULL)&&(ie_buf[index][i].length==0)){
			ie_buf[index][i].length=length;
			ie_buf[index][i].paddr=p;
			return 0;
		}
	}
	return 1;
}
U8 get_ie_buffer(U16 index,U8 *outbuf,U16 *length)
{
	U8 *p;
	U16 i,le;
	p=outbuf;
	le=0;
	for(i=0;i<MAX_IE_BUFFER;i++){
		if(ie_buf[index][i].paddr!=NULL){
			le+=ie_buf[index][i].length;
			if((le==0)||(le>MAX_SEND_LENGTH)){
				return 1;
			}else{
				memcpy(p,ie_buf[index][i].paddr,ie_buf[index][i].length);
				p+=ie_buf[index][i].length;
			}
		}else{
			if(le!=0){
				*length=le;
				return 0;
			}else{
				return 1;
			}
		}
	}
	return 0;
}
thread_struct dru_thread_struct={
	.status = THREAD_NULL, 
	.p_id = 0,
};
///////////////////////////////////////////////////////
//消息编号和消息处理函数对应表
function function_buf[]={
{RRU_STATUS_REQ,state_req_deal},
{RRU_PARAMETER_REQ,parameter_req_deal},
{RRU_TO_BBU,transimt_rsp_deal},
{BBU_TO_RRU,transimt_rsp_deal},
{BBU_ALIVE_MSG,timing_rru_deal},
{RRU_ALIVE_MSG,timing_bbu_deal},
{CREATE_CHANNEL_REQ,create_ch_req_deal},
{CREATE_CHANNEL_CFG,create_ch_cfg_deal},
{CREATE_CHANNEL_CFG_RSP,create_ch_cfg_rsp_deal},
{VERSION_DOWNLOARD_RESULT_DIS,update_result_deal},
{TIME_DELAY_MEASURE_REQ,delay_measure_req_deal},
{RRU_PARAMETER_CFG,parameter_config_deal},
{RRU_PARAMETER_CFG_RSP,parameter_config_deal_rsp},
{TIME_DELAY_MEASURE_RSP,delay_mearsure_rsp_deal},
{TIME_DELAY_CFG,delay_time_config_deal},
{TIME_DELAY_CFG_RSP,delay_time_config_rsp_deal},
};
//取得对应表的长度。
U16 get_sizeof_fbuffer(void)
{
	return sizeof(function_buf)/sizeof(function);
}
///////////////////////////////////////////////////////////
//???
//----------------参数定义-----------------//
/****************通道建立请求***********/
rru_mark dru_rru_mark={
	.ie_symbol	=	1,
	.ie_length	=	100,
};
create_channel_reason dru_create_channel_reason={
	.ie_symbol	=	2,
	.ie_length	=	9,
};
rru_series dru_rru_series={
	.ie_symbol	=	4,
	.ie_length	=	69,
};
rru_hardware_info dru_rru_hardware_info={
	.ie_symbol	=	5,
	.ie_length	=	52,
};
rru_software_info dru_rru_software_info={
	.ie_symbol	=	6,
	.ie_length	=	84,
};
/***************通道建立配置************/
sys_time dru_sys_time={
	.ie_symbol	=	11,
	.ie_length	=	11,
	.sec		=	0,
	.min		=	0,
	.hour	=	0,
	.date	=	1,
	.mon	=	1,
	.year	=	2012,
};
ftp_addr dru_ftp_addr={
	.ie_symbol	=	12,
	.ie_length	=	8,
};
rru_mode dru_rru_mode={
	.ie_symbol	=	13,
	.ie_length	=	8,
};
software_version_result dru_software_version_result={
	.ie_symbol	=	14,
	.ie_length	=	289,
};
ir_mode_config dru_ir_mode_config={
	.ie_symbol	=	504,
	.ie_length	=	8,
};
/***************通道建立配置应答******/
create_channel_rsp dru_create_channel_rsp={
	.ie_symbol	=	21,
	.ie_length	=	8,
};
/***************版本更新结果************/
update_version_result dru_update_version_result={
	.ie_symbol	=	31,
	.ie_length	=	9,
};
/**************版本下载响应*************/
downloard_version_rsp dru_downloard_version_rsp={
	.ie_symbol	=	101,
	.ie_length	=	9,
};
/**************版本传输完成指示*******/
downloard_result dru_downloard_result={
	.ie_symbol	=	111,
	.ie_length	=	9,
};
/**************版本激活应答*************/
software_activate_rsp dru_software_activate_rsp={
	.ie_symbol	=	201,
	.ie_length	=	9,
};
/*************状态查询********************/
//射频通道状态
fchannel_state_req dru_fchannel_state_req={
	.ie_symbol	=	302,
	.ie_length	=	5,
};
//载波状态
carrier_wave_req dru_carrier_wave_req={
	.ie_symbol	=	303,
	.ie_length	=	4,
};
//本振状态
local_osc_req dru_local_osc_req={
	.ie_symbol	=	304,
	.ie_length	=	4,
};
//时钟状态
clock_req dru_clock_req={
	.ie_symbol	=	305,
	.ie_length	=	4,
};
//rru运行状态
run_state_req dru_run_state_req={
	.ie_symbol	=	306,
	.ie_length	=	4,
};
//ir工作模式
ir_mode_req dru_ir_mode_req={
	.ie_symbol	=	307,
	.ie_length	=	4,
};
//初始化校准结果
init_result_req dru_init_result_req={
	.ie_symbol	=	308,
	.ie_length	=	4,
};
//光口信息查询
light_info_req dru_light_info_req={
	.ie_symbol	=	309,
	.ie_length	=	5,
};
/***************rru状态响应**************************/
//射频通路状态响应
fchannel_state_rsp dru_fchannel_state_rsp={
	.ie_symbol	=	352,
	.ie_length	=	13,
};
//载波状态响应
carrier_wave_rsp dru_carrier_wave_rsp={
	.ie_symbol	=	353,
	.ie_length	=	10,
};
//本振状态响应
local_osc_rsp dru_local_osc_rsp={
	.ie_symbol	=	354,
	.ie_length	=	12,
};
//时钟状态响应
clock_rsp dru_clock_rsp={
	.ie_symbol	=	355,
	.ie_length	=	8,
};
//rru运行状态响应
run_state_rsp dru_run_state_rsp={
	.ie_symbol	=	356,
	.ie_length	=	8,
};
//Ir口工作模式查询响应
ir_mode_rsp	dru_ir_mode_rsp={
	.ie_symbol	=	357,
	.ie_length	=	8,
};
//初始化校准结果响应
init_result_rsp dru_init_result_rsp={
	.ie_symbol	=	358,
	.ie_length	=	20,
};
//光口信息查询响应
light_info_rsp dru_light_info_rsp={
	.ie_symbol	=	358,
	.ie_length	=	20,
};
/***************参数查询**************************/
//系统时间查询
sys_time_req dru_sys_time_req={
	.ie_symbol	=	401,
	.ie_length	=	4,
};
//cpu 占用率查询
cpu_occ_req dru_cpu_occ_req={
	.ie_symbol	=	402,
	.ie_length	=	4,
};
//cpu占用率查询周期
cpu_cycle_req dru_cpu_cycle_req={
	.ie_symbol	=	403,
	.ie_length	=	4,
};
//温度查询
rru_temp_req dru_rru_temp_req={
	.ie_symbol	=	404,
	.ie_length	=	8,
};
//驻波比状态查询
rru_swr_req dru_rru_swr_req={
	.ie_symbol	=	405,
	.ie_length	=	5,
};
//驻波比门限查询
swr_limit_req dru_swr_limit_req={
	.ie_symbol	=	406,
	.ie_length	=	4,//协议中为5但未定义
};
//过温门限查询
temp_limit_req dru_temp_limit_req={
	.ie_symbol	=	407,
	.ie_length	=	4,
};
//输出功率查询
out_capy_req dru_out_capy_req={
	.ie_symbol	=	408,
	.ie_length	=	5,
};
//状态机查询
state_machine_req dru_state_machine_req={
	.ie_symbol	=	409,
	.ie_length	=	4,
};
/***************参数查询响应**************************/
//cpu 占用率响应
cpu_occ_rsp dru_cpu_occ_rsp={
	.ie_symbol	=	451,
	.ie_length	=	8,
};
//cpu 占用率查询周期响应
cpu_cycle_rsp dru_cpu_cycle_rsp={
	.ie_symbol	=	452,
	.ie_length	=	8,
};
//温度查询响应
rru_temp_rsp dru_rru_temp_rsp={
	.ie_symbol	=	453,
	.ie_length	=	13,
};
//驻波比状态查询响应
rru_swr_rsp dru_rru_swr_rsp={
	.ie_symbol	=	454,
	.ie_length	=	9,
};
//驻波比门限响应
swr_limit_rsp dru_swr_limit_rsp={
	.ie_symbol	=	455,
	.ie_length	=	12,
};
//过温门限响应
temp_limit_rsp dru_temp_limit_rsp={
	.ie_symbol	=	456,
	.ie_length	=	12,
};
//输出功率查询响应
out_capy_rsp dru_out_capy_rsp={
	.ie_symbol	=	457,
	.ie_length	=	7,
};
//状态机查询响应
state_machine_rsp dru_state_machine_rsp={
	.ie_symbol	=	458,
	.ie_length	=	5,
};
/***************参数配置**************************/
//iq数据通道配置
iq_config dru_iq_config={
	.ie_symbol	=	501,
	.ie_length	=	8,
};
//cpu占用率统计周期配置
cpu_scycle_config dru_cpu_scycle_config={
	.ie_symbol	=	502,
	.ie_length	=	8,
};
//驻波比门限配置
swr_limit_config dru_swr_limit_config={
	.ie_symbol	=	503,
	.ie_length	=	12,
};
//过温门限配置
temp_limit_config dru_temp_limit_config={
	.ie_symbol	=	505,
	.ie_length	=	16,
};
//rru 级联功能配置
connection_config dru_connection_config={
	.ie_symbol	=	506,
	.ie_length	=	8,
};
/***************参数配置响应**************************/
//系统时间配置响应
sys_time_rsp dru_sys_time_rsp={
	.ie_symbol	=	551,
	.ie_length	=	8,
};
//iq数据通道配置响应
iq_config_rsp dru_iq_config_rsp={
	.ie_symbol	=	552,
	.ie_length	=	8,
};
//cpu占用率周期配置响应
cpu_scycle_config_rsp dru_cpu_scycle_config_rsp={
	.ie_symbol	=	553,
	.ie_length	=	8,
};
//驻波比门限配置响应
swr_limit_config_rsp dru_swr_limit_config_rsp={
	.ie_symbol	=	554,
	.ie_length	=	8,
};
//ir工作模式配置响应
ir_mode_config_rsp dru_ir_mode_config_rsp={
	.ie_symbol	=	555,
	.ie_length	=	10,
};
//过温门限配置响应
temp_limit_config_rsp dru_temp_limit_config_rsp={
	.ie_symbol	=	556,
	.ie_length	=	12,
};
//射频通道状态配置响应
fchannel_state_config_rsp dru_fchannel_state_config_rsp={
	.ie_symbol	=	557,
	.ie_length	=	9,
};
//rru级联功能配置响应
connection_config_rsp dru_connection_config_rsp={
	.ie_symbol	=	558,
	.ie_length	=	8,
};
/***************时延测量请求**************************/
//周期性时延测量请求
time_delay_req dru_time_delay_req={
	.ie_symbol	=	901,
	.ie_length	=	5,//协议为4
};
//时延测量请求响应
time_delay_rsp dru_time_delay_rsp={
	.ie_symbol	=	911,
	.ie_length	=	21,
};
//时延配置
time_delay_config dru_time_delay_config={
	.ie_symbol	=	921,
	.ie_length	=	17,
};
//时延配置结果响应
time_delay_config_rsp dru_time_delay_config_rsp={
	.ie_symbol	=	931,
	.ie_length	=	6,
};
/***************告警**************************/
//告警上报
alarm_rep dru_alarm_rep={
	.ie_symbol	=	1001,
	.ie_length	=	138,
};
//告警查询请求
alarm_req dru_alarm_req={
	.ie_symbol	=	1101,
	.ie_length	=	12,
};
/***************透传**************************/
//透传目标
transimt_aim dru_transimt_aim={
	.ie_symbol	=	1601,
	.ie_length	=	5,
};
//透传内容
transmit_content dru_transmit_tx={
	.ie_symbol	=	1602,
	.ie_length	=	1028,
};
transmit_content dru_transmit_rx={
	.ie_symbol	=	1602,
	.ie_length	=	1028,
};
//funcation
/////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////
/*
**
**			通道建立处理函数
**
**
*/
////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////
/*******************************************************************************
* 函数名称: ir_data_init
* 功    能:初始化ir协议数据结构
* 参    数:
* 参数名称         类型                描述
* 返回值:
* 
* 说   明:
* 日   期     版本    作者   修改人      DEBUG
* -----------------------------------------------------------------------------
* 2012/11/19  V1.0     H4     无       ：
*******************************************************************************/
void ir_data_init(void)
{
	
}
/*******************************************************************************
* 函数名称: create_ch_req
* 功    能:通道建立请求处理函数
* 参    数:
* 参数名称         类型                描述
* 返回值:
* 
* 说   明:
* 日   期     版本    作者   修改人      DEBUG
* -----------------------------------------------------------------------------
* 2012/11/19  V1.0     H4     无       ：
*******************************************************************************/
void resolve_package(U8 *inbuf)
{
	U16 i;
	struct msg_head_t *pmh=((struct msg_head_t*)inbuf);
	DEBUG_PRINTF("resolve_package %d %d\n",(int)pmh->msg_number,(int)pmh->msg_length);
	for(i=0;i<get_sizeof_fbuffer();i++){
		if(pmh->msg_number==function_buf[i].number){
			function_buf[i].deal(inbuf);
			break;
		}
	}
	if(i>=get_sizeof_fbuffer()){
		DEBUG_PRINTF("no such message \n");
	}
}
/*******************************************************************************
* 函数名称: create_ch_req
* 功    能:通道建立请求函数
* 参    数:
* 参数名称         类型                描述
* 返回值:
* 
* 说   明:
* 日   期     版本    作者   修改人      DEBUG
* -----------------------------------------------------------------------------
* 2012/11/19  V1.0     H4     无       ：
*******************************************************************************/
void create_ch_req(void)
{
	msg_head mh;
	U8 outbuf[MAX_SEND_LENGTH];
	U8 *pbuf;
	pbuf=outbuf;
	mh.BBU_ID=0;
	mh.light_port_number=1;
	mh.serial_number=1;
	mh.msg_number=CREATE_CHANNEL_REQ;
	mh.msg_length=sizeof(mh)+dru_rru_mark.ie_length+dru_create_channel_reason.ie_length+\
				  dru_rru_series.ie_length+dru_rru_hardware_info.ie_length+dru_rru_software_info.ie_length;
	memcpy(pbuf,(U8 *)&mh,sizeof(mh));
	pbuf=pbuf+sizeof(mh);
	memcpy(pbuf,(U8 *)&dru_rru_mark,sizeof(dru_rru_mark));
	pbuf=pbuf+sizeof(dru_rru_mark);
	memcpy(pbuf,(U8 *)&dru_create_channel_reason,sizeof(dru_create_channel_reason));
	pbuf=pbuf+sizeof(dru_create_channel_reason);
	memcpy(pbuf,(U8 *)&dru_rru_series,sizeof(dru_rru_series));
	pbuf=pbuf+sizeof(dru_rru_series);
	memcpy(pbuf,(U8 *)&dru_rru_hardware_info,sizeof(dru_rru_hardware_info));
	pbuf=pbuf+sizeof(dru_rru_hardware_info);
	memcpy(pbuf,(U8 *)&dru_rru_software_info,sizeof(dru_rru_software_info));
	socket_send(0, outbuf, mh.msg_length, get_device_type());
}

/*******************************************************************************
* 函数名称: create_ch_req_deal
* 功    能:通道建立请求处理函数
* 参    数:
* 参数名称         类型                描述
* 返回值:
* 
* 说   明:
* 日   期     版本    作者   修改人      DEBUG
* -----------------------------------------------------------------------------
* 2012/12/10  V1.0     H4     无       ：
*******************************************************************************/
void create_ch_req_deal(U8 *inbuf)
{
	U8 i;
	msg_head mh;
	ie_head *ih;
	struct msg_head_t *pmh=((struct msg_head_t*)inbuf);
	U8 outbuf[MAX_SEND_LENGTH];
	U8 *p;
	int index;
	U16 le,symbol,length;
	p=inbuf;
	le=0;
	index=get_index_bydev(current_link_dev);
	if(index==-1){
		DEBUG_PRINTF("have no link of this device\n");
		return;	
	}
	mh.BBU_ID=pmh->BBU_ID;
	mh.RRU_ID=pmh->RRU_ID;
	mh.light_port_number=pmh->light_port_number;
	mh.serial_number=pmh->serial_number;
	mh.msg_number=CREATE_CHANNEL_CFG;
	//clean_ie_buffer(RECEIVE_THREAD);
	//add_ie_buffer(RECEIVE_THREAD,(U8 *)&mh,sizeof(mh));
	//le+=sizeof(mh);
	p+=sizeof(mh);
	do{
		ih=(ie_head *)p;
		symbol=ih->symbol;
		length=ih->length;
		DEBUG_PRINTF("create_ch_req_deal: symbol=%d,length=%d\n",symbol,length);
		if(symbol==1){
			memcpy((U8 *)&(connect_buffer[index].p_info->r_mark),(U8 *)p,sizeof(rru_mark));
			DEBUG_PRINTF("copy rru_mark to bbu\n");
		}else if(symbol==2){
			memcpy((U8 *)&(connect_buffer[index].p_info->c_reason),(U8 *)p,sizeof(create_channel_reason));
			DEBUG_PRINTF("copy create_channel_reason to bbu\n");
		}else if(symbol==4){
			memcpy((U8 *)&(connect_buffer[index].p_info->r_series),(U8 *)p,sizeof(rru_series));
			connect_buffer[index].dev_type=connect_buffer[index].p_info->r_series.series;
			for(i=0;i<8;i++){
				if(connect_buffer[index].p_info->r_series.port_number[i]>0xF00000000000){
					connect_buffer[index].dev_id=connect_buffer[index].p_info->r_series.port_number[i];
					break;
				}
			}
			delay_measure_start();
			DEBUG_PRINTF("copy rru_series to bbu\n");
		}else if(symbol==5){
			memcpy((U8 *)&(connect_buffer[index].p_info->r_hardware),(U8 *)p,sizeof(rru_hardware_info));
			DEBUG_PRINTF("copy rru_hardware_info to bbu\n");
		}else if(symbol==6){
			memcpy((U8 *)&(connect_buffer[index].p_info->r_software),(U8 *)p,sizeof(rru_software_info));
			get_compare_result(index);
			DEBUG_PRINTF("copy rru_software_info to bbu\n");
		}else{
			DEBUG_PRINTF("no this mark flag\n");
			break;
		}
		p+=length;
	}while(p<(inbuf+pmh->msg_length));
	clean_ie_buffer(RECEIVE_THREAD);
	le+=sizeof(mh);
	add_ie_buffer(RECEIVE_THREAD,(U8 *)&mh,sizeof(mh));
	get_system_time(&dru_sys_time);	
	le+=sizeof(dru_sys_time);
	if(add_ie_buffer(RECEIVE_THREAD,(U8 *)&dru_sys_time, sizeof(dru_sys_time))!=0){
		DEBUG_PRINTF("add ie buffer error !\n");
	}
	//get_ftp_addr();
	le+=sizeof(dru_ftp_addr);
	if(add_ie_buffer(RECEIVE_THREAD,(U8 *)&dru_ftp_addr, sizeof(dru_ftp_addr))!=0){
		DEBUG_PRINTF("add ie buffer error !\n");
	}
	le+=sizeof(dru_rru_mode);
	dru_rru_mode.mode=0;
	if(add_ie_buffer(RECEIVE_THREAD,(U8 *)&dru_rru_mode, sizeof(dru_rru_mode))!=0){
		DEBUG_PRINTF("add ie buffer error !\n");
	}
	le+=sizeof(dru_software_version_result);
	//get_compare_result(index);
	if(add_ie_buffer(RECEIVE_THREAD,(U8 *)&dru_software_version_result, sizeof(dru_software_version_result))!=0){
		DEBUG_PRINTF("add ie buffer error !\n");
	}
	le+=sizeof(dru_ir_mode_config);
	//get_ir_mode();
	if(add_ie_buffer(RECEIVE_THREAD,(U8 *)&dru_ir_mode_config, sizeof(dru_ir_mode_config))!=0){
		DEBUG_PRINTF("add ie buffer error !\n");
	}
	mh.msg_length=le;
	get_ie_buffer(RECEIVE_THREAD,outbuf,&le);
	DEBUG_PRINTF("length %d",le);
	socket_send(index, outbuf, le, get_device_type());
}
/***********************************************************************
** Funcation Name : get_compare_result
** Author         : H4
** Description    :	get the result of the sowftware version 
** Input          : 
** Output         : 
** date           : 2013年04月25日 星期四 14时46分55秒
** Version        : V1.0
** Modify         : 
***********************************************************************/
void get_compare_result(int index)
{
	int ret;
	ret=strcmpasc(connect_buffer[index].p_info->r_software.software_version,dru_rru_software_info.software_version,40);
	if(ret==0){
		dru_software_version_result.result=0;
	}else{
		dru_software_version_result.result=1;
	}
}
/*******************************************************************************
* 函数名称: create_ch_cfg_deal
* 功    能:通道建立配置应答
* 参    数:
* 参数名称         类型                描述
* 返回值:
* 
* 说   明:
* 日   期     版本    作者   修改人      DEBUG
* -----------------------------------------------------------------------------
* 2012/12/10  V1.0     H4     无       ：
*******************************************************************************/
void create_ch_cfg_deal(U8 *inbuf)
{
	
	msg_head mh;
	ie_head *ih;
	struct msg_head_t *pmh=((struct msg_head_t*)inbuf);
	U8 outbuf[MAX_SEND_LENGTH];
	U8 *p;
	U16 le,symbol,length;
	p=inbuf;
	le=0;
	mh.BBU_ID=pmh->BBU_ID;
	mh.RRU_ID=pmh->RRU_ID;
	mh.light_port_number=pmh->light_port_number;
	mh.serial_number=pmh->serial_number;
	mh.msg_number=CREATE_CHANNEL_CFG_RSP;
	//clean_ie_buffer(RECEIVE_THREAD);
	//add_ie_buffer(RECEIVE_THREAD,(U8 *)&mh,sizeof(mh));
	//le+=sizeof(mh);
	p+=sizeof(mh);
	do{
		ih=(ie_head *)p;
		symbol=ih->symbol;
		length=ih->length;
		DEBUG_PRINTF("create_ch_cfg_deal: symbol=%d, length=%d\n",symbol,length);
		if(symbol==11){
			memcpy((U8 *)&(dru_sys_time),(U8 *)p,sizeof(dru_sys_time));
			set_system_time(&dru_sys_time);
			DEBUG_PRINTF("set the system time\n");
		}else if(symbol==12){
			memcpy((U8 *)&(dru_ftp_addr),(U8 *)p,sizeof(dru_ftp_addr));
			DEBUG_PRINTF("set the ftp server address\n");
		}else if(symbol==13){
			memcpy((U8 *)&(dru_rru_mode),(U8 *)p,sizeof(dru_rru_mode));
			DEBUG_PRINTF("set the rru run mode\n");//只有正常运行一项其它功能待开发
		}else if(symbol==14){
			memcpy((U8 *)&(dru_software_version_result),(U8 *)p,sizeof(dru_software_version_result));
			//software_downloard_deal();
			if(dru_software_version_result.result==0){
			DEBUG_PRINTF("no downloard\r\n");	
			}else{
				DEBUG_PRINTF("downloard\r\n");
				//software_downloard_deal();	
			}
			DEBUG_PRINTF("check out the software version result\n");
		}else if(symbol==504){
			memcpy((U8 *)&(dru_ir_mode_config),(U8 *)p,sizeof(dru_ir_mode_config));
			//set_ir_mode();
			DEBUG_PRINTF("set the ir mode\n");
		}else{
			DEBUG_PRINTF("no this mark flag\n");
			break;
		}
		p+=length;
	}while(p<(inbuf+pmh->msg_length));
	clean_ie_buffer(RECEIVE_THREAD);
	le+=sizeof(mh);
	add_ie_buffer(RECEIVE_THREAD,(U8 *)&mh,sizeof(mh));
	le+=sizeof(dru_create_channel_rsp);
	dru_create_channel_rsp.status=0;
	if(add_ie_buffer(RECEIVE_THREAD,(U8 *)&dru_create_channel_rsp,sizeof(dru_create_channel_rsp))!=0){
		DEBUG_PRINTF("add ie buffer error !\n");
	}
	le+=sizeof(dru_ir_mode_config_rsp);
	//get_ir_mode();
	if(add_ie_buffer(RECEIVE_THREAD,(U8 *)&dru_ir_mode_config_rsp,sizeof(dru_ir_mode_config_rsp))!=0){
		DEBUG_PRINTF("add ie buffer error !\n");
	}
	le+=sizeof(dru_sys_time);
	get_system_time(&dru_sys_time);
	if(add_ie_buffer(RECEIVE_THREAD,(U8 *)&dru_sys_time,sizeof(dru_sys_time))!=0){
		DEBUG_PRINTF("add ie buffer error !\n");
	}
	mh.msg_length=le;
	get_ie_buffer(RECEIVE_THREAD,outbuf,&le);
	//mh.msg_length=le;
	socket_send(0, outbuf, le, get_device_type());

}
/*******************************************************************************
* 函数名称: create_ch_cfg_deal
* 功    能:通道建立配置应答
* 参    数:
* 参数名称         类型                描述
* 返回值:
* 
* 说   明:
* 日   期     版本    作者   修改人      DEBUG
* -----------------------------------------------------------------------------
* 2012/12/10  V1.0     H4     无       ：
*******************************************************************************/
void create_ch_cfg_rsp_deal(U8 *inbuf)
{
	ie_head *ih;
	struct msg_head_t *pmh=((struct msg_head_t*)inbuf);
	CU8 *p;
	U16 le,symbol,length;
	p=inbuf;
	le=0;
	le+=sizeof(msg_head);
	p+=sizeof(msg_head);
	do{
		ih=(ie_head *)p;
		symbol=ih->symbol;
		length=ih->length;
		DEBUG_PRINTF("create_ch_cfg_rsp_deal:symbol= %d length= %d\n",symbol,length);
		if(symbol==11){
			memcpy((U8 *)&(dru_sys_time),(U8 *)p,sizeof(dru_sys_time));
			set_system_time(&dru_sys_time);
			DEBUG_PRINTF("set the system time\n");
		}else if(symbol==21){
			DEBUG_PRINTF("21ok\n");
		}else if(symbol==555){
			DEBUG_PRINTF("555ok\n");
		}else{
			DEBUG_PRINTF("no such flag");
			break;
		}
		p+=length;
	}while(p<(inbuf+pmh->msg_length));
}
/////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////
/*
**
**		SOFTWARE COMPARE AND UPDATA	
**
**
*/
////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////
/***********************************************************************
** Funcation Name : update_result
** Author         : H4
** Description    : report the result of software update
** Input          : 
** Output         : 
** date           : 2013年04月25日 星期四 16时13分11秒
** Version        : V1.0
** Modify         : 
***********************************************************************/
void update_result(unsigned long status)
{
	msg_head mh;
	U8 outbuf[MAX_SEND_LENGTH];
	U8 *pbuf;
	pbuf=outbuf;
	mh.BBU_ID=0;
	mh.light_port_number=1;
	mh.serial_number=1;
	mh.msg_number=VERSION_DOWNLOARD_RESULT_DIS;
	mh.msg_length=sizeof(mh)+dru_downloard_result.ie_length;
	memcpy(pbuf,(U8 *)&mh,sizeof(mh));
	pbuf=pbuf+sizeof(mh);
	dru_update_version_result.status=status;
	memcpy(pbuf,(U8 *)&dru_downloard_result,sizeof(dru_downloard_result));
	socket_send(0, outbuf, mh.msg_length, get_device_type());
}
/***********************************************************************
** Funcation Name : update_result_deal
** Author         : H4
** Description    : deal with update result 
** Input          : 
** Output         : 
** date           : 2013年04月25日 星期四 16时33分08秒
** Version        : V1.0
** Modify         : 
***********************************************************************/
void update_result_deal(U8 *inbuf)
{
	ie_head *ih;
	struct msg_head_t *pmh=((struct msg_head_t*)inbuf);
	CU8 *p;
	U16 le,symbol,length;
	update_version_result * p_up;
	p=inbuf;
	le=0;
	le+=sizeof(msg_head);
	p+=sizeof(msg_head);
	do{
		ih=(ie_head *)p;
		symbol=ih->symbol;
		length=ih->length;
		DEBUG_PRINTF("update_result_deal:symbol= %d length= %d\n",symbol,length);
		if(symbol==31){
			p_up=(update_version_result *)p;
			if(p_up->status==0){
				DEBUG_PRINTF("UPDATE IS OK\r\n");
			}else{
				DEBUG_PRINTF("UPDATE IS ERROR\r\n");
			}
		}else{
			DEBUG_PRINTF("no such flag");
			break;
		}
		p+=length;
	}while(p<(inbuf+pmh->msg_length));
}
/////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////
/*
**
**			状态查询请求及响应处理函数
**
**
*/
////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////
/*******************************************************************************
* 函数名称: state_req
* 功    能:状态查询函数
* 参    数:
* 参数名称         类型                描述
* 返回值:
* 
* 说   明:
* 日   期     版本    作者   修改人      DEBUG
* -----------------------------------------------------------------------------
* 2012/11/19  V1.0     H4     无       ：
*******************************************************************************/
void state_req(void)
{
	msg_head mh;
	U8 outbuf[MAX_SEND_LENGTH];
	U16 le;
	mh.BBU_ID=0;
	mh.RRU_ID=1;
	mh.serial_number=5;
	mh.msg_number=RRU_STATUS_REQ;
	clean_ie_buffer(BBU_THREAD);
	if(add_ie_buffer(BBU_THREAD,(U8 *)&mh, sizeof(mh))!=0){
		DEBUG_PRINTF("add ie buffer error !\n");
	}
	dru_fchannel_state_req.number=0;
	if(add_ie_buffer(BBU_THREAD,(U8 *)&dru_fchannel_state_req, sizeof(dru_fchannel_state_req))!=0){
		DEBUG_PRINTF("add ie buffer error !\n");
	}
	if(add_ie_buffer(BBU_THREAD,(U8 *)&dru_carrier_wave_req, sizeof(dru_carrier_wave_req))!=0){
		DEBUG_PRINTF("add ie buffer error !\n");
	}
	if(add_ie_buffer(BBU_THREAD,(U8 *)&dru_local_osc_req, sizeof(dru_local_osc_req))!=0){
		DEBUG_PRINTF("add ie buffer error !\n");
	}
	if(add_ie_buffer(BBU_THREAD,(U8 *)&dru_clock_req, sizeof(dru_clock_req))!=0){
		DEBUG_PRINTF("add ie buffer error !\n");
	}
	if(add_ie_buffer(BBU_THREAD,(U8 *)&dru_run_state_req, sizeof(dru_run_state_req))!=0){
		DEBUG_PRINTF("add ie buffer error !\n");
	}
	if(add_ie_buffer(BBU_THREAD,(U8 *)&dru_ir_mode_req, sizeof(dru_ir_mode_req))!=0){
		DEBUG_PRINTF("add ie buffer error !\n");
	}
	if(add_ie_buffer(BBU_THREAD,(U8 *)&dru_init_result_req, sizeof(dru_init_result_req))!=0){
		DEBUG_PRINTF("add ie buffer error !\n");
	}
	if(add_ie_buffer(BBU_THREAD,(U8 *)&dru_light_info_req, sizeof(dru_light_info_req))!=0){
		DEBUG_PRINTF("add ie buffer error !\n");
	}
	get_ie_buffer(BBU_THREAD,outbuf, &le);
	socket_send(0, outbuf, le,get_device_type());
}
/*******************************************************************************
* 函数名称: state_req_deal
* 功    能:状态查询处理函数
* 参    数:
* 参数名称         类型                描述
* 返回值:
* 
* 说   明:
* 日   期     版本    作者   修改人      DEBUG
* -----------------------------------------------------------------------------
* 2012/11/22  V1.0     H4     无       ：
*******************************************************************************/
void state_req_deal(U8 *inbuf)
{
	msg_head mh;
	ie_head *ih;
	u8_struct *u8_t;
	struct msg_head_t *pmh=((struct msg_head_t*)inbuf);
	U8 outbuf[MAX_SEND_LENGTH];
	U8 *p;
	int index;
	U16 le,symbol,length;
	p=inbuf;
	le=0;
	//pmh=(msg_head *)inbuf;
	mh.BBU_ID=pmh->BBU_ID;
	mh.RRU_ID=pmh->RRU_ID;
	mh.light_port_number=pmh->light_port_number;
	mh.serial_number=pmh->serial_number;
	mh.msg_number=RRU_STATUS_RSP;
	clean_ie_buffer(RECEIVE_THREAD);
	add_ie_buffer(RECEIVE_THREAD,(U8 *)&mh,sizeof(mh));
	le+=sizeof(mh);
	p+=sizeof(mh);
	do{
		ih=(ie_head *)p;
		symbol=ih->symbol;
		length=ih->length;
		DEBUG_PRINTF("%d,%d\n",symbol,length);
		if(symbol==302){
			u8_t=(u8_struct *)(p+3);
			dru_fchannel_state_rsp.number=u8_t->data;
			dru_fchannel_state_rsp.down=2;
			dru_fchannel_state_rsp.up=5;
			add_ie_buffer(RECEIVE_THREAD,(U8 *)&dru_fchannel_state_rsp,sizeof(dru_fchannel_state_rsp));
			le+=sizeof(dru_fchannel_state_rsp);
			DEBUG_PRINTF("dru_fchannel_state_rsp\n");
		}else if(symbol==303){
			u8_t=(u8_struct *)(p+3);
			dru_carrier_wave_rsp.number=u8_t->data;
			dru_carrier_wave_rsp.status=1;
			add_ie_buffer(RECEIVE_THREAD,(U8 *)&dru_carrier_wave_rsp,sizeof(dru_carrier_wave_rsp));
			le+=sizeof(dru_carrier_wave_rsp);
			DEBUG_PRINTF("dru_carrier_wave_rsp\n");
		}else if(symbol==304){
			dru_local_osc_rsp.hz=2017;
			dru_local_osc_rsp.status=1;
			add_ie_buffer(RECEIVE_THREAD,(U8 *)&dru_local_osc_rsp,sizeof(dru_local_osc_rsp));
			le+=sizeof(dru_local_osc_rsp);
			DEBUG_PRINTF("dru_local_osc_rsp\n");
		}else if(symbol==305){
			dru_clock_rsp.status=1;
			add_ie_buffer(RECEIVE_THREAD,(U8 *)&dru_clock_rsp,sizeof(dru_clock_rsp));
			le+=sizeof(dru_clock_rsp);
			DEBUG_PRINTF("dru_clock_rsp\n");
		}else if(symbol==306){
			dru_run_state_rsp.status=1;
			add_ie_buffer(RECEIVE_THREAD,(U8 *)&dru_run_state_rsp,sizeof(dru_run_state_rsp));
			le+=sizeof(dru_run_state_rsp);
			DEBUG_PRINTF("dru_run_state_rsp\n");
		}else if(symbol==307){
			dru_ir_mode_rsp.result=1;
			add_ie_buffer(RECEIVE_THREAD,(U8 *)&dru_ir_mode_rsp,sizeof(dru_ir_mode_rsp));
			le+=sizeof(dru_ir_mode_rsp);
			DEBUG_PRINTF("dru_ir_mode_rsp\n");
		}else if(symbol==308){
			dru_init_result_rsp.rx_channel_number=1;
			dru_init_result_rsp.tx_channel_number=0;
			dru_init_result_rsp.rx_status=1;
			dru_init_result_rsp.tx_status=1;
			add_ie_buffer(RECEIVE_THREAD,(U8 *)&dru_init_result_rsp,sizeof(dru_init_result_rsp));
			le+=sizeof(dru_init_result_rsp);
			DEBUG_PRINTF("dru_init_result_rsp\n");
		}else if(symbol==309){
			u8_t=(u8_struct *)(p+3);
			dru_light_info_rsp.port=u8_t->data;
			dru_light_info_rsp.is_being=1;
			dru_light_info_rsp.length_50umm_10m=100;
			dru_light_info_rsp.length_625umm_10m=100;
			dru_light_info_rsp.length_9ums_100m=50;
			dru_light_info_rsp.length_9ums_km=5;
			dru_light_info_rsp.mv=3;
			dru_light_info_rsp.ma=2;
			dru_light_info_rsp.speed=2000;
			dru_light_info_rsp.rx_capy=1;
			dru_light_info_rsp.tx_capy=10;
			dru_light_info_rsp.temp=5;
			add_ie_buffer(RECEIVE_THREAD,(U8 *)&dru_light_info_rsp,sizeof(dru_light_info_rsp));
			le+=sizeof(dru_light_info_rsp);
			DEBUG_PRINTF("dru_light_info_rsp\n");
		}
		p+=length;
	}while(p<(inbuf+pmh->msg_length));
	mh.msg_length=le;
	get_ie_buffer(RECEIVE_THREAD,outbuf,&le);
	index=get_index_bydev(current_link_dev);
	if(index==-1){
		DEBUG_PRINTF("have no link of this device\n");
		return;	
	}
	socket_send(index, outbuf, le, get_device_type());
}
/////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////
/*
**
**			参数查询请求及响应处理函数
**
**
*/
////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////
/*******************************************************************************
* 函数名称: parameter_req
* 功    能:参数查询处理函数
* 参    数:
* 参数名称         类型                描述
* 返回值:
* 
* 说   明:
* 日   期     版本    作者   修改人      DEBUG
* -----------------------------------------------------------------------------
* 2012/11/22  V1.0     H4     无       ：
*******************************************************************************/
void parameter_req(void)
{
	msg_head mh;
	U8 outbuf[MAX_SEND_LENGTH];
	U16 le;
	mh.BBU_ID=0;
	mh.RRU_ID=1;
	mh.serial_number=5;
	mh.msg_number=RRU_PARAMETER_REQ;
	mh.msg_length=sizeof(mh)+dru_sys_time_req.ie_length\
						+dru_cpu_occ_req.ie_length\
						+dru_cpu_cycle_req.ie_length\
						+dru_rru_temp_req.ie_length\
						+dru_rru_swr_req.ie_length\
						+dru_swr_limit_req.ie_length\
						+dru_temp_limit_req.ie_length\
						+dru_out_capy_req.ie_length\
						+dru_state_machine_req.ie_length;
	clean_ie_buffer(BBU_THREAD);
	if(add_ie_buffer(BBU_THREAD,(U8 *)&mh, sizeof(mh))!=0){
		DEBUG_PRINTF("add ie buffer error !\n");
	}
	if(add_ie_buffer(BBU_THREAD,(U8 *)&dru_sys_time_req, sizeof(dru_sys_time_req))!=0){
		DEBUG_PRINTF("add ie buffer error !\n");
	}
	if(add_ie_buffer(BBU_THREAD,(U8 *)&dru_cpu_occ_req, sizeof(dru_cpu_occ_req))!=0){
		DEBUG_PRINTF("add ie buffer error !\n");
	}
	if(add_ie_buffer(BBU_THREAD,(U8 *)&dru_cpu_cycle_req, sizeof(dru_cpu_cycle_req))!=0){
		DEBUG_PRINTF("add ie buffer error !\n");
	}
	dru_rru_temp_req.type=0;
	if(add_ie_buffer(BBU_THREAD,(U8 *)&dru_rru_temp_req, sizeof(dru_rru_temp_req))!=0){
		DEBUG_PRINTF("add ie buffer error !\n");
	}
	dru_rru_swr_req.number=1;
	if(add_ie_buffer(BBU_THREAD,(U8 *)&dru_rru_swr_req, sizeof(dru_rru_swr_req))!=0){
		DEBUG_PRINTF("add ie buffer error !\n");
	}
	if(add_ie_buffer(BBU_THREAD,(U8 *)&dru_swr_limit_req, sizeof(dru_swr_limit_req))!=0){
		DEBUG_PRINTF("add ie buffer error !\n");
	}
	if(add_ie_buffer(BBU_THREAD,(U8 *)&dru_temp_limit_req, sizeof(dru_temp_limit_req))!=0){
		DEBUG_PRINTF("add ie buffer error !\n");
	}
	dru_out_capy_req.number=1;
	if(add_ie_buffer(BBU_THREAD,(U8 *)&dru_out_capy_req, sizeof(dru_out_capy_req))!=0){
		DEBUG_PRINTF("add ie buffer error !\n");
	}
	if(add_ie_buffer(BBU_THREAD,(U8 *)&dru_state_machine_req, sizeof(dru_state_machine_req))!=0){
		DEBUG_PRINTF("add ie buffer error !\n");
	}
	get_ie_buffer(BBU_THREAD,outbuf, &le);
	socket_send(0, outbuf, le, get_device_type());
}
/*******************************************************************************
* 函数名称: parameter_req_deal
* 功    能:参数查询处理函数
* 参    数:
* 参数名称         类型                描述
* 返回值:
* 
* 说   明:
* 日   期     版本    作者   修改人      DEBUG
* -----------------------------------------------------------------------------
* 2012/11/22  V1.0     H4     无       ：
*******************************************************************************/
void parameter_req_deal(U8 *inbuf)
{
	int index;
	ie_head *ih;
	msg_head mh;
	struct msg_head_t *pmh=((struct msg_head_t*)inbuf);
	U8 outbuf[MAX_SEND_LENGTH];
	U8 *p;
	U16 le,symbol,length;
	p=inbuf;
	le=0;
	mh.BBU_ID=pmh->BBU_ID;
	mh.RRU_ID=pmh->RRU_ID;
	mh.light_port_number=pmh->light_port_number;
	mh.serial_number=pmh->serial_number;
	mh.msg_number=RRU_PARAMETER_RSP;
	clean_ie_buffer(RECEIVE_THREAD);
	add_ie_buffer(RECEIVE_THREAD,(U8 *)&mh,sizeof(mh));
	le+=sizeof(mh);
	p+=sizeof(mh);
	do{
		ih=(ie_head *)p;
		symbol=ih->symbol;
		length=ih->length;
		DEBUG_PRINTF("%d,%d\n",symbol,length);
		if(symbol==401){
			//dru_sys_time.
			add_ie_buffer(RECEIVE_THREAD,(U8 *)&dru_sys_time,sizeof(dru_sys_time));
			le+=sizeof(dru_sys_time);
			DEBUG_PRINTF("dru_sys_time\n");
		}else if(symbol==402){
			dru_cpu_occ_rsp.occ=35;
			add_ie_buffer(RECEIVE_THREAD,(U8 *)&dru_cpu_occ_rsp,sizeof(dru_cpu_occ_rsp));
			le+=sizeof(dru_cpu_occ_rsp);
			DEBUG_PRINTF("dru_cpu_occ_rsp\n");
		}else if(symbol==403){
			dru_cpu_cycle_rsp.sec=2000;
			add_ie_buffer(RECEIVE_THREAD,(U8 *)&dru_cpu_cycle_rsp,sizeof(dru_cpu_cycle_rsp));
			le+=sizeof(dru_cpu_cycle_rsp);
			DEBUG_PRINTF("dru_local_osc_rsp\n");
		}else if(symbol==404){
			dru_rru_temp_rsp.number=0;
			dru_rru_temp_rsp.type=0;
			dru_rru_temp_rsp.temp=45;
			add_ie_buffer(RECEIVE_THREAD,(U8 *)&dru_rru_temp_rsp,sizeof(dru_rru_temp_rsp));
			le+=sizeof(dru_rru_temp_rsp);
			DEBUG_PRINTF("dru_rru_temp_rsp\n");
		}else if(symbol==405){
			dru_rru_swr_rsp.number=0;
			dru_rru_swr_rsp.value=14;
			add_ie_buffer(RECEIVE_THREAD,(U8 *)&dru_rru_swr_rsp,sizeof(dru_rru_swr_rsp));
			le+=sizeof(dru_rru_swr_rsp);
			DEBUG_PRINTF("dru_rru_swr_rsp\n");
		}else if(symbol==406){
			dru_swr_limit_rsp.one_level=16;
			dru_swr_limit_rsp.two_level=17;
			add_ie_buffer(RECEIVE_THREAD,(U8 *)&dru_swr_limit_rsp,sizeof(dru_swr_limit_rsp));
			le+=sizeof(dru_swr_limit_rsp);
			DEBUG_PRINTF("dru_swr_limit_rsp\n");
		}else if(symbol==407){
			dru_temp_limit_rsp.down_limit=80;
			dru_temp_limit_rsp.up_limit=75;
			add_ie_buffer(RECEIVE_THREAD,(U8 *)&dru_temp_limit_rsp,sizeof(dru_temp_limit_rsp));
			le+=sizeof(dru_temp_limit_rsp);
			DEBUG_PRINTF("dru_temp_limit_rsp\n");
		}else if(symbol==408){
			dru_out_capy_rsp.number=0;
			dru_out_capy_rsp.value=27;
			add_ie_buffer(RECEIVE_THREAD,(U8 *)&dru_out_capy_rsp,sizeof(dru_out_capy_rsp));
			le+=sizeof(dru_out_capy_rsp);
			DEBUG_PRINTF("dru_out_capy_rsp\n");
		}else if(symbol==409){
			dru_state_machine_rsp.state=2;
			add_ie_buffer(RECEIVE_THREAD,(U8 *)&dru_state_machine_rsp,sizeof(dru_state_machine_rsp));
			le+=sizeof(dru_state_machine_rsp);
			DEBUG_PRINTF("dru_state_machine_rsp\n");
		}
		p+=length;
	}while(p<(inbuf+pmh->msg_length));
	get_ie_buffer(RECEIVE_THREAD,outbuf, &le);
	index=get_index_bydev(get_current_dev());	
	socket_send(index, outbuf, le, get_device_type());
}
/////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////
/*
**
**	透明传输处理函数	
**
**
*/
////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////
/*******************************************************************************
* 函数名称: transimt_deal
* 功    能:透传处理
* 参    数:
* 参数名称         类型                描述
* 返回值:
* 
* 说   明:
* 日   期     版本    作者   修改人      DEBUG
* -----------------------------------------------------------------------------
* 2012/11/23 V1.0     H4     无       ：
*******************************************************************************/
void transimt_deal(void)
{
	msg_head mh;
	U8 outbuf[MAX_SEND_LENGTH];
	U16 le,index;
	if(g_OMC_IrTranCom.RecvLen!=0){
		DEBUG_PRINTF("\nhas message in omc buffer%d\n",g_OMC_IrTranCom.RecvLen);
		mh.BBU_ID=0;
		mh.RRU_ID=current_link_dev;
		mh.serial_number=create_serial_number();
		if(get_device_type()==DEVICE_TYPE_MAIN){
			mh.msg_number=BBU_TO_RRU;
		}else{
			mh.msg_number=RRU_TO_BBU;
		}
		mh.msg_length=sizeof(msg_head)+dru_transimt_aim.ie_length\
							+dru_transmit_tx.ie_length;
		clean_ie_buffer(TRANSMIT_THREAD);
		if(add_ie_buffer(TRANSMIT_THREAD,(U8 *)&mh, sizeof(msg_head))!=0){
			DEBUG_PRINTF("add ie buffer error !\n");
			return;
		}
		dru_transimt_aim.aim=0;
		if(add_ie_buffer(TRANSMIT_THREAD,(U8 *)&dru_transimt_aim, sizeof(dru_transimt_aim))!=0){
			DEBUG_PRINTF("add ie buffer error !\n");
			return;
		}
		memcpy(dru_transmit_tx.buffer,g_OMC_IrTranCom.Buf,COMBUF_SIZE);
		if(add_ie_buffer(TRANSMIT_THREAD,(U8 *)&dru_transmit_tx, sizeof(dru_transmit_tx))!=0){
			DEBUG_PRINTF("add ie buffer error !\n");
			return;
		}
		if(get_ie_buffer(TRANSMIT_THREAD,outbuf,&le)!=0){
			DEBUG_PRINTF("get ie buffer error");
			return;
		}
		index=get_index_bydev(g_OMC_IrTranCom.Fd);	
		g_OMC_IrTranCom.RecvLen=0;
		DEBUG_PRINTF("\n SEND TRANSMIT DATA\n");
		socket_send(index, outbuf, le, get_device_type());
	}else{
		;
	}
}
/*******************************************************************************
* 函数名称: transimt_rsp_deal
* 功    能:透传返回处理
* 参    数:
* 参数名称         类型                描述
* 返回值:
* 
* 说   明:
* 日   期     版本    作者   修改人      DEBUG
* -----------------------------------------------------------------------------
* 2012/11/23  V1.0     H4     无       ：
*******************************************************************************/
void transimt_rsp_deal(U8 *inbuf)
{
	U16 i;
	ie_head *ih;
	struct msg_head_t *pmh=((struct msg_head_t*)inbuf);
	CU8 *p;
	U16 le,symbol,length;
	p=inbuf;
	le=0;
	le+=sizeof(msg_head);
	p+=sizeof(msg_head);
	do{
		ih=(ie_head *)p;
		symbol=ih->symbol;
		length=ih->length;
		DEBUG_PRINTF("transmit_rsp_deal%d %d\n",symbol,length);
		if(symbol==1601){
			dru_transimt_aim.aim=*(p+4);
			le+=sizeof(dru_transimt_aim);
			DEBUG_PRINTF("dru_transimt_aim\n");
		}else if(symbol==1602){
			memcpy(dru_transmit_rx.buffer,(U8 *)(p+4),1024);
			i=0;
			while((g_Ir_OMCTranCom.RecvLen!=0)&&(i<1000)){
				usleep(5*1000);
				i++;
			}
			if(i>=1000){
				DEBUG_PRINTF("OMC buffer is not empty\n");
				return;
			}
			memcpy(g_Ir_OMCTranCom.Buf,dru_transmit_rx.buffer,COMBUF_SIZE);
			g_Ir_OMCTranCom.Fd=current_link_dev;
			g_Ir_OMCTranCom.RecvLen=COMBUF_SIZE;
			le+=sizeof(dru_transmit_rx);
			DEBUG_PRINTF("dru_transmit_tx\n");
			printf("\n\n====================================\n");
		}
		p+=length;
	}while(p<(inbuf+pmh->msg_length));
}

/////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////
/*
**
**	心跳发起及处理函数	
**
**
*/
////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////
/*******************************************************************************
* 函数名称:timing_rru
* 功    能:rru端心跳包发起程序。 
* 参    数:
* 参数名称         类型                描述
* 返回值:
* 
* 说   明:
* 日   期     版本    作者 H4  修改人      DEBUG
* -----------------------------------------------------------------------------
*2012.12.3  v1.0		h4		
*2012.12.5  v1.1		h4		h4			将心跳处理改为心跳发起程序             
*******************************************************************************/
void timing_rru(void)
{
	U8 outbuf[MAX_SEND_LENGTH];
	U16 le;
	if(client_time>OUT_TIME){
		close_client();		
		return;
	}
	if((client_time%10)==9){
		msg_head mh;
		mh.BBU_ID=0;
		mh.RRU_ID=1;
		mh.light_port_number=0;
		mh.msg_number=RRU_ALIVE_MSG;
		mh.serial_number=create_serial_number();
		clean_ie_buffer(TIMING_THREAD);
		if(add_ie_buffer(TIMING_THREAD,(U8 *)&mh, sizeof(msg_head))!=0){
			DEBUG_PRINTF("add ie buffer error !\n");
			return;
		}
		get_ie_buffer(TIMING_THREAD,outbuf, &le);
		socket_send(0, outbuf, le, get_device_type());
		DEBUG_PRINTF("rru send timing package\n");
	}
	client_time++;
}
/*******************************************************************************
* 函数名称:timing_rru_deal
* 功    能:rru端处理心跳包的程序。 
* 参    数:
* 参数名称         类型                描述
* 返回值:
* 
* 说   明:
* 日   期     版本    作者 H4  修改人      DEBUG
* -----------------------------------------------------------------------------
*2012.12.4  v1.0		h4		
*******************************************************************************/
void timing_rru_deal(U8 *inbuf)
{
	client_time=0;
	DEBUG_PRINTF("rru receive timing package \n");
}
/*******************************************************************************
* 函数名称:timing_bbu_deal
* 功    能:bbu端处理心跳包的程序。 
* 参    数:
* 参数名称         类型                描述
* 返回值:
* 
* 说   明:
* 日   期     版本    作者 H4  修改人      DEBUG
* -----------------------------------------------------------------------------
*2012.12.4  v1.0		h4		
*******************************************************************************/
void timing_bbu_deal(U8 *inbuf)
{
	int index;
	U8 outbuf[MAX_SEND_LENGTH];
	U16 le;
	msg_head mh;
	mh.BBU_ID=0;
	mh.RRU_ID=1;
	mh.light_port_number=0;
	mh.msg_number=BBU_ALIVE_MSG;
	mh.serial_number=create_serial_number();
	clean_ie_buffer(RECEIVE_THREAD);
	if(add_ie_buffer(RECEIVE_THREAD,(U8 *)&mh, sizeof(msg_head))!=0){
		DEBUG_PRINTF("add ie buffer error !\n");
		return;
	}
	get_ie_buffer(RECEIVE_THREAD,outbuf, &le);
	index=get_index_bydev(get_current_dev());	
	socket_send(index, outbuf, le, get_device_type());
	connect_buffer[index].time=0;
	DEBUG_PRINTF("bbu receive timing package %d %d\n",connect_buffer[index].dev,index);
}

/*******************************************************************************
* 函数名称:timing_bbu
* 功    能:心跳包bbu端定时检测函数 
* 参    数:
* 参数名称         类型                描述
* 返回值:
* 
* 说   明:
* 日   期     版本    作者 H4  修改人      DEBUG
* -----------------------------------------------------------------------------
*2012.12.5  v1.0		h4		
*******************************************************************************/
void timing_bbu(void)
{
	U8 outbuf[MAX_SEND_LENGTH];
	U16 le;
	msg_head mh;
	mh.BBU_ID=0;
	mh.RRU_ID=1;
	mh.light_port_number=0;
	mh.msg_number=BBU_ALIVE_MSG;
	mh.serial_number=create_serial_number();
	clean_ie_buffer(TIMING_THREAD);
	if(add_ie_buffer(TIMING_THREAD,(U8 *)&mh, sizeof(msg_head))!=0){
		DEBUG_PRINTF("add ie buffer error !\n");
		return;
	}
	get_ie_buffer(TIMING_THREAD,outbuf, &le);
	socket_send(0, outbuf, le, get_device_type());
}
/////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////
/*
**
**		参数配置及处理函数	
**
**
*/
////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////
/***********************************************************************
** Funcation Name :parameter_config
** Author         : H4
** Description    : 配置参数。 
** Input          : 
** Output         : 
** date           : 2014年05月05日 
** Version        : V1.0
** Modify         : 
***********************************************************************/
void set_system_time(sys_time *st)
{
	time_t timep,now;
	struct timeval tv; 
	struct tm t;

	t.tm_year=st->year;
	t.tm_mon=st->mon;
	t.tm_mday=st->date;
	t.tm_hour=st->hour;
	t.tm_min=st->min;
	t.tm_sec=st->sec;
	time(&now);
	timep=mktime(&t);
	if(timep<=now){
		printf("not set time for this dev!\n");
		return;
	}
	printf("set time for this dev!\n");
	tv.tv_sec = timep;
	tv.tv_usec = 0; 
	if(settimeofday (&tv, (struct timezone *) 0)<0){
		printf("Set system datatime error!\n");
		return;
	}
	system_time_config();
}
/***********************************************************************
** Funcation Name :parameter_config
** Author         : H4
** Description    : 配置参数。 
** Input          : 
** Output         : 
** date           : 2014年05月05日 
** Version        : V1.0
** Modify         : 
***********************************************************************/
void get_system_time(sys_time *st)
{
	time_t now; 
	struct tm *timenow; 
	time(&now);
	timenow = localtime(&now);      
	st->year=timenow->tm_year;
	st->mon=timenow->tm_mon;
	st->date=timenow->tm_mday;
	st->hour=timenow->tm_hour;
	st->min=timenow->tm_min;
	st->sec=timenow->tm_sec;
}
/***********************************************************************
** Funcation Name : delay_measure_config
** Author         : H4
** Description    :	配置时延 
** Input          : 
** Output         : 
** date           : 2013年02月27日 星期三 15时59分28秒
** Version        : V1.0
** Modify         : 
***********************************************************************/
int system_time_config(void)
{
	U16 i;
	if(get_device_type()==DEVICE_TYPE_MAIN){
		for(i=0;i<MAX_CONNECT_NUMBER;i++){
			if(connect_buffer[i].connect_status==CLIENT_CONNECTED){
				if(connect_buffer[i].dev_type==DEVICE_TYPE_MAIN){
					;
				}else{
					parameter_config(i);
				}
				usleep(20);
			}
		}
	}
	return 0;
}
/***********************************************************************
** Funcation Name :parameter_config
** Author         : H4
** Description    : 配置参数。 
** Input          : 
** Output         : 
** date           : 2014年05月05日 
** Version        : V1.0
** Modify         : 
***********************************************************************/
void parameter_config(unsigned int index)
{
	U8 outbuf[MAX_SEND_LENGTH];
	U16 le;
	msg_head mh;
	mh.BBU_ID=0;
	mh.RRU_ID=get_dev_byindex(index);
	mh.light_port_number=0;
	mh.msg_number=RRU_PARAMETER_CFG;
	mh.serial_number=create_serial_number();
	mh.msg_length=sizeof(mh)+sizeof(dru_sys_time);
	clean_ie_buffer(CONFIG_THREAD);
	if(add_ie_buffer(CONFIG_THREAD,(U8 *)&mh, sizeof(msg_head))!=0){
		DEBUG_PRINTF("add ie buffer error !\n");
		return;
	}
	get_system_time(&dru_sys_time);
	if(add_ie_buffer(CONFIG_THREAD,(U8 *)&dru_sys_time, sizeof(dru_sys_time))!=0){
		DEBUG_PRINTF("add ie buffer error !\n");
		return;
	}
	get_ie_buffer(CONFIG_THREAD,outbuf, &le);
	socket_send(index, outbuf, le, get_device_type());
}
/***********************************************************************
** Funcation Name : parameter_config_deal
** Author         : H4
** Description    : 处理配置时间。如果设置的时间大于当前时间配置否则不配置
** Input          : 
** Output         : 
** date           : 2014年05月05日
** Version        : V1.0
** Modify         : 
***********************************************************************/
void parameter_config_deal(U8 *inbuf)
{
	msg_head mh;
	ie_head *ih;
	struct msg_head_t *pmh=((struct msg_head_t*)inbuf);
	U8 outbuf[MAX_SEND_LENGTH];
	CU8 *p;
	U16 le,symbol,length;
	mh.BBU_ID=pmh->BBU_ID;
	mh.RRU_ID=pmh->RRU_ID;
	mh.light_port_number=0;
	mh.msg_number=RRU_PARAMETER_CFG_RSP;
	mh.serial_number=pmh->serial_number;
	p=inbuf;
	le=0;
	le+=sizeof(msg_head);
	p+=sizeof(msg_head);
	clean_ie_buffer(CONFIG_BACK_THREAD);
	if(add_ie_buffer(CONFIG_BACK_THREAD,(U8 *)&mh, sizeof(msg_head))!=0){
		DEBUG_PRINTF("add ie buffer error !\n");
		return;
	}
	do{
		ih=(ie_head *)p;
		symbol=ih->symbol;
		length=ih->length;
		DEBUG_PRINTF("delay_measure_config_deal %d %d\n",symbol,length);
		if(symbol==11){
			memcpy((U8 *)&(dru_sys_time),(U8 *)p,sizeof(dru_sys_time));//2014.05.05
			set_system_time(&dru_sys_time);
			le+=sizeof(dru_sys_time_rsp);
			if(add_ie_buffer(CONFIG_BACK_THREAD,(U8 *)&dru_sys_time_rsp, sizeof(dru_sys_time_rsp))!=0){
				DEBUG_PRINTF("add ie buffer error !\n");
				return;
			}
			DEBUG_PRINTF("dru_sys_time_rsp\n");
		}
		p+=length;
	}while((p<(inbuf+pmh->msg_length))&&(length!=0));
	mh.msg_length = le;
	get_ie_buffer(CONFIG_BACK_THREAD,outbuf, &le);
	socket_send(0, outbuf, le, get_device_type());
}
/***********************************************************************
** Funcation Name : parameter_config_deal_rsp
** Author         : H4
** Description    : 处理配置时间。如果设置的时间大于当前时间配置否则不配置
** Input          : 
** Output         : 
** date           : 2014年05月05日
** Version        : V1.0
** Modify         : 
***********************************************************************/
void parameter_config_deal_rsp(U8 *inbuf)
{
	
}
/////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////
/*
**
**		时延测量处理函数
**
**
*/
////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////
/*******************************************************************************
* 函数名称:delay_measure_req
* 功    能:时延测量请求 
* 参    数:
* 参数名称         类型                描述
* 返回值:
* 
* 说   明:
* 日   期     版本    作者 H4  修改人      DEBUG
* -----------------------------------------------------------------------------
*2013.1.5  v1.0		h4		
*******************************************************************************/
void delay_measure_req(unsigned int index)
{
	U8 outbuf[MAX_SEND_LENGTH];
	U16 le;
	msg_head mh;
	mh.BBU_ID=0;
	mh.RRU_ID=get_dev_byindex(index);
	mh.light_port_number=0;
	mh.msg_number=TIME_DELAY_MEASURE_REQ;
	mh.serial_number=create_serial_number();
	mh.msg_length=sizeof(mh)+sizeof(dru_time_delay_req);
	clean_ie_buffer(DELAY_THREAD);
	if(add_ie_buffer(DELAY_THREAD,(U8 *)&mh, sizeof(msg_head))!=0){
		DEBUG_PRINTF("add ie buffer error !\n");
		return;
	}
	if(add_ie_buffer(DELAY_THREAD,(U8 *)&dru_time_delay_req, sizeof(dru_time_delay_req))!=0){
		DEBUG_PRINTF("add ie buffer error !\n");
		return;
	}
	get_ie_buffer(DELAY_THREAD,outbuf, &le);
	socket_send(index, outbuf, le, get_device_type());
}
/*******************************************************************************
* 函数名称:delay_measure_req_deal
* 功    能:时延测量请求响应
* 参    数:
* 参数名称         类型                描述
* 返回值:
* 
* 说   明:
* 日   期     版本    作者 H4  修改人      DEBUG
* -----------------------------------------------------------------------------
*2013.1.30  v1.0		h4		
*******************************************************************************/
void delay_measure_req_deal(U8 *inbuf)
{
	msg_head mh;
	ie_head *ih;
	struct msg_head_t *pmh=((struct msg_head_t*)inbuf);
	U8 outbuf[MAX_SEND_LENGTH];
	CU8 *p;
	U16 le,symbol,length;
	mh.BBU_ID=pmh->BBU_ID;
	mh.RRU_ID=pmh->RRU_ID;
	mh.light_port_number=0;
	mh.msg_number=TIME_DELAY_MEASURE_RSP;
	mh.serial_number=pmh->serial_number;
	p=inbuf;
	le=0;
	le+=sizeof(msg_head);
	p+=sizeof(msg_head);
	clean_ie_buffer(DELAY_BACK_THREAD);
	if(add_ie_buffer(DELAY_BACK_THREAD,(U8 *)&mh, sizeof(msg_head))!=0){
		DEBUG_PRINTF("add ie buffer error !\n");
		return;
	}
	do{
		ih=(ie_head *)p;
		symbol=ih->symbol;
		length=ih->length;
		DEBUG_PRINTF("delay_measure_req_deal %d %d\n",symbol,length);
		if(symbol==901){
			measure_dalay_time();
			le+=sizeof(dru_time_delay_rsp);
			if(add_ie_buffer(DELAY_BACK_THREAD,(U8 *)&dru_time_delay_rsp, sizeof(dru_time_delay_rsp))!=0){
				DEBUG_PRINTF("add ie buffer error !\n");
				return;
			}
			DEBUG_PRINTF("dru_time_delay_rsp\n");
		}
		p+=length;
	}while((p<(inbuf+pmh->msg_length))&&(length!=0));
	mh.msg_length = le;
	get_ie_buffer(DELAY_BACK_THREAD,outbuf, &le);
	socket_send(0, outbuf, le, get_device_type());
}
/*******************************************************************************
* 函数名称:measure_delay_time
* 功    能:读取计算时延，读取所有的时延根据同步字留下有效的数据其余数据归零
* 参    数:
* 参数名称         类型                描述
* 返回值:
* 
* 说   明:
* 日   期     版本    作者 H4  修改人      DEBUG
* -----------------------------------------------------------------------------
*2013.1.31  v1.0		h4		
*******************************************************************************/
void measure_dalay_time(void)
{
	U16 read_syn,read_opt;
	U16	i,j;
	drv_read_fpga(0x100,&read_syn);
	drv_read_fpga(0x10d,&read_opt);
	DEBUG_PRINTF("read syn is %d read opt is %d  \r\n",read_syn,read_opt);
	for(i=0;i<8;i++){
		drv_read_fpga(0x130+i,&j);
		if(((read_syn&(1<<i))!=0)){
			if(j<=DELAY_TIME_OFFSIZE){
				j=1;
			}else{
				j-=DELAY_TIME_OFFSIZE;
			}
			dru_time_delay_rsp.read_delay[i]=j/2;
		}else{
			dru_time_delay_rsp.read_delay[i]=0;
		}
		if((((read_opt)&(1<<i))!=0)){
			dru_time_delay_rsp.port=i;
		}
		DEBUG_PRINTF("read delay j %d \r\n",j);
		DEBUG_PRINTF("read delay %d \r\n",dru_time_delay_rsp.read_delay[i]);
	}
}
/***********************************************************************
** Funcation Name : delay_mearsure_rsp_deal
** Author         : H4
** Description    : 处理时延查询返回函数并置位标识位
** Input          : 
** Output         : 
** date           : 2013年02月21日 星期四 10时43分22秒
** Version        : V1.0
** Modify         : 
***********************************************************************/
void delay_mearsure_rsp_deal(U8 *inbuf)
{
	U16 i,index;
	ie_head *ih;
	struct msg_head_t *pmh=((struct msg_head_t*)inbuf);
	CU8 *p;
	time_delay_rsp *pd_rsp;
	U16 le,symbol,length;
	p=inbuf;
	le=0;
	le+=sizeof(msg_head);
	p+=sizeof(msg_head);
	do{
		ih=(ie_head *)p;
		symbol=ih->symbol;
		length=ih->length;
		DEBUG_PRINTF("delay_measure_req_deal %d %d\n",symbol,length);
		if(symbol==911){
			pd_rsp=(time_delay_rsp *)p;
			index=get_index_bydev(pmh->RRU_ID);
			if(connect_buffer[index].p_dc->delay_flag==READ_DELAY_TIME_START){
				for(i=0;i<8;i++){
					connect_buffer[index].p_dc->next_delay[i]=pd_rsp->read_delay[i];
				}
				connect_buffer[index].p_dc->up_link_port=pd_rsp->port;
				connect_buffer[index].p_dc->delay_flag=READ_DELAY_TIME_DONE;
			}else{
				DEBUG_PRINTF("time delay has been got!");
			}
			DEBUG_PRINTF("dru_time_delay_rsp\n");
		}
		p+=length;
	}while((p<(inbuf+pmh->msg_length))&&(length!=0));
}
/***********************************************************************
** Funcation Name : delay_time_config
** Author         : H4
** Description    : 配置时延，只有远端单元存在配置时延。 
** Input          : 
** Output         : 
** date           : 2013年02月25日 星期一 10时10分39秒
** Version        : V1.0
** Modify         : 
***********************************************************************/
void delay_time_config(unsigned int index)
{
	U8 outbuf[MAX_SEND_LENGTH];
	U16 le;
	msg_head mh;
	mh.BBU_ID=0;
	mh.RRU_ID=get_dev_byindex(index);
	mh.light_port_number=0;
	mh.msg_number=TIME_DELAY_CFG;
	mh.serial_number=create_serial_number();
	mh.msg_length=sizeof(mh)+sizeof(dru_time_delay_config);
	clean_ie_buffer(DELAY_THREAD);
	if(add_ie_buffer(DELAY_THREAD,(U8 *)&mh, sizeof(msg_head))!=0){
		DEBUG_PRINTF("add ie buffer error !\n");
		return;
	}
	if(tim_delay_config_flag==1){//2014.05.05
		dru_time_delay_config.delay12=connect_buffer[index].p_dc->max_delay-connect_buffer[index].p_dc->path_delay;
	}else{
		dru_time_delay_config.delay12=0;
	}
	DEBUG_PRINTF("@@@@DEV %d CONFIG DELAY IS %d\r\n",connect_buffer[index].dev,dru_time_delay_config.delay12);
	dru_time_delay_config.delay34=connect_buffer[index].p_dc->path_delay;
	if(add_ie_buffer(DELAY_THREAD,(U8 *)&dru_time_delay_config, sizeof(dru_time_delay_config))!=0){
		DEBUG_PRINTF("add ie buffer error !\n");
		return;
	}
	get_ie_buffer(DELAY_THREAD,outbuf, &le);
	socket_send(index, outbuf, le, get_device_type());
}
/***********************************************************************
** Funcation Name : delay_time_config_deal
** Author         : H4
** Description    : 
** Input          : 
** Output         : 
** date           : 2013年02月25日 星期一 10时36分10秒
** Version        : V1.0
** Modify         : 
***********************************************************************/
void delay_time_config_deal(U8 *inbuf)
{
	msg_head mh;
	ie_head *ih;
	struct msg_head_t *pmh=((struct msg_head_t*)inbuf);
	U8 outbuf[MAX_SEND_LENGTH];
	CU8 *p;
	U16 le,symbol,length;
	mh.BBU_ID=pmh->BBU_ID;
	mh.RRU_ID=pmh->RRU_ID;
	mh.light_port_number=0;
	mh.msg_number=TIME_DELAY_CFG_RSP;
	mh.serial_number=pmh->serial_number;
	p=inbuf;
	le=0;
	le+=sizeof(msg_head);
	p+=sizeof(msg_head);
	clean_ie_buffer(DELAY_BACK_THREAD);
	if(add_ie_buffer(DELAY_BACK_THREAD,(U8 *)&mh, sizeof(msg_head))!=0){
		DEBUG_PRINTF("add ie buffer error !\n");
		return;
	}
	do{
		ih=(ie_head *)p;
		symbol=ih->symbol;
		length=ih->length;
		DEBUG_PRINTF("delay_measure_config_deal %d %d\n",symbol,length);
		if(symbol==921){
			memcpy((U8 *)&(dru_time_delay_config),(U8 *)p,sizeof(dru_time_delay_config));//2014.05.05
			set_delay_time(&dru_time_delay_config);
			le+=sizeof(dru_time_delay_config_rsp);
			if(add_ie_buffer(DELAY_BACK_THREAD,(U8 *)&dru_time_delay_config_rsp, sizeof(dru_time_delay_config_rsp))!=0){
				DEBUG_PRINTF("add ie buffer error !\n");
				return;
			}
			DEBUG_PRINTF("dru_time_delay_rsp\n");
		}
		p+=length;
	}while((p<(inbuf+pmh->msg_length))&&(length!=0));
	mh.msg_length = le;
	get_ie_buffer(DELAY_BACK_THREAD,outbuf, &le);
	socket_send(0, outbuf, le, get_device_type());
}
/***********************************************************************
** Funcation Name : set_delay_time
** Author         : H4
** Description    : 设置fpga的时延寄存器
** Input          : 
** Output         : 
** date           : 2013年02月25日 星期一 10时41分20秒
** Version        : V1.0
** Modify         : 
***********************************************************************/
void set_delay_time(time_delay_config *tdc)
{
	U16 i;
	i=tdc->delay12;
	drv_write_fpga(0x013a,i);
	drv_write_fpga(0x013a,i+1);
	drv_write_fpga(0x013a,i+2);
}
/***********************************************************************
** Funcation Name : delay_time_config_rsp_deal
** Author         : H4
** Description    : 时延配置返回处理函数
** Input          : 
** Output         : 
** date           : 2013年02月25日 星期一 10时59分44秒
** Version        : V1.0
** Modify         : 
***********************************************************************/
void delay_time_config_rsp_deal(U8 *inbuf)
{
	U16 index;
	ie_head *ih;
	struct msg_head_t *pmh=((struct msg_head_t*)inbuf);
	CU8 *p;
	U16 le,symbol,length;
	p=inbuf;
	le=0;
	le+=sizeof(msg_head);
	p+=sizeof(msg_head);
	do{
		ih=(ie_head *)p;
		symbol=ih->symbol;
		length=ih->length;
		DEBUG_PRINTF("dru_time_delay_config_rsp %d %d\n",symbol,length);
		if(symbol==931){
			index=get_index_bydev(pmh->RRU_ID);
			if(connect_buffer[index].p_dc->delay_flag==CONFIG_DELAY_TIME_START){
				connect_buffer[index].p_dc->delay_flag=CONFIG_DELAY_TIME_DONE;
			}else{
				DEBUG_PRINTF("time delay has been config!");
			}
			DEBUG_PRINTF("dru_time_delay_config_rsp\n");
		}
		p+=length;
	}while((p<(inbuf+pmh->msg_length))&&(length!=0));
}
/***********************************************************************
** Funcation Name : set_delay_enable
** Author         : H4
** Description    :	set the enable flag of device .some one connect after this 
**					will not be enable
** Input          : 
** Output         : 
** date           : 2013年04月17日 星期三 10时15分04秒
** Version        : V1.0
** Modify         : 
***********************************************************************/
void set_delay_enable(void)
{
	U16 i;
	for(i=0;i<MAX_CONNECT_NUMBER;i++){ 
		if(connect_buffer[i].connect_status==CLIENT_CONNECTED){
			connect_buffer[i].p_dc->enable_flag=DELAY_MEASURE_ENABLE;
		}
	}
}
/***********************************************************************
** Funcation Name : delay_measure_read
** Author         : H4
** Description    : 将所有的在位设备时延查询回到主机
** Input          : 
** Output         : 
** date           : 2013年02月25日 星期一 16时57分36秒
** Version        : V1.0
** Modify         : 
***********************************************************************/
int delay_measure_read(void)
{
	U16 i,j;
	for(i=0;i<MAX_CONNECT_NUMBER;i++){
		if(connect_buffer[i].connect_status==CLIENT_CONNECTED){
			if(connect_buffer[i].p_dc->enable_flag==DELAY_MEASURE_ENABLE){
				if(connect_buffer[i].p_dc->delay_flag!=READ_DELAY_TIME_DONE){
					if(connect_buffer[i].p_dc->delay_flag!=READ_DELAY_TIME_START){
						connect_buffer[i].p_dc->delay_flag=READ_DELAY_TIME_START;
					}
					if(connect_buffer[i].dev_type==DEVICE_TYPE_MAIN){
						measure_dalay_time();
						DEBUG_PRINTF("READ MAIN DELAY \r\n");
						for(j=0;j<8;j++){
							connect_buffer[i].p_dc->next_delay[j]=dru_time_delay_rsp.read_delay[j];
						}
						connect_buffer[i].p_dc->delay_flag=READ_DELAY_TIME_DONE;
						return 1;
					}else{
						DEBUG_PRINTF("READ DELAY %d \r\n",i);
						delay_measure_req(i);
						return 1;
					}
				}
			}
		}
	}
	return 0;
}
/***********************************************************************
** Funcation Name : path_delay_calculate
** Author         : H4
** Description    : 计算路径时延 
** Input          : 
** Output         : 
** date           : 2013年02月26日 星期二 09时32分10秒
** Version        : V1.0
** Modify         : 
***********************************************************************/
int fact_delay_calculate(void)
{
	U8 port;
	U16 i,j,tmp;
	U16 d_t,d_m;
	U64 id,id_t,t;
	int ret;
	for(i=0;i<MAX_CONNECT_NUMBER;i++){
		if(connect_buffer[i].connect_status==CLIENT_CONNECTED){
			if(connect_buffer[i].dev_type==DEVICE_TYPE_MAIN){
				for(j=0;j<8;j++){
					//sleep(1);
					id_t=0xF00000000008+j;
					ret=get_index_byid(id_t);
					if((ret!=-1)&&(ret!=0)){
						port=connect_buffer[ret].p_dc->up_link_port;
						
						connect_buffer[i].p_dc->next_delay[j] -=connect_buffer[ret].p_dc->next_delay[port] ;
						printf("TIME DELAY MAIN id =%llx  port = %d delay = %d \r\n",id_t,port,connect_buffer[i].p_dc->next_delay[j]);
					}
				}
					
			}else{
				id=connect_buffer[i].dev_id;
				t=id;
				for(tmp=0;tmp<10;tmp++){
					if((t&(8<<(tmp*4)))!=0){
						continue;
					}else{
						break;
					}
				}
				//t=id+(0x80<<(tmp*4));
				for(j=0;j<8;j++){
					t=0x08+j;
					id_t=id+(t<<(tmp*4));
					//sleep(1);
					ret=get_index_byid(id_t);
					if((ret!=-1)&&(ret!=0)){
						port=connect_buffer[ret].p_dc->up_link_port;
						//printf("TIME DELAY MAIN id =%llx  port = %d",id_t,port);
						
						connect_buffer[i].p_dc->next_delay[j] -=connect_buffer[ret].p_dc->next_delay[port] ;
					}
				}

			}
		}
	}
	return 0;

}
/***********************************************************************
** Funcation Name : path_delay_calculate
** Author         : H4
** Description    : 计算路径时延 
** Input          : 
** Output         : 
** date           : 2013年02月26日 星期二 09时32分10秒
** Version        : V1.0
** Modify         : 
***********************************************************************/
int path_delay_calculate(void)
{
	U8 i_tmp;
	U16 i,j,delay_tmp;
	U64 id_tmp,f_id,data_tmp;
	int ret;
	for(i=0;i<MAX_CONNECT_NUMBER;i++){
		delay_tmp=0;
		if(connect_buffer[i].connect_status==CLIENT_CONNECTED){
			if(connect_buffer[i].p_dc->enable_flag==DELAY_MEASURE_ENABLE){
				if(connect_buffer[i].dev_type==DEVICE_TYPE_MAIN){
					connect_buffer[i].p_dc->path_delay=0;
				}else{
					id_tmp=connect_buffer[i].dev_id;
					if(id_tmp>0xF00000000000){
						i_tmp=(U8)id_tmp;
						i_tmp&=0x07;
						delay_tmp+=connect_buffer[0].p_dc->next_delay[i_tmp];	
						for(j=1;j<11;j++){
							data_tmp=8;
							if((id_tmp&(data_tmp<<(j*4)))>0){
								data_tmp=0xffffffffffff;
								f_id=(0xf00000000000|(id_tmp&(~(data_tmp<<(j*4)))));
								i_tmp=(U8)((id_tmp>>(j*4))&0x07);
								DEBUG_PRINTF("FID IS %x I_TMP IS %d\r\n",f_id,i_tmp);
								ret=get_index_byid(f_id);
								if((ret==-1)||(ret>=MAX_CONNECT_NUMBER)){
									return -1;
								}
								delay_tmp+=connect_buffer[ret].p_dc->next_delay[i_tmp];
							}else{
								break;
							}
						}
						connect_buffer[i].p_dc->path_delay=delay_tmp;
						DEBUG_PRINTF("THE %d PATH IS %d\r\n",connect_buffer[i].dev,delay_tmp);
					}
				}
			}
		}
	}
	return 0;
}
/***********************************************************************
** Funcation Name : max_delay_calculate
** Author         : H4
** Description    : 查找最大时延并设置 
** Input          : 
** Output         : 
** date           : 2013年02月27日 星期三 15时41分07秒
** Version        : V1.0
** Modify         : 
***********************************************************************/
int max_delay_calculate(void)
{
	U16 i,max_delay=0;
	for(i=0;i<MAX_CONNECT_NUMBER;i++){
		if(connect_buffer[i].connect_status==CLIENT_CONNECTED){
			if(connect_buffer[i].p_dc->enable_flag==DELAY_MEASURE_ENABLE){
				if(connect_buffer[i].dev_type==DEVICE_TYPE_RAU){
					if(max_delay<connect_buffer[i].p_dc->path_delay){
						max_delay=connect_buffer[i].p_dc->path_delay;	
					}else{
						;
					}
				}
			}
		}
	}
	for(i=0;i<MAX_CONNECT_NUMBER;i++){
		if(connect_buffer[i].connect_status==CLIENT_CONNECTED){
			connect_buffer[i].p_dc->max_delay=max_delay;
		}
	}
	return 0;
}
int get_max_delay(void)
{
	int i = 0;
	
	for(i = 0; i < MAX_CONNECT_NUMBER; i++){
		if(connect_buffer[i].connect_status == CLIENT_CONNECTED){
			return (connect_buffer[i].p_dc->max_delay);
		}
	}
	return 0;
}
/***********************************************************************
** Funcation Name : delay_measure_config
** Author         : H4
** Description    :	配置时延 
** Input          : 
** Output         : 
** date           : 2013年02月27日 星期三 15时59分28秒
** Version        : V1.0
** Modify         : 
***********************************************************************/
int delay_measure_config(void)
{
	U16 i;
	for(i=0;i<MAX_CONNECT_NUMBER;i++){
		if(connect_buffer[i].connect_status==CLIENT_CONNECTED){
			if(connect_buffer[i].p_dc->enable_flag==DELAY_MEASURE_ENABLE){
				if(connect_buffer[i].p_dc->delay_flag!=CONFIG_DELAY_TIME_DONE){
					if(connect_buffer[i].p_dc->delay_flag!=CONFIG_DELAY_TIME_START){
						connect_buffer[i].p_dc->delay_flag=CONFIG_DELAY_TIME_START;
					}
					if(connect_buffer[i].dev_type==DEVICE_TYPE_MAIN||connect_buffer[i].dev_type==DEVICE_TYPE_EXPEND){
						connect_buffer[i].p_dc->delay_flag=CONFIG_DELAY_TIME_DONE;
						return 1;
					}else{
						delay_time_config(i);
						return 1;
					}
				}
			}
		}
	}
	return 0;
}
/***********************************************************************
** Funcation Name : delay_measure_action
** Author         : H4
** Description    : 时延测量启动 
** Input          : 
** Output         : 
** date           : 2013年03月08日 星期五 16时46分49秒
** Version        : V1.0
** Modify         : 
***********************************************************************/
void *delay_measure_action(void *vp)
{
	if(get_device_type()==TYPE_MAIN){
		set_delay_enable();
		DEBUG_PRINTF("++++++++++++++++++++++++++++++++++++++++++++++++++++\r\n");
		do{
			sleep(1);
		}while(delay_measure_read());
		DEBUG_PRINTF("time_delay_read ok!\r\n");
		DEBUG_PRINTF("-----------------------------------------------------\r\n");
			sleep(1);
		fact_delay_calculate();//40m计算真实光纤时延
			sleep(1);
		if(path_delay_calculate()==-1){
			dru_thread_struct.status=THREAD_EXIT;
			return ;
		}
		DEBUG_PRINTF("path_delay_calculate ok!\r\n");
		DEBUG_PRINTF("=======================================================\r\n");
		do{
			sleep(1);
		}while(max_delay_calculate());
		DEBUG_PRINTF("max_delay_calculate ok!\r\n");
		DEBUG_PRINTF("!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!\r\n");
		do{
			sleep(1);
		}while(delay_measure_config());
		DEBUG_PRINTF("time_delay_config ok!\r\n");
		DEBUG_PRINTF("$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$\r\n");
	}else{
		DEBUG_PRINTF("IS NOT SUPPORT THIS DEVICE\r\n");
	}
	dru_thread_struct.status=THREAD_EXIT;
}
/***********************************************************************
** Funcation Name : delay_measure_thread
** Author         : H4
** Description    : 
** Input          : 
** Output         : 
** date           : 2013年03月11日 星期一 14时21分22秒
** Version        : V1.0
** Modify         : 
***********************************************************************/
void delay_measure_thread(void)
{
	pthread_t pid;
	if(dru_thread_struct.control==THREAD_START){
		if(dru_thread_struct.status!=THREAD_RUNNING){
			dru_thread_struct.status=THREAD_RUNNING;
			if(pthread_create(&pid,0,delay_measure_action,0)){
				DEBUG_PRINTF("pthread_creat hardware_alarm_thread error.\r\n");
				dru_thread_struct.status=THREAD_NULL;
				return 1;
			}
			dru_thread_struct.control=THREAD_DONE;
		}
	}
}
/***********************************************************************
** Funcation Name :
** Author         : H4
** Description    : 
** Input          : 
** Output         : 
** date           : 2013年04月17日 星期三 14时03分22秒
** Version        : V1.0
** Modify         : 
***********************************************************************/
unsigned long delay_time=0;
void time_control_delay(void)
{
	delay_time++;
	if(delay_time>=100*60*60*1){
		delay_measure_start();
		delay_time=0;
	}
}
/***********************************************************************
** Funcation Name : delay_measure_start
** Author         : H4
** Description    : set the flag of delay measeure thread to boot
** Input          : 
** Output         : 
** date           : 2013年04月17日 星期三 09时56分26秒
** Version        : V1.0
** Modify         : 
***********************************************************************/
void delay_measure_start(void)
{
	dru_thread_struct.control=THREAD_START;
}
/***********************************************************************
** Funcation Name : delay_measure_action_change
** Author         : H4
** Description    : 
** Input          : 
** Output         : 
** date           : 2013年03月08日 星期五 17时26分51秒
** Version        : V1.0
** Modify         : 
***********************************************************************/

void delay_measure_action_change(int argc, char * argv[])
{
	msg_tmp.mtype = MSG_FUN_RESULT;	
	sprintf(msg_tmp.mtext, "display...\n");
	if(argc != 1){
		printf("input para cnt is not 1.\r\n");
		sprintf(msg_tmp.mtext, "input para cnt is not 1.\r\n");
		msg_send(TASK2_MODULE, (char *)&msg_tmp, strlen(msg_tmp.mtext));
		return ;
	}
	//tim_delay_config_flag=1;//2014.05.05
	delay_measure_start();	
	msg_send(TASK2_MODULE, (char *)&msg_tmp, strlen(msg_tmp.mtext));
	return ;
}
int set_auto_delay_sw(int val)
{
	tim_delay_config_flag = val;
	delay_measure_start();	
	printf("\n\ndelay_sw=%d\n", val);
}
