#
#	XPL pretty printer
#	XPL Cross reference
#
XPLDIR=..
CFLAGS= -I$(XPLDIR)
XPL=$(XPLDIR)/xpl
LDLIBS=$(XPLDIR)/libxpl.a

TARGETS=xformat xxref

%.c:	%.xpl
	$(XPL) $< -o $@

all:	$(TARGETS)
	cp $(TARGETS) ..

xformat:	xformat.o
xformat.o:	xformat.c
xformat.c:	xformat.xpl

xxref:		xxref.o
xxref.o:	xxref.c
xxref.c:	xxref.xpl

test:	$(TARGETS)
	./xxref xxref.xpl

format: xformat
	cp xformat.xpl xformat.tmp
	./xformat xformat.tmp -i4 -T0 -o xformat.xpl.new
	mv xformat.xpl.new xformat.xpl
	cp xxref.xpl xxref.tmp
	./xformat xxref.tmp -i4 -T0 -o xxref.xpl.new
	mv xxref.xpl.new xxref.xpl

clean:
	rm -f *.o *.xh *.c *.tmp $(TARGETS)

