/* getdate.c  - returns ASCII local time of day     */
/*  mod 6 Sep 19 - make it a void function (not return anything) and clean up
    mod 25 Mar20 - declare index i outside of for loop for older c compilers
*/

#include <time.h>

void getdate1_(char *date) {
    int i;
    long ltime;
    time(&ltime);
    struct tm *tm = localtime(&ltime);
    char *ascitime = asctime(tm);
    for ( i = 0; i <= 24; ++i)
       date[i] = ascitime[i];
}
