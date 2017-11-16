/*
**	XPL to C source language translator
**
**	A C style printf() function implemented for XPL.
**
**	Author: Daniel Weaver
*/

#include <stdio.h>
#include <stdarg.h>
#include <errno.h>
#include <string.h>

#include "xpl.h"

#define BYTE __xpl_get_byte
#define SUBSTR __xpl_substr3

static __xpl_string x1 = {1, " "};
static __xpl_string null_format = {6, "(null)"};

//	%[flags][width][.precision][length]specifier
#define FLAG_PLUS	0x0001
#define FLAG_MINUS	0x0002
#define FLAG_SPACE	0x0004
#define FLAG_POUND	0x0008
#define FLAG_ZERO	0x0010
#define FLAG_PRECISION	0x1000
#define FLAG_NEGATIVE	0x2000
static char xp_flag_char[] = {"+- #0"};
static int xp_flags;		/* Flag field */
static int xp_width;		/* Field width */
static int xp_precision;	/* Precision */
static int xp_length;		/* Length */

enum xp_arg_length {ARG_CHAR, ARG_SHORT_INT, ARG_LONG_LONG_INT, ARG_LONG_INT,
	ARG_INTMAX_T, ARG_SIZE_T, ARG_PTRDIFF_T, ARG_LONG_DOUBLE, ARG_INT};

static char xp_buffer[64];
static int xp_bp;		/* First valid character in xp_buffer[] */
static int xp_used;		/* Number of characters printed for the current format specifier */

static int xp_char_count;	/* Total number of characters printed */

/*
**	print_default(int value, void *ref)
**
**	Print one character on unit zero.
*/
static void
print_default(int value, void *ref)
{
	putc(value, __xpl_FILE_out[0]);
	xp_char_count++;
}

/*
**	print_unit(int value, void *ref)
**
**	Print one character on the selected unit.
*/
static void
print_unit(int value, void *ref)
{
	putc(value, (FILE *) ref);
	xp_char_count++;
}

/*
**	build_char(int value, void *ref)
**
**	Add one character to the selected string.
*/
static void
build_char(int value, void *ref)
{
	__xpl_string *s = (__xpl_string *) ref;

	if (s->_Length > 0) {
		__xpl_cat(s, s, &x1);
	} else {
		__xpl_cat(s, &x1, &x1);
		__xpl_substr2(s, s, 1);
	}
	__xpl_put_byte(s, s->_Length - 1, value);
	xp_char_count++;
}

/*
**	build_charfix(int value, void *ref)
**
**	Write the buffer into a fixed length character string.
*/
static void
build_charfix(int value, void *ref)
{
	__xpl_string *s = (__xpl_string *) ref;

	__xpl_put_byte(s, xp_char_count++, value);
}

/*
**	xp_convert_integer(value)
**
**	Convert an unsigned number to character string.
*/
static void
xp_convert_integer(XPL_UNSIGNED_LONG l)
{
	XPL_UNSIGNED_LONG n;

	for (xp_bp = sizeof(xp_buffer) - 1; xp_bp > 0; xp_bp--) {
		n = l % 10;
		xp_buffer[xp_bp] = (char) (n + '0');
		l /= 10;
		if (l == 0) {
			break;
		}
	}
}

/*
**	xp_convert_binary(value, type)
**
**	Convert a number to a hexadecimal or octal character string.
*/
static void
xp_convert_binary(XPL_UNSIGNED_LONG l, int t)
{
	XPL_UNSIGNED_LONG n, mask;
	int shift;
	char *digits;
	static char lc[] = {"0123456789abcdef"};
	static char uc[] = {"0123456789ABCDEF"};

	if (t == 'o') {	/* Octal */
		mask = 7;
		shift = 3;
	} else {
		mask = 15;
		shift = 4;
	}
	if (t == 'X') {	/* Upper case hex */
		digits = uc;
	} else {
		digits = lc;
	}
	for (xp_bp = sizeof(xp_buffer) - 1; xp_bp > 0; xp_bp--) {
		n = l & mask;
		xp_buffer[xp_bp] = digits[n];
		l >>= shift;
		if (l == 0) {
			break;
		}
	}
}

/*
**	xprintf(function, reference, format, ...)
**
**	Implement xsprintf(), xfprintf, xprintf.
**	These functions operate as close as possible to the C language
**	equivalants but without floating point.
**
**	%[flags][width][.precision][length]specifier
*/
int
xprintf(int func, void *ref, __xpl_string *xfmt, ...)
{
	va_list args;
	int i, c, len, n, recover, sign;
	void (*print) (int, void *);
	XPL_LONG value;
	__xpl_string *s;
	short signed_short_value;
	int signed_int_value;
	unsigned char unsigned_char_value;
	unsigned short unsigned_short_value;
	unsigned int unsigned_int_value;
	void *void_star;

	va_start(args, xfmt);
	__xpl__errno = 0;
	if (!xfmt) {
		xfmt = &null_format;
	}
	/* Validate the function as much as possible. */
	switch (func) {
	case 0:
		print = print_default;
		break;
	case 1:
		i = (int) ((XPL_ADDRESS) ref);
		if (i < 0 || i > __XPL_FILE_MAX) {
			__xpl__errno = -2;
			return 0;
		}
		ref = (void *) __xpl_FILE_out[i];
		print = print_unit;
		break;
	case 2:
		print = build_char;
		((__xpl_string *) ref)->_Length = 0;
		break;
	case 3:
		if (((__xpl_string *) ref)->_Length < 2) {
			__xpl__errno = -3;
			return 0;
		}
		print = build_charfix;
		break;
	default:
		__xpl__errno = -1;
		return 0;
	}
	xp_char_count = 0;
	len = xfmt->_Length;
	for (i = 0; i < len; ) {
		c = BYTE(xfmt, i++);
		if (c != '%') {
			print(c, ref);
			continue;
		}
		recover = i;
		c = BYTE(xfmt, i++);
		if (c == '%') {
			print(c, ref);
			continue;
		}
		/* decode the flags */
		xp_flags = 0;
		for (n = 0; xp_flag_char[n]; n++) {
			if (c == xp_flag_char[n]) {
				xp_flags |= 1 << n;
				c = BYTE(xfmt, i++);
				n = -1;
			}
		}
		/* Get the Width */
		if (c == '*') {
			xp_width = va_arg(args, int);
			c = BYTE(xfmt, i++);
		} else {
			xp_width = 0;
			/* The BYTE function returns ZERO if out of range */
			while (c >= '0' && c <= '9') {
				xp_width = (xp_width * 10) + (c - '0');
				c = BYTE(xfmt, i++);
			}
		}
		/* Get the Precision */
		xp_precision = 0;
		if (c == '.') {
			xp_flags |= FLAG_PRECISION;
			c = BYTE(xfmt, i++);
			if (c == '*') {
				xp_precision = va_arg(args, int);
				c = BYTE(xfmt, i++);
			} else {
				while (c >= '0' && c <= '9') {
					xp_precision = (xp_precision * 10) + (c - '0');
					c = BYTE(xfmt, i++);
				}
			}
		}
		/* Get the Length */
		switch (c) {
		case 'h':
			if (BYTE(xfmt, i) == 'h') {
				xp_length = ARG_CHAR;
				i++;
			} else {
				xp_length = ARG_SHORT_INT;
			}
			c = BYTE(xfmt, i++);
			break;
		case 'l':
			if (BYTE(xfmt, i) == 'l') {
				xp_length = ARG_LONG_LONG_INT;
				i++;
			} else {
				xp_length = ARG_LONG_INT;
			}
			c = BYTE(xfmt, i++);
			break;
		case 'j':
			xp_length = ARG_INTMAX_T;
			c = BYTE(xfmt, i++);
			break;
		case 'z':
			xp_length = ARG_SIZE_T;
			c = BYTE(xfmt, i++);
			break;
		case 't':
			xp_length = ARG_PTRDIFF_T;
			c = BYTE(xfmt, i++);
			break;
		case 'L':
			xp_length = ARG_LONG_DOUBLE;
			c = BYTE(xfmt, i++);
			break;
		default:
			xp_length = ARG_INT;
			break;
		}
		sign = 0;
		/* Fetch the argument */
		switch (c) {
		case 'd':
		case 'i':
			switch (xp_length) {
			case ARG_CHAR:
				/* XPL does not have signed characters */
				unsigned_char_value = va_arg(args, int);
				value = unsigned_char_value;
				break;
			case ARG_SHORT_INT:
				signed_short_value = va_arg(args, int);
				value = signed_short_value;
				break;
			case ARG_INT:
				signed_int_value = va_arg(args, int);
				value = signed_int_value;
				break;
			default:
				value = va_arg(args, XPL_LONG);
				break;
			}
			if (value < 0) {
				value = - value;
				xp_flags |= FLAG_NEGATIVE;
			}
			if ((xp_flags & FLAG_NEGATIVE) != 0) {
				sign = '-';
			} else
			if ((xp_flags & FLAG_PLUS) != 0) {
				sign = '+';
			} else
			if ((xp_flags & FLAG_SPACE) != 0) {
				sign = ' ';
			}
			break;
		case 'u':
		case 'o':
		case 'x':
		case 'X':
			switch (xp_length) {
			case ARG_CHAR:
				unsigned_char_value = va_arg(args, int);
				value = unsigned_char_value;
				break;
			case ARG_SHORT_INT:
				unsigned_short_value = va_arg(args, int);
				value = unsigned_short_value;
				break;
			case ARG_INT:
				unsigned_int_value = va_arg(args, int);
				value = unsigned_int_value;
				break;
			default:
				value = va_arg(args, XPL_LONG);
				break;
			}
			break;
		default:
			break;
		}
		/* Process final character */
		switch (c) {
		case 'd':
		case 'i':
		case 'u':
			if ((xp_flags & FLAG_PRECISION) != 0 && xp_precision == 0 && value == 0) {
				xp_bp = sizeof(xp_buffer);
			} else {
				xp_convert_integer((XPL_UNSIGNED_LONG) value);
			}
			xp_used = sizeof(xp_buffer) - xp_bp;
			if ((xp_flags & (FLAG_PRECISION | FLAG_MINUS | FLAG_ZERO)) == FLAG_ZERO) {
				if (sign) {
					print(sign, ref);
					xp_used++;
				}
				while (xp_used < xp_width) {
					print('0', ref);
					xp_used++;
				}
			} else {
				/* Convert precision to the number of zeros needed */
				if (xp_precision > xp_used) {
					xp_precision -= xp_used;
				} else {
					xp_precision = 0;
				}
				if ((xp_flags & FLAG_MINUS) == 0) {
					n = xp_precision + xp_used;
					if (sign) {
						n++;
					}
					while (n++ < xp_width) {
						print(' ', ref);
						xp_used++;
					}
				}
				if (sign) {
					/* Print the sign */
					print(sign, ref);
					xp_used++;
				}
				while (xp_precision-- > 0) {
					print('0', ref);
					xp_used++;
				}
			}
			for (n = xp_bp; n < sizeof(xp_buffer); n++) {
				print(xp_buffer[n], ref);
			}
			while (xp_used < xp_width) {
				print(' ', ref);
				xp_used++;
			}
			break;
		case 'o':
		case 'x':
		case 'X':
			if ((xp_flags & FLAG_PRECISION) != 0 && xp_precision == 0 && value == 0) {
				xp_bp = sizeof(xp_buffer);
			} else {
				xp_convert_binary((XPL_UNSIGNED_LONG) value, c);
			}
			xp_used = sizeof(xp_buffer) - xp_bp;
			if ((xp_flags & (FLAG_PRECISION | FLAG_MINUS | FLAG_ZERO)) == FLAG_ZERO) {
				if ((xp_flags & FLAG_POUND) != 0) {
					print('0', ref);
					xp_used++;
					if (c == 'x' || c == 'X') {
						print(c, ref);
						xp_used++;
					}
				}
				while (xp_used < xp_width) {
					print('0', ref);
					xp_used++;
				}
			} else {
				if (value == 0 && xp_bp != sizeof(xp_buffer)) {
					xp_flags &= ~FLAG_POUND;
				}
				/* Convert precision to the number of zeros needed */
				if (xp_precision > xp_used) {
					xp_precision -= xp_used;
				} else {
					xp_precision = 0;
				}
				/* Convert octal # to precision */
				if ((xp_flags & FLAG_POUND) != 0 && c == 'o') {
					if (xp_precision == 0) {
						xp_precision = 1;
					}
					xp_flags &= ~FLAG_POUND;
				}
				if ((xp_flags & FLAG_MINUS) == 0) {
					n = xp_precision + xp_used;
					if ((xp_flags & FLAG_POUND) != 0) {
						n += 2;
					}
					while (n++ < xp_width) {
						print(' ', ref);
						xp_used++;
					}
				}
				if ((xp_flags & FLAG_POUND) != 0) {
					print('0', ref);
					print(c, ref);
					xp_used += 2;
				}
				while (xp_precision-- > 0) {
					print('0', ref);
					xp_used++;
				}
			}
			for (n = xp_bp; n < sizeof(xp_buffer); n++) {
				print(xp_buffer[n], ref);
			}
			while (xp_used < xp_width) {
				print(' ', ref);
				xp_used++;
			}
			break;
		case 's':
			s = va_arg(args, __xpl_string *);
			xp_bp = s->_Length;
			if ((xp_flags & FLAG_PRECISION) != 0) {
				/* Precision is used to truncate the string */
				if (xp_bp > xp_precision) {
					xp_bp = xp_precision;
				}
			}
			xp_used = xp_bp;
			if ((xp_flags & FLAG_MINUS) == 0) {
				for (n = xp_bp; n < xp_width; n++) {
					print(' ', ref);
					xp_used++;
				}
			}
			for (n = 0; n < xp_bp; n++) {
				print(BYTE(s, n), ref);
			}
			while (xp_used < xp_width) {
				print(' ', ref);
				xp_used++;
			}
			break;
		case 'p':
			void_star = va_arg(args, void *);
			xp_convert_binary((XPL_ADDRESS) void_star, 'x');
			xp_used = 2 + sizeof(xp_buffer) - xp_bp;
			if ((xp_flags & FLAG_MINUS) == 0) {
				while (xp_used < xp_width) {
					print(' ', ref);
					xp_used++;
				}
			}
			print('0', ref);
			print('x', ref);
			for (n = xp_bp; n < sizeof(xp_buffer); n++) {
				print(xp_buffer[n], ref);
			}
			while (xp_used < xp_width) {
				print(' ', ref);
				xp_used++;
			}
			break;
		case 'c':
			c = va_arg(args, int);
			xp_used = 1;
			if ((xp_flags & FLAG_MINUS) == 0) {
				while (xp_used < xp_width) {
					print(' ', ref);
					xp_used++;
				}
			}
			print(c, ref);
			while (xp_used < xp_width) {
				print(' ', ref);
				xp_used++;
			}
			break;
		case 'n':
			void_star = va_arg(args, void *);
			switch (xp_length) {
			case ARG_CHAR:
				*((char *) void_star) = xp_char_count;
				break;
			case ARG_SHORT_INT:
				*((short *) void_star) = xp_char_count;
				break;
			case ARG_INT:
				*((int *) void_star) = xp_char_count;
				break;
			default:
				*((XPL_LONG *) void_star) = xp_char_count;
				break;
			}
			break;
		default:
			/* Unknown terminating character */
			print('%', ref);
			i = recover;
			break;
		}
	}
	if (func == 3) {	/* Fixed length character strings */
		/* Set the terminating null */
		s = (__xpl_string *) ref;
		__xpl_put_byte(s, xp_char_count, 0);
		__xpl_put_byte(s, s->_Length - 1, 0);
	}
	va_end(args);
	return xp_char_count;
}
