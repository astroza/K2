include $(BASE)/arch/$(ARCH)/defs.mak

BUILD_DIR=$(PWD)/build_$(ARCH)
MCU=$(patsubst Makefile.%,%,$(notdir $(firstword $(MAKEFILE_LIST)))) # Con esto no es necesario definir MCU directamente :)
OBJ=$(patsubst %.c,%.o,$(notdir $(FILE)))

tree:
	mkdir -p $(BUILD_DIR)

cos: tree
	$(MAKE) --quiet -C $(BASE)/arch/$(ARCH) MCU=$(MCU) F_CPU=$(CPU_FREQUENCY) BD=$(BUILD_DIR)
	$(MAKE) --quiet -C $(BASE)/core CC=$(CC) ARCH=$(ARCH) BD=$(BUILD_DIR)
	$(MAKE) --quiet -C $(BASE)/net CC=$(CC) ARCH=$(ARCH) SP=$(SERIAL_SPEED) BD=$(BUILD_DIR) ADDRSZ=$(ADDR_SIZE)

compile:
	@echo "CC $(OBJ)"
	$(CC) -mmcu=$(MCU) -O3 -Wall -I$(BASE)/include/ -I$(BASE)/arch/$(ARCH)/include/ -c $(FILE) -o $(BUILD_DIR)/$(OBJ)

build_image:
	@echo "LD $(IMAGE)"
	$(CC) -mmcu=$(MCU) -Wl,-T,$(BASE)/arch/$(ARCH)/k2.lds $(BUILD_DIR)/*.o -o $(IMAGE)
