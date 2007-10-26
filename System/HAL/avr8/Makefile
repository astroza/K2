CC=avr-gcc
CFLAGS=-mmcu=atmega168 -D__AVR_ATmega168__ -DF_CPU=16000000 -O3
INCLUDE=-I./include

all:
	$(CC) $(CFLAGS) $(INCLUDE) hal_serial.c -c
	$(CC) $(CFLAGS) $(INCLUDE) hal_timer.c -c
