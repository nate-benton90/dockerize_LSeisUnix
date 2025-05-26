#include <stdio.h>
#include <string.h>
#include <math.h>
#include "su.h"
#include "cwp.h"
#define SIGN(a, b) ( (b) < 0 ? -fabs(a) : fabs(a) )
static float sqrarg;
#define SQR(a) ((sqrarg=(a)) == 0.0 ? 0.0 : sqrarg*sqrarg)

float **matrix(int n, int m);
void free_matrix();
float *vector(int n);
void free_vector();
void tqli(float *d, float *e,int n,float ** z);
float pythag(float a, float b);
void tred2(float **a,int n,float *d,float *e);
void scpcol(float **data,int n,int m,float **symmat);
void pca_alloc(int n,int m);
void pca_free(int n, int m);

static float **symmat, *evals, *interm;
static float *seigv, eigv;

/* THIS VERSION DOES NOT COMPUTE EIGEN VECTORS  THEREFORE IT IS FASTER*/

float pca(float **data, int n, int m, int eig)
{
int  i;

    scpcol(data, n, m, symmat);

    tred2(symmat, m, evals, interm);  /* Triangular decomposition */

    tqli(evals, interm, m, symmat);   /* Reduction of sym. trid. matrix */
    
    for(i=1;i<=m;i++) seigv[i-1] = evals[i];
    hpsort(m,seigv);
    if(seigv[m-1]>0) { 
    	eigv = seigv[m-eig]/seigv[m-1];
    } else {
    	eigv=0;
    }
    
    return(eigv);
}


void scpcol(float **data,int n,int m,float **symmat)
/* Create m * m sums-of-cross-products matrix from n * m data matrix. */
{
int i, j1, j2;

for (j1 = 1; j1 <= m; j1++)
    {
    for (j2 = j1; j2 <= m; j2++)
        {
        symmat[j1][j2] = 0.0;
        for (i = 1; i <= n; i++)
            {
/*            symmat[j1][j2] += data[i][j1] * data[i][j2]; */
            symmat[j1][j2] += data[j1][i] * data[j2][i];
            }
        symmat[j2][j1] = symmat[j1][j2];
        }
    }

return;

}

/*  Reduce a real, symmetric matrix to a symmetric, tridiag. matrix. */

void tred2(float **a, int n, float d[], float e[])
{
	int l,k,j,i;
	float scale,hh,h,g,f;

	for (i=n;i>=2;i--) {
		l=i-1;
		h=scale=0.0;
		if (l > 1) {
			for (k=1;k<=l;k++)
				scale += fabs(a[i][k]);
			if (scale == 0.0)
				e[i]=a[i][l];
			else {
				for (k=1;k<=l;k++) {
					a[i][k] /= scale;
					h += a[i][k]*a[i][k];
				}
				f=a[i][l];
				g=(f >= 0.0 ? -sqrt(h) : sqrt(h));
				e[i]=scale*g;
				h -= f*g;
				a[i][l]=f-g;
				f=0.0;
				for (j=1;j<=l;j++) {
					a[j][i]=a[i][j]/h;
					g=0.0;
					for (k=1;k<=j;k++)
						g += a[j][k]*a[i][k];
					for (k=j+1;k<=l;k++)
						g += a[k][j]*a[i][k];
					e[j]=g/h;
					f += e[j]*a[i][j];
				}
				hh=f/(h+h);
				for (j=1;j<=l;j++) {
					f=a[i][j];
					e[j]=g=e[j]-hh*f;
					for (k=1;k<=j;k++)
						a[j][k] -= (f*e[k]+g*a[i][k]);
				}
			}
		} else
			e[i]=a[i][l];
		d[i]=h;
	}
	d[1]=0.0;
	e[1]=0.0;
	/* Contents of this loop can be omitted if eigenvectors not
			wanted except for statement d[i]=a[i][i]; */
	for (i=1;i<=n;i++) {
/*		l=i-1;
		if (d[i]) {
			for (j=1;j<=l;j++) {
				g=0.0;
				for (k=1;k<=l;k++)
					g += a[i][k]*a[k][j];
				for (k=1;k<=l;k++)
					a[k][j] -= g*a[k][i];
			}
		} */
		d[i]=a[i][i];
/*		a[i][i]=1.0;
		for (j=1;j<=l;j++) a[j][i]=a[i][j]=0.0; */
	}
}


/*  Tridiagonal QL algorithm -- Implicit  **********************/

void tqli(float d[], float e[], int n, float **z)
{
	float pythag(float a, float b);
	int m,l,iter,i,k;
	float s,r,p,g,f,dd,c,b;

	for (i=2;i<=n;i++) e[i-1]=e[i];
	e[n]=0.0;
	for (l=1;l<=n;l++) {
		iter=0;
		do {
			for (m=l;m<=n-1;m++) {
				dd=fabs(d[m])+fabs(d[m+1]);
				if ((float)(fabs(e[m])+dd) == dd) break;
			}
			if (m != l) {
				if (iter++ == 50) { warn("Too many iterations in tqli");
						    break;
				}
				g=(d[l+1]-d[l])/(2.0*e[l]);
				r=pythag(g,1.0);
				g=d[m]-d[l]+e[l]/(g+SIGN(r,g));
				s=c=1.0;
				p=0.0;
				for (i=m-1;i>=l;i--) {
					f=s*e[i];
					b=c*e[i];
					e[i+1]=(r=pythag(f,g));
					if (r == 0.0) {
						d[i+1] -= p;
						e[m]=0.0;
						break;
					}
					s=f/r;
					c=g/r;
					g=d[i+1]-p;
					r=(d[i]-g)*s+2.0*c*b;
					d[i+1]=g+(p=s*r);
					g=c*r-b;
		/* No eigenvectors wanted */
/*					for (k=1;k<=n;k++) {
						f=z[k][i+1];
						z[k][i+1]=s*z[k][i]+c*f;
						z[k][i]=c*z[k][i]-s*f;
					}
*/				}
				if (r == 0.0 && i >= l) continue;
				d[l] -= p;
				e[l]=g;
				e[m]=0.0;
			}
		} while (m != l);
	}
}

float pythag(float a, float b)
{
	float absa,absb;
	absa=fabs(a);
	absb=fabs(b);
	if (absa > absb) return absa*sqrt(1.0+SQR(absb/absa));
	else return (absb == 0.0 ? 0.0 : absb*sqrt(1.0+SQR(absa/absb)));
}


void pca_alloc(int n, int m)
{
    symmat = matrix(m, m);  /* Allocation of correlation (etc.) matrix */
    evals = vector(m);     /* Storage alloc. for vector of eigenvalues */
    interm = vector(m);    /* Storage alloc. for 'intermediate' vector */
    seigv = ealloc1float(m);
}	

void pca_free(int n, int m)
{
    free1float(seigv);
    free_matrix(symmat, m, m);
    free_vector(evals, m);
    free_vector(interm, m);
}
