CFLAGS=-O3
INCLUDE=-I../include -I../arch/$(ARCH)/include
OBJ=$(BD)/ucmp.o

$(OBJ): ucmp.c
	@echo "CC $(@F)"
	$(CC) -DADDR_SIZE=$(ADDRSZ) -DSERIAL_SPEED=$(SP) $(CFLAGS) $(INCLUDE) -c $? -o $@

all: $(OBJ)
