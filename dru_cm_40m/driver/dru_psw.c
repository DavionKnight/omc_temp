#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <sys/mman.h>
#include <string.h>
#include <time.h>
#include "../../driver/dru_psw.h"
#include "../../driver/omap_gpio.h"
#include "../../driver/omap_pinmux.h"
#include "../../driver/dru_reg.h"
#define POWER_OUT_PIN ((0x00000001<<0)|(0x00000001<<2)|(0x00000001<<3)|(0x00000001<<4)|(0x00000001<<6))

void power_sw_init(void)
{
	unsigned int tmp;
	tmp = gpio_t->dir01;
	tmp&=(~(0x00000001<<0))&(~(0x00000001<<2))&(~(0x00000001<<3))&(~(0x00000001<<4))&(~(0x00000001<<6));
	gpio_t->dir01=tmp;
}

void power_sw_open(void)
{
	gpio_t->clr_data01=POWER_OUT_PIN;
}

void power_sw_close(void)
{
	
	gpio_t->set_data01=POWER_OUT_PIN;
}
