/*
**	XPL to C source language translator runtime
**
**	Support for fixed length character strings
**
**	Author: Daniel Weaver
*/

#include <stdio.h>
#include <string.h>
#include "xpl.h"

/*
**	__xpl_build_descriptor(__xpl_string *outstr, int length, void *addr)
**
**	Create a string descriptor using length and address.
**	return the descriptor in outstr
*/
__xpl_string *
__xpl_build_descriptor(__xpl_string *outstr, int length, void *addr)
{
	outstr->_Length = length;
	outstr->_Address = addr;
	return outstr;
}

/*
**	__xpl_get_charfixed(__xpl_string *outstr, int maxlen, void *addr)
**
**	Create a string descriptor using maxlen and addr.
**	The string will be scanned for a terminating null up to maxlen characters.
**	The result will be either strlen(addr) or maxlen which ever is smaller.
**	return the descriptor in outstr
*/
__xpl_string *
__xpl_get_charfixed(__xpl_string *outstr, int maxlen, void *addr)
{
	int i;
	char *s;

	s = (char *) addr;
	for (i = 0; i < maxlen; i++) {
		if (*s++ == 0) {
			break;
		}
	}
	outstr->_Length = i;
	outstr->_Address = addr;
	return outstr;
}

/*
**	__xpl_put_charfixed(int maxlen, void *addr, __xpl_string *string)
**
**	Copy the string into a charfixed array.
**	return the source descriptor
*/
__xpl_string *
__xpl_put_charfixed(int length, void *addr, __xpl_string *string)
{
	char *s;

	if (length <= 0) {
		/* destination buffer too short */
		return string;
	}
	length--;
	if (length > string->_Length) {
		length = string->_Length;
		if (length < 0) {
			length = 0;
		}
	}
	strncpy(addr, string->_Address, (size_t) length);
	s = (char *) addr;
	s[length] = '\0';
	return string;
}

