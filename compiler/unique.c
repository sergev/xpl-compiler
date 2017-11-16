/*
**	XPL to C source language translator runtime
**
**	Builtin function: unique()
**
**	Author: Daniel Weaver
*/

#include <stdio.h>
#include <string.h>
#include "xpl.h"

/*
**	__xpl_unique(__xpl_string *outstr, __xpl_string *str)
**
**	Move a string to the top of the string space.
**	If the length is non-zero the string is always moved.
**	Return a descriptor pointing to the new string.
*/
__xpl_string *
__xpl_unique(__xpl_string *outstr, __xpl_string *str)
{
	if (str->_Length == 0) {
		outstr->_Length = 0;
		return outstr;
	}
	if (str->_Length >= freelimit - freepoint) {
		space_needed = str->_Length;
		compactify();
	}
	memmove((void *) (freepoint), str->_Address, str->_Length);
	outstr->_Length = str->_Length;
	outstr->_Address = (char *) freepoint;
	freepoint += str->_Length;
	return outstr;
}

/*
**	unique(string)
**
**	User level interface to __xpl_unique().
**	The receiving sequence is responsible for moving the string descriptor.
*/
__xpl_string *
unique(__xpl_string *str)
{
	return __xpl_unique(__xpl_pool(), str);
}
