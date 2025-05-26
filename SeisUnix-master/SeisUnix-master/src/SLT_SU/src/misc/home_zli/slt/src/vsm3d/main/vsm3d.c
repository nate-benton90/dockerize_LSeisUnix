#include <par.h>
  
void vsm3d(float ***v, int n3, int n2, int n1, int iter, int depth,
         float r3, float r2, float r1, float lambda, int slowness)
/***************************************************************************
Smooth 3d-velocity.  
*************************************************************************/

{
	int  n, i2, i1, i3, i;		
	float **d, **e, **f, *w, ww=1.0;
 
 /*      compute the weight function */
	w = alloc1float(n1+n2+n3-2);
	if(depth==1){
		lambda = (lambda*lambda-1.0)/(n1*n1);
		for(i1=0; i1<n1; ++i1) w[i1] = 1.0/(1+i1*i1*lambda);
	}
	if(depth==2){
 		lambda = (lambda*lambda-1.0)/(n2*n2);
		for(i2=0; i2<n2; ++i2) w[i2] = 1.0/(1+i2*i2*lambda);
	}
	if(depth==3){
 		lambda = (lambda*lambda-1.0)/(n3*n3);
		for(i3=0; i3<n3; ++i3) w[i3] = 1.0/(1+i3*i3*lambda);
	}
 

/*	scale  smoothing parameters according to the iteration number	*/
	if(iter==1) {
		r1 /= 3.39*3.39;
		r2 /= 3.39*3.39;
		r3 /= 3.39*3.39;
	} else if(iter==2){
		r1 /= 5.19*5.19;
		r2 /= 5.19*5.19;
		r3 /= 5.19*5.19;
	} else {
		r1 /= 6.60*6.60;
		r2 /= 6.60*6.60;
		r3 /= 6.60*6.60;
	}


	if(slowness) {
	/*  smoothing on slowness  */
		recipr_(v[0][0],&n1,&n2,&n3);
 	}

      if(r2>0.) {
 
/*      smoothing velocity in the second direction */

	/* allocate space */
	n = n2-1;
 	d = alloc2float(n1,n);
	e = alloc2float(n1,n);
	f = alloc2float(n1,n);
 
 
        for(i3=0; i3<n3; ++i3){
		if(depth==3) ww = w[i3];
	 	for(i2=0; i2<n; ++i2){
			if(depth==2) ww = w[i2+1];
			for(i1=0; i1<n1; ++i1){
				if(depth==1) ww = w[i1];
				d[i2][i1] = ww+r2*2.0;
				e[i2][i1] = -r2;
 				f[i2][i1] = ww*v[i3][i2+1][i1];
			}
		}
       		for(i1=0; i1<n1; ++i1){
          		d[n-1][i1] -= r2;
			f[0][i1] += r2*v[i3][0][i1];
  		}
         	tripd2_(d[0],e[0],f[0],&n,&n1);

	    for(i=1; i<iter; ++i) {
	 	for(i2=0; i2<n; ++i2){
			if(depth==2) ww = w[i2+1];
			for(i1=0; i1<n1; ++i1){
				if(depth==1) ww = w[i1];
				d[i2][i1] = ww+r2*2.0;
				e[i2][i1] = -r2;
 				f[i2][i1] *= ww;
			}
		}
       		for(i1=0; i1<n1; ++i1){
          		d[n-1][i1] -= r2;
			f[0][i1] += r2*v[i3][0][i1];
  		}
         	tripd2_(d[0],e[0],f[0],&n,&n1);
	    }

	 	for(i2=0; i2<n2-1; ++i2)
			for(i1=0; i1<n1; ++i1)
				v[i3][i2+1][i1] = f[i2][i1];
	}
      }
 
      if(r3>0.) {
/*      smooth velocity in  the third  direction */

	/* allocate space */
	n = n3-1;
 	d = alloc2float(n1,n);
	e = alloc2float(n1,n);
	f = alloc2float(n1,n); 
 
        for(i2=0; i2<n2; ++i2){
		if(depth==2) ww = w[i2];
	 	for(i3=0; i3<n; ++i3){
			if(depth==3) ww = w[i3+1];
			for(i1=0; i1<n1; ++i1){
				if(depth==1) ww = w[i1];
				d[i3][i1] = ww+2.*r3;
				e[i3][i1] = -r3;
 				f[i3][i1] = ww*v[i3+1][i2][i1];
			}
 		}
       		for(i1=0; i1<n1; ++i1){
          		d[n-1][i1] -= r3;
			f[0][i1] += r3*v[0][i2][i1];
  		}
         	tripd2_(d[0],e[0],f[0],&n,&n1);

	    for(i=1; i<iter; ++i){
	 	for(i3=0; i3<n; ++i3){
			if(depth==3) ww = w[i3+1];
			for(i1=0; i1<n1; ++i1){
				if(depth==1) ww = w[i1];
				d[i3][i1] = ww+2.*r3;
				e[i3][i1] = -r3;
 				f[i3][i1] *= ww;
			}
 		}
       		for(i1=0; i1<n1; ++i1){
          		d[n-1][i1] -= r3;
			f[0][i1] += r3*v[0][i2][i1];
  		}
         	tripd2_(d[0],e[0],f[0],&n,&n1);
	    }

	 	for(i3=0; i3<n3-1; ++i3)
			for(i1=0; i1<n1; ++i1)
				v[i3+1][i2][i1] = f[i3][i1];
	}
      }
      
      if(r1>0.) {
/*      smooth velocity in  the first direction */

	/* allocate space */
	n = n1-1;
 	d = alloc2float(n2,n);
	e = alloc2float(n2,n);
	f = alloc2float(n2,n);
 
        for(i3=0; i3<n3; ++i3){
		if(depth==3) ww = w[i3];
	 	for(i2=0; i2<n2; ++i2){
			if(depth==2) ww = w[i2];
			for(i1=0; i1<n1-1; ++i1){
				if(depth==1) ww = w[i1+1];
				d[i1][i2] = ww+r1*2.0;
				e[i1][i2] = -r1;
 				f[i1][i2] = ww*v[i3][i2][i1+1];
			}
          		d[n-1][i2] -= r1;
			f[0][i2] += r1*v[i3][i2][0];
		}

         	tripd2_(d[0],e[0],f[0],&n,&n2);

	    for(i=1; i<iter; ++i) {
	 	for(i2=0; i2<n2; ++i2){
			if(depth==2) ww = w[i2];
			for(i1=0; i1<n1-1; ++i1){
				if(depth==1) ww = w[i1+1];
				d[i1][i2] = ww+r1*2.0;
				e[i1][i2] = -r1;
 				f[i1][i2] *= ww;
			}
          		d[n-1][i2] -= r1;
			f[0][i2] += r1*v[i3][i2][0];
		}

         	tripd2_(d[0],e[0],f[0],&n,&n2);
	    }

	 	for(i2=0; i2<n2; ++i2) 
			for(i1=0; i1<n1-1; ++i1)
				v[i3][i2][i1+1] = f[i1][i2];
		 
	}
      }

 	if(slowness) {
		recipr_(v[0][0],&n1,&n2,&n3);
	}
       free1float(w);
	if(r1>0. || r2>0. || r3>0.) {
		free2float(d);
		free2float(e);
		free2float(f);
 	}
 }
        
       
	
	
