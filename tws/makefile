#
#	XPL grammar analyzer makefile for GNU/Linux make.
#
#CFLAGS= -Wall
#CFLAGS= -g
XPL=../xpl
LIBXPL=../libxpl.a

%.c:	%.xpl
	$(XPL) -I $< -o $@

analyzer:	analyzer.xpl
	$(XPL) analyzer.xpl
	$(CC) $(CFLAGS) -I.. analyzer.c $(LIBXPL) -o analyzer

xa:	xa256.xpl
	$(XPL) -G xa256.xpl
	$(CC) $(CFLAGS) -I.. xa256.c $(LIBXPL) -o xa

skeleton:	skeleton.xpl
	$(XPL) -I skeleton.xpl
	$(CC) $(CFLAGS) -I.. skeleton.c $(LIBXPL) -o skeleton

xpl.msp:	xa xpl.bnf
	./xa xpl.bnf -o xpl.msp

clean:
	rm -f *.o *.xh *.c analyzer skeleton xa

