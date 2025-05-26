#include "suhdr.h"
#include <sys/time.h>

struct timeval gettimeofday_time_diff(struct timeval t1, struct timeval t2)
{
     struct timeval diff;

     diff.tv_sec = t1.tv_sec - t2.tv_sec;
     diff.tv_usec = t1.tv_usec - t2.tv_usec;
     /* normalize */
     while (diff.tv_usec < 0) {
	  diff.tv_usec += 1000000L;
	  diff.tv_sec -= 1;
     }

     return diff;
}
