INCLUDE=-I../include
CFLAGS=-O3

OBJ=$(BD)/sched.o

$(OBJ): sched.c
	@echo "CC $(@F)"
	$(CC) $(CFLAGS) $(INCLUDE) -c $? -o $@

all: $(OBJ)
