/*
**	XPL to C source language translator runtime
**
**	Builtin function: xunlink()
**
**	Author: Daniel Weaver
*/

#include <stdio.h>
#include <errno.h>
#include <unistd.h>

#include "xpl.h"

static __xpl_string null_terminator = {1, "\000"};

/*
**	__xpl_xunlink(filename)
**
**	Call the C library function unlink() to delete a file.
**
**	Return 0 if sucessful.  Return -1 and set __xpl_xerrno if error.
*/
int
__xpl_xunlink(__xpl_string *filename)
{
	__xpl_string path;
	int result;

	if (filename->_Length == 0) {
		__xpl_xerrno = ENOENT;
		return -1;
	}
	__xpl_xerrno = 0;
	__xpl_cat(&path, filename, &null_terminator);
	result = unlink(path._Address);
	if (result < 0) {
		__xpl_xerrno = errno;
	}
	return result;
}
