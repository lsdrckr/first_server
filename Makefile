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


start:
	sudo cd ./Sioux/ && ./sioux

.PHONY : clean 

clean:
	for dir in $(DIRS); do \
		$(MAKE) -C $$dir -f Makefile $@; \
	done
	rm -rf csv

