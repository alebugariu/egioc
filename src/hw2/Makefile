CC = gcc
CFLAGS = -Wall -g

all: xpmimage.o
	$(CC) $(CFLAGS) xpmimage.o main.c -o main

xpmimage.o: xpmimage.c xpmimage.h
	$(CC) -c $(CFLAGS) -o $@ $< 

clean:
	rm -f *.o
	rm -f *.xpm
	rm -f main
