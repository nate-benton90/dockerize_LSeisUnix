/* Copyright (c) Colorado School of Mines, 1990.*/
/* All rights reserved.                       */

/*****************************************************************************
Functions to compute valid n for prime factor (PFA) fft.

npfa		valid n for complex-to-complex PFA
npfar		valid n for real-to-complex/complex-to-real PFA
npfao		optimal n for complex-to-complex PFA
npfaro		optimal n for real-to-complex/complex-to-real PFA
******************************************************************************
Author:  Dave Hale, Colorado School of Mines, 08/04/91
*****************************************************************************/

/*****************************************************************************
Table of valid n and cost for prime factor fft.  For each n, cost
was estimated to be the inverse of the number of ffts done in 1 sec
on an IBM RISC System/6000 Model 320H, by Dave Hale, 08/04/91.  Cost
estimates are least accurate for very small n.  An alternative method
for estimating cost would be to count multiplies and adds, but this
method fails to account for the overlapping of multiplies and adds
that is possible on some computers, such as the IBM RS/6000 family.
****************************************************************************/
#define NTAB 240
static struct {
int n;  float c;
} nctab[NTAB] = {
{      1, 0.000031 },
{      2, 0.000030 },
{      3, 0.000075 },
{      4, 0.000041 },
{      5, 0.000039 },
{      6, 0.000053 },
{      7, 0.000065 },
{      8, 0.000067 },
{      9, 0.000051 },
{     10, 0.000067 },
{     11, 0.000058 },
{     12, 0.000030 },
{     13, 0.000091 },
{     14, 0.000084 },
{     15, 0.000087 },
{     16, 0.000063 },
{     18, 0.000073 },
{     20, 0.000061 },
{     21, 0.000068 },
{     22, 0.000086 },
{     24, 0.000071 },
{     26, 0.000112 },
{     28, 0.000082 },
{     30, 0.000090 },
{     33, 0.000115 },
{     35, 0.000093 },
{     36, 0.000118 },
{     39, 0.000157 },
{     40, 0.000126 },
{     42, 0.000128 },
{     44, 0.000133 },
{     45, 0.000141 },
{     48, 0.000156 },
{     52, 0.000180 },
{     55, 0.000166 },
{     56, 0.000153 },
{     60, 0.000126 },
{     63, 0.000181 },
{     65, 0.000209 },
{     66, 0.000187 },
{     70, 0.000152 },
{     72, 0.000187 },
{     77, 0.000210 },
{     78, 0.000274 },
{     80, 0.000227 },
{     84, 0.000212 },
{     88, 0.000234 },
{     90, 0.000274 },
{     91, 0.000281 },
{     99, 0.000267 },
{    104, 0.000305 },
{    105, 0.000297 },
{    110, 0.000331 },
{    112, 0.000287 },
{    117, 0.000359 },
{    120, 0.000318 },
{    126, 0.000309 },
{    130, 0.000416 },
{    132, 0.000373 },
{    140, 0.000403 },
{    143, 0.000455 },
{    144, 0.000378 },
{    154, 0.000437 },
{    156, 0.000485 },
{    165, 0.000517 },
{    168, 0.000431 },
{    176, 0.000501 },
{    180, 0.000525 },
{    182, 0.000554 },
{    195, 0.000644 },
{    198, 0.000592 },
{    208, 0.000658 },
{    210, 0.000645 },
{    220, 0.000632 },
{    231, 0.000689 },
{    234, 0.000760 },
{    240, 0.000737 },
{    252, 0.000728 },
{    260, 0.000823 },
{    264, 0.000756 },
{    273, 0.000866 },
{    280, 0.000796 },
{    286, 0.000939 },
{    308, 0.000872 },
{    312, 0.001010 },
{    315, 0.000956 },
{    330, 0.001059 },
{    336, 0.000985 },
{    360, 0.001063 },
{    364, 0.001119 },
{    385, 0.001242 },
{    390, 0.001404 },
{    396, 0.001211 },
{    420, 0.001346 },
{    429, 0.001531 },
{    440, 0.001374 },
{    455, 0.001524 },
{    462, 0.001481 },
{    468, 0.001553 },
{    495, 0.001631 },
{    504, 0.001431 },
{    520, 0.001721 },
{    528, 0.001730 },
{    546, 0.001916 },
{    560, 0.001718 },
{    572, 0.001934 },
{    585, 0.001976 },
{    616, 0.001779 },
{    624, 0.002146 },
{    630, 0.002012 },
{    660, 0.002278 },
{    693, 0.002283 },
{    715, 0.002660 },
{    720, 0.002309 },
{    728, 0.002342 },
{    770, 0.002639 },
{    780, 0.002890 },
{    792, 0.002488 },
{    819, 0.002660 },
{    840, 0.002681 },
{    858, 0.003311 },
{    880, 0.002941 },
{    910, 0.003268 },
{    924, 0.003106 },
{    936, 0.003106 },
{    990, 0.003534 },
{   1001, 0.003690 },
{   1008, 0.003165 },
{   1040, 0.003690 },
{   1092, 0.003937 },
{   1144, 0.003891 },
{   1155, 0.004115 },
{   1170, 0.004386 },
{   1232, 0.004167 },
{   1260, 0.004255 },
{   1287, 0.004878 },
{   1320, 0.004673 },
{   1365, 0.005263 },
{   1386, 0.004831 },
{   1430, 0.005714 },
{   1456, 0.005236 },
{   1540, 0.005525 },
{   1560, 0.005780 },
{   1584, 0.005525 },
{   1638, 0.006173 },
{   1680, 0.006173 },
{   1716, 0.006757 },
{   1820, 0.006944 },
{   1848, 0.006410 },
{   1872, 0.007042 },
{   1980, 0.007463 },
{   2002, 0.008000 },
{   2145, 0.009009 },
{   2184, 0.008130 },
{   2288, 0.008929 },
{   2310, 0.009091 },
{   2340, 0.009434 },
{   2520, 0.009009 },
{   2574, 0.010526 },
{   2640, 0.010417 },
{   2730, 0.011494 },
{   2772, 0.010204 },
{   2860, 0.012048 },
{   3003, 0.012658 },
{   3080, 0.011494 },
{   3120, 0.012821 },
{   3276, 0.012987 },
{   3432, 0.014493 },
{   3465, 0.013889 },
{   3640, 0.014493 },
{   3696, 0.014706 },
{   3960, 0.015625 },
{   4004, 0.016949 },
{   4095, 0.016949 },
{   4290, 0.018519 },
{   4368, 0.018519 },
{   4620, 0.019608 },
{   4680, 0.019231 },
{   5005, 0.022222 },
{   5040, 0.020408 },
{   5148, 0.023256 },
{   5460, 0.025000 },
{   5544, 0.022222 },
{   5720, 0.025641 },
{   6006, 0.027778 },
{   6160, 0.027027 },
{   6435, 0.029412 },
{   6552, 0.027778 },
{   6864, 0.032258 },
{   6930, 0.031250 },
{   7280, 0.033333 },
{   7920, 0.035714 },
{   8008, 0.035714 },
{   8190, 0.038462 },
{   8580, 0.043478 },
{   9009, 0.041667 },
{   9240, 0.041667 },
{   9360, 0.043478 },
{  10010, 0.047619 },
{  10296, 0.047619 },
{  10920, 0.052632 },
{  11088, 0.050000 },
{  11440, 0.055556 },
{  12012, 0.058824 },
{  12870, 0.062500 },
{  13104, 0.062500 },
{  13860, 0.066667 },
{  15015, 0.076923 },
{  16016, 0.076923 },
{  16380, 0.083333 },
{  17160, 0.090909 },
{  18018, 0.090909 },
{  18480, 0.090909 },
{  20020, 0.100000 },
{  20592, 0.100000 },
{  21840, 0.111111 },
{  24024, 0.125000 },
{  25740, 0.125000 },
{  27720, 0.125000 },
{  30030, 0.166667 },
{  32760, 0.166667 },
{  34320, 0.200000 },
{  36036, 0.166667 },
{  40040, 0.200000 },
{  45045, 0.250000 },
{  48048, 0.250000 },
{  51480, 0.250000 },
{  55440, 0.250000 },
{  60060, 0.333333 },
{  65520, 0.333333 },
{  72072, 0.333333 },
{  80080, 0.333333 },
{  90090, 0.500000 },
{ 102960, 0.500000 },
{ 120120, 0.500000 },
{ 144144, 0.500000 },
{ 180180, 1.000000 },
{ 240240, 1.000000 },
{ 360360, 1.000000 },
{ 720720, 1.000000 },
};

int npfa (int nmin)
/*****************************************************************************
Return smallest valid n not less than nmin for prime factor fft.
******************************************************************************
Input:
nmin		lower bound on returned value (see notes below)

Returned:	valid n for prime factor fft
******************************************************************************
Notes:
The returned n will be composed of mutually prime factors from
the set {2,3,4,5,7,8,9,11,13,16}.  Because n cannot exceed
720720 = 5*7*9*11*13*16, 720720 is returned if nmin exceeds 720720.
******************************************************************************
Author:  Dave Hale, Colorado School of Mines, 04/28/89
Modified:  Dave Hale, Colorado School of Mines, 08/05/91
	For efficiency, use pre-computed table of valid n and costs.
*****************************************************************************/
{
	int i;
	for (i=0; i<NTAB-1 && nctab[i].n<nmin; ++i);
	return nctab[i].n;
}

int npfao (int nmin, int nmax)
/*****************************************************************************
Return optimal n between nmin and nmax for prime factor fft.
******************************************************************************
Input:
nmin		lower bound on returned value (see notes below)
nmax		desired (but not guaranteed) upper bound on returned value

Returned:	valid n for prime factor fft
******************************************************************************
Notes:
The returned n will be composed of mutually prime factors from
the set {2,3,4,5,7,8,9,11,13,16}.  Because n cannot exceed
720720 = 5*7*9*11*13*16, 720720 is returned if nmin exceeds 720720.
If nmin does not exceed 720720, then the returned n will not be 
less than nmin.  The optimal n is chosen to minimize the estimated
cost of performing the fft, while satisfying the constraint, if
possible, that n not exceed nmax.
******************************************************************************
Author:  Dave Hale, Colorado School of Mines, 06/13/89
Modified:  Dave Hale, Colorado School of Mines, 08/05/91
	For efficiency, use pre-computed table of valid n and costs.
*****************************************************************************/
{
	int i,j;
	for (i=0; i<NTAB-1 && nctab[i].n<nmin; ++i);
	for (j=i+1; j<NTAB-1 && nctab[j].n<=nmax; ++j)
		if (nctab[j].c<nctab[i].c) i = j;
	return nctab[i].n;
}

int npfar (int nmin)
/*****************************************************************************
Return smallest valid n not less than nmin for real-to-complex or 
complex-to-real prime factor ffts.
******************************************************************************
Input:
nmin		lower bound on returned value

Returned:	valid n for real-to-complex/complex-to-real prime factor fft
******************************************************************************
Notes:
Current implemenations of real-to-complex and complex-to-real prime 
factor ffts require that the transform length n be even and that n/2 
be a valid length for a complex-to-complex prime factor fft.  The 
value returned by npfar satisfies these conditions.  Also, see notes 
for npfa.
******************************************************************************
Author:  Dave Hale, Colorado School of Mines, 06/16/89
*****************************************************************************/
{
    return 2*npfa((nmin+1)/2);
}

int npfaro (int nmin, int nmax)
/*****************************************************************************
Return optimal n between nmin and nmax for real-to-complex or 
complex-to-real prime factor ffts
******************************************************************************
Input:
nmin		lower bound on returned value
nmax		desired (but not guaranteed) upper bound on returned value

Returned:	valid n for real-to-complex/complex-to-real prime factor fft
******************************************************************************
Notes:
Current implemenations of real-to-complex and complex-to-real prime 
factor ffts require that the transform length n be even and that n/2 
be a valid length for a complex-to-complex prime factor fft.  The 
value returned by npfaro satisfies these conditions.  Also, see notes 
for npfao.
******************************************************************************
Author:  Dave Hale, Colorado School of Mines, 06/16/89
*****************************************************************************/
{
    return 2*npfao((nmin+1)/2,(nmax+1)/2);
}

#ifdef TEST
#include "cwp.h"
main()
{
	int nmin,n,no,nfft,nffto;
	float cpu,total;
	complex *c=alloc1complex(720720);
	int npfao2 (int nmin, int nmax);
		
	for (n=0; n<720720; ++n)
		c[n].r = c[n].i = 0.0;

	for (nmin=npfa(100); nmin<=10000; nmin=npfa(nmin+1)) {
		n = npfa(nmin);
		for (nfft=0,total=0.0; total<1.0; ++nfft) {
			cpu = cpusec();
			pfacc(1,n,c);
			total += cpusec()-cpu+FLT_EPSILON;
		}
		no = npfao(nmin,2*nmin);
		for (nffto=0,total=0.0; total<1.0; ++nffto) {
			cpu = cpusec();
			pfacc(1,no,c);
			total += cpusec()-cpu+FLT_EPSILON;
		}
		printf("valid n=%d cost=%g optimal n=%d cost=%g\n",
			n,1.0/nfft,no,1.0/nffto);
	}
}
#endif /* TEST */
