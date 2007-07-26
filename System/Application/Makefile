CC=gcc
UCMP=../Network
machine=i386_unix
ucmp_obj=ucmp.o
i386_obj=dependent.o

all: sensor_sim

sensor_sim: ucmp
	$(CC) sim_test.c -c -I$(UCMP) -I$(UCMP)/machine/$(machine)
	$(CC) sim_test.o $(ucmp_obj) $(i386_obj) -o sim_test -lpthread
	

ucmp:
	make ucmp -C $(UCMP) machine=$(machine)
clean:
	rm -f $(i386_obj) $(ucmp_obj) sim_test sim_test.o
