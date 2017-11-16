/*
**	XPL to C source language translator runtime
**
**	Builtin function: hex()
**
**	Author: Daniel Weaver
*/

#include <stdio.h>
#include <string.h>
#include "xpl.h"

/*
**	__xpl_hex(__xpl_string *outstr, XPL_LONG number)
**
**	Convert the number to a hexadecimal character string.
**	Return the new descriptor.
*/
__xpl_string *
__xpl_hex(__xpl_string *outstr, XPL_LONG number)
{
	__xpl_string xpl_descriptor;
	char numbuf[32];
	static char digit[] = {"0123456789abcdef"};
	XPL_UNSIGNED_LONG n, value;
	int i;

	value = number;
	for (i = (int) sizeof(numbuf) - 1; i > 0; i--) {
		n = value & 15;
		numbuf[i] = digit[n];
		value >>= 4;
		if (value == 0) {
			break;
		}
	}
	xpl_descriptor._Length = (int) sizeof(numbuf) - i;
	xpl_descriptor._Address =  &numbuf[i];
	return __xpl_move_to_top(outstr, &xpl_descriptor);
}

/*
**	hex(number)
**
**	User level unterface to __xpl_hex().
**	The receiving sequence is responsible for moving the string descriptor.
*/
__xpl_string *
hex(XPL_LONG number)
{
	return __xpl_hex(__xpl_pool(), number);
}
