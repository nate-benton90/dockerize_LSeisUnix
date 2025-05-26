/* Copyright (c) Colorado School of Mines, 1997.*/
/* All rights reserved.                       */

#ifndef VIEWER3_H
#define VIEWER3_H

#include "GL/glu.h"
#include "GL/glut.h"
#include "MGL/trackball.h"

typedef struct {
	int **ixgh;	/*triangle positions in x*/
	int **iygh;     /*triangle positions in y*/
	float **xhz;	/*nonuniform grid for x on horizon, varying from hz to hz*/
	float **yhz;	/*nonuniform grid for y on horizon, varying from hz to hz*/
	float **zhz;      /*nonuniform grid for z on horizon, varying from hz to hz*/
	float **vhz;  	/*velocity on the nonuniform horizon*/
	float **dvdzhz; 	/*velocity z-gradient*/
	float *rixr;  	/*real index for source before and receiver after called*/
	float *riyr;    /*real index for source before and receiver after called*/
	float *rizr;    /*real index for source before and receiver after called*/ 
	float *temt;  	/*traveltime table for upper/lower face before/after called*/
	float **n1llhz;	/*1_normals to the lower left cell horizon face*/
	float **n2llhz;	/*2_normals to the lower left cell horizon face*/
	float **n3llhz;	/*3_normals to the lower left cell horizon face*/
        float **n1urhz;   /*1_normals to the upper right  cell horizon face*/
        float **n2urhz;   /*2_normals to the upper right cell horizon face*/
        float **n3urhz;   /*3_normals to the upper right cell horizon face*/

	int nd;	/*number of samples in delta*/
	int np;	/*number of samples in azimuth psi*/
} Horizon;

#endif /* VIEWER3_H */
