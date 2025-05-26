#ifndef FXYMIG_H
#define FXYMIG_H 

typedef struct {        /* fxymig header */

        long lhd;       
        long nx;         /* number of cdp per line */
        long ny;         /* number of lines */
        long ntau;       
        long nw;       
        long itau0;       
        long istep;       
        long icstep;       
        long nvs;       
        long nkx;       
        long nky;       
        long nq;       
        long nv;       
	long lvec;
	long lplane;
        long iorder;
	long naux1;
	long naux2;
	long naux;
	long ncpu;
	float dt;
	float dx;
	float dy;
	float dtau;
	float dfc;
	float vref;
	float dvs;
	float wmin;
	long iisave;
	float dw;	
	float wminl;
	long nwl;
	long indxw;
	long inode;
} fxyhd;

#endif
