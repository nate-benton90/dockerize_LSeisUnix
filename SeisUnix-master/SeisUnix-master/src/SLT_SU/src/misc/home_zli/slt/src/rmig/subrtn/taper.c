#include "rmig.h"

/* amplitude tapering arount predicted arrival times */
/* 
   input:
	nt		number of samples of trace
	t0		time of first sample (in s)
	dt 		sampling interval (in s)
	trace[nt] 	trace to be tapered 
	nray		number of rays computed at this trace location 
	tray[nray]	arrival times of the rays  (in s)
	lpass 		length of passing zone at each side of arrival time
			(in s)
	ltaper 		length of tapering zone at each side of passing zones
			(in s)
   output:
	trace[nt]       tapered trace output

   author:
	zhiming li	      		3/4/93
*/

void taper(int nt, float t0, float dt, float *trace, int nray, float *tray,
	float lpass, float ltaper) {

	int *index;
	float *amp, tmp;
	int it;
	int lt, lp;
	int ir, ir1, ir2, ip2, ip1, it2, it1;
	int i1,i2;

	if(lpass>0.5*nt*dt) return;

	tmp = lpass/dt + 0.5;
	lp = tmp;
	index = (int*) malloc(nray*sizeof(int));

	if(ltaper>=0.) {
		tmp = ltaper/dt + 0.5;
		lt = tmp;
		amp = (float*) malloc(lt*sizeof(float));
		tmp = 0.;
		if(lt>0.) tmp = 1./lt;
		for(it=0;it<lt;it++) {
			amp[it] = (lt - it)*tmp;
		}
	}

	for(ir=0;ir<nray;ir++) index[ir] = ir;
       	qkisort(nray,tray,index);
	
/* before first arrival time */
	tmp = (tray[index[0]]-t0)/dt + 0.5;
	ir2 = tmp;
	ip2 = ir2 -lp;
	it2 = ip2 - lt;
	ir1 = 0;
	it1 = 0;
	ip1 = 0;

	if(ip1<ip2) {
		if(ltaper>=0.) {
			if(it1>ip2) it1 = ip2;
			i1 = (ip1>=0)?ip1:0;
			i2 = (it1<nt)?it1:nt-1;
			for(it=i1;it<i2;it++) {
				trace[it] = trace[it]*amp[it-ip1];
			}
			if(it2<ip1) it2 = ip1;
			i1 = (it2>=0)?it2:0;
			i2 = (ip2<nt)?ip2:nt-1;
			for(it=i1;it<i2;it++) {
				trace[it] = trace[it]*amp[ip2-it-1];
			}
			i1 = (it1>=0)?it1:0;
			i2 = (it2<nt)?it2:nt-1;
			for(it=i1;it<i2;it++) {
				trace[it] = 0.;
			}
		}else{
			it2 = 0;
			tmp = ip2 - it2;
			if(tmp!=0.) tmp = 1./tmp;
			i1 = (it2>=0)?it2:0;
			i2 = (ip2<nt)?ip2:nt-1;
			for(it=i1;it<i2;it++) {
				trace[it] = trace[it]*(it-it2+1)*tmp;
			}
		}
	}

/* between first and last arrival times */

        for(ir=0;ir<nray-1;ir++) {
		tmp = (tray[index[ir]]-t0)/dt + 0.5;
		ir1 = tmp;
		ip1 = ir1 + lp;
		it1 = ip1 + lt;
		tmp = (tray[index[ir+1]]-t0)/dt + 0.5;
		ir2 = tmp;
		ip2 = ir2 - lp;
		it2 = ip2 - lt;
		if(ip1>=ip2) continue;
		if(ltaper>=0.) {
			if(it1>ip2) it1 = ip2;
			i1 = (ip1>=0)?ip1:0;
			i2 = (it1<nt)?it1:nt-1;
			for(it=i1;it<i2;it++) {
				trace[it] = trace[it]*amp[it-ip1];
			}
			if(it2<ip1) it2 = ip1;
			i1 = (it2>=0)?it2:0;
			i2 = (ip2<nt)?ip2:nt-1;
			for(it=i1;it<i2;it++) {
				trace[it] = trace[it]*amp[ip2-it-1];
			}
			i1 = (it1>=0)?it1:0;
			i2 = (it2<nt)?it2:nt-1;
			for(it=i1;it<i2;it++) {
				trace[it] = 0.;
			}
		} else {
			it1 = (ip1 + ip2)/2;
			it2 = it1+1;
			tmp = it1-ip1;
			if(tmp!=0.) tmp = 1./tmp;
			i1 = (ip1>=0)?ip1:0;
			i2 = (it1<nt)?it1:nt-1;
			for(it=i1;it<i2;it++) {
				trace[it] = trace[it]*(it1-it)*tmp;
			}
			tmp = ip2 - it2;
			if(tmp!=0.) tmp = 1./tmp;
			i1 = (it2>=0)?it2:0;
			i2 = (ip2<nt)?ip2:nt-1;
			for(it=i1;it<i2;it++) {
				trace[it] = trace[it]*(it-it2+1)*tmp;
			}
		}
    	}

/* after last arrival times */ 
	tmp = (tray[index[nray-1]]-t0)/dt + 0.5;
	ir1 = tmp;
	ip1 = ir1 + lp;
	it1 = ip1 + lt;
	ir2 = nt - 1;
	it2 = nt - 1;
	ip2 = nt - 1;

	if(ip1<ip2) {
		if(ltaper>=0.) {
			if(it1>ip2) it1 = ip2;
			i1 = (ip1>=0)?ip1:0;
			i2 = (it1<nt)?it1:nt-1;
			for(it=i1;it<i2;it++) {
				trace[it] = trace[it]*amp[it-ip1];
			}
			if(it2<ip1) it2 = ip1;
			i1 = (it2>=0)?it2:0;
			i2 = (ip2<nt)?ip2:nt-1;
			for(it=i1;it<i2;it++) {
				trace[it] = trace[it]*amp[ip2-it-1];
			}
			i1 = (it1>=0)?it1:0;
			i2 = (it2<nt)?it2:nt-1;
			for(it=i1;it<i2;it++) {
				trace[it] = 0.;
			}
		}else{	
			it1 = nt-1;
			tmp = it1 - ip1; 
			if(tmp!=0.) tmp = 1./tmp; 
			i1 = (ip1>=0)?ip1:0;
			i2 = (it1<nt)?it1:nt-1;
			for(it=i1;it<i2;it++) {
				trace[it] = trace[it]*tmp*(it1-it);
			}
			
		}
	}

        free(index);
	if(ltaper>=0.) free(amp);

}
