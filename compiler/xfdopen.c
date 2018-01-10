/*
**	XPL to C source language translator runtime
**
**	Builtin function:  xfdopen()
**
**	Author: Daniel Weaver
*/

#include <stdio.h>
#include <stdlib.h>
#include <errno.h>
#include <sys/types.h>
#include <unistd.h>

#define __xpl_FILE FILE
#include "xpl.h"

/*
**	__xpl_xfdopen(int fd, __xpl_string *mode)
**
**	The XPL interface to the fdopen() call
**	On sucessful completion:
**		1) the stream pointer is placed in both the
**			__xpl_FILE_in[] and __xpl_FILE_out[] arrays.
**		2) __xpl_xerrno is set to return status
**
**	Returns the index of the new stream pointers.
**		Returns a negative number for errors.
*/
int
__xpl_xfdopen(int fd, __xpl_string *mode)
{
	int i, l, x;
	char m[8];
	FILE *stream;

	if (fd < 0) {
		__xpl_xerrno = EBADF;
		return -1;
	}
	/* Find an empty slot for the I/O stream */
	for (x = 0; ; x++) {
		if (x >= __XPL_FILE_MAX) {
			__xpl_xerrno = ENFILE;
			return -1;
		}
		if (!__xpl_FILE_in[x] && !__xpl_FILE_out[x]) {
			break;
		}
	}
	/* Move the mode bytes into a C string */
	l = mode->_Length;
	__xpl_FILE_eol[x] = 0;
	__xpl_FILE_flags[x] = 0;
	for (i = 0; i < l && i < (sizeof(m) - 1); i++) {
		if ((m[i] = mode->_Address[i]) == 'b') {
			__xpl_FILE_flags[x] = 1;
		}
	}
	m[i] = '\0';
	__xpl_xerrno = 0;
	stream = fdopen(fd, m);
	if (stream) {
		__xpl_FILE_in[x] = __xpl_FILE_out[x] = stream;
		return x;
	}
	__xpl_xerrno = errno;
	return -1;
}
