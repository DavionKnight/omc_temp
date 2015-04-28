CROSS=arm-linux-
#CROSS=
CC=$(CROSS)gcc
OBJS=$(patsubst %.c,%.o,$(wildcard *.c))
SRC=$(wildcard *.c)
CFLAGS=-Wall -O -g 
TARGET=test

$(TARGET):$(OBJS)
	$(CC) $(CFLAGS) $(OBJS) -o $@ 
%.o:%.c
	$(CC) $(CFLAGS) -c $< -o $@ 
clean:
	rm -rf *.o $(TARGET)
