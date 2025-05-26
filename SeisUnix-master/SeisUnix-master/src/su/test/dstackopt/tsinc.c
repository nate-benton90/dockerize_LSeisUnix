#define SMALL 0.000001
#define pi 3.14159265
tsinc(p,q,n,d,l,buf)
int n,l; float *p,*q,d,*buf;
/*
 * subroutine to shift trace p[t] by d
 *
 *	input = P[t]; output = Q[t];
 *		Q(t) = P(t+d) =
 *
 *			 t+l		    t+l
 *	sin(pi*d)/pi * { SUM P(k+t)/(d-k) - SUM P(k+t)/(d-k) }
 *		        k=t-l,even	   k=t-l,odd
 */
{
	int k,id,ks,ke;
	float h,omh,sind,*bufe,*pps;
	register float *qq,*pp,*qe,*dmk;
	id = (int)d;
	qe = q + n;
	qe = q + n;
	h  = fabs(d-id);
	omh = 1. - h;
	if( h < SMALL) {
		for(qq=q,pp=p+id;qq<qe;qq++,pp++)
			*qq = *pp;
		return(0);
		}
	else if( omh < SMALL) {
		id++;
		for(qq=q,pp=p+id;qq<qe;qq++,pp++)
			*qq = *pp;
		return(0);
		}
	ks = id - l + 1;
	ke = id + l + 1;
	bufe = buf + 2*l;
	for(dmk=buf,k=ks;k<ke;dmk++,k++)
		*dmk = 1./(d-k);
	for(qq=q,pps=p+ks;qq<qe;qq++,pps++) {
		for(pp=pps,dmk=buf;dmk<bufe;pp++,dmk++) {
			*qq -= (*(pp++))*(*(dmk++));
			*qq += *pp*(*dmk);
			}
		}
	sind = (sin(pi*d))/pi;
	if( (id-l)%2 ) sind = -sind;
	for(qq=q;qq<qe;qq++)
		*qq *= sind;
	return(1);
}
