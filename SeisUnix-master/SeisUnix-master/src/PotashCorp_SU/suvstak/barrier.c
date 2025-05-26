#include "pthread.h"
#include "barrier.h"
#include <sys/types.h>
#include <string.h>
#include <stdio.h>
#include <stdlib.h>


barrier_t barrier_init( int n_thr) 
{
	barrier_t barrier = (barrier_t) malloc(sizeof ( struct barrier_struct));
	if (barrier !=NULL) {
		barrier->n_thr = n_thr;
		barrier->waiting = 0;
		barrier->phase = 0;
		barrier->sum = 0;
		pthread_mutex_init(&barrier->b_lock,NULL);
		pthread_cond_init(&barrier->bwait_cv,NULL);
	}
	return(barrier);
}

void barrier_destroy(barrier_t barrier) 
{
	pthread_mutex_destroy(&barrier->b_lock);
	pthread_cond_destroy(&barrier->bwait_cv);
	free(barrier);
}

int barrier_wait(barrier_t barrier)
{
	int my_phase;
	pthread_mutex_lock(&barrier->b_lock);
	my_phase = barrier->phase;
	barrier->sum ++;
	barrier->waiting ++;
	if(barrier->waiting == barrier->n_thr) {
		barrier->result = barrier->sum;
		barrier->sum =0;
		barrier->waiting =0;
		barrier->phase = 1-my_phase;
		pthread_cond_broadcast(&barrier->bwait_cv);
	}
	while(barrier->phase == my_phase) {
		pthread_cond_wait(&barrier->bwait_cv,&barrier->b_lock);
	}
	pthread_mutex_unlock(&barrier->b_lock);
	return(barrier->result);
}
