#
#	XPL compiler makefile for GNU/Linux make.
#
CFLAGS= -Wall
LIBFILES=xplrt.o unique.o expand.o xio.o hex.o chrfixed.o xprintf.o

%.c:	%.xpl
	xpl $< -o $@

xpl:	libxpl.a xcom.o xpl.h
	gcc $(CFLAGS) xcom.o libxpl.a -o xpl
	cp -p xpl ..
	cp -p xpl.h ..

xcom.o: xcom.c

libxpl.a: $(LIBFILES)
	rm -f libxpl.a
	ar -r -sv libxpl.a $(LIBFILES)
	cp -p libxpl.a ..

samp:	libxpl.a xpl xpl.h samp.xpl
	xpl samp.xpl
	gcc samp.c libxpl.a -o samp

clean:
	rm -f *.o *.xh xpl libxpl.a samp.c samp.xh samp

tags:	xcom.c $(LIBFILES:.o=.c)
	ctags xcom.c $(LIBFILES:.o=.c)
