INCLUDE=-I../include
CFLAGS=-O3

OBJ=$(BD)/kepler.o

$(OBJ): kepler.c
	@echo "CC $@"
	$(CC) $(CFLAGS) $(INCLUDE) -c $? -o $@

all: $(OBJ)
