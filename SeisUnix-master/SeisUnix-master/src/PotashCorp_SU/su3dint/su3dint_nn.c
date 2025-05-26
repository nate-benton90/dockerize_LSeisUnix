#include "suhdr.h"

/*********************** self documentation **********************/
char *sdoc[] = {
"                                                               ",
" SU3DINT   - interpolation of velocity values in a 3d cube     ",
"                                                               ",
" su3dint  <stdin    >stdout              		        ",
"                                                               ",
" Required parameters:						",
"                                                               ",
"      n2=            size of the cube in direction 2           ",
"      n3=            size of the cibe in direction 3           ",
"      fs2=1	      first sample in direction 2               ",
"      fs3=1          first sample in direction 3               ",
"      p=1.0	      distance power weight                     ",
"      r=500	      search radius in coord units              ",
"      Note: 	      direction 1 is equal to tr.ns             ",
"                                                               ",
" Optional parameters:                                          ",
"      key2=fldr   index of direction 2                      ",
"      key3=tracf  index of direction 3                      ",
"								",
NULL};

segy tr;

void nnwi(float **data,int nx,int ny,float dx,float dy,float p,float r);

int
main(int argc, char **argv)
{
	
	float **data;
        char **hdrdata;
	float **workm;
	float *tmpa;
	
        int ival2;              /* int value of key2                */
        int ival3;              /* int value of key3                */
        Value val2;      	/* ... its value                        */
        Value val3;      	/* ... its value                        */
	int index2;
	int index3;
	
	int n1,n2,n3;
	int fs2,fs3;
	int i1,i2,i3;
	float d2,d3;
	float p;
	float r;
        char *key2=NULL;      /* header key word from segy.h    */
        char *type2=NULL;     /* ... its type                   */
        char *key3=NULL;      /* header key word from segy.h    */
        char *type3=NULL;     /* ... its type                   */
	
        /* Initialize */
        initargs(argc, argv);
        requestdoc(1);
       	
       MUSTGETPARINT("n2", &n2);
       MUSTGETPARINT("n3", &n3);
       if (!getparint("fs2", &fs2))   fs2 = 1; 
       if (!getparint("fs3", &fs3))   fs3 = 1; 
       if (!getparfloat("d2", &d2))  d2 = 20.0; 
       if (!getparfloat("d3", &d3))  d3 = 20.0; 
       if (!getparfloat("p", &p))    p = 1.0; 
       if (!getparfloat("r", &r))    r = 500.0; 
       
       /* get key2*/
        if (!getparstring("key2", &key2))  key2 = "fldr"; 
        type2 = hdtype(key2);
        index2 = getindex(key2);
	
       /* get key3*/
        if (!getparstring("key3", &key3))  key3 = "tracf"; 
        type3 = hdtype(key3);
        index3 = getindex(key3);
        
	/* Get info from first trace */
        if (!gettr(&tr))  err ("can't get first trace");
        n1 = tr.ns;

	data = bmalloc(n1*sizeof(float),n2,n3);
		
	hdrdata = bmalloc(HDRBYTES,n2,n3);
	
/*	for(i1=0;i1<n1;i1++) memset( (void *) data[i1], (int) '\0', n3*n2*FSIZE);
        for(i3=0;i3<n3;i3++) memset( (void *) &hdrdata[i3], (int) '\0', HDRBYTES*n2);

*/
        do {
		gethval(&tr, index2, &val2);
		ival2 = vtoi(type2,val2);
		gethval(&tr, index3, &val3);
		ival3 = vtoi(type3,val3);

		if(ival2>n2+fs2 || ival2<fs2 ) err(" Array in dimension 2 out of bound\n");
		if(ival3>n3+fs3 || ival3<fs3 ) err(" Array in dimension 3 out of bound\n");
		
		
      		bmwrite(data,1,ival2-fs2,ival3-fs3,1,tr.data);
		bmwrite(hdrdata,1,ival2-fs2,ival3-fs3,1,&tr);
		
	} while (gettr(&tr));
	
	
	/* INTERPOLATION on 2D slices */
	workm = alloc2float(n3,n2);
	tmpa = ealloc1float(n1);
	{int isl;
		for(isl=0;isl<n1;isl++) {
			for(i2=0;i2<n2;i2++) {
				for(i3=0;i3<n3;i3++) {
      					bmread(data,1,i2,i3,1,tmpa);
					workm[i2][i3]=tmpa[isl];
				}
			}
			nnwi(workm,n2,n3,d2,d3,p,r);
			for(i2=0;i2<n2;i2++) {
				for(i3=0;i3<n3;i3++) {
      					bmread(data,1,i2,i3,1,tmpa);
					tmpa[isl]=workm[i2][i3];
      					bmwrite(data,1,i2,i3,1,tmpa);
				}
			}
			fprintf(stderr," Timeslice = %d\n",isl);
		}
	}
	free2float(workm);
	free1float(tmpa);
	
	for(i2=0;i2<n2;i2++) {
		for(i3=0;i3<n3;i3++) {
      			bmread(data,1,i2,i3,1,tr.data);
			bmread(hdrdata,1,i2,i3,1,&tr);			
			tr.ns=n1;
			puttr(&tr);
		}
		
	}
	
	bmfree(data);
	bmfree(hdrdata);	
	return EXIT_SUCCESS;
}

void nnwi(float **data,int nx,int ny,float dx,float dy,float p,float r)
{

	int ix;
	int iy;
	int n=0;
	int in;
	float *x;
	float *y;
	float *z;
	float maxr=0.0;
	
	
	/* find the number of non zero values in the data matrix */
	for(ix=0;ix<nx;ix++) {
		for(iy=0;iy<ny;iy++) {
			if(data[ix][iy]!=0.0) n++;
		}
	}
	
	if(n==0) return;
	
	x=ealloc1float(n);
	y=ealloc1float(n);
	z=ealloc1float(n);
	
	in=0;
	for(ix=0;ix<nx;ix++) {
		for(iy=0;iy<ny;iy++) {
			if(data[ix][iy]!=0.0) {
				x[in]=ix*dx;
				y[in]=iy*dy;
				z[in]=data[ix][iy];
				in++;
			}	
		}
	}
	
	maxr=MAX((nx-1)*dx/2.0,(ny-1)*dy/2.0);

	nnw2di(n,x,y,z,nx,ny,0.0,0.0,dx,dy,data,1.0,maxr);
	nnw2di(n,x,y,z,nx,ny,0.0,0.0,dx,dy,data,p,r);
	
	free1float(x);
	free1float(y);
	free1float(z);
}	
