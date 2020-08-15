#
#	XPL pretty printer
#	XPL Cross reference
#	XPL tags file generator for VI
#
XPLDIR=..
CFLAGS= -I$(XPLDIR)
XPL=$(XPLDIR)/xpl
LDLIBS=$(XPLDIR)/libxpl.a

TARGETS=xformat xtags xxref

%.c:	%.xpl
	$(XPL) $< -o $@

all:	$(TARGETS)
	cp $(TARGETS) ..

xformat:	xformat.o
xformat.o:	xformat.c
xformat.c:	xformat.xpl

xtags:		xtags.o
xtags.o:	xtags.c
xtags.c:	xtags.xpl

xxref:		xxref.o
xxref.o:	xxref.c
xxref.c:	xxref.xpl

test:	$(TARGETS)
	./xxref xxref.xpl

format: xformat
	cp xformat.xpl xformat.tmp
	./xformat xformat.tmp -i4 -T0 -o xformat.xpl.new
	mv xformat.xpl.new xformat.xpl
	cp xtags.xpl xtags.tmp
	./xformat xtags.tmp -i4 -T0 -o xtags.xpl.new
	mv xtags.xpl.new xtags.xpl
	cp xxref.xpl xxref.tmp
	./xformat xxref.tmp -i4 -T0 -o xxref.xpl.new
	mv xxref.xpl.new xxref.xpl

clean:
	rm -f *.o *.xh *.c *.tmp tags $(TARGETS)

