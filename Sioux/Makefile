CFLAGS = -Wall -Wextra -Werror -I../libs/Reseau -I../libs/Shmem -g
LDFLAGS = -L../libs/Reseau -L../libs/Shmem
LIBS = -lreseau -lshmem
TARGET = sioux
SRCS = $(wildcard *.c)
HEADERS = $(wildcard *.h)
OBJS = $(SRCS:.c=.o)

all: $(TARGET)

$(TARGET): $(OBJS)
	$(CC) $(LDFLAGS) -o $@ $^ $(LIBS)

%.o: %.c $(HEADERS)
	$(CC) $(CFLAGS) -c -o $@ $<

.PHONY: clean

clean:
	rm -f $(TARGET) $(OBJS)
