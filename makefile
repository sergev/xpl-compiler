#
#	XPL compiler makefile for GNU/Linux make.
#
#	Before running this makefile check the xpl.h file for
#	correct definition of 64 bit integers on your system.
#

all:
	$(MAKE) $(MFLAGS) -C compiler
	$(MAKE) $(MFLAGS) -C tws
	$(MAKE) $(MFLAGS) -C tools

clean:
	$(MAKE) $(MFLAGS) -C compiler clean
	$(MAKE) $(MFLAGS) -C tws clean
	$(MAKE) $(MFLAGS) -C tools clean
	rm -f xpl xpl.h libxpl.a xformat xtags xxref

