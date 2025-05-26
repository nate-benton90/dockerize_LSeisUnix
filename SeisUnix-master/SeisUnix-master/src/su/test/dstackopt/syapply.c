#include <math.h>
#include <stdio.h>
#include <segy.h>
#define APSIZE 65535
char *sdoc = "syapply <stdin >stdout\n";
int xargc; char **xargv;
main(ac,av)
int ac; char **av;
{
	int nt,app,aph,apq,aplast,aptime,id,nt2,arg1,arg2,arg3;
	float d,h[2];
	struct segy tr;

	if(!gettr_(&tr)) err("cant read first trace\n");
	nt = tr.ns;
	nt2 = nt + nt;

	xargc=ac; xargv=av;

	app = nt;
	aph = app + nt2;
	apq = aph + 2;
	aplast = apq + nt;
	if(aplast>APSIZE) err("can't fit in the ap: aplast=%d nt=%d\n",aplast,nt);

	apclr_();
	arg2=2;
	clrch_(&nt,&nt2);
	do {
		apput_(tr.data,&app,&nt,&arg2);
		id = tr.sstat + tr.gstat + tr.srstat + tr.grstat;
		tr.tstat += id; /* update applied static */
		d = ((float) id) / ((float) tr.dt);
		id = (int)d;
		h[0] = d - id;
		if(h[0]<0.0) {
			++h[0];
			--id;
		}
		h[1] = 1. - h[0];
/* 		fprintf(stderr,"d=%f id=%d h=%f\n",d,id,h[0]); */
		apput_(h,&aph,&arg2,&arg2);
		apwr_(); apwd_();
		arg1=app+id+1; apshft_(&nt,&arg1,&aph,&apq);
		apwr_();
		apget_(tr.data,&apq,&nt,&arg2);
		apwd_();
		puttr_(&tr);
	} while(gettr_(&tr));
	exit(0);
}
