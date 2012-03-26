# application configuration region
PRJNAME = lineout
CC = gcc
CFLAGS = -Wall -g
LDFLAGS = -lm
OBJS = xpmimage.o xpmps.o gtransforms.o

#!!! you shouldn't modify anything beyond this point
all: $(OBJS)
	$(CC) $(CFLAGS) $(OBJS) main.c -o $(PRJNAME) $(LDFLAGS)

./%.o: ./%.c
	$(CC) -c $(CFLAGS) -o $@ $<

clean:
	rm -f *.o
	rm -f *.xpm
	rm -f $(PRJNAME)
