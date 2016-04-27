# Makefile for mysh program
# Dug Threewitt - dstgh6
# CS2750 - Mon / Wed

CC = gcc
CFLAGS = -g 
TARGET = mysh
OBJS = mysh.o func.o func.h

$(TARGET): $(OBJS)
	$(CC) -o $@ $(OBJS)

.c.o:
	$(CC) $(CFLAGS) -c $<

clean:
	/bin/rm -f *.o *~
