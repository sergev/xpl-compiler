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
**	xrewind(unit)
**
**	Call the C library function rewind() to set the file position to zero.
**
**	Return 0 if sucessful.  Return -1 and set xerrno if error.
*/
int
xrewind(int unit)
{
	int result;

	if (unit < 0 || unit >= __XPL_FILE_MAX) {
		fprintf(stderr, "Unit number out of range: %d.\n", unit);
		xerrno = EBADF;
		return -1;
	}
	if (!__xpl_FILE_out[unit]) {
		fprintf(stderr, "Unit number not open: %d.\n", unit);
		xerrno = ENODEV;
		return -1;
	}
	result = fseek(__xpl_FILE_out[unit], 0L, SEEK_SET);
	if (result != 0) {
		xerrno = errno;
		return result;
	}
	xerrno = 0;
	return 0;
}
