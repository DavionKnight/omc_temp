/*************************************************************************
***********************************************************************
** FILE NAME: hdalsrm.h
** AUTHOR:h4
** MAIL: harry.zhu.h4@gmail.com
** TIME: 2013年03月04日 星期一 10时51分03秒
************************************************************************
************************************************************************/
#ifndef _HDALARM_H
#define _HDALARM_H
void *hardware_alarm_thread(void *pvoid);
int create_hardware_alarm_thread(void);
void *alarm_irq_thread(void *args);
int create_irq_thread(void);
#endif
