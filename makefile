#
#	XPL compiler makefile for GNU/Linux make.
#
#	Before running this makefile check the xpl.h file for
#	correct definition of 64 bit integers on your system.
#

all:
	make -C compiler
	make -C tws

clean:
	make -C compiler clean
	make -C tws clean
	rm -f xpl xpl.h libxpl.a

