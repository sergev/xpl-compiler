/*
**	XPL to C source language translator runtime
**
**	Implement date and time
**
**	Author: Daniel Weaver
*/

#include <stdio.h>
#include <time.h>
#include <sys/time.h>

#include "xpl.h"

/*
**	__xpl_date()
**
**	Return the date as (day_of_year + 1000 * (year - 1900))
*/
int
__xpl_date(void)
{
	time_t unixtime;
	struct tm *now;

	time(&unixtime);
	now = localtime(&unixtime);

	return ((now->tm_yday + 1) + (1000 * now->tm_year));
}

/*
**	__xpl_time()
**
**	Return the time in centiseconds since midnight
*/
int
__xpl_time(void)
{
#if defined(MSDOS) && !defined(DJGPP)
	struct tm *now;
	clock_t ticks; 
	time_t sec, old;
	int i;

	time(&old);
	for (i = 0; i < 3; i++) {
		ticks = clock();
		time(&sec);
		if (old == sec) break;
		old = sec;
	}
	now = localtime(&sec);
	sec = ((now->tm_hour * 60) + now->tm_min) * 60 + now->tm_sec;

	return (((unsigned long)ticks / 10) % 100) + sec * 100;
#else
	struct tm *now;
	struct timeval tv;
	time_t sec;

	gettimeofday(&tv, (void *) 0);
	sec = (time_t) tv.tv_sec;
	now = localtime(&sec);
	sec = ((now->tm_hour * 60) + now->tm_min) * 60 + now->tm_sec;

	return (tv.tv_usec / 10000) + sec * 100;
#endif
}
