/* Copyright (c) Colorado School of Mines, 1990.
/* All rights reserved.                       */

/*
 * t - time and date for non-military types
 *
 * Credit: Jack
 */

# include	<time.h>

char am_pm[] = " am";
static char *wday[] = {
			"  Sun,",
			"  Mon,",
			"  Tues,",
			"  Weds,",
			"  Thurs,",
			"  Fri,",
			"  Sat,"
	     };
static char *mon[] = {
			" Jan",
			" Feb",
			" Mar",
			" Apr",
			" May",
			" June",
			" July",
			" Aug",
			" Sept",
			" Oct",
			" Nov",
			" Dec"
		     };

main() {
	struct tm *timer;
	time_t seconds;

	seconds = time(0);		/* get the time */
	timer = localtime(&seconds);	/* extract date and time info */

	if (timer->tm_hour >= 12) am_pm[1] = 'p'; /* switch to pm */

	if (timer->tm_hour > 12) timer->tm_hour -= 12; /* 12 hour clock */

	printf("%d:%02d%s%s%s %d 19%d\n",
		timer->tm_hour, timer->tm_min, am_pm, wday[timer->tm_wday],
			mon[timer->tm_mon], timer->tm_mday, timer->tm_year);

}
