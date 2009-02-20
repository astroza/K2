INCLUDE=-I../include
CFLAGS=-O3

OBJ=$(BD)/kepler.o

$(OBJ): kepler.c
	@echo "CC $(@F)"
	$(CC) $(CFLAGS) $(INCLUDE) -c $? -o $@

all: $(OBJ)
