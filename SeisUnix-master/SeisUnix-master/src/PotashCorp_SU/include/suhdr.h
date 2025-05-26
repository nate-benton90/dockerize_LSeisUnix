#include<stdio.h>
#include<sys/time.h>
#include"su.h"
#include "segy.h"
#include "header.h"
#include<math.h>
/*#define cwp_true TRUE
#define cwp_false false 
*/
/*#define CLOSETO(x, y) ((ABS((x) - (y)) <= FLT_EPSILON*ABS(y))?TRUE:FALSE)
*/	  
#define SQR(a) ((a)*(a))

#define ISGN(x) ( (x) < 0 ? -1 : 1)

float distance(float x0, float y0, float x1, float y1);

float slope(float x0, float y0, float x1, float y1);

void p2line_pr(float x,float y,float m,float b,float *xp,float *yp);

void bin(double x0, double y0, double xp, double yp, float degr, float dbx, 
	float dby, int dirx, int diry, 
	int *binx, int *biny, double *binxc, double *binyc,
	float *fbinx, float *fbiny);

void do_facor(float *data1,float *data2, float *acor,int n,int f,int nc);

void do_bccorr(float *data2,float *data1, float *acor,int n,int fi,int nc,
	float f1f,float f2f,float dt);
	
int bp_filter(float f1,float f2,float f3, float f4, 
	      float *tr, float dt, int nt);
int bp_filter_padd(float f1,float f2,float f3, float f4, 
	      float *tr, float dt, int nt,float fftpad);
	      
void polygonalFilter(float *f, float *amps, int npoly,
				int nfft, float dt, float *filter);
				
void cxcor (int lx, int ifx, complex *x,
            int ly, int ify, complex *y, 
            int lz, int ifz, complex *z);
	    
void cconv (int lx, int ifx, complex *x,
       	    int ly, int ify, complex *y,
	    int lz, int ifz, complex *z);
	    
void fcconv(int lx, int ifx, complex *x,
       	    int ly, int ify, complex *y,
	    int lz, int ifz,complex *z);

void fcconv2d (int lx2,int lx1,int ifx2,int ifx1, complex **x,
	     int ly2,int ly1,int ify2,int ify1, complex **y,
	     int lz2,int lz1,int ifz2,int ifz1, complex **z);
	    
int fputtra(FILE *fp,segy *tp,int itr);

int fgettra2(FILE *fp,segy *tp,int itr);

unsigned long fgettra_l(FILE *fp, segy *tp, unsigned long itr);

segy **get_gather(cwp_String *key,cwp_String *type,Value *n_val, int *nt,int *ntr,
		      float *dt,int *first);
		      
segy **put_gather(segy **rec,int *nt, int *ntr);

void shuffle(int *ind, int m);

void sm_st(float *indx,float *val,int n,int sm,int inc,int m);

void segm_bnd(float *val,int n,float inc,float gap,int *smst,int *ns);

void uqsort(int n, int *un, float *a);

void c_window(int ntr,int nw,int no,float w[],float ww[],int *rnp,int ws[],int we[]);
void hanning_w(int n,float *w);

float *vector(int n);

void free_vector(int v,int n);

float **matrix(int n,int m);

void free_matrix(float **mat,int n,int m);

void transpose_2d(void **mat,int n2,int n1,void **matt,size_t nbpe);

float pca(float **data, int n, int m, int eig);

void  tqli(float *d, float *e,int n,float ** z);

float pythag(float a, float b);

void  tred2(float **a,int n,float *d,float *e);

void  scpcol(float **data,int n,int m,float **symmat);

void  pca_alloc(int n,int m);

void  pca_free(int n, int m);

void fit( float *x, float *y, int ndata, float *sig, int mwt,
          float *a, float *b, float *siga, float *sigb,
	  float *chi2, float *q);

#include"pthread.h"

typedef struct barrier_struct {
        pthread_mutex_t b_lock;
        int     n_thr;
        int     waiting;
        int     phase;
        int     sum;
        int     result;
        pthread_cond_t  bwait_cv;
} *barrier_t;

/* function declarations */
barrier_t barrier_init( int n_thr);     /* barrier synchronization routines */ 
void barrier_destroy(barrier_t barrier);
int barrier_wait(barrier_t barrier);

void regrid3(float ***gridi,int ni1,int ni2,int ni3,
             float ***grido,int no1,int no2,int no3); 


float mean( float *x, int ndata);

float median(float *x,int ndata);
    

void do_agc(float *data, int iwagc, int nt);

void do_agc_scale(float *data, int iwagc,float *scale, int nt);

void qsort_lu(unsigned long *a, unsigned int n);
void qsort_u(unsigned int *a, unsigned int n);
void qsort_i(int *a, unsigned int n);
void qsort_f(float *a, unsigned int n);
void qsort_d(int *a, unsigned int n);
void unqsort(void *a, size_t n,size_t size,int(*compar)(const void *, const void *),size_t *un);
void uqsort_ul(unsigned long *a, unsigned int n,unsigned int *un);
void uqsort_u(unsigned int *a, unsigned int n,unsigned int *un);
void uqsort_i(int*a, unsigned int n,unsigned int *un);
void uqsort_f(float*a, unsigned int n,unsigned int *un);
void uqsort_d(double*a, unsigned int n,unsigned int *un);

int LU_cmp(const void *a, const void *b);
int U_cmp(const void *a, const void *b);
int I_cmp(const void *a, const void *b);
int F_cmp(const void *a, const void *b);
int D_cmp(const void *a, const void *b);

void set_intersect_lu(unsigned long *a, unsigned int na,
                      unsigned long *b, unsigned int nb,
               	      unsigned long *i, unsigned int *ni);


void xindexf (unsigned int nx, void * ax,void *x, size_t size,
       int(*compar)(const void *, const void *),
       unsigned int *index);


char *ealloc1bit(unsigned int n);
void free1bit(char *a);
unsigned int readbit(char *a,unsigned int n);
void zerobit(char *a,unsigned int n);
void setbit(char *a,unsigned int n);
void clearbit(char *a,unsigned int n);
void ORbit(char *a,char *b ,char *c,unsigned int n);
void ANDbit(char *a,char *b ,char *c,unsigned int n);

int isamins(int n, float *a,int inc);
int isamaxs(int n, float *a,int inc);
float fract_D( float *a, int n,int minl,int maxl,int dl);
	
void wrap_array(void *base,size_t nmemb,size_t size,int f);

complex goertzel_r(float *a, int n, int k);
complex goertzel_c(complex *a, int n, int k,int init);

struct timeval gettimeofday_time_diff(struct timeval t1, struct timeval t2);

float nnw2di (int n, float x[], float y[], float z[],
             int nx, int ny, float fx, float fy, float dx,float dy,float **data,float p,float r);

float safe_exp(float a);



