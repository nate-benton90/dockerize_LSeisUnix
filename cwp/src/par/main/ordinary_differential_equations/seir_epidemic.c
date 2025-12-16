/* Copyright (c) Colorado School of Mines, 2011.*/
/* All rights reserved.			*/

/* SEIREPIDEMIC: $Revision: 1.4 $ ; $Date: 2020/06/12 23:37:18 $	*/

#include "par.h"
#include "rke.h"

/*********************** self documentation **********************/

char *sdoc[] = {
"								",
" SEIREPIDEMIC - the SEIR and SEIRS EPIDEMIC models		",
"								",
"  seirepidemic > [stdout]					",
"								",
" Required Parameters: none					",
" Optional Parameters:						",
" normalize=1		normalize S,E,I by N; =0 don't normalize",
" scale=0		don't scale				",
"			=1 scale output S,E,I by N		",
"			=2 scale output S,E,I by s0		",
" N=1000		total population			",
" s0=N			initial number of susceptibles		",
" e0=1			initial number of exposed		",
" i0=1			initial number of infectives		",
" r0=0.0		initial number of removed (should be 0)	",
" 								",
" k=.5			transmission rate			",
" eti=.3333		exposure to infective rate		",
" b=.3333		removal rate = death + recovery rates	",
" 								",
" .... with vital dynamics (i.e. birth and death rate) 		",
" mu=0.0		birth rate 				",
" nu=0.0		death rate 				",
"  ... SEIRS ... with reinfection				",
" xi=0.0		re-infection parameter		  ",
" 								",
" h=1			increment in time			",
" tol=1.e-08		error tolerance				",
" stepmax=40		maximum number of steps to compute	",
" mode=SEIR		S -> E -> I -> R			",
"			=S only, =E only, =I only, =R only	",
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
" About compartmentalized models: The population is assumed to  ",
" move from being Susceptible, to Infective, and finally to the ",
" Removed, who are dead and the recovered.			",
"								",
" Important quantities:						",
" r0 = number of new infections per single infected host	",
"  1 < r0 < 1.5 for influenza, (2.2 to 2.7 for Covid-19), 12 to ",
" 18 for measles.						",
"  b, k, s0, and r0 are related via				",
"  k = b*s0/r0 = b/r0 when s0/N and s0=N			",
"								",
" S = number susceptible to the infection			",
" E = number of exposed						",
" I = number of those capable of passing on the infection	",
" R = number removed = dead + recovered				",
" Examples:							",
" Default:							",
" seirpidemic | xgraph n=40 nplot=4 d1=1 style=normal &		",
"								",
" Hypothetical flu in a school models:				",
" here there is a 0.3 exposure to infective rate		",
" seirepidemic h=0.1 stepmax=100 s=.3 b=.3 k=.9 N=100 mode=SIR |",
"	xgraph n=100 nplot=4 d1=1 style=normal &		",
"								",
" Hong Kong Flu 1968-1969:					",
" https://services.math.duke.edu/education/ccp/materials/diffcalc/sir/sir1.html",
" Population is N=s0=7.9 million, r0=1.5, the average period of 	",
" infectiveness is  3 days so k=1/3, b=r0*k=(3/2)(1/3)=0.54, and initial",
" infected is i0=10. An exposed to infective rate s=1/3 is assumed	",
"									",
"  seirepidemic h=1 stepmax=200 s=.3333 k=.3333 b=.5 N=7.9e6 mode=SIR |	",
"	xgraph n=200 nplot=4 d1=1 style=normal &			",
"									",
" Related programs: sir_epidemic, sird_epidemic 			",
NULL};

/*
 * References:
 *
 * Kermack, W. O. and A. G. McKendrick (1927) A contribution to the 
 *  mathematical theory of epidemics, Procedings of the Royal Socieity A.
 *
 * The SRI model describes an epidemic in terms of
 *   S = susceptibles in a population
 *   E = exposed in a population
 *   I = infectives in a population
 *   R = removed = recovered + dead
 *
 *   s0 = initial value of the susceptibles
 *   e0 = initial value of the exposed
 *   i0 = initial value of the infectives
 *   r0 = initial removed value = 0
 *   
 *   S(t) + E(t) + I(t) + R(t) = N 
 *   
 *   S(t) + E(t) + I(t) + R(t) = 1  when S, E, and I are normalized by N
 *   
 *   r0 = b*s0/k  = basic reproduction rate
 *   b = rate of exposure
 *   s = rate of infection of the exposed
 *   k = rate removal = recovery rate + death rate
 *
 *   Without vital dynamics.
 *   The encounters between susceptibles and infectives is represented
 *   by the product S*I  
 *	S'(t) = - b*S*I		( newly exposed )
 *	E'(t) = b*S*I - s*E	( exposed - newly infected )
 *	I'(t) = s*E - k*I 	( infectives - newly removed )
 *	R'(t) = k*I 		( removed )	
 *
 *  Without vital dynamics.
 *  SIR model (with Baker 2020 reactive social distancing):
 *   As infective number increases, social distancing increases and the  
 *   infectivity value b decreases.
 *
 *      s'(t) =  - b*s(t)*i(t)/(1+gamma*i(t))
 *      i'(t) = b*s(t)*i(t)/(1+gamma*i(t)) - k*i(t)
 *      r'(t) = k*i(t)
 *
 *   With vital dynamics (birth and death rates added).
 *   The encounters between susceptibles and infectives is represented
 *   by the product S*I  
 *	S'(t) = mu - nu*S -  b*S*I  (birth - dead - newly exposed)
 *	E'(t) = b*S*I - nu*E - s*E  (exposed - dead - newly infected)
 *	I'(t) = s*E - k*I - nu*I	(infected - newly removed - dead))
 *	R'(t) = k*I - nu*R		(removed - dead )	
 *
 * S(t)= susceptible members (no births, deaths, immigration, or emigration)
 * E(t)= Exposed number 
 * I(t)= infective number 
 * R(t)= removed members = recovered + dead + sequestered
 *
 * There is an impiled flow from S(t) -> E(t) -> I(t) -> R(t), though infected
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
sir_epidemic_equations(double t, double y[4] , double yprime[4]);

/* Define values of imode */
#define SEIR_MODE 0
#define S_MODE 1
#define E_MODE 2
#define I_MODE 3
#define R_MODE 4

int
main(int argc, char **argv)
{
	register int i=0, j=0;		/* counters */
	register int number=4; 		/* the four dependent variables */
	int verbose=0;		/* verbose flag =1 chatty, =0 silent */
	int stepmax=0;		/* maximum number of steps */

	/* initializations */
	int normalize=1;	/* normalize S and I by N; =0 don't normalize */
	int scale=0;		/* don't scale; =1 scale S,I,E,R by N */
				/* =2 scale output s,i,r by s0  */
	float scalar=0;		/* output scale factor */
	double N=0.0;		/* total population size */

	double s0=0.0;		/* initial value of susceptible population */
	double e0=0.0;		/* initial value of expose */
	double i0=0.0;		/* initial value of infectives */
	double r0=0.0;		/* initial value of removed */

	double t=0.0;		/* time */
	double h=.001;		/* time increment */
	double tol=0.0;		/* time increment */

	double y[4]={0.0,0.0,0.0,0.0};	/* dependent variable of ODE system */

	rke_variables p;	/* variable used by RKE routines */

	float **yout=NULL; 	/* output array */
	float *tempout=NULL;	/* temporary output array */

	FILE *out_file=stdout;	/* pointer to file that we write out to */

	cwp_String mode="SEIR";	/* output mode of program */
	int imode=SEIR_MODE;	/* integer flag for mode */


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

	/* Initial conditions y[0] = S  y[1]=E  y[2]=I  y[3]=R */
	if (!getparint("normalize", &normalize))	normalize=1;
	if (!getpardouble("N", &N))		N=1000;
	if (!getpardouble("s0", &s0))	   s0=N;
		 y[0] = (normalize ? s0/N: s0);
	if (!getpardouble("e0", &e0))	   e0=1.0;
		 y[1] = (normalize ? i0/N: i0);
	if (!getpardouble("i0", &i0))	   i0=1.0;
		 y[1] = (normalize ? i0/N: i0);
	if (!getpardouble("r0", &r0))		r0=0.0;
		y[3] = (normalize ? r0/N: r0);

	/* scale output flag */
	if (!getparint("scale", &scale))	scale=0;

	if (!getpardouble("h", &h))		h = 1.0;
	if (!getpardouble("tol", &tol))		tol = RKE_ERR_BIAS_INIT;

	/* Get output mode, recall imode initialized to the default FABS */
	if (!getpardouble("h", &h))		h = 1.0;
	getparstring("mode", &mode);
	if (STREQ(mode, "S"))		imode = S_MODE;
	else if (STREQ(mode, "E"))	imode = E_MODE;
	else if (STREQ(mode, "I"))	imode = I_MODE;
	else if (STREQ(mode, "R"))	imode = R_MODE;
	else if (!STREQ(mode, "SEIR"))
		err("unknown operation=\"%s\", see self-doc", mode);

	/* allocate space in the output array */
	yout = ealloc2float(4,4*stepmax);

	/* zero out the array */
	memset((void *) yout[0], 0 , 4*stepmax*FSIZE);
	
	/* initialize Runge-Kutta-England routines */
	p = (rke_variables)
		rke_init(4, sir_epidemic_equations);

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
			tempout[i] = (scale ? scalar*yout[i][0]: yout[i][0]);
	} else if (imode==E_MODE) {
		for (i=0; i<stepmax; ++i)
			tempout[i] = (scale ? scalar*yout[i][1]: yout[i][1]);
	} else if (imode==I_MODE) {
		for (i=0; i<stepmax; ++i)
			tempout[i] = (scale ? scalar*yout[i][2]: yout[i][2]);
	} else if (imode==R_MODE) {
		for (i=0; i<stepmax; ++i)
			tempout[i] = (scale ? scalar*yout[i][3]: yout[i][3]);
	} else if (imode==SEIR_MODE) {

		for (i=0; i<stepmax; ++i)
			tempout[i] = (scale ? scalar*yout[i][0]: yout[i][0]);
		for (i=0; i<stepmax; ++i)
			tempout[i+stepmax] = (scale ? scalar*yout[i][1]: yout[i][1]);
		for (i=0; i<stepmax; ++i)
			tempout[i+2*stepmax] = (scale ? scalar*yout[i][2]: yout[i][2]);
		for (i=0; i<stepmax; ++i)
			tempout[i+3*stepmax] = (scale ? scalar*yout[i][3]: yout[i][3]);
	}

	if (imode==SEIR_MODE) {
		efwrite(tempout,sizeof(float),4*stepmax,out_file);
	} else {
		efwrite(tempout,sizeof(float),stepmax,out_file);
	}

	/* end the session with rke */
	rke_term(p);

	return EXIT_SUCCESS;
}


static int
sir_epidemic_equations(double t, double y[4] , double yprime[4])
/*********************************************************************
seir_epidemic_equations - the system of ODE's descibing the SEIR epidemic
  model
**********************************************************************
t	independent variable "time"
y 	dependent variable being solved for y(t)
yprime	derivative of dependent variable  y'(t)
**********************************************************************
Notes: This is an example of an autonomous system of ODE's
**********************************************************************/
{
	double b=0.0;	/* exposure rate		*/
	double eti=0.0;	/* exposure to infection rate	*/
	double k=0.0;	/* removal rate			*/
	
	/* ... vital dynamics ... */
	double mu=0.0;	/* birth rate			*/
	double nu=0.0;	/* death rate			*/

	/* ... reinfection ... */
	double xi=0.0;	/* reinfection rate */
	
	/* parameters */
	if (!getpardouble("b", &b))		b = .5;
	if (!getpardouble("eti", &k))		eti = .333;
	if (!getpardouble("k", &k))		k = .333;

	/* ... vital dynamics ... */
	if (!getpardouble("mu", &mu))		mu = 0.0;
	if (!getpardouble("nu", &nu))		nu = 0.0;

	/* reinfection */
	if (!getpardouble("xi", &xi))		xi = 0.0;

	yprime[0] = mu - nu*y[0] - b*y[0]*y[1] + xi*y[3] ;
	yprime[1] = b*y[0]*y[1]  - nu*y[1] -  eti*y[1];
	yprime[2] = eti*y[1] - k*y[2] - nu*y[2]; 
	yprime[3] = k*y[2] - nu*y[3] - xi*y[3]; 

	return 1;
}
