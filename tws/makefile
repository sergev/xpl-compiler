#
#	XPL grammar analyzer makefile for GNU/Linux make.
#
XPLDIR=..
CFLAGS= -I$(XPLDIR)
XPL=$(XPLDIR)/xpl
LDLIBS=$(XPLDIR)/libxpl.a

TARGETS=analyzer xa skeleton

%.c:	%.xpl
	$(XPL) $< -o $@

all:	$(TARGETS)

analyzer:	analyzer.o
analyzer.o:	analyzer.c
analyzer.c:	analyzer.xpl

xa:	xa.o
xa.o:	xa.c
xa.c:	xa256.xpl
	$(XPL) xa256.xpl -o xa.c

skeleton:	skeleton.o
skeleton.o:	skeleton.c
skeleton.c:	skeleton.xpl

xpl.msp:	xa xpl.bnf
	./xa xpl.bnf -o xpl.msp

clean:
	rm -f *.o *.xh *.c $(TARGETS) xpl.msp

