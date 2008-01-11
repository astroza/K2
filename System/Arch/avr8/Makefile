include defs.mak

CFLAGS=-mmcu=$(MCU) -DF_CPU=$(F_CPU) -O3
INCLUDE=-Iinclude

SERIAL_OBJ=$(BD)/hal_serial.o
TIMER_OBJ=$(BD)/hal_timer.o

all: $(SERIAL_OBJ) $(TIMER_OBJ)

$(SERIAL_OBJ): hal_serial.c
	@echo "CC $(@F)"
	$(CC) -DF_CPU=$(F_CPU) $(CFLAGS) $(INCLUDE) -c $? -o $@

$(TIMER_OBJ): hal_timer.c
	@echo "CC $(@F)"
	$(CC) $(CFLAGS) $(INCLUDE) -c $? -o $@
