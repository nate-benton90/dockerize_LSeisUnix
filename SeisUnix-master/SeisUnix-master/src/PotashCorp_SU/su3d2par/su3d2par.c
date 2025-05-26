#include "su.h"
#include "segy.h"
#include "header.h"

/*********************** self documentation **********************/
char *sdoc[] = {
"                                                               ",
" SU3D2PAR   - convert su trace to su or bin parfile            ",
"                                                               ",
" su3d2par  <stdin    >stdout              		        ",
"                                                               ",
" Required parameters:						",
"                                                               ",
"      n2=            size of the cube in direction 2           ",
"      n3=            size of the cube in direction 3           ",
"      dir=2	     direction to slice the cube                ",
"                    first and list line of the direction that  ",
"      lins=1	     goes to the output                         ",
"      line=n2	       			                        ",
"                                                               ",
"                                                               ",
"      Note: 	      direction 1 is equal to tr.ns             ",
"                                                               ",
" Optional parameters:                                          ",
"      key2=fldr   index of direction 2                      ",
"      key3=tracf  index of direction 3                      ",
"      su=1        su format output				",
"		   su=0 stripped binary: cdps, tnmos,vnmos/per cdp",
NULL};

segy tr;

int
main(int argc, char **argv)
{
	
	float **data;
        char **hdrdata;
	float ***workm;
	
        int ival2;              /* int value of key2                */
        int ival3;              /* int value of key3                */
        Value val2;      	/* ... its value                        */
        Value val3;      	/* ... its value                        */
	int index2;
	int index3;
	
	int n1,n2,n3;
	int i1,i2,i3;
        char *key2=NULL;      /* header key word from segy.h    */
        char *type2=NULL;     /* ... its type                   */
        char *key3=NULL;      /* header key word from segy.h    */
        char *type3=NULL;     /* ... its type                   */
	
	int lins;
	int line;
	int nl;
	int dir=2;
	int il;
	int su;
	float cdp;
	float t;
	float dt;
	
        /* Initialize */
        initargs(argc, argv);
        requestdoc(1);
       	
       MUSTGETPARINT("n2", &n2);
       MUSTGETPARINT("n3", &n3);
       
       /* get key2*/
        if (!getparstring("key2", &key2))  key2 = "fldr"; 
        type2 = hdtype(key2);
        index2 = getindex(key2);
	
       /* get key3*/
        if (!getparstring("key3", &key3))  key3 = "tracf"; 
        type3 = hdtype(key3);
        index3 = getindex(key3);

        if (!getparint("lins", &lins))  lins = 1;
	if(lins<1) err(" lins must be larger than 0");
        if (!getparint("line", &line)) line = n2; 
         if (!getparint("su", &su)) su = 1; 
      
	/* Get info from first trace */
        if (!gettr(&tr))  err ("can't get first trace");
        n1 = tr.ns;
	
	if (!getparfloat("dt", &dt))	dt = ((float) tr.dt)/1000000.0;
	if (!dt) {
		dt = .01;
		warn("dt not set, assumed to be .01");
	}



	data = bmalloc(n1*sizeof(float),n2,n3);
		
	hdrdata = bmalloc(HDRBYTES,n2,n3);
	
        do {
		gethval(&tr, index2, &val2);
		ival2 = vtoi(type2,val2);
		gethval(&tr, index3, &val3);
		ival3 = vtoi(type3,val3);

		if(ival2>n2 || ival2<0 ) err(" Array in dimension 2 out of bound\n");
		if(ival3>n3 || ival3<0 ) err(" Array in dimension 3 out of bound\n");
		
		
      		bmwrite(data,1,ival2-1,ival3-1,1,tr.data);
		bmwrite(hdrdata,1,ival2-1,ival3-1,1,&tr);
		
	} while (gettr(&tr));
	
	
	
	nl=line-lins+1;
	if(dir==2) {
		/* n2 direction */
		workm = alloc3float(n1,n3,nl);
		for(il=lins-1;il<nl;il++) {
			for(i3=0;i3<n3;i3++) bmread(data,1,lins+il-1,i3,1,workm[il][i3]);
		}
		
		if(su==1) {	
			fprintf(stdout,"cdp=");
			for(il=lins-1;il<nl;il++) {
				for(i3=0;i3<n3;i3++) fprintf(stdout,"%d,",(lins+il)*1000+i3+1);
			}
		
			for(il=lins-1;il<nl;il++) {
				for(i3=0;i3<n3;i3++){
					fprintf(stdout,"\ntnmo=");
					for(i1=0;i1<n1;i1++) fprintf(stdout,"%.3f,",dt*i1);
					fprintf(stdout,"\nvnmo=");
					for(i1=0;i1<n1;i1++)
						fprintf(stdout,"%.3f,",workm[il][i3][i1]);
				}
		
			}
		} else {
			for(il=lins-1;il<nl;il++) {
				for(i3=0;i3<n3;i3++) {
					 cdp=(lins+il)*1000+i3+1;
					 fwrite(&cdp,sizeof(float),1,stdout);
				}
			}
		
			for(il=lins-1;il<nl;il++) {
				for(i3=0;i3<n3;i3++){
					for(i1=0;i1<n1;i1++) {
						t=tr.d1*i1;
						fwrite(&t,sizeof(float),1,stdout);
					}
					for(i1=0;i1<n1;i1++)
						fwrite(&workm[il][i3][i1],sizeof(float),1,stdout);
				}
		
			}
		}
			free3float(workm);	
	}
	
	bmfree(data);
	bmfree(hdrdata);	
	return EXIT_SUCCESS;
}
