static char *SccsId="@(#)getclip.c	1.4 11/15/88\n";
float getclip(data,n)
int n;
float *data;
{
	int k;
	float clip,qclip,*a,dclip=1.0;

	if(!fgetpar("clip",&clip)) {
		if(!fgetpar("qclip",&qclip))	qclip = 98.0;
		k = qclip/100.0*n - 0.5;
		if( (k<=0) || (k>n-1) )
			err(__FILE__,__LINE__,"bad k=%d (n=%d qclip=%f)\n",k,n,qclip);
		a = (float*) malloc(n*sizeof(float));
		copyabs(data,a,n);
		quant(k,a,n);
		clip = a[k];
		free( (char*)a );
		if(fgetpar("dclip",&dclip)) clip *= dclip;
/* 		warn(__FILE__,__LINE__,"clip=%f from qclip=%f and dclip=%f\n", */
/* 				clip,qclip,dclip); */
	}
/* else {
		warn(__FILE__,__LINE__,"given clip=%f\n",clip);
	}
*/
/* 	if( clip == 0.) err(__FILE__,__LINE__,"zero clip=%f\n",clip); */
	return(clip);
}
