/*
**	XPL to C source language translator runtime
**
**	Support for dynamic character strings, date, time, abort and exit.
**
**	Author: Daniel Weaver
*/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <sys/time.h>
#include "xpl.h"

static int dx_size;		/* Number of entries in descriptor index array */
static short *dx;		/* Descriptor Index array.  Used for sorting. */

static int max_nd;
int __xpl_colections[3];	/* Count the number of collections */
XPL_ADDRESS freebase, lower_bound, freepoint, freelimit;
XPL_ADDRESS space_needed;

void __xpl_io_init(void);

/*
**	The first 16 entries in the descriptor table are used by the runtime.
*/
static int descriptor_pool;
#define STRING_POOL_MASK 15

/* String variables */
static __xpl_string null_terminator = {1, "\000"};

/*
**	__xpl_runtime_init(int size)
**
**	Initialize the XPL runtime.
**	size is the number of bytes to allocate for the free string area.
**	Returns a non-zero number for errors.
*/
int
__xpl_runtime_init(int size)
{
	__xpl_io_init();	/* Initialize the I/O routines */
	if (size <= 0) {
		size = 32768;
	}
	freebase = (XPL_ADDRESS) malloc((size_t) size);
	if (freebase == 0) {
		fprintf(stderr, "Free string area malloc(%d) failed\n", size);
		return 1;
	}
	freepoint = freebase;
	freelimit = freebase + size;
	return 0;
}

/*
**	compactify()
**
**	The garbage collection routine for XPL dynamic strings.
*/
void
compactify(void)
{
	int i, j, k, l, nd, tried;
	XPL_ADDRESS addr, tc, bc, delta;

	/* Set the lower bound if this is the first call */
	if (lower_bound < freebase || space_needed > 256) {
		lower_bound = freebase;
	}
	if (!dx) {
		/* Allocate the descriptor index array */
		dx_size = ndescript + 1;
		dx = (short *) malloc(dx_size * sizeof(short));
		if (!dx) {
			fprintf(stderr, "*** Notice from compactify(): DX array malloc() failed. Job abandoned. ***\n");
			abort();
		}
	}
	for (tried = 0; tried < 2; tried++) {
		try_again:
		nd = -1;
		/* Find the collectable descriptors */
		for (i = 0; i <= ndescript; i++) {
			addr = (XPL_ADDRESS) (descriptor[i]._Address + descriptor[i]._Length);
			if (addr > lower_bound && addr < freelimit && descriptor[i]._Length > 0) {
				if (++nd >= dx_size) {
					/* Count the number of recoverable errors */
					__xpl_colections[2]++;
					max_nd = nd;
					bc = freelimit - lower_bound;
					if (bc < 256) {
						fprintf(stderr, "*** Notice from compactify(): Disastrous string overflow. Job abandoned. ***\n");
						abort();
					}
					lower_bound = freelimit - (bc >> 1);
					goto try_again;
				}
				dx[nd] = i;
			}
		}
		if (nd == -1) {
			/* Tell R2D2 to shut down ALL the trash compactors. */
			if (lower_bound == freebase) {
				/* This is not a real world error case */
				freepoint = freebase;
				break;
			}
			lower_bound = freebase;
			continue;
		}
		if (nd > max_nd) {
			max_nd = nd;
		}
		/* Sort the descriptors in assending order. */
		k = l = nd;
		while (k <= l) {
			int limit;

			l = -2;
			for (i = 1, limit = k; i <= limit; i++) {
				l = i - 1;
				if ((XPL_ADDRESS) descriptor[dx[l]]._Address > (XPL_ADDRESS) descriptor[dx[i]]._Address) {
					j = dx[l];
					dx[l] = dx[i];
					dx[i] = j;
					k = l;
				}
			}
		}
		/* Setup initial condition */
		if ((XPL_ADDRESS) descriptor[dx[0]]._Address < lower_bound) {
			lower_bound = (XPL_ADDRESS) descriptor[dx[0]]._Address;
		}
		freepoint = lower_bound;
		tc = bc = delta = 0;
		/* Move the active strings down. */
		for (i = 0; i <= nd; i++) {
			addr = (XPL_ADDRESS) descriptor[dx[i]]._Address;
			if (addr > tc) {
				if (delta) {
					memmove((void *) (bc - delta), (void *) bc, tc - bc);
				}
				freepoint = freepoint + tc - bc;
				delta = (bc = addr) - freepoint;
			}
			descriptor[dx[i]]._Address -= delta;
			addr += descriptor[dx[i]]._Length;
			if (tc < addr) {
				tc = addr;
			}
		}
		if (delta) {
			memmove((void *) (bc - delta), (void *) bc, tc - bc);
		}
		freepoint = freepoint + tc - bc;
		if (lower_bound == freebase) {
			__xpl_colections[0]++;	/* Count the number of major collections */
			lower_bound = freepoint;
			break;
		}
		__xpl_colections[1]++;	/* Count the number of minor collections */
		if ((freelimit - freepoint) > space_needed) {
			/* Got enough free string space */
			lower_bound = freepoint;
			return;
		}
		/* Try again with a smaller lower_bound */
		lower_bound = freebase;
	}
	if (freelimit - freepoint < space_needed) {
		fprintf(stderr, "*** Notice from compactify(): Insufficient string space. Job abandoned. ***\n");
		__xpl_info();
		abort();
	}
	space_needed = 0;
	return;
}

/*
**	__xpl_pool()
**
**	Return the next descriptor in the descriptor pool.
**	The pool should minimize the number of times when the runtime clobbers
**	it's own descriptors.  There is no way of knowing if the descriptor is still
**	in use.  Good Luck.
*/
__xpl_string *
__xpl_pool(void)
{
	descriptor_pool = (descriptor_pool + 1) & STRING_POOL_MASK;
	return &descriptor[descriptor_pool];
}

/*
**	__xpl_get_byte(__xpl_string *str, int position)
**
**	Return the character at position.
**	Returns zero if out of range.
*/
int
__xpl_get_byte(__xpl_string *str, int position)
{
	if (position >= str->_Length || position < 0) {
		return 0;
	}
	return str->_Address[position] & 255;
}

/*
**	__xpl_put_byte(__xpl_string *str, int position, int value)
**
**	Modify the character at position.
**	Does nothing if out of range.
**
**	Returns value
*/
int
__xpl_put_byte(__xpl_string *str, int position, int value)
{
	if (position >= str->_Length || position < 0) {
		return value;
	}
	str->_Address[position] = (char) value;
	return value;
}

/*
**	__xpl_wrapper(__xpl_string *outstr, __xpl_string *str)
**
**	This is an outer layer wrapper for user defined functions that return
**	a character string.  The descriptor returned by the user function is placed
**	into a vacant descriptor controlled by the calling procedure.  This allows
**	more than one usage of the same function within a single statement.
**
**	Return a pointer to the descriptor of the new string.
*/
__xpl_string *
__xpl_wrapper(__xpl_string *outstr, __xpl_string *str)
{
	outstr->_Length = str->_Length;
	outstr->_Address = str->_Address;
	return outstr;
}

/*
**	__xpl_move_to_top(__xpl_string *outstr, __xpl_string *str)
**
**	Move a string to the top of the string space.
**	Return a descriptor pointing to the new string.
*/
__xpl_string *
__xpl_move_to_top(__xpl_string *outstr, __xpl_string *str)
{
	if (str->_Length == 0) {
		outstr->_Length = 0;
		return outstr;
	}
	if ((XPL_ADDRESS) str->_Address + str->_Length == freepoint) {
		outstr->_Length = str->_Length;
		outstr->_Address = str->_Address;
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
**	__xpl_decimal(__xpl_string *outstr, XPL_LONG number)
**
**	Convert the number to a character string.
**	Return the new descriptor.
*/
__xpl_string *
__xpl_decimal(__xpl_string *outstr, XPL_LONG number)
{
	__xpl_string xpl_ret_decimal;
	char numbuf[64];
	XPL_UNSIGNED_LONG n, value;
	int i, sign = 0;

	if (number < 0) {
		sign = 1;
		number = - number;
	}
	value = number;
	for (i = (int) sizeof(numbuf) - 1; i > 1; i--) {
		n = value % 10;
		numbuf[i] = (char) (n + '0');
		value /= 10;
		if (value == 0) {
			break;
		}
	}
	if (sign) {
		numbuf[--i] = '-';
	}
	xpl_ret_decimal._Length = (int) sizeof(numbuf) - i;
	xpl_ret_decimal._Address = &numbuf[i];
	return __xpl_move_to_top(outstr, &xpl_ret_decimal);
}

/*
**	__xpl_cat(__xpl_string *outstr, __xpl_string *left, __xpl_string *right)
**
**	Concatenate two strings
**	Return a descriptor pointing to the new string.
*/
__xpl_string *
__xpl_cat(__xpl_string *outstr, __xpl_string *left, __xpl_string *right)
{
	__xpl_string *s, *t;

	if (left->_Length == 0) {
		if (right->_Length == 0) {
			/* both strings are null */
			outstr->_Length = 0;
			return outstr;
		}
		outstr->_Length = right->_Length;
		outstr->_Address = right->_Address;
		return outstr;
	}
	if (right->_Length == 0) {
		outstr->_Length = left->_Length;
		outstr->_Address = left->_Address;
		return outstr;
	}
	if (left->_Length + right->_Length >= freelimit - freepoint) {
		space_needed = left->_Length + right->_Length;
		compactify();
	}
	s = __xpl_move_to_top(__xpl_pool(), left);
	t = __xpl_move_to_top(__xpl_pool(), right);
	outstr->_Length = s->_Length + t->_Length;
	outstr->_Address = s->_Address;

	s->_Length = 0;	/* Housekeeping */
	t->_Length = 0;	/* Housekeeping */
	return outstr;
}

/*
**	__xpl_cat_int(__xpl_string *outstr, __xpl_string *left, XPL_LONG number)
**
**	Concatenate a string and a number
**	Return a descriptor pointing to the new string.
*/
__xpl_string *
__xpl_cat_int(__xpl_string *outstr, __xpl_string *left, XPL_LONG number)
{
	__xpl_string *s;

	if (left->_Length == 0) {
		return __xpl_decimal(outstr, number);
	}
	__xpl_move_to_top(outstr, left);
	s = __xpl_decimal(__xpl_pool(), number);
	outstr->_Length += s->_Length;

	s->_Length = 0;	/* Housekeeping */
	return outstr;
}

/*
**	__xpl_substr2(__xpl_string *outstr, __xpl_string *str, int pos)
**
**	The builtin function substr() with two arguments.
**	Return a descriptor pointing to the new string.
*/
__xpl_string *
__xpl_substr2(__xpl_string *outstr, __xpl_string *str, int pos)
{
	if (pos >= str->_Length || pos < 0) {
		outstr->_Length = 0;
		return outstr;
	}
	outstr->_Length = str->_Length - pos;
	outstr->_Address = str->_Address + pos;
	return outstr;
}

/*
**	__xpl_substr3(__xpl_string *outstr, __xpl_string *str, int pos, int len)
**
**	The builtin function substr() with three arguments.
**	Return a descriptor pointing to the new string.
*/
__xpl_string *
__xpl_substr3(__xpl_string *outstr, __xpl_string *str, int pos, int len)
{
	if (pos >= str->_Length || pos < 0 || len <= 0) {
		outstr->_Length = 0;
		return outstr;
	}
	outstr->_Length = str->_Length - pos;
	outstr->_Address = str->_Address + pos;
	if (outstr->_Length > len) {
		outstr->_Length = len;
	}
	return outstr;
}

/*
**	__xpl_compare(__xpl_string *left, __xpl_string *right)
**
**	String compare.  Compares first on length.
**	Returns a negative number for less than, zero for equal and a positive
**	number for greater than.
*/
int
__xpl_compare(__xpl_string *left, __xpl_string *right)
{
	unsigned char *a, *b;
	int i, l;

	if (left->_Length != right->_Length) {
		return left->_Length - right->_Length;
	}
	if (left->_Address == right->_Address) {
		return 0;
	}
	a = (unsigned char *) left->_Address;
	b = (unsigned char *) right->_Address;
	l = left->_Length;
	for (i = 0; i < l; i++) {
		if (*a != *b) {
			return *a - *b;
		}
		a++;
		b++;
	}
	return 0;
}

/*
**	__c_decimal(XPL_LONG number)
**
**	Convert the number to a character string.
**	Return a pointer to the string.
*/
char *
__c_decimal(XPL_LONG number)
{
	static char numbuf[64];
	XPL_UNSIGNED_LONG n, value;
	int i, sign = 0;

	if (number < 0) {
		sign = 1;
		number = - number;
	}
	value = number;
	numbuf[sizeof(numbuf) - 1] = '\0';
	for (i = (int) sizeof(numbuf) - 2; i > 1; i--) {
		n = value % 10;
		numbuf[i] = (char) (n + '0');
		value /= 10;
		if (value == 0) {
			break;
		}
	}
	if (sign) {
		numbuf[--i] = '-';
	}
	return &numbuf[i];
}

/*
**	__xpl_info()
**
**	Dump out some interesting stuff about the runtime.
*/
void
__xpl_info(void)
{
	int i, j, desc = 0, len = 0;
	long bc = 0;
	XPL_ADDRESS addr;

#if defined(__STDC_VERSION__)
        i = (int) __STDC_VERSION__;
        printf("C Language Standard: C%d __STDC_VERSION__ = %d\n", (i / 100) % 100, i);
#endif /* defined(__STDC_VERSION__) */
	for (i = j = 0; i <= ndescript; i++) {
		bc += descriptor[i]._Length;
		addr = (XPL_ADDRESS) (descriptor[i]._Address + descriptor[i]._Length);
		if (addr > freebase && addr < freelimit && descriptor[i]._Length > 0) {
			if (descriptor[i]._Length > len) {
				/* Save the info on the largest string */
				len = descriptor[i]._Length;
				desc = i;
			}
			j++;
		}
	}
	printf("Number of active descriptors = %d\n", j);
	printf("String space used = %s, bc = %ld\n", __c_decimal(freepoint - freebase), bc);
	printf("Largest string descriptor[%d]._Length = %d\n", desc, len);
	printf("Available string space = %s\n", __c_decimal(freelimit - freepoint));
	if (lower_bound) {
		printf("Lower limit of search = %s\n", __c_decimal(lower_bound - freebase));
	} else {
		printf("Lower limit not set\n");
	}
	printf("Max ND = %d\n", max_nd + 1);
	printf("ND overflow = %d\n", __xpl_colections[2]);
	printf("ndescript = %d\n", ndescript);
	printf("Major collections = %d\n", __xpl_colections[0]);
	printf("Minor collections = %d\n", __xpl_colections[1]);
}

/*
**	x2c_string(xpl_string)
**
**	Convert an XPL string to a C string.
**	The string is moved to the top of the free string area and concatenated
**	with a string containing a null.
**	Return the address of the new string.
*/
char *
x2c_string(__xpl_string *str)
{
	__xpl_string *s;

	if (freelimit - freepoint < str->_Length + 1) {
		/* Compress if it won't fit */
		space_needed = str->_Length + 1;
		compactify();
	}
	s = __xpl_cat(__xpl_pool(), str, &null_terminator);
	s->_Length = 0;
	return s->_Address;
}

/*
**	c2x_string(__xpl_string *outstr, c_string)
**
**	Convert a null terminated string to an XPL string.
**	The string is not moved.
**	Return the new string.
*/
__xpl_string *
c2x_string(__xpl_string *outstr, char *str)
{
	int len;

	len = (int) strlen(str);
	outstr->_Length = len;
	outstr->_Address = str;
	return outstr;
}

/*
**	__xpl__exit()
**
**	Gracefully exit the program.  This is the Unix function call exit()
**	The original XPL compiler implemented this as an abnormal exit.
*/
void
__xpl__exit(int status)
{
	exit(status);
}

/*
**	__xpl__abort()
**
**	Abort the program.  This is the Unix function call abort()
*/
void
__xpl__abort(void)
{
	abort();
}

/*
**	__xpl__date()
**
**	Return the date as (day_of_year + 1000 * (year - 1900))
*/
int
__xpl__date(void)
{
	time_t unixtime;
	struct tm *now;

	time(&unixtime);
	now = localtime(&unixtime);

	return ((now->tm_yday + 1) + (1000 * now->tm_year));
}

/*
**	__xpl__time()
**
**	Return the time in centiseconds since midnight
*/
int
__xpl__time(void)
{
	struct tm *now;
	struct timeval tv;
	time_t sec;

	gettimeofday(&tv, (void *) 0);
	sec = (time_t) tv.tv_sec;
	now = localtime(&sec);
	sec = ((now->tm_hour * 60) + now->tm_min) * 60 + now->tm_sec;

	return (tv.tv_usec / 10000) + sec * 100;
}

/*
**	trace()
**
**	Not implemented
*/
void
trace(void)
{
	;
}

/*
**	untrace()
**
**	Not implemented
*/
void
untrace(void)
{
	;
}
