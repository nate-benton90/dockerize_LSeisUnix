#ifndef DFT_H
#define DFT_H

typedef struct{
    float r, i;
} complex;

int fwdsign = -1;
int invsign = +1;

int	npfa(int nmin);
int	npfao(int nmin, int nmax);
void	pfacc(int isign, int n, complex cz[]);

#endif

