CC = gcc
CFLAGS = -Wall -g
LDFLAGS = -lm

all: xpmimage.o xpmps.o
	$(CC) $(CFLAGS) xpmimage.o xpmps.o main.c -o main $(LDFLAGS)

xpmimage.o: xpmimage.c xpmimage.h
	$(CC) -c $(CFLAGS) -o $@ $< 

xpmps.o : xpmps.c xpmps.h
	$(CC) -c $(CFLAGS) -o $@ $<

clean:
	rm -f *.o
	rm -f *.xpm
	rm -f main
