CC=gcc
OBJS=detox64.o
CFLAGS=-Wall -O2 -pedantic -ansi
TARGET=detox64

.c.o:	$(CC) -c $*.c

all:	$(OBJS)
	$(CC) -o $(TARGET) $(OBJS)

clean:
	rm -f $(OBJS) $(TARGET)

