CFLAGS = -Wall -Wextra -Werror -I../libs/Reseau
LDFLAGS = -L../libs/Reseau
LIBS = -lreseau
TARGET = sioux
SRCS = $(TARGET).c
OBJS = $(SRCS:.c=.o)

all: $(TARGET)

$(TARGET): $(OBJS) http.o
	$(CC) $(LDFLAGS) -o $@ $^ $(LIBS)

http.o: http.h http.c
	clang $(CFLAGS) -c http.c


%.o: %.c
	$(CC) $(CFLAGS) -c -o $@ $<

.PHONY: clean

clean:
	rm -f $(TARGET) $(OBJS)
