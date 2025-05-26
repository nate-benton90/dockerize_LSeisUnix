static void intdiff(int *a,int na,int *b,int nb,
	     int *ints,int *ns,int *difa,int *nda,int *difb,int *ndb)
/* find the intersection and diffrence of two integer arrays */
/* a array one, na its dimension */
/* b array two, nb its dimension */
/* ints array of intersection, ns number of intersecting values */
/* difa array of a difference, nda number of differing values */
/* difb array of b difference, ndb number of differing values */
{
        
	int noint=0,is=0;
	
	/* initialize */
        *ns=*nda=*ndb=0;
	
	{ register int ia,ib;
	
		/* find intersect and diff a */
		for(ia=0;ia<na;ia++) {
			ib=0; noint=0;
			while(a[ia]!=b[ib]) {
				ib++;
				if(ib>=nb) { /* no intersect */
					difa[*nda]=a[ia];
					*nda+=1;
					noint=1;
					break;
				}				
			}
			if(noint==0) { /* intersect */
				ints[*ns]=a[ia];
				*ns+=1;
			}
		}
		/* find diff b */
		for(ib=0;ib<nb;ib++) {
			is=0;
			while(ints[is]!=b[ib]) {
				is++;
				if(is>=*ns) { /* no intersect */
					difb[*ndb]=b[ib];
					*ndb+=1;
					break;
				}				
			}
		}				
	}		
}
