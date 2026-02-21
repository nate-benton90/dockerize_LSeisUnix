/* Copyright (c) Colorado School of Mines, 2011.*/
/* All rights reserved.		       */

/* SIRD_EPIDEMIC: $Revision: 1.4 $ ; $Date: 2020/06/12 23:37:18 $	*/

#include "par.h"
#include "rke.h"

/*********************** self documentation **********************/

char *sdoc[] = {
"								",
" SIRD_EPIDEMIC - the SIRD and SIRDS EPIDEMIC models 		",
"								",
"  sirdepidemic > [stdout]					",
"								",
" Required Parameters: none					",
" Optional Parameters:						",
" normalize=1		normalize S,I by N; =0 don't normalize	",
" scale=0		don't scale output			",
"			=1 scale output S,I,R by N		",
"			=2 scale output S,I,R by s0		",
" N=1000		total population			",
" s0=N			initial number of susceptibles		",
" i0=1			initial number of infectives		",
" r0=0			initial number of removed (usually 0)	",
"	 		(not the basic reproducion rate R0)	",
" d0=0			initial number of dead (usually 0)	",
" 								",
" k=.5			transmission rate			",
" b=.3333		removal rate = death + recovery rates	",
" nu=0.01		mortality rate 				",
" 			(0.01 = 1% mortality)			",
" gamma=0		no social distancing; gamma > 0 social	",
" 			distancing (Baker 2020 model)		",
" h=1			increment in time			",
" tol=1.e-08		error tolerance				",
" stepmax=40		maximum number of steps to compute	",
" mode=SIRD		S, I, R, D successively output		",
"			=S only, =I only, =R only, =D only	",
" Notes:							",
" This program is really just a demo showing how to use the 	",
" differential equation solver rke_solve written by Francois 	",
" Pinard, based on a modified form of the 4th order Runge-Kutta ",
" method, which employs the error checking method of R. England ",
" 1969.								",
"								",
" The output consists of unformated C-style binary floats, of	",
" either pairs or triplets as specified by the \"mode\" paramerter.",
"								",
" About the SIRD epidemic model:				",
" R0 = number of new infections per single infected host  	",
"  1 < R0 < 1.5 for influenza (2.2 to 2.7 for Covid-19)		",
"  b, k, s0, and R0 are related via				",
"  k = b*s0/R0. When s is normalized by N, k=b/R0		",
"  It is often easier to determine the recovery rate k (in units",
"  of h and to determine reasonable estimate of s0 and of R0 	",
"  and to calculate the infection rate b = k*R0/s0, which becomes",
"  b = k*R0 for s normalized by N.				",
"  nu= mortality rate						",
"								",
" S = total number susceptible to the infection			",
" I = total number of those capable of passing on the infection	",
" R = total number recovered					",
" D = total number dead						",
"								",
" Examples:							",
" Default:							",
" sird_epidemic | xgraph n=40 nplot=3 d1=1 style=normal &	",
" 								",
" Hypothetical Influenza in a boarding school:			",
" N=762 i0=1,  2 students infected per day, 1/2 of the infected	",
" population removed per day. Take b=2 k=0.5			",
" assume 10% mortality nu=0.1					",
"								",
" Normalized by N:						",
" sird_epidemic h=0.1 stepmax=200 i0=1 b=2 k=.5 nu=.1 N=762 mode=SIRD |",
"  xgraph n=200 nplot=4 d1=.1 style=normal label1=\"days\"  &	",
" 								",
" Normalized by N, output scaled by N:				",
" sird_epidemic h=0.1 stepmax=200 i0=1 b=2 k=.5 nu=.1 N=762 mode=SIRD scale=1 |",
"  xgraph n=200 nplot=4 d1=.1 style=normal label1=\"days\" &	",
" 								",
" Kong Flu 1968-1969:						",
" https://services.math.duke.edu/education/ccp/materials/diffcalc/sir/sir1.html",
" Population is N=s0=7.9 million, r0=1.5, the average period of	",
" infectiveness is  3 days so k=1/3, b=R0*k=(3/2)(1/3)=0.5, and initial",
" infected is i0=10.						",
"								",
"  Normalized by N						",
"  sird_epidemic h=1 i0=10 stepmax=200 k=.3333 b=.5 N=7.9e6 mode=SIR |	",
"      xgraph n=200 nplot=3 d1=1 style=normal &			",
"								",
"  Normalized by N, with scaling of the output by N:		",
"  sird_epidemic h=1 scale=1 i0=10 stepmax=200 k=.3333 b=.5 N=7.9e6 mode=SIR |",
"      xgraph n=200 nplot=3 d1=1 style=normal &			",
"     								", 
" Related programs: sir_epidemic, seir_epidemic			", 
NULL};

/*
 * References:
 * Kermack, W.O. and A.G. McKendrick, A Contribution to the Mathematical Theory of Epidemics.
 *   Proceedings of the Royal Society of London. Series A, 115:700-721, 1927.
 *
 * The SRI model describes an epidemic in terms of
 *   S = susceptibles in a population
 *   I = infectives in a population
 *   R = recovered
 *   D = dead
 *
 *   s0 = initial value of the susceptibles
 *   i0 = initial value of the infectives
 *   r0 = initial recovered = usually 0
 *   d0 = initial dead = usually 0 
 *   
 *   S(t) + I(t) + R(t) + D(t) = s0 + i0  = 1   (normalized by N)
 *   S(t) + I(t) + R(t) + D(t) = s0 + i0  = N   (not normalized by N)
 *   
 *   r0 = b*s0/k  = basic reproduction rate = b/k when S is normalized by N
 *   b = rate of infection
 *   k = rate removal
 *   nu = mortality rate  
 *  
 *   The encounters between susceptibles and the infectives is represented
 *   by the product S*I  
 *	S'(t) =  - b*S*I   (S'(t) is always negative)
 *	I'(t) = b*S*I - k*I - nu*I
 *	R'(t) = k*I 
 *	D'(t) = nu*I
 * S(t)= susceptible members (no births, deaths, immigration, or emigration)
 * I(t)= infective number (includes asymptomatic carriers)
 * R(t)= recovered members
 * D(t)= dead members
 *
 * There is an impiled flow from S(t) -> I(t) -> R(t) or D(t), though infected
 * who are quarantined immediately become part of R(t).
 *
 * The product b*S*I denotes the interaction of the infective population with
 * the susceptible population..
 *
 * Author:  April 2020: John Stockwell
 */

/**************** end self doc ********************************/

/* Prototype of function used internally */
static int
sird_epidemic_equations(double t, double y[4] , double yprime[4]);

/* Define values of imode */
#define SIRD_MODE 0
#define S_MODE 1
#define I_MODE 2
#define R_MODE 3
#define D_MODE 4

int
main(int argc, char **argv)
{
	register int i=0, j=0;		/* counters */
	register int number=4; 		/* the three dependent variables */
	int verbose=0;		/* verbose flag =1 chatty, =0 silent */
	int stepmax=0;		/* maximum number of steps */

	/* initial values of S, I, R */
	int normalize=1;	/* normalize S and I by R; =0 don't normalize */
	int scale=0;		/* don't scale; =1 scale output S,I,R,D by N */
				/* =2 scale output s,i,r by s0  */
	float scalar=0.0;	/* output scale factor */
	double N=0.0;		/* total population size */
	double s0=0.0;		/* initial value of susceptible population */
	double i0=0.0;		/* initial value of infectives */
	double r0=0.0;		/* initial value of recovered */
	double d0=0.0;		/* initial value of dead */

	double t=0.0;		/* time */
	double h=.001;		/* time increment */
	double tol=0.0;		/* time increment */

	double y[4]={0.0,0.0,0.0,0.0};	/* dependent variable of ODE system */

	rke_variables p;	/* variable used by RKE routines */

	float **yout=NULL; 	/* output array */
	float *tempout=NULL;	/* temporary output array */

	FILE *out_file=stdout;	/* pointer to file that we write out to */

	cwp_String mode="SIRD";	/* output mode of program */
	int imode=SIRD_MODE;	/* integer flag for mode */


	/* Hook up getpar */
	initargs(argc, argv);
	requestdoc(0);

	switch(filestat(STDOUT)) { /* Prevent floats from dumping on screen */
	case BADFILETYPE:
		warn("stdout is illegal filetype");
		pagedoc();
	break;
	case TTY:
		warn("stdout can't be tty");
		pagedoc();
	break; 
	default:			   /* rest are OK */
	break;

	}

	/* Get parameters */
	if (!getparint("stepmax", &stepmax))	stepmax = 40;
	if (!getparint("verbose", &verbose))	verbose = 0;

	/* Initial conditions y[0] = S  y[1]=I  y[2]=R */
	if (!getparint("normalize", &normalize))	normalize=1;
	if (!getpardouble("N", &N))		N=1000;
	if (!getpardouble("s0", &s0))		s0=N;
		y[0] = (normalize ? s0/N: s0);
	if (!getpardouble("i0", &i0))		i0=1.0;
		y[1] = (normalize ? i0/N: i0);
	if (!getpardouble("r0", &r0))		r0=0.0;
		y[2] = (normalize ? r0/N: r0);
	if (!getpardouble("d0", &d0))		d0=0.01;
		y[3] = (normalize ? d0/N: d0);

	/* scale output factor */
	if (!getparint("scale", &scale))	scale=0;

	if (!getpardouble("h", &h))		h = 1.0;
	if (!getpardouble("tol", &tol))		tol = RKE_ERR_BIAS_INIT;

	/* Get output mode, recall imode initialized to the default FABS */
	if (!getpardouble("h", &h))		h = 1.0;
	getparstring("mode", &mode);
	if (STREQ(mode, "S"))    	imode = S_MODE;
	else if (STREQ(mode, "I"))	imode = I_MODE;
	else if (STREQ(mode, "R"))      imode = R_MODE;
	else if (STREQ(mode, "D"))      imode = D_MODE;
	else if (!STREQ(mode, "SIRD"))
	    err("unknown operation=\"%s\", see self-doc", mode);

	/* allocate space in the output array */
	yout = ealloc2float(4,4*stepmax);

	/* zero out the array */
	memset((void *) yout[0], 0 , 4*stepmax*FSIZE);
	
	/* initialize Runge-Kutta-England routines */
	p = (rke_variables)
		rke_init(4, sird_epidemic_equations);

	/* set tolerance */
	p->error_bias=tol;

	for (i=0; i<stepmax; ++i) {
		double aimed_t;
		t=i*h;
		aimed_t=t+h;
  		if (verbose) {
			warn("using %3d accepted and %3d rejected steps",
	 			p->accepted_steps, p->rejected_steps);
			if (verbose) warn("error tolerance = %10.24f",p->error_bias);
		}

		/* convert doubles in y to floats in yout and write out */
		for(j=0; j<number; ++j) yout[i][j] = (float) y[j];

		/* run the Runge-Kutta-England solver */
  		rke_solve (p, &t, y, aimed_t);
	}

	/* write out according to the mode */
	tempout = ealloc1float(4*stepmax);

	/* set output scalar */
	if (scale==1) {
		scalar=N;
	} else if (scale==2) {
		scalar=s0;
	}


	if (imode==S_MODE) {
		for (i=0; i<stepmax; ++i)
			tempout[i] = (scale ?  scalar*yout[i][0]: yout[i][0]);
	} else if (imode==I_MODE) {
		for (i=0; i<stepmax; ++i)
			tempout[i] = (scale ? scalar*yout[i][1]: yout[i][1]);
	} else if (imode==R_MODE) {
		for (i=0; i<stepmax; ++i)
			tempout[i] = (scale ? scalar*yout[i][2]: yout[i][2]);
	} else if (imode==D_MODE) {
		for (i=0; i<stepmax; ++i)
			tempout[i] = (scale ? scalar*yout[i][3]: yout[i][3]);
	} else if (imode==SIRD_MODE) {

		for (i=0; i<stepmax; ++i)
			tempout[i] = (scale ? scalar*yout[i][0]: yout[i][0]);
		for (i=0; i<stepmax; ++i)
			tempout[i+stepmax] = (scale ? scalar*yout[i][1]: yout[i][1]);
		for (i=0; i<stepmax; ++i)
			tempout[i+2*stepmax] = (scale ? scalar*yout[i][2]: yout[i][2]);
		for (i=0; i<stepmax; ++i)
			tempout[i+3*stepmax] = (scale ? scalar*yout[i][3]: yout[i][3]);;
	}

	if (imode==SIRD_MODE) {
		efwrite(tempout,sizeof(float),4*stepmax,out_file);
	} else {
		efwrite(tempout,sizeof(float),stepmax,out_file);
	}

	/* end the session with rke */
	rke_term(p);

	return EXIT_SUCCESS;
}


static int
sird_epidemic_equations(double t, double y[4] , double yprime[4])
/*********************************************************************
sird_epidemic_equations - the system of ODE's descibing the SIRD epidemic
  model
**********************************************************************
t	independent variable "time"
y 	dependent variable being solved for y(t)
yprime	derivative of dependent variable  y'(t)
**********************************************************************
Notes: This is an example of an autonomous system of ODE's
**********************************************************************/
{
	double b=0.0;	/* infection rate	*/
	double k=0.0;	/* recovery rate	*/
	double nu=0.0;	/* mortality rate	*/
	double gamma=0.0;	/* social distancing parameter */

	/* parameters */
	if (!getpardouble("b", &b))		b = 0.5;
	if (!getpardouble("k", &k))		k = 0.333;
	if (!getpardouble("nu", &nu))		nu = 0.0;

	/* social distancing */
	if (!getpardouble("gamma", &gamma))		gamma = 0.0;
	

	yprime[0] =  -b*y[0]*y[1]/(1 + gamma*y[1])  ;
	yprime[1] = b*y[0]*y[1]/(1 + gamma*y[1])  - k*y[1] -  nu*y[1];
	yprime[2] = k*y[1]; 
	yprime[3] = nu*y[1]; 

    return 1;
}
