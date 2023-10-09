#Makefile général
export CC = gcc
export LD = gcc
export CLIB = ar cq
export CFLAGS = -Wall
export CDEBUG = -g -DDEBUG

DIRS = libs/Reseau Sioux

all:
	@for dir in $(DIRS); do \
        echo "Building in $$dir..."; \
        $(MAKE) -C $$dir; \
	done

.PHONY : clean 

clean:
	# A faire
