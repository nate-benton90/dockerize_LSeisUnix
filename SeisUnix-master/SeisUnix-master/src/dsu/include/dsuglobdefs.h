/*
        DSU (AEM) additions to par.h
*/
#include "dsulib.h"

#ifdef EXIT_FAILURE
#undef EXIT_FAILURE
#endif

/* 99 is going to be the msgtag to be sent to the GUI */
#define EXIT_FAILURE (DsuExit(&ThisDsuTask, 99))


#ifdef EXIT_SUCCESS
#undef EXIT_SUCCESS
#endif

/* 100 is going to be the msgtag to be sent to the GUI */
#define EXIT_SUCCESS (DsuExit(&ThisDsuTask, 100))
