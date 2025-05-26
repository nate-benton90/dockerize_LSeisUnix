#include <stdio.h>
#include <sys/times.h>
#include "su.h"


void shuffle(int *ind, int m)
/* randomly shuffles n integers from 0 to m-1, the values are returned in ind */
{
        int i,j;
        int *seq;
        int pick;
        int left;
        struct tms tm;

        /* seed the random number generator with the system time */
        sranuni((int)times(&tm));
        
        seq = ealloc1int(m);
                
        for(i=0;i<m;i++) seq[i]=i;
        left = m;
        for(i=0;i<m;i++) {
                        pick = (int)left*franuni();
                        ind[i] = seq[pick];
                        for(j=pick;j<m-1;j++) seq[j] = seq[j+1];
                        left--;
        }
        free1int(seq);
        
}               
