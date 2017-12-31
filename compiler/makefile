#
#	XPL compiler makefile for GNU/Linux make.
#
CFLAGS= -Wall
XPLDIR=..
LDLIBS=libxpl.a
LIBFILES=xplrt.o unique.o expand.o xio.o hex.o chrfixed.o xprintf.o\
	xunlink.o xrewind.o xmkstemp.o

%.c:	%.xpl
	./xpl $< -o $@

all:	xpl.h libxpl.a xpl
	cp -p xpl $(XPLDIR)
	cp -p xpl.h $(XPLDIR)
	cp -p libxpl.a $(XPLDIR)

xpl:	xcom.c xpl.h libxpl.a
	$(CC) $(CFLAGS) xcom.c $(LDLIBS) -o xpl

libxpl.a: $(LIBFILES)
	rm -f libxpl.a
	$(AR) -r -sv libxpl.a $(LIBFILES)

samp.c:	samp.xpl xpl.h xpl
samp:	samp.c libxpl.a
	$(CC) samp.c libxpl.a -o samp

clean:
	rm -f *.o *.xh xpl libxpl.a samp.c samp tags

tags:	xcom.c $(LIBFILES:.o=.c)
	ctags xcom.c $(LIBFILES:.o=.c)
