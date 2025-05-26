#include <usgrid.h>
#include <grid.h>

char * sdoc = 
"gridmerge - merge grid file	\n"
"\n"
"USAGE:\n"
"gridmerge gridin=file1 gridin=file2 gridin=file3,... [parameters] > newfile\n"
"\n"
"OPTIONAL PARAMETERS \n"
"mergeaxis=5  axis of grid merge (1,2,3,4,5) \n"
"             when mergeaxis=1, the program will read n1 samples \n"
"             from file1,file2,... in sequence and output \n"
"             untill all the records are read	\n"
"             when mergeaxis=2, the program will read n1*n2 samples \n"
"             from file1,file2,... in sequence and output \n"
"             untill all the records are read	\n"
"             when mergeaxis=3, the program will read n1*n2*n3 samples \n"
"             from file1,file2,... in sequence and output \n"
"             untill all the records are read	\n"
"             when mergeaxis=4, the program will read n1*n2*n3*n4 samples \n"
"             from file1,file2,... in sequence and output \n"
"             untill all the records are read	\n"
"             when mergeaxis=5, the program will read n1*n2*n3*n4*n5 samples\n"
"             from file1,file2,... in sequence and output \n"
"             untill all the records are read	\n"
"headupdate=5 grid header updated on axis 5 \n"
"             the n5 parameter will be updated if headupdate=5. \n"
"             when headupdate=5 and mergeaxis=4, both n5 and d5 will \n"
"             be updated \n"
"the following four windowing parameters are used only when mergeaxis=5 \n"
"si3=1        starting INDEX of third dimension to output \n"
"ni3=         ending INDEX of third dimension to output \n"
"             (default to last samples in input)  \n"
"si5=1        starting INDEX of fifth dimension to output \n"
"ni5=         ending INDEX of fifth dimension to output \n"
"             (default to last samples in input)  \n"
"ddupdate=    when specified and headerupdate=i (i=1,2,3,4,5) \n"
"             the output di in the grid header will be updated with \n"
"             this value (e.g., ddupdate=20 headupdate=2 then d2=20).  \n"
"AUTHOR:		Z. Li	2/8/99	\n"
"\n"
;

main(int argc,char *argv[]) {
	int mergeaxis, nin, headupdate;
	int i, nread;
	string *datain;
	FILE *outfp = stdout;
	FILE **infp;
	char *grid, *grido;

	usghed usgh, usgh2;
	int ierr;

	int n1, n2, n3, n4, n5;
	int i1, i2, i3, i4, i5;

	int si3, ni3=0, si5, ni5=0;
	int nn3=1, nn5=1, ii5;
	int i50;
	long long lpos;
	int n1out;

	float ddupdate;
	int iddupdate=1;

	int *m5;
	initargs(argc,argv) ;
	askdoc(1);

		
	if( !getparint("mergeaxis",&mergeaxis) ) mergeaxis=5;
	if( !getparint("headupdate",&headupdate) ) headupdate=5;
	if( !getparfloat("ddupdate",&ddupdate) ) {
		iddupdate = 0;
	}
	file2g(outfp);

	nin = countparname("gridin");
	fprintf(stderr," gridmerge with %d input grid files \n",nin);
	datain = (string *) malloc(nin*sizeof(string));
	infp = (FILE**) malloc(nin*sizeof(FILE *));

 	for(i=0;i<nin;i++) { 
		getnparstring(i+1,"gridin",&datain[i]);
		fprintf(stderr,"   Input %d :  gridin=%s \n",i+1,datain[i]);
		/*
		if((infp[i] = fopen(datain[i],"r"))==NULL)
			err(" %s not found ",datain[i]);
		*/
		infp[i] = fopen(datain[i],"r");
		file2g(infp[i]);
	}

	ierr = fgetusghdr(infp[0],&usgh);
	if(ierr!=0) err(" error open gridin=%s \n",datain[0]);

	grid = (char*)malloc(usgh.n1*usgh.dtype);
	m5 = (int*)malloc(nin*sizeof(int));

	if( !getparint("si3",&si3) ) si3=1;
	if( !getparint("si5",&si5) ) si5=1;
	if( !getparint("ni3",&ni3) ) { ni3=0; nn3=0; }
	if( !getparint("ni5",&ni5) ) { ni5=0; nn5=0; }
	si3 = si3 - 1;
	si5 = si5 - 1;

	if(mergeaxis==5) {
			n5 = 0;
			ii5 = 0;
			for(i=0;i<nin;i++) {
				ierr = fgetusghdr(infp[i],&usgh2);
				if(ierr!=0) err(" error open gridin=%s \n",datain[i]);
				if(usgh.n1*usgh.n2*usgh.n3*usgh.n4 
			 	 !=usgh2.n1*usgh2.n2*usgh2.n3*usgh2.n4)
					err(" check gridin=%s \n",datain[i]);
				n5 += usgh2.n5;
				fseek64(infp[i],0,0);
				nread = usgh2.n5 * usgh2.n4 * usgh2.n3 *usgh2.n2;
				if(nn3==0) ni3 = usgh2.n3;
				if(nn5==0) ni5 += usgh2.n5;
				fprintf(stderr," ii5=%d si5=%d ni5=%d \n",ii5+1,si5+1,ni5);
				fprintf(stderr," si3=%d ni3=%d \n",si3+1,ni3);
				if(ii5+usgh2.n5<=si5) {
					ii5 = ii5 + usgh2.n5; 
					fprintf(stderr," skip gridin=%s ... o5=%g n5=%d d5=%g \n",
						datain[i],usgh2.o5,usgh2.n5,usgh2.d5);
				} else {
					if(ii5>=ni5) {
						ii5 = ii5 + usgh2.n5; 
						break;
					}
					i50 = si5 - ii5;
					if(i50>0) {
						lpos = i50*usgh2.n1*usgh2.n2;
						lpos = lpos*usgh2.n3*usgh2.n4*usgh2.dtype;
						fseek64(infp[i],lpos,0);
						ii5 = si5;
					} else {
						i50 = 0;
					}

					for(i5=i50;i5<usgh2.n5;i5++) {
					for(i4=0;i4<usgh2.n4;i4++) {
					if(si3>=0) {
						lpos = si3+(i4+i5*usgh2.n4)*usgh2.n3;
						lpos = lpos*usgh2.n2*usgh2.n1*usgh2.dtype;
						fseek64(infp[i],lpos,0);
					}
					for(i3=si3;i3<ni3;i3++) {
					for(i2=0;i2<usgh2.n2;i2++) {
						fread(grid,usgh.dtype,usgh.n1,infp[i]);
						fwrite(grid,usgh.dtype,usgh.n1,outfp);
					}
					}
					}
					ii5 = ii5 + 1;
					if(ii5>=ni5 && nn5>0) {
						ii5 = ii5 + (usgh2.n5-i5);
						break;
					}

					}
				fprintf(stderr," merge gridin=%s ... o5=%g n5=%d d5=%g \n",
					datain[i],usgh2.o5,usgh2.n5,usgh2.d5);
				}

			}
			usgh.o3 = si3 * usgh.d3 + usgh.o3;
			usgh.n3 = (ni3 - si3);
			usgh.o5 = si5 * usgh.d5 + usgh.o5;
			usgh.n5 = (ni5 - si5);
	} else if(mergeaxis==4) {
			n4 = 0;
			for(i=0;i<nin;i++) {
				ierr = fgetusghdr(infp[i],&usgh2);
				if(ierr!=0) err(" error open gridin=%s \n",datain[i]);
				if(usgh.n1*usgh.n2*usgh.n3*usgh.n5 
			 	 !=usgh2.n1*usgh2.n2*usgh2.n3*usgh2.n5)
					err(" check gridin=%s \n",datain[i]);
				n4 += usgh2.n4;
				m5[i] = usgh2.n4;
				fseek64(infp[i],0,0);
				fprintf(stderr," merge gridin=%s ... o4=%g n4=%d d4=%g \n",
					datain[i],usgh2.o4,usgh2.n4,usgh2.d4);
			}

			for(i5=0;i5<usgh.n5;i5++) {
				for(i=0;i<nin;i++) {
					nread = m5[i] * usgh.n3 * usgh.n2;
					for(i2=0;i2<nread;i2++) {
						fread(grid,usgh.dtype,usgh.n1,infp[i]);
						fwrite(grid,usgh.dtype,usgh.n1,outfp);
					}
				}
			}
			if(headupdate==4) {
				usgh.n4 = n4;
			} else if(headupdate==5) {
				usgh.n5 = n5*nin;
				usgh.d5 = usgh.d5/nin;
			}
	} else if(mergeaxis==3) {
			n3 = 0;
			for(i=0;i<nin;i++) {
				ierr = fgetusghdr(infp[i],&usgh2);
				if(ierr!=0) err(" error open gridin=%s \n",datain[i]);
				if(usgh.n1*usgh.n2*usgh.n4*usgh.n5 
			 	 !=usgh2.n1*usgh2.n2*usgh2.n4*usgh2.n5)
					err(" check gridin=%s \n",datain[i]);
				n3 += usgh2.n3;
				m5[i] = usgh2.n3;
				fseek64(infp[i],0,0);
				fprintf(stderr," merge gridin=%s ... o3=%g n3=%d d3=%g \n",
					datain[i],usgh2.o3,usgh2.n3,usgh2.d3);
			}

			for(i5=0;i5<usgh.n5*usgh.n4;i5++) {
				for(i=0;i<nin;i++) {
					nread = m5[i] * usgh.n2;
					for(i2=0;i2<nread;i2++) {
						fread(grid,usgh.dtype,usgh.n1,infp[i]);
						fwrite(grid,usgh.dtype,usgh.n1,outfp);
					}
				}
			}
			usgh.n3 = n3;
	} else if(mergeaxis==2) {
			n2 = 0;
			for(i=0;i<nin;i++) {
				ierr = fgetusghdr(infp[i],&usgh2);
				if(ierr!=0) err(" error open gridin=%s \n",datain[i]);
				if(usgh.n1*usgh.n3*usgh.n4*usgh.n5 
			 	 !=usgh2.n1*usgh2.n3*usgh2.n4*usgh2.n5)
					err(" check gridin=%s \n",datain[i]);
				n2 += usgh2.n2;
				m5[i] = usgh2.n2;
				fseek64(infp[i],0,0);
				fprintf(stderr," merge gridin=%s ... o2=%g n2=%d d2=%g \n",
					datain[i],usgh2.o2,usgh2.n2,usgh2.d2);
			}

			for(i5=0;i5<usgh.n5*usgh.n4*usgh.n3;i5++) {
				for(i=0;i<nin;i++) {
					nread = m5[i];
					for(i2=0;i2<nread;i2++) {
						fread(grid,usgh.dtype,usgh.n1,infp[i]);
						fwrite(grid,usgh.dtype,usgh.n1,outfp);
					}
				}
			}
			usgh.n2 = n2;
	} else if(mergeaxis==1) {
			for(i=0;i<nin;i++) {
				ierr = fgetusghdr(infp[i],&usgh2);
				if(ierr!=0) err(" error open gridin=%s \n",datain[i]);
				if(usgh.n2*usgh.n3*usgh.n4*usgh.n5 
			 	 !=usgh2.n2*usgh2.n3*usgh2.n4*usgh2.n5)
					err(" check gridin=%s \n",datain[i]);
				n1 += usgh2.n1;
				m5[i] = usgh2.n1;
				fseek64(infp[i],0,0);
				fprintf(stderr," merge gridin=%s ... o1=%g n1=%d d1=%g \n",
					datain[i],usgh2.o1,usgh2.n1,usgh2.d1);
			}
			n1out = 0;
			for(i2=0;i2<nin;i2++) n1out = n1out + m5[i2];
			grido = (char*)malloc(n1out*usgh.dtype);
			for(i5=0;i5<usgh.n5*usgh.n4*usgh.n3*usgh.n2;i5++) {
				i2 = 0;
				for(i=0;i<nin;i++) {
					nread = m5[i];
					fread(grid,usgh.dtype,nread,infp[i]);
					bcopy(grid,grido+i2,nread*usgh.dtype);
					i2 = i2 + nread*usgh.dtype;
				}
				fwrite(grido,usgh.dtype,n1out,outfp);
			}
			usgh.n1 = n1out;
	}

	usgh.scale = 1.0;
	if(iddupdate==1) {
		if(headupdate==1) {
			usgh.d1 = ddupdate;
		} else if(headupdate==2) {
			usgh.d2 = ddupdate;
		} else if(headupdate==3) {
			usgh.d3 = ddupdate;
		} else if(headupdate==4) {
			usgh.d4 = ddupdate;
		} else if(headupdate==5) {
			usgh.d5 = ddupdate;
		}
	}

	ierr = fputusghdr(outfp,&usgh);
	if(ierr!=0) err(" error output ");

	free(grid);
	free(m5);
	if(mergeaxis==1) free(grido);

	exit (0);

}
