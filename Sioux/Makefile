CC = gcc
CFLAGS = -Wall -Wextra -Werror -I../libs/Reseau
LDFLAGS = -L../libs/Reseau
LIBS = -lreseau
TARGET = sioux
SRCS = $(TARGET).c
OBJS = $(SRCS:.c=.o)

all: $(TARGET)

$(TARGET): $(OBJS)
	$(CC) $(LDFLAGS) -o $@ $^ $(LIBS)

%.o: %.c
	$(CC) $(CFLAGS) -c -o $@ $<

.PHONY: clean

clean:
	rm -f $(TARGET) $(OBJS)
