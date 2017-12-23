/*
**	XPL to C source language translator runtime
**
**	Builtin I/O functions.  INPUT, OUTPUT and FILE.
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

/* I/O definitions */
__xpl_FILE *__xpl_FILE_in[__XPL_FILE_MAX];
__xpl_FILE *__xpl_FILE_out[__XPL_FILE_MAX];
int __xpl_FILE_flags[__XPL_FILE_MAX];
int xerrno;
XPL_ADDRESS file_record_size;	/* Max record size for builtin function FILE() */

/*
**	__xpl_io_init()
**
**	Initialize the XPL I/O module.
**	This function must be called at startup.
*/
void
__xpl_io_init(void)
{
	__xpl_FILE_in[0] = stdin;
	__xpl_FILE_out[0] = stdout;
	__xpl_FILE_out[1] = stderr;
}

/*
**	__xpl_output(int num, __xpl_string *str)
**
**	Output a string.
**	Returns the input string
*/
__xpl_string *
__xpl_output(int num, __xpl_string *str)
{
	int err, i, l;

	if (num < 0 || num >= __XPL_FILE_MAX) {
		fprintf(stderr, "Unit number out of range: %d.\n", num);
		xerrno = EBADF;
		return str;
	}
	if (!__xpl_FILE_out[num]) {
		fprintf(stderr, "Unit number not open: %d.\n", num);
		xerrno = ENODEV;
		return str;
	}
	clearerr(__xpl_FILE_out[num]);
	xerrno = 0;
	err = 0;
	l = str->_Length;
	for (i = 0; i < l; i++) {
		err = putc(str->_Address[i], __xpl_FILE_out[num]);
		if (err == EOF) {
			break;
		}
	}
	if (__xpl_FILE_flags[num] == 0 && err != EOF) {
		err = putc('\n', __xpl_FILE_out[num]);
	}
	if (err == EOF) {
		/* Should never get an EOF so it must be an error */
		xerrno = ferror(__xpl_FILE_out[num]);
	}
	return str;
}

/*
**	__xpl_input(__xpl_string *outstr, int num)
**
**	Read a string.  Read characters up to a <cr> or <lf>
**	Returns the string and sets xerrno if errors
*/
__xpl_string *
__xpl_input(__xpl_string *outstr, int num)
{
	long i, sz;
	char *cp;
	int chr;

	if (num < 0 || num >= __XPL_FILE_MAX) {
		xerrno = EBADF;
		outstr->_Length = 0;
		return outstr;
	}
	if (!__xpl_FILE_in[num]) {
		xerrno = ENODEV;
		outstr->_Length = 0;
		return outstr;
	}
	outstr->_Length = 0;
	if (freelimit - freepoint < 1024) {
		/* Try to get at least 1024 bytes for the input record */
		compactify();
	}
	xerrno = 0;
	cp = (char *) freepoint;
	outstr->_Length = 0;
	outstr->_Address = cp;
	sz = freelimit - freepoint;
	/* Really big reads clog up the free string area */
	if (sz > 1024) {
		sz = 1024;
	}
	for (i = 0; i < sz; i++) {
		chr = getc(__xpl_FILE_in[num]);
		if (chr == EOF) {
			if (feof(__xpl_FILE_in[num])) {
				xerrno = __XPL_EOF;
			} else {
				xerrno = ferror(__xpl_FILE_in[num]);
			}
			break;
		}
		if (__xpl_FILE_flags[num]) {
			/* Return all characters.  Including <cr> and <lf> */
			outstr->_Length++;
			*cp++ = chr;
			if (chr == '\n') {
				break;
			}
			continue;
		}
		if (chr == '\n') {
			/* If this is a null string then pad it with one blank */
			if (outstr->_Length == 0) {
				/* This allows the user to detect EOF */
				outstr->_Length++;
				*cp++ = ' ';
			}
			break;
		}
		if (chr != '\r') {
			outstr->_Length++;
			*cp++ = chr;
		}
	}
	if (outstr->_Length) {
		freepoint += outstr->_Length;
	}
	return outstr;
}

/*
**	xfopen(__xpl_string *filename, __xpl_string *mode)
**
**	The XPL interface to the fopen() call
**	On sucessful completion the stream pointer is placed in both the
**	__xpl_FILE_in[] and __xpl_FILE_out[] arrays.
**
**	Returns the index of the new stream pointers.  A negative number for errors.
*/
int
xfopen(__xpl_string *filename, __xpl_string *mode)
{
	int i, l, x;
	char m[8];
	__xpl_string *name;
	FILE *stream;

	/* Find an empty slot for the I/O stream */
	for (x = 0; ; x++) {
		if (x >= __XPL_FILE_MAX) {
			xerrno = ENFILE;
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
	name = __xpl_cat(__xpl_pool(), filename, &null_terminator);
	xerrno = 0;
	stream = fopen(name->_Address, m);
	if (stream) {
		__xpl_FILE_in[x] = __xpl_FILE_out[x] = stream;
		return x;
	}
	xerrno = errno;
	return -1;
}

/*
**	xclose(int unit)
**
**	The XPL interface to the fclose() call
**	This function will close both the input and output streams.
**
**	Returns a non-zero number for errors.
*/
int
xfclose(int unit)
{
	int val;
	FILE *fp;

	if (unit < 0 || unit >= __XPL_FILE_MAX) {
		xerrno = EBADF;
		return -1;
	}
	xerrno = 0;
	if ((fp = __xpl_FILE_in[unit])) {
		val = fclose(__xpl_FILE_in[unit]);
		if (val == EOF) {
			xerrno = errno;
		}
	}
	if (fp != __xpl_FILE_out[unit]) {
		if (__xpl_FILE_out[unit]) {
			val = fclose(__xpl_FILE_out[unit]);
			if (val == EOF) {
				xerrno = errno;
			}
		}
	}
	__xpl_FILE_in[unit] = __xpl_FILE_out[unit] = (void *) 0;
	return xerrno;
}

/*
**	__xpl_read_file(int unit, int rec, void *buffer, unsigned long rec_size)
**
**	Direct I/O read
**	Return xerrno
*/
int
__xpl_read_file(int unit, int rec, void *buffer, unsigned long rec_size)
{
	FILE *fp;
	int val;

	if (unit < 0 || unit >= __XPL_FILE_MAX) {
		xerrno = EBADF;
		return -1;
	}
	if (file_record_size) {
		if (rec_size > file_record_size) rec_size = file_record_size;
	}
	xerrno = 0;
	if ((fp = __xpl_FILE_in[unit])) {
		val = lseek(fileno(fp), (off_t) (rec * rec_size), SEEK_SET);
		if (val == -1) {
			xerrno = errno;
			return -1;
		}
		val = read(fileno(fp), buffer, (size_t) rec_size);
		if (val < 0) {
			xerrno = errno;
			return -1;
		}
		return val;
	}
	xerrno = ENODEV;
	return -1;
}

/*
**	__xpl_write_file(int unit, int rec, void *buffer, unsigned long rec_size)
**
**	Direct I/O write
**	Return xerrno
*/
int
__xpl_write_file(int unit, int rec, void *buffer, unsigned long rec_size)
{
	FILE *fp;
	int val;

	if (unit < 0 || unit >= __XPL_FILE_MAX) {
		xerrno = EBADF;
		return -1;
	}
	if (file_record_size) {
		if (rec_size > file_record_size) rec_size = file_record_size;
	}
	xerrno = 0;
	if ((fp = __xpl_FILE_out[unit])) {
		val = lseek(fileno(fp), (off_t) (rec * rec_size), SEEK_SET);
		if (val == -1) {
			xerrno = errno;
			return -1;
		}
		val = write(fileno(fp), buffer, (size_t) rec_size);
		if (val < 0) {
			xerrno = errno;
			return -1;
		}
		return val;
	}
	xerrno = ENODEV;
	return -1;
}
