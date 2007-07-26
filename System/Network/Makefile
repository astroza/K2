CC=gcc
CFLAGS=-O3 -Wall
DEPENDENT_CODE=machine
SELECTED=$(DEPENDENT_CODE)/$(machine)

dependent:
	make -C $(SELECTED) to=$(PWD)

ucmp: dependent
	$(CC) $(CFLAGS) ucmp.c -I. -I$(SELECTED) -c -o $(PWD)/ucmp.o
