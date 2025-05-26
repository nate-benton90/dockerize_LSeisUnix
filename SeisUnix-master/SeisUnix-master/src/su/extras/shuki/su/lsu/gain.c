gain(data,nt,nx)
float *data;
{
	char *agc,*cliping;
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
	agc = "no"; sgetpar("agc",&agc);
	if(fgetpar("wagc",&wagc) || *agc == 'y' || *agc == 'Y') {
		warn(__FILE__,__LINE__,"wagc=%f\n",wagc);
		doagc(data,nt,nx,wagc);
	}

	/* NORMALIZE AND CLIP */
	cliping = "yes"; sgetpar("cliping",&cliping);
	if(*cliping=='y' || *cliping=='Y') {
		clip = getclip(data,nt*nx);
		balclip(data,nt*nx,clip);
	}
}
