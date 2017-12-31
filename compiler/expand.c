/*
**	XPL to C source language translator runtime
**
**	Tab expansion routine
**
**	Author: Daniel Weaver
*/

#include <stdio.h>
#include "xpl.h"

/*
**	__xplrt_expand(__xpl_string *outstr, __xpl_string *str, int tabstop)
**
**	Replace tabs with blanks.
**	Returns the new string.
**	If the string does not fit then the input string is returned.
*/
__xpl_string *
__xplrt_expand_tabs(__xpl_string *outstr, __xpl_string *str, int tabstop)
{
	__xpl_string s;
	long i, sz;
	char *cp;
	int chr, blanks, j;

	if (str->_Length <= 0 || tabstop < 1) {
		outstr->_Length = str->_Length;
		outstr->_Address = str->_Address;
		return outstr;
	}
	if (freelimit - freepoint < 1024) {
		/* Try to get at least 1024 bytes for the new string */
		space_needed = 1024;
		compactify();
	}
	cp = (char *) freepoint;
	s._Length = 0;
	s._Address = cp;
	sz = freelimit - freepoint;
	blanks = j = chr = 0;
	for (i = 0; i < sz; i++) {
		if (blanks) {
			blanks--;
		} else
		if (j >= str->_Length) {
			freepoint += s._Length;
			outstr->_Length = s._Length;
			outstr->_Address = s._Address;
			return outstr;
		} else {
			chr = str->_Address[j++];
			if (chr == '\t') {
				blanks = tabstop - (s._Length % tabstop);
				chr = ' ';
				blanks--;
			}
		}
		s._Length++;
		*cp++ = chr;
	}
	/* String overflow.  Return the old string. */
	outstr->_Length = str->_Length;
	outstr->_Address = str->_Address;
	return outstr;
}

/*
**	__xpl_expand_tabs(string, tabstop)
**
**	User level interface to __xplrt_expand_tabs()
**	The receiving sequence is responsible for moving the string descriptor.
*/
__xpl_string *
__xpl_expand_tabs(__xpl_string *str, int tabstop)
{
	return __xplrt_expand_tabs(__xpl_pool(), str, tabstop);
}
