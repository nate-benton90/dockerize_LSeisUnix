
void vconint(float *depth, float *vel, int npairs, float *z, float *vtable,
	     int nz, int invtyp, int otvtyp, float *dvdz) { 
/* velocity conversion and interpolation */
/* input:
    	depth[npairs]	----	depth of picks
	vel[npairs]     ----	velocity picks
	npairs          ----    number of depth-vel pairs
        z[nz]           ----    output depth positions
        nz              ----    number of output depth positions
        invtyp          ----    type of input velocity picks
				(0=average velocity; 1=interval velocity)
	otvtyp		----    type of output velocity picks 
				(0=average velocity; 1=interval velocity)
	dvdz[nz]	----    interval velocity gradient at z
   output:
        vtable[nz]      ----    velocity at output z

   author: 
	Zhiming Li		      		10/91 
*/
  
	float *vi;
	int *indx,iz,ip,nn,i1,i2, ii, nzmax;
	float z1,z2,v1,v2,gra; 

	nzmax = nz;
	if(nzmax<npairs) nzmax = npairs;

	indx = (int *) malloc(nzmax*sizeof(int));
	vi = (float *) malloc(nzmax*sizeof(float));

	/* compute interval velocity at z */
	/* first compute interval velocity at depth */
	if(invtyp==0) {
		/* input vel is average velocity */
		for(ip=0;ip<npairs;ip++) {
			z2 = depth[ip];
			v2 = vel[ip];
			if(ip==0) {
				z1 = 0.;
				v1 = v2;
			} else {
				z1 = depth[ip-1];
				v1 = vel[ip-1];
			} 
			/* determine gradient */
			if(depth[ip]< z[0]) {
				gra = 0.;
			} else if (depth[ip]>z[nz-1]) {
				gra = 0.;
			} else {
				nn = 1;
           			bisear_(&nz,&nn,z,&z1,&i1);
           			bisear_(&nz,&nn,z,&z2,&i2);
				gra = 0.;
				for(ii=i1-1;ii<i2;ii++) gra += dvdz[ii];
				if(i2>i1) gra = gra/(i2-i1+1);
			}
			/* determine velocity at depth[ip] */
			if(z2>z1) {
				vi[ip]=(z2*v2-z1*v1)/(z2-z1) + 0.5*gra*(z2-z1);
			} else {
				vi[ip] = v2;
			}
		} 
	} else {
		/* input vel is interval velocity already*/
		for(ip=0;ip<npairs;ip++) vi[ip] = vel[ip];
	}
	/* now detemine interval velocity at z */
       	bisear_(&npairs,&nz,depth,z,indx);
	for(iz=0;iz<nz;iz++) {
		if(z[iz]<depth[0]) {
			vtable[iz] = vi[0];
		} else if(z[iz]>=depth[npairs-1]) {
			vtable[iz] = vi[npairs-1];
		} else { 
			vtable[iz] = vi[indx[iz]] + 
				dvdz[iz]*(z[iz]-depth[indx[iz]]);
		}
	}
	/* convert to average velocity if needed */
     	if(otvtyp==0) {
		vi[0] = vtable[0]*z[0];
		for(iz=1;iz<nz;iz++) vi[iz] = vi[iz-1] + 
						(z[iz]-z[iz-1])*vtable[iz];
		for(iz=1;iz<nz;iz++) vtable[iz]=vi[iz]/z[iz];
	}
	free(indx);
	free(vi);
}


void dvdzint(float *ztop, float *zbot, float *dvdz, int npairs, float *z, 
	  	float *dvdztable, int nz) { 
/* velocity gradient interpolation */
/* input:
    	ztop[npairs]	----	depths of the tops of the gradient zones 
    	zbot[npairs]	----	depths of the bottoms of the gradient zones 
	dvdz[npairs]     ----	velocity gradients at [ztop,zbot] 
	npairs          ----    number of gradient inputs 
        z[nz]           ----    output depth positions
        nz              ----    number of output depth positions
    output:
	dvdztable[nz]	----    interval velocity gradient at z

    author: 
	Zhiming Li		      		10/91 
*/
	int iz, ip;
	float gra;
	if(npairs<1) {
		for(iz=0;iz<nz;iz++) dvdztable[iz] = 0.;
		return;
	}
	for(iz=0;iz<nz;iz++) {
		gra = 0.;
		for(ip=0;ip<npairs;ip++) {
			if(z[iz]>ztop[ip] && z[iz]<=zbot[ip]) {
				gra=dvdz[ip];
				break;
			}
		}
		dvdztable[iz] = gra;
	}
}

/* find depth zz and average velocity picks with ztop and zbot */
void zandva(float ztop, float zbot, float *zp, float *vp, int np,
        float *zz, float *vaa, int *nn) {

/* input:
	ztop		top of layer
	zbot		bottom of layer
	zp		depths of velocity picks		
	vp		velocities picked
	np		number of velocities picked
   output:
	zz		depths of velocity picks within the layer
	vaa		velocity picks whose depths are within the layer
	nn		number of velocity picks whose depths are within 
			the layer
   author:		Zhiming Li		      		10/91 
*/
	

        int iz, izt, izb;

        izb = 0;
        izt = 0;
        if ( ztop <= zp[0] ) {
                izt = 0;
        } else if ( ztop > zp[np-1] ) {
                *nn = 0;
                return;
        } else {
                for(iz=1;iz<np;iz++) {
                        if(zp[iz]>=ztop && zp[iz-1]<ztop) {
                                izt = iz;
                                break;
                        }
                }
        }
        if ( zbot >= zp[np-1] ) {
                izb = np-1;
        } else if ( zbot < zp[0] ) {
                *nn = 0;
                return;
        } else {
                for(iz=np-2;iz>=0;iz--) {
                        if(zp[iz]<=zbot && zp[iz+1]>zbot) {
                                izb = iz;
                                break;
                        }
                }
        }

        for(iz=izt;iz<=izb;iz++) {
                zz[iz-izt] = zp[iz];
                vaa[iz-izt] = vp[iz];
        }
        *nn = izb - izt + 1;
}

