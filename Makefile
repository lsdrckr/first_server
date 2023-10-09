#Makefile général
export CC = gcc
export LD = gcc
export CLIB = ar cq
export CFLAGS = -Wall
export CDEBUG = -g -DDEBUG

DIRS = Sioux libs

all : 
all:
	cd Sioux/ && make
	cd libs/ && make

.PHONY : clean 

clean:
	rm -f *.o
	cd Sioux/ && make
	cd libs/ && make
