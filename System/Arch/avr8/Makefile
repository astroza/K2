include avr8.mak

CFLAGS=-mmcu=$(MCU) -DF_CPU=$(F_CPU) -O3
INCLUDE=-I./include

all: hal_serial.o hal_timer.o

hal_serial.o: hal_serial.c
	$(CC) -DF_CPU=$(F_CPU) $(CFLAGS) $(INCLUDE) -c $? -o $(BD)/$@

hal_timer.o: hal_timer.c
	$(CC) $(CFLAGS) $(INCLUDE) -c $? -o $(BD)/$@

clean:
	rm -f *.o
