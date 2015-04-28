CROSS=arm-linux-
#CROSS = /usr/armtool/arm-2009q1/bin/arm-none-linux-gnueabi-
#CROSS=
CC=$(CROSS)gcc
LD=$(CROSS)ld
AR=$(CROSS)ar

CFLAGS=-Wall -O -g -Werror -lpthread
TARGET=common
OBJS=$(patsubst %.c,%.o,$(wildcard *.c))
SRC=$(wildcard *.c)
LIB=libcom.a

$(LIB):$(OBJS)
	$(AR) rcs  $@ $(OBJS)  
%.o:%.c
	$(CC) $(CFLAGS) -c $< -o $@
clean:
	rm -rf *.o $(LIB)
