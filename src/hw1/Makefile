CC = gcc
CFLAGS = -Wall

all: xpmimage.o
	$(CC) $(CFLAGS) xpmimage.o main.c -o main

xpmimage.o: xpmimage.c xpmimage.h
	$(CC) -c $(CFLAGS) -o $@ $< 

clean:
	rm -f *.o
	rm -f main
