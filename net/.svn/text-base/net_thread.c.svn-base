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
#include <pthread.h>
#include "../common/status.h"
#include "dru_connect.h"
#include "../protocol/irdeal.h"
extern volatile unsigned char connectflag;	

 void creat_net_task(void) 
{ 
	pthread_t sl,sr,cr;
	int err;
//	hardware_init();
	if(get_device_type()==DEVICE_TYPE_MAIN){
		for(;;){
			err=dru_tcp_server_init();
			if(err!=0){
				DEBUG_PRINTF("\n server init error\n");
				sleep(10);
				continue;
			}else{
				connectflag=CLIENT_CONNECTED;
				break;
			}
		}
		err=pthread_create(&sl,0,listen_connect_thread,0);
		if(err!=0){
			DEBUG_PRINTF("\n create listen connect thread  error\n");
			return;
		}
		err=pthread_create(&sr,NULL,receive_client_thread,NULL);
		if(err!=0){
			DEBUG_PRINTF("\n receive client thread error  \n");
			return;
		}
		err=pthread_create(&cr,NULL,time_send_thread,NULL);
		if(err!=0){
			DEBUG_PRINTF("\n send to client thread error  \n");
			return;
		}
		err=pthread_create(&cr,NULL,time_transimt,NULL);
		if(err!=0){
			DEBUG_PRINTF("\n send to client thread error  \n");
			return;
		}
	}else if(get_device_type()==DEVICE_TYPE_RAU){
		/*err=dru_tcp_client_init();
		if(err!=0){
			DEBUG_PRINTF("\n client init error\n");
			return;
		}*/
		err=pthread_create(&cr,NULL,time_transimt,NULL);
		if(err!=0){
			DEBUG_PRINTF("\n send to client thread error  \n");
			return;
		}
		err=pthread_create(&cr,NULL,time_send_thread,NULL);
		if(err!=0){
			DEBUG_PRINTF("\n send to client thread error  \n");
			return;
		}
		err=pthread_create(&sr,NULL,receive_server_thread,NULL);
		if(err!=0){
			DEBUG_PRINTF("\n receive server thread error  \n");
			return;
		}
	}else if(get_device_type()==DEVICE_TYPE_EXPEND){
		/*err=dru_tcp_client_init();
		if(err!=0){
			DEBUG_PRINTF("\n client init error\n");
			return;
		}*/
		err=pthread_create(&cr,NULL,time_transimt,NULL);
		if(err!=0){
			DEBUG_PRINTF("\n send to client thread error  \n");
			return;
		}
		err=pthread_create(&cr,NULL,time_send_thread,NULL);
		if(err!=0){
			DEBUG_PRINTF("\n send to client thread error  \n");
			return;
		}else{
			DEBUG_PRINTF("\n time send thread ok\n");
		}
		err=pthread_create(&sr,NULL,receive_server_thread,NULL);
		if(err!=0){
			DEBUG_PRINTF("\n receive server thread error  \n");
			return;
		}else{
			DEBUG_PRINTF("\n receive server thread ok \n");
		}
	}
	return;
}
