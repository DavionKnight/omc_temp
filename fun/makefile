CROSS=arm-linux-
#CROSS=
CC=$(CROSS)gcc
OBJS=$(patsubst %.c,%.o,$(wildcard *.c))
SRC=$(wildcard *.c)
CFLAGS=-Wall -O -g -L../common
TARGET=fun

$(TARGET):$(OBJS)
	$(CC) $(CFLAGS) $(OBJS) -o $@ -lcom 
%.o:%.c
	$(CC) $(CFLAGS) -c $< -o $@ -lcom
clean:
	rm -rf *.o $(TARGET)
