h16849
s 00003/00003/00036
d D 1.2 88/11/15 14:01:33 shuki 2 1
c 
e
s 00039/00000/00000
d D 1.1 88/04/14 13:47:39 shuki 1 0
c date and time created 88/04/14 13:47:39 by shuki
e
u
U
f e 0
t
T
I 1
gain(data,nt,nx)
float *data;
{
D 2
	char *agc="no             ",*cliping="yes         ";
E 2
I 2
	char *agc,*cliping;
E 2
	float tpow,epow,gpow,clip,getclip(),wagc;

	/* TPOW */
	if(fgetpar("tpow",&tpow)) {
		warn(__FILE__,__LINE__,"tpow=%f\n",tpow);
		dotpow(data,nt,nx,tpow);
	}

	/* EPOW */
	if(fgetpar("epow",&epow)) {
		warn(__FILE__,__LINE__,"epow=%f\n",epow);
		doepow(data,nt,nx,epow);
	}

	/* GPOW */
	if(fgetpar("gpow",&gpow)) {
		warn(__FILE__,__LINE__,"gpow=%f\n",gpow);
		dogpow(data,nt*nx,gpow);
	}

	/* AGC */
	wagc = 40.0;
D 2
	sgetpar("agc",agc);
E 2
I 2
	agc = "no"; sgetpar("agc",&agc);
E 2
	if(fgetpar("wagc",&wagc) || *agc == 'y' || *agc == 'Y') {
		warn(__FILE__,__LINE__,"wagc=%f\n",wagc);
		doagc(data,nt,nx,wagc);
	}

	/* NORMALIZE AND CLIP */
D 2
	sgetpar("cliping",cliping);
E 2
I 2
	cliping = "yes"; sgetpar("cliping",&cliping);
E 2
	if(*cliping=='y' || *cliping=='Y') {
		clip = getclip(data,nt*nx);
		balclip(data,nt*nx,clip);
	}
}
E 1
