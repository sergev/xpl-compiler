/*
**	XPL to C source language translator runtime
**
**	Builtin functions.  xmkstemp()
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

static __xpl_string null_terminator = {1, "\000"};
static __xpl_string default_template = {7, "xXXXXXX"};

/*
**	xmkstemp(__xpl_string *template, __xpl_string *mode)
**
**	The XPL interface to the mkstemp() call
**	On sucessful completion:
**		1) the stream pointer is placed in both the
**			__xpl_FILE_in[] and __xpl_FILE_out[] arrays.
**		2) The template will be updated to hold the new filename.
**
**	Returns the index of the new stream pointers.
**		Returns a negative number for errors.
**		Updates the template to hold the new filename.
**
**	The file is not deleted when the fd is closed.
*/
int
xmkstemp(__xpl_string *template, __xpl_string *mode)
{
	int i, l, x, fd;
	char m[8];
	__xpl_string name;
	FILE *stream;

	if (!template) {
		/* A template is required */
		xerrno = EFAULT;
		return -1;
	}
	/* Validate the filename template */
	if (template->_Length == 0) {
		/* If the user call was xmkstemp('', '') this will fix
		   the error but the file cannot be deleted. */
		template = &default_template;
	}
	/* Find an empty slot for the I/O stream */
	for (x = 0; ; x++) {
		if (x >= __XPL_FILE_MAX) {
			xerrno = ENOMEM;
			return -1;
		}
		if (!__xpl_FILE_in[x] && !__xpl_FILE_out[x]) {
			break;
		}
	}
	/* Move the mode bytes into a C string */
	l = mode->_Length;
	__xpl_FILE_flags[x] = 0;
	for (i = 0; i < l && i < (sizeof(m) - 1); i++) {
		if ((m[i] = mode->_Address[i]) == 'b') {
			__xpl_FILE_flags[x] = 1;
		}
	}
	m[i] = '\0';
	__xpl_cat(&name, template, &null_terminator);
	xerrno = 0;
	fd = mkstemp(name._Address);
	if (fd < 0) {
		xerrno = errno;
		return fd;
	}
	stream = fdopen(fd, m);
	if (stream) {
		__xpl_FILE_in[x] = __xpl_FILE_out[x] = stream;
		template->_Length = name._Length;
		template->_Address = name._Address;
		return x;
	}
	xerrno = errno;
	return -1;
}
