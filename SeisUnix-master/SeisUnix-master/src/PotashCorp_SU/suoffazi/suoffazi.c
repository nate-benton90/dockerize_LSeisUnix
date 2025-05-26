/* Copyright (c) Colorado School of Mines, 2001.*/
/* All rights reserved.                       */

/* SUAZIMUTH: $Revision: 1.8 $ ; $Date: 1999/02/23 18:18:40 $		*/

#include "su.h"
#include "segy.h"
#include "header.h"

/*********************** self documentation **********************/
char *sdoc[] = {
"									",
" SUOFFAZI -  compute trace AZIMUTH and OFFSET given the sx,sy,gx,gy    ",
"	      header fields and store in  user-specified header fields  ",
"									",
"  suoffazi <stdin >stdout [optional parameters]			",
"									",
" Required parameters:							",
" none									",
"									",
" Optional parameters:							",
" keya=otrav		header field to store computed azimuths  	",
" keysx=sx		header field to take shot x from   		",
" keysy=sy		header field to take shot y from		",
" keygx=gx		header field to take receiver x from            ",
" keygy=gy		header field to take receiver y from		",
"									",
" scale=1.0		value(key) = scale * azimuth			",
" az=0			0-179.9999 deg convention, reciprocity assumed	",
"			=1 0-359.999 deg convention, points from source ",
"			to receiver					",
" sector=1.0		if set, defines output in sectors of size	",
"			sector=degrees_per_sector, the default mode is  ",
"			the full range of angles specified by az	",
" off=0			=1 compute offsets 				",
" keyo=offset		header filed to store offset			",
"									",
"									",
" Notes:								",
" The value of header word \"keya\" is computed from the values of	",
" sx,sy,gx,gy. The output field \"otrav\" was chosen arbitrarily as an	",
" example of a little-used header field, however, the user may choose 	",
" any field that is convenient for his or her application.		",

" Setting the sector=number_of_degrees_per_sector sets key field to	",
" sector number rather than an angle in degrees.			",
"  									",
" For az=0, azimuths are measured from the North, however, reciprocity  ",
" is assumed, so azimuths go from 0 to 179.9999 degrees. If sector option",
" is set, then the range is from 0 to 180/sector. 			",
"  									",
" For az=1, azimuths are measured from the North, with the assumption that",
" the direction vector points from the receiver to the source, no	",
" reciprocity is assumed, so the angles go from 0 to 359.999 degrees.	",
" If the sector option is set, then the range is from 0 to 360/sector.	",
"									",
" Type: sukeyword -o    to see the keywords and descriptions of all	",
"		       header fields.					",
"									",
" To plot midpoints, use:     su3dchart					",
"									",
NULL};

/* Credits:
 *	based on suchw, su3dchart
 *      CWP: John Stockwell and  UTulsa: Chris Liner, Oct 1998
 * 
 *  Algorithm:
 *    midpoint x  value  xm = (sx + gx)/2
 *    midpoint y  value  ym = (sy + gy)/2
 * 
 *   Azimuth will be defined as the angle, measured in degrees,
 *   turned from North, of a vector pointing to the source from the midpoint, 
 *   or from the midpoint to the source. Azimuths go from 0-179.000 degrees
 *   or from 0-180.0 degrees.
 *   
 *   value(key) = scale*[90.0 - (180.0/PI)*(atan((sy - ym)/(sx - xm))) ]
 *     or
 *   value(key) = scale*[180.0 - (180.0/PI)*(atan2((ym - sy),(xm - sx)) ]
 * 
 *   Trace header fields accessed: sx, sy, gx, gy, scalco. 
 */
/**************** end self doc ***********************************/

/* prototype for function used internally */
void computeAzimuth(cwp_String type_out, Value *val_out,
			double sx, double sy, double gx, double gy,
			double mx, double my, double scale, int az);
void computeOffset(cwp_String type_out, Value *val_out,
			double sx, double sy, double gx, double gy);

segy tr;

int
main(int argc, char **argv)
{
	/* azimuth */
	cwp_String keya;	/* output key for azimuth	*/
	cwp_String typea;	/*  type for output key		*/
	int indexa;		/*  index for output key 	*/
	Value vala;		/* value of output key		*/
	/* offset */
	cwp_String keyo;	/* output key for offset	*/
	cwp_String typeo;	/*  type for output key		*/
	int indexo;		/*  index for output key 	*/
	Value valo;		/* value of output key		*/
	/* sx */
	cwp_String keysx;	/* input key for sx	*/
	cwp_String typesx;	/*  type for output key		*/
	int indexsx;		/*  index for output key 	*/
	Value valsx;
	/* sy */		
	cwp_String keysy;	/* sy */
	cwp_String typesy;	
	int indexsy;		
	Value valsy;		
	/* gx */
	cwp_String keygx;	/* gx */
	cwp_String typegx;	
	int indexgx;		
	Value valgx;		
	/* gy */
	cwp_String keygy;	/* gy */
	cwp_String typegy;	
	int indexgy;		
	Value valgy;		

	double sx=0.0;		
	double sy=0.0;		
	double gx=0.0;		
	double gy=0.0;		
	double factor=0.0;	
	double scale=0.0;	

	double mx, my;		/* x,y midpoint coordinates	*/
	int az=0;		/* azimuth convention flag	*/
	double sector=0;	/* azimuth by sectors		*/
	int off=0;
	
	/* Initialize */
	initargs(argc, argv);
	requestdoc(1);

	/* Get parameters */
	/* get key's */
	if (!getparstring("keya",&keya)) 	keya = "otrav";		
	if (!getparstring("keyo",&keyo)) 	keyo = "offset";		
	if (!getparstring("keysx",&keysx)) 	keysx = "sx";		
	if (!getparstring("keysy",&keysy)) 	keysy = "sy";		
	if (!getparstring("keygx",&keygx)) 	keygx = "gx";		
	if (!getparstring("keygy",&keygy)) 	keygy = "gy";		
	if (!getpardouble("scale",&scale)) 	scale = 1.0;
	if (!getparint("az",&az))		az = 0;
	if (!getparint("off",&off))		off = 0;
	if (!getpardouble("sector",&sector)) 	sector = 1.0;

	/* absorb sector into scale */
	scale = scale/sector;

	/* get type and index values of output */
	typea  = hdtype(keya);
	indexa = getindex(keya);
	typeo  = hdtype(keyo);
	indexo = getindex(keyo);
	typesx  = hdtype(keysx);
	indexsx = getindex(keysx);
	typesy  = hdtype(keysy);
	indexsy = getindex(keysy);
	typegx  = hdtype(keygx);
	indexgx = getindex(keygx);
	typegy  = hdtype(keygy);
	indexgy = getindex(keygy);

	/* loop over traces */
	while (gettr(&tr)) {

		/* get header values */
		gethval(&tr,indexsx,&valsx);
		sx = vtod(typesx,valsx);
		
		gethval(&tr,indexsy,&valsy);
		sy = vtod(typesy,valsy);
		
		gethval(&tr,indexgx,&valgx);
		gx = vtod(typegx,valgx);

		gethval(&tr,indexgy,&valgy);
		gy = vtod(typegy,valgy);
		
		/* scaler tr.scalco */
		factor = pow(10.0,tr.scalco);
		sx=sx*factor;
		sy=sy*factor;
		gx=gx*factor;
		gy=gy*factor;
		

		/* compute midpoints */
		mx = (double) (0.5*(sx + gx));
		my = (double) (0.5*(sy + gy));

		/* compute the azimuths */
		computeAzimuth(typea, &vala, sx, sy,
				gx, gy, mx, my, scale, az);

		/* put the new header value */
		puthval(&tr, indexa, &vala);
		
		/* compute the offset */
		if(off) {
			computeOffset(typeo, &valo, sx, sy,
					gx, gy);

			/* put the new header value */
			puthval(&tr, indexo, &valo);
		}

		/* write traces */
		puttr(&tr);

	}

	return EXIT_SUCCESS;
}

#define DRADTODEG ((double) (180.0/PI))

void computeAzimuth(cwp_String type_out, Value *val_out,
			double sx, double sy, double gx, double gy,
			double mx, double my, double scale, int az)
/************************************************************************
computeAzimuth - compute the azimuth of traces

*************************************************************************
Input:
sx		source x coordinate
sy		source y coordinate
gx		geophone x coordinate
gy		geophone y coordinate
mx		midpoint x coordinate
my		midpoint y coordinate
scale		scale factor
az		azimuth convention flag

Output:
val_out		value of output field

*************************************************************************
Notes:
Based on suchw and su3dchart. Does computations as doubles. Outputs
either as 0-179.999 or 0-359.999 degrees, or in sectors of "sector"
degrees in size.

*************************************************************************
Author: CWP: John Stockwell, 28 Oct 1998
*************************************************************************/
{
	double dval_out=0.0;	/* output value			*/

	/* compute the output */
	if (az==0) { /* 0-179.999 convention */
		/* compute angle */
 		dval_out = scale*(NINT(90.0-
				DRADTODEG*atan((sy-my)/(sx-mx)))); 

		/* Make output go from 0-179.99999 degrees. */
		if (dval_out == 180.000) dval_out = 0.0;

	} else { /* 0-359.999 */

		/* compute angle */
		dval_out = scale*DRADTODEG*atan2((sx-mx),(sy-my));

		/* Make output go from 0-359.99999 degrees. */
		if (dval_out < 0.) dval_out += 360.0;
	}

	/* Convert output to appropriate type */
	switch (*type_out) {
	case 's':
		err("can't change char header word");
	break;
	case 'h':
		val_out->h = (short) dval_out;
	break;
	case 'u':
		val_out->u = (unsigned short) dval_out;
	break;
	case 'l':
		val_out->l = (long) dval_out;
	break;
	case 'v':
		val_out->v = (unsigned long) dval_out;
	break;
	case 'i':
		val_out->i = (int) dval_out;
	break;
	case 'p':
		val_out->p = (unsigned int) dval_out;
	break;
	case 'f':
		val_out->f = (float) dval_out;
	break;
	case 'd':
		val_out->d = (double) dval_out;
	break;
	default:
		err("unknown type %s", type_out);
	break;
	}
}

void computeOffset(cwp_String type_out, Value *val_out,
			double sx, double sy, double gx, double gy)
/************************************************************************
computeOffset - compute the offset of traces

*************************************************************************
Input:
sx		source x coordinate
sy		source y coordinate
gx		geophone x coordinate
gy		geophone y coordinate

Output:
val_out		value of output field

*************************************************************************/
{
	double dval_out=0.0;	/* output value			*/
	
	dval_out=sqrt((sx-gx)*(sx -gx)+(sy-gy)*(sy-gy));
	
	/* Convert output to appropriate type */
	switch (*type_out) {
	case 's':
		err("can't change char header word");
	break;
	case 'h':
		val_out->h = (short) dval_out;
	break;
	case 'u':
		val_out->u = (unsigned short) dval_out;
	break;
	case 'l':
		val_out->l = (long) dval_out;
	break;
	case 'v':
		val_out->v = (unsigned long) dval_out;
	break;
	case 'i':
		val_out->i = (int) dval_out;
	break;
	case 'p':
		val_out->p = (unsigned int) dval_out;
	break;
	case 'f':
		val_out->f = (float) dval_out;
	break;
	case 'd':
		val_out->d = (double) dval_out;
	break;
	default:
		err("unknown type %s", type_out);
	break;
	}
}
