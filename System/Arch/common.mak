include $(BASE)/Arch/$(ARCH)/defs.mak

BUILD_DIR=$(PWD)/build_$(ARCH)
OBJ=$(patsubst %.c,%.o,$(notdir $(FILE)))

tree:
	mkdir -p $(BUILD_DIR)

cos: tree
	make --quiet -C $(BASE)/Arch/$(ARCH) MCU=$(MCU) F_CPU=$(CPU_FREQUENCY) BD=$(BUILD_DIR)
	make --quiet -C $(BASE)/TaskManager CC=$(CC) ARCH=$(ARCH) BD=$(BUILD_DIR)
	make --quiet -C $(BASE)/Network CC=$(CC) ARCH=$(ARCH) SP=$(SERIAL_SPEED) BD=$(BUILD_DIR)

compile:
	@echo "CC $(OBJ)"
	$(CC) -mmcu=$(MCU) -O3 -Wall -I$(BASE)/include/ -I$(BASE)/Arch/$(ARCH)/include/ -c $(FILE) -o $(BUILD_DIR)/$(OBJ)

build_image:
	@echo "LD $(IMAGE)"
	$(CC) -mmcu=$(MCU) -Wl,-T,$(BASE)/Arch/$(ARCH)/cos.lds $(BUILD_DIR)/*.o -o $(IMAGE)
