CFLAGS=-O3
INCLUDE=-I../include -I../Arch/$(ARCH)/include
OBJ=$(BD)/ucmp.o

$(OBJ): ucmp.c
	@echo "CC $(@F)"
	$(CC) -DSERIAL_SPEED=$(SP) $(CFLAGS) $(INCLUDE) -c $? -o $@

all: $(OBJ)
