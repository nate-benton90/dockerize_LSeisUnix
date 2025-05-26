
#include "gridhd.h"
#include "ghdr.h"
#include "grid.h"
#include "par.h"

int fgetghdr(FILE  *gfp, ghed *gh) {
/*  read in a grid header from a file pointer
  input:
	gfp 	--	file pointer of disk grid file
  output:
	gh	--	grid header 
  return:
	0	--	grid header found
	1	--	not a standard grid file (no header found)
  author:
	Zhiming Li, 	8/17/92		      

  notes:
	the grid file consists of n1*n2*n3*n4*n5 grid data values followed 
        by NGHDRS (25) floating-point header numbers 
*/
	
	int ierr;
	float tmp; 
	int nread, idtype;
	int n1, n2, n3, n4, n5;
	float scale;

	filetype ftype;

#ifdef __convex__
	long long isize, jsize, iseek;
#else
	int isize, jsize, iseek;
#endif
	
	

	ierr = 0;
	/* check to see if gfp is pipe */
	ftype = filestat(fileno(gfp));
	
	if(ftype == DISK) {
		/* retain the pointer position */
		iseek = ftell2g(gfp);
		/* check grid file size */ 
		fseek2g(gfp,0L,2);
		jsize = ftell2g(gfp);
		if(jsize <  GHDRBYTES ) {
			ierr = 1;
			warn(" warning: grid file size less than %d bytes",
				GHDRBYTES);
			return ierr;
		}

		fseek2g(gfp,-GHDRBYTES,2);
	}

	/* read in header */
	nread = efread( (char *)gh,1,GHDRBYTES,gfp);
	if(nread!=GHDRBYTES) {
		warn(" warning: grid header read error \n");
		ierr = 1;
		return ierr; 
	} 
	/* check parameters */
	scale = gh->scale;
	if(scale==0.) {
		warn(" warning: gh.scale = 0.; 1 used \n");
		scale = 1.;
	}
	tmp  = gh->n1 / scale + 0.5;
	n1 = (int) tmp;
	if(n1==0) n1 = 1; 
	tmp  = gh->n2 / scale + 0.5;
	n2 = (int) tmp;
	if(n2==0) n2 = 1;
	tmp  = gh->n3 / scale + 0.5;
	n3 = (int) tmp;
	if(n3==0) n3 = 1;
	tmp  = gh->n4 / scale + 0.5;
	n4 = (int) tmp;
	if(n4==0) n4=1;
	tmp  = gh->n5 / scale + 0.5;
	n5 = (int) tmp;
	if(n5==0) n5 = 1;

	/* get grid data type */
	tmp = gh->dtype / scale + 0.5;
	idtype = (int) tmp;
	if(idtype!=1 && idtype!=2 && idtype!=4 && idtype!=8) {
		warn(" warning: invalid dtype=%d; dtype=4 used \n",idtype);
		idtype = 4;
	} 

	if(ftype==DISK) {
		isize = n1*n2*n3;
		isize = isize*n4*n5*idtype + GHDRBYTES;
		if(jsize != isize ) {
			ierr = 1;
			warn(
	" warning: grid file size: %g bytes not consistant with grid header\n",
				(float)jsize);
			return ierr;
		}
		/* return to the original pointer position */
		fseek2g(gfp,iseek,0);
	}


	return ierr;
}
	  
	
int fputghdr(FILE  *gfp, ghed *gh) {
/*  write a grid header to a grid file
  input:
        gfp     --      file pointer of disk grid file
        gh      --      grid header
  return:
        0       --      grid header written/updated
        1       --      grid size error
  author:
        Zhiming Li,     8/17/92               

  notes:
        the grid file consists of n1*n2*n3*n4*n5 grid data values followed
        by NGHDRS (25) floating-point header numbers
*/
	int ierr;
        float tmp;
        int nwrite, idtype;
        int n1, n2, n3, n4, n5;
        float scale;

	filetype ftype;
#ifdef __convex__
	long long isize, jsize;
#else
	int isize, jsize;
#endif

	ierr = 0;

	/* check if gfp is pipe */
	ftype = filestat(fileno(gfp));

	if(ftype==DISK) {
		/* flush gfp buffer, in case call program forgets to do so */
        	fflush(gfp);
        	/* check grid file size */
        	fseek2g(gfp,0L,2);
        	jsize = ftell2g(gfp);
			/*
        	if(jsize <  GHDRBYTES ) {
                	ierr = 1;
                	warn(" warning: grid file size less than %d bytes",
				GHDRBYTES);
                	return ierr;
        	}
			*/
	}

	/* check parameters */
	/* make sure n1,n2,n3,n4,n5 are not zero */
        scale = gh->scale;
        if(scale==0.) {
                warn(" warning: gh.scale = 0.; 1 used \n");
                scale = 1.;
        }
        tmp  = gh->n1 / scale + 0.5;
        n1 = (int) tmp;
        if(n1==0) n1 = 1;
        tmp  = gh->n2 / scale + 0.5;
        n2 = (int) tmp;
        if(n2==0) n2 = 1;
        tmp  = gh->n3 / scale + 0.5;
        n3 = (int) tmp;
        if(n3==0) n3 = 1;
        tmp  = gh->n4 / scale + 0.5;
        n4 = (int) tmp;
        if(n4==0) n4=1;
        tmp  = gh->n5 / scale + 0.5;
        n5 = (int) tmp;
        if(n5==0) n5 = 1;

   	/* get grid data type */
        tmp = gh->dtype / scale + 0.5;
        idtype = (int) tmp;
	if(idtype!=1 && idtype!=2 && idtype!=4 && idtype!=8) {
		warn(" warning: invalid dtype=%d; dtype=4 used \n",idtype);
		idtype = 4;
	} 


	gh->scale = scale;
	gh->dtype = (float)idtype * scale;
	gh->n1 = (float)n1 * scale;
	gh->n2 = (float)n2 * scale;
	gh->n3 = (float)n3 * scale;
	gh->n4 = (float)n4 * scale;
	gh->n5 = (float)n5 * scale;

	/* position gfp for grid header */
	if(ftype==DISK) {
		isize = n1*n2*n3;
		isize = isize*n4*n5*idtype + GHDRBYTES;
        	if(jsize==isize || jsize==isize-GHDRBYTES) {
			fseek2g(gfp,isize-GHDRBYTES,0);
		} else {
                	ierr = 1;
        		warn(
	" warning: grid file size: %g bytes not consistant with grid header\n",
                	(float)jsize);
			/* position to the end of the file */
			fseek2g(gfp,0L,2);
        	}
	}
	nwrite = efwrite((char*)gh,1,GHDRBYTES,gfp); 
	if(nwrite!=GHDRBYTES) {
		ierr=1;
		warn(" warning: grid header write error \n");
	}

	return ierr;
}

void fromghdr(ghed *gh, float *scale, int *dtype, int *n1, int *n2, int *n3,
	int *n4, int *n5, float *d1, float *d2, float *d3, float *d4,
	float *d5, float *o1, float *o2, float *o3, float *o4, float *o5,
	float *dcdp2, float *dline3, float *ocdp2, float *oline3,
	float *gmin, float *gmax, int *orient, int *gtype) {
/* from grid header gh, find the values of the header parameters */ 
/* return values with scale factor removed */ 

	float tmp, sc;
	
	sc = gh->scale;
	if(sc==0.) err(" grid file header value: gh.scale = 0. ");
	*scale = sc;
	tmp = gh->dtype / sc + 0.5;
	*dtype = (int) tmp;
	tmp = gh->n1 / sc + 0.5;
	*n1 = (int) tmp;
	tmp = gh->n2 / sc + 0.5;
	*n2 = (int) tmp;
	tmp = gh->n3 / sc + 0.5;
	*n3 = (int) tmp;
	tmp = gh->n4 / sc + 0.5;
	*n4 = (int) tmp;
	tmp = gh->n5 / sc + 0.5;
	*n5 = (int) tmp;
	*d1 = gh->d1 / sc; 	
	*d2 = gh->d2 / sc; 	
	*d3 = gh->d3 / sc; 	
	*d4 = gh->d4 / sc; 	
	*d5 = gh->d5 / sc; 	
	*o1 = gh->o1 / sc; 	
	*o2 = gh->o2 / sc; 	
	*o3 = gh->o3 / sc; 	
	*o4 = gh->o4 / sc; 	
	*o5 = gh->o5 / sc; 	
	*dcdp2 = gh->dcdp2 / sc; 	
	*dline3 = gh->dline3 / sc; 	
	*ocdp2 = gh->ocdp2 / sc; 	
	*oline3 = gh->oline3 / sc; 	
	*gmin = gh->gmin / sc; 	
	*gmax = gh->gmax / sc; 	
	tmp = gh->orient / sc + 0.5; 	
	*orient = tmp; 
	tmp = gh->gtype / sc + 0.5; 	
	*gtype = tmp; 
}
		
void toghdr(ghed *gh, float *scale, int *dtype, int *n1, int *n2, int *n3,
	int *n4, int *n5, float *d1, float *d2, float *d3, float *d4,
	float *d5, float *o1, float *o2, float *o3, float *o4, float *o5,
	float *dcdp2, float *dline3, float *ocdp2, float *oline3,
	float *gmin, float *gmax, int *orient, int *gtype) {
/* update the grid header gh, using the values of the header parameters */ 
/* header values will be updated with scale factor applied */ 

	float sc;
	
	sc = *scale;
	if(sc==0.) err(" input scale factor: scale = 0. ");
	gh->scale = sc;
	gh->dtype = *dtype * sc;
	gh->n1 = *n1 * sc;
	gh->n2 = *n2 * sc;
	gh->n3 = *n3 * sc;
	gh->n4 = *n4 * sc;
	gh->n5 = *n5 * sc;
	gh->d1 = *d1 * sc;
	gh->d2 = *d2 * sc;
	gh->d3 = *d3 * sc;
	gh->d4 = *d4 * sc;
	gh->d5 = *d5 * sc;
	gh->o1 = *o1 * sc;
	gh->o2 = *o2 * sc;
	gh->o3 = *o3 * sc;
	gh->o4 = *o4 * sc;
	gh->o5 = *o5 * sc;
	gh->dcdp2 = *dcdp2 * sc;
	gh->dline3 = *dline3 * sc;
	gh->ocdp2 = *ocdp2 * sc;
	gh->oline3 = *oline3 * sc;
	gh->gmin = *gmin * sc;
	gh->gmax = *gmax * sc;
	gh->orient = *orient * sc;
	gh->gtype = *gtype * sc;
}

/* get header value from key */
void getgval(ghed *gh, char *key, float *val) { 

	int index = getgindex(key);
	char *tp = (char*) gh;

	float scale = *( (float*) (tp + ghdr[0].offs) );
	*val = *( (float*) (tp + ghdr[index].offs) );
	*val = *val / scale;
}

/* put header value from key */
void putgval(ghed *gh, char *key, float val) { 

	int index = getgindex(key);
	char *tp = (char*) gh;
	float scale = *( (float*) (tp + ghdr[0].offs) );
	if ( -1 == (index))
                err("%s: key word not in ghdr.h: '%s'", __FILE__, key);


	if(scale==0.) scale = 1.;
	*( (float*) (tp + ghdr[index].offs) ) = val * scale;
}

/* get grid header index by key */

int getgindex(char *key)        /* get index for this key */
{
        register int i;

        for (i = 0; i < NGHDRS; i++)
                if (STREQ(ghdr[i].key, key))
                        return i;       /* key found */

        /* not found */
        return -1;
}

/* get key from grid header index  */
char *getgkey(int index)
{
        return (index < NGHDRS && index >= 0) ? ghdr[index].key : NULL;
}

/* get grid header value type from key */
char *ghdtype(char *key)
{
        int index = getgindex(key);

        if (-1 == (index))
                err("%s: key word not in ghdr.h: '%s'", __FILE__, key);

        return ghdr[index].type;
}


/* find min and max absolute values of a short int array */
void sminmax(short *sa, int n, float *gmin, float *gmax) {
	int i;
	float norm;
	norm = (float) sa[0];
	*gmin = *gmax = norm;
	for(i=0;i<n;i++) {
		norm  = (float) sa[i];
		if(*gmin>norm) *gmin  = norm;
		if(*gmax<norm) *gmax  = norm;
	}
}

/* find min and max values of a float array */
void fminmax(float *fa, int n, float *gmin, float *gmax) {
	int i;
	*gmin = *gmax = fa[0];
	for(i=0;i<n;i++) {
		if(*gmin>fa[i]) *gmin  = fa[i];
		if(*gmax<fa[i]) *gmax  = fa[i];
	}
}

/* find min and max absolute values of a complex array */
void cminmax(complex *ca, int n, float *gmin, float *gmax) {
	int i;
	float norm;
	*gmin = *gmax = sqrt(ca[0].r*ca[0].r+ca[0].i*ca[0].i);
	for(i=0;i<n;i++) {
		norm  = sqrt(ca[i].r*ca[i].r+ca[i].i*ca[i].i);
		if(*gmin>norm) *gmin  = norm;
		if(*gmax<norm) *gmax  = norm;
	}
}

