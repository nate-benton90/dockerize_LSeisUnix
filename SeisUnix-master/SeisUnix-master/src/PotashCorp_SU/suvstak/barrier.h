#include "pthread.h"
#include <sys/types.h>
#include <string.h>
#include <stdio.h>

pthread_cond_t tdcv;
pthread_mutex_t lock;

typedef struct barrier_struct {
	pthread_mutex_t	b_lock;
	int 	n_thr;
	int	waiting;
	int	phase;
	int	sum;
	int	result;
	pthread_cond_t	bwait_cv;
}	*barrier_t;

/* function declarations */
barrier_t barrier_init( int n_thr);	/* barrier synchronization routines */ 
void barrier_destroy(barrier_t barrier);
int barrier_wait(barrier_t barrier);	
