# application configuration region
PRJNAME = lineout
CC = gcc
CFLAGS = -Wall -g
LDFLAGS = -lm
OBJS = xpmimage.o xpmps.o gtransforms.o

#!!! you shouldn't modify anything beyond this point
all: $(OBJS)
	$(CC) $(CFLAGS) $(OBJS) main.c -o $(PRJNAME) $(LDFLAGS)
hw4: all
	./$(PRJNAME) -f test2.ps -w 200 -h 200 -t transf.tsf -o out.xpm

./%.o: ./%.c
	$(CC) -c $(CFLAGS) -o $@ $<

clean:
	rm -f *.o
	rm -f *.xpm
	rm -f $(PRJNAME)
