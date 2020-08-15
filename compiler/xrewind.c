/*
**	XPL to C source language translator runtime
**
**	Builtin function: xrewind()
**
**	Author: Daniel Weaver
*/

#include <stdio.h>
#include <errno.h>

#define __xpl_FILE FILE
#include "xpl.h"

/*
**	__xpl_xrewind(unit)
**
**	Call the C library function rewind() to set the file position to zero.
**
**	Return 0 if sucessful.  Return -1 and set __xpl_xerrno if error.
*/
int
__xpl_xrewind(int unit)
{
	if (unit < 0 || unit >= __XPL_FILE_MAX) {
		fprintf(stderr, "Unit number out of range: %d.\n", unit);
		__xpl_xerrno = EBADF;
		return -1;
	}
	if (!__xpl_FILE_out[unit]) {
		fprintf(stderr, "Unit number not open: %d.\n", unit);
		__xpl_xerrno = ENODEV;
		return -1;
	}
	fflush(__xpl_FILE_out[unit]);
	rewind(__xpl_FILE_out[unit]);
	__xpl_FILE_eol[unit] = 0;
	__xpl_xerrno = 0;
	return 0;
}
