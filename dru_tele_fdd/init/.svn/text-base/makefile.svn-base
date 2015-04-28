CROSS=arm-linux-
#CROSS = /usr/armtool/arm-2009q1/bin/arm-none-linux-gnueabi-
#CROSS=
CC=$(CROSS)gcc

SRC=$(wildcard *.c) $(wildcard ../driver/*.c) \
$(wildcard ../../net/*.c) $(wildcard ../../protocol/*.c) \
$(wildcard ../../task/msg/*.c) $(wildcard ../../task/rs485/*.c) \
$(wildcard ../../task/localcom/*.c) $(wildcard ../../task/omccom/*.c) \
$(wildcard ../../task/udpcom/*.c)  \
$(wildcard ../../task/hdalarm/*.c)  \
$(wildcard ../../task/log/*.c)  \
$(wildcard ../../task/*.c) \
$(wildcard ../../sqlite/*.c)

NODIR=$(notdir $(SRC))
OBJS=$(patsubst %.c,%.o,$(NODIR))
DEBUG=
CFLAGS=$(DEBUG) -Wall -O -g -L../../common -I../../common -lcom -lpthread -lsqlite3 -lm
TARGET=dat13-h0173a-xxx

$(TARGET):$(OBJS)
	$(CC) $(OBJS) $(CFLAGS)  -o $@  
%.o:%.c
	$(CC) $(CFLAGS) -c $< -o $@
%.o:../driver/%.c
	$(CC) $(CFLAGS) -c $< -o $@
%.o:../../net/%.c
	$(CC) $(CFLAGS) -c $< -o $@
%.o:../../protocol/%.c
	$(CC) $(CFLAGS) -c $< -o $@
%.o:../../sqlite/%.c
	$(CC) $(CFLAGS) -c $< -o $@
%.o:../../task/msg/%.c
	$(CC) $(CFLAGS) -c $< -o $@
%.o:../../task/rs485/%.c
	$(CC) $(CFLAGS) -c $< -o $@
%.o:../../task/localcom/%.c
	$(CC) $(CFLAGS) -c $< -o $@
%.o:../../task/omccom/%.c
	$(CC) $(CFLAGS) -c $< -o $@
%.o:../../task/udpcom/%.c
	$(CC) $(CFLAGS) -c $< -o $@
%.o:../../task/hdalarm/%.c
	$(CC) $(CFLAGS) -c $< -o $@
%.o:../../task/log/%.c
	$(CC) $(CFLAGS) -c $< -o $@
%.o:../../task/%.c
	$(CC) $(CFLAGS) -c $< -o $@

clean:

	rm -rf *.o $(TARGET)
