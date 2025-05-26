h18069
s 00000/00000/00029
d D 1.4 88/11/15 14:01:34 shuki 4 3
c 
e
s 00006/00002/00023
d D 1.3 88/06/22 09:44:09 shuki 3 2
c 
e
s 00001/00001/00024
d D 1.2 88/05/15 07:58:20 shuki 2 1
c 
e
s 00025/00000/00000
d D 1.1 88/04/14 13:47:40 shuki 1 0
c date and time created 88/04/14 13:47:40 by shuki
e
u
U
f e 0
t
T
I 3
static char *SccsId="%W% %G%\n";
E 3
I 1
float getclip(data,n)
int n;
float *data;
{
	int k;
	float clip,qclip,*a,dclip=1.0;

	if(!fgetpar("clip",&clip)) {
D 2
		if(!fgetpar("qclip",&qclip))	qclip = 100.0;
E 2
I 2
		if(!fgetpar("qclip",&qclip))	qclip = 98.0;
E 2
		k = qclip/100.0*n - 0.5;
D 3
		if( (k<=0) || (k>n-1) ) err(__FILE__,__LINE__,"bad qclip=%d\n",qclip);
E 3
I 3
		if( (k<=0) || (k>n-1) )
			err(__FILE__,__LINE__,"bad k=%d (n=%d qclip=%f)\n",k,n,qclip);
E 3
		a = (float*) malloc(n*sizeof(float));
		copyabs(data,a,n);
		quant(k,a,n);
		clip = a[k];
		free( (char*)a );
		if(fgetpar("dclip",&dclip)) clip *= dclip;
/* 		warn(__FILE__,__LINE__,"clip=%f from qclip=%f and dclip=%f\n", */
/* 				clip,qclip,dclip); */
D 3
	} else {
E 3
I 3
	}
/* else {
E 3
		warn(__FILE__,__LINE__,"given clip=%f\n",clip);
	}
I 3
*/
E 3
/* 	if( clip == 0.) err(__FILE__,__LINE__,"zero clip=%f\n",clip); */
	return(clip);
}
E 1
