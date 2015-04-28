#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <sys/mman.h>
#include <string.h>
#include <time.h>
#include "omap_gpio.h"
unsigned int iic_gpio_save;
void dru_iic_dir(unsigned int pin,unsigned char level)
{
	volatile unsigned int tmp;
	//pin_tmp=pin;
	//reg=(unsigned int *)(&(gpio_t->dir01));
	tmp=gpio_t->dir01;
	if(level==1){
		tmp&=~pin;
	}else{

		tmp|=pin;
	}
	//usleep(1);
	gpio_t->dir01=tmp;
}
unsigned int   dru_iic_gpio_in(unsigned int pin)
{
	volatile unsigned int tmp ;
	tmp=gpio_t->in_data01;
	//printf( " indata iic=%x\n",tmp);
	if((tmp&pin)!=0)
		return 1;
	else
		return 0;
}
