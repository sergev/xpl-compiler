/*
**	XPL to C source language translator runtime
**
**	Get and Set functions for I/O flags.
**
**	Author: Daniel Weaver
*/

#include <stdio.h>
#include <errno.h>

#include "xpl.h"

/*
**	__xpl_xio_get_flags(unit)
**
**	Get the I/O flag
**
**	Returns the current flag value.
**	If unit is out of range returns -1 and sets __xpl_xerrno to EBADF.
*/
int
__xpl_xio_get_flags(int unit)
{
	if (unit < 0 || unit >= __XPL_FILE_MAX) {
		__xpl_xerrno = EBADF;
		return -1;
	}
	__xpl_xerrno = 0;
	return __xpl_FILE_flags[unit];
}

/*
**	__xpl_xio_set_flags(unit, value)
**
**	Set the I/O flag to value.
**
**	Returns zero on success.
**	If unit is out of range returns -1 and sets __xpl_xerrno to EBADF.
*/
int
__xpl_xio_set_flags(int unit, int value)
{
	if (unit < 0 || unit >= __XPL_FILE_MAX) {
		__xpl_xerrno = EBADF;
		return -1;
	}
	__xpl_FILE_flags[unit] = value;
	__xpl_xerrno = 0;
	return 0;
}
