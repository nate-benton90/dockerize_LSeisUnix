

	subroutine t2z(

	real t(n1),z(n1) 
	real zo(nz),to(nz)

	
	z(1) = t(1)*v(1)*0.0005;
	do it=2,n1
		z(it) = z(it-1) + d1*v(it)*0.0005;
	end do

	odt = 1./dt
	call lin1d(z,t,n1,zo,to,nz,indx,d)
	do iz=1,nz
		if(
	end do
	if(


			for(it=0;it<n1;it++) t[it] = o1 + it*d1;
			for(it=1;it<n1;it++) 
			odt = 1./dt;
			for(iz=0;iz<nz;iz++) {
				zo = fz + iz*dz;
				if(zo<z[0]) {
					ti = zo*t[0]/z[0]; 
				} else if(zo>=z[n1-1]) {
					ti = z[n1-1]+(zo-z[n1-1])/v[n1-1]*2000.;
				} else {
					lin1d_(xin,yin,nin,xout,yout,nout,indx);
				}
					ti = (ti-ft)*odt;

					iti = ti;
					res = ti-iti;
					if(iti>=0


	if(torz.eq.0) then


		if(torz==0) {
			}
		} else {
			for(iz=0;iz<n1;iz++) z[iz] = o1 + iz*d1;
			t[0] = z[0]/v[0]*2000.;
			for(iz=1;iz<n1;iz++) 
				z[iz] = z[iz-1] + d1/v[iz]*2000.;
		}
		
			
		/* time-depth convertion */
		tzconv(torz,f1,dt,n1,v,ft,dt,nt,tr.data,fz,dz,nz,tro.data);
		tro.ns = nz;
		if(torz==0) {
			tro.dz = dz;
			tro.fz = fz;
			tro.dt = dz*1000.;
			tro.delrt = fz * 1000.;
		} else {
			tro.dt = dz*1000.;
			tro.delrt = fz * 1000.;
			tro.dz = 0.;
			tro.fz = 0.;
		} 
		fputtr(outfp,&tro);

	} while(!fgettr(infp,&tr));

	return 0;

}

void tzconv(int torz, float f1v, float d1v, int n1v, float *vi,
	float f1i, float d1i, int n1i, float *datai,   
	float f1o, float d1o, int n1o, float *datao)
{

	int i1; 

	
	for(i1=0;i1<
	for (i1=0;i1<no;i1++) {
		xo = f1o + i1*d1o;
		tmp = (xo-f1v)/d1v;
		itmp = tmp;
		res = tmp - itmp;
		if(itmp<0) {
			vio = vi[0];
		} else if(itmp>=n1v-1) {  
			vio = vi[n1v-1];
		} else {
			vio = (1.-res)*vi[itmp] + res*vi[itmp+1];
		}
		tmp = xo - xopre;
		if(torz==0) {
			dxo = tmp*vio;

	}


}   
