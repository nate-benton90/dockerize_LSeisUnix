#include <stdio.h>
#include <su.h>
#include <gridhd.h>

string sdoc =
"REGRID - reshape grid using linear interpolation (not recommended for seismic data)\n"
"\n"
"usage: regrid in=vgrid [ out=vgrid template=vgrid ] [ windowing options ]\n"
"\n"
"if out= not given then no output written; just parameters printed\n"
"\n"
"windowing options:\n"
"template=""		change input into this shape\n"
"o1=e1-(n1-1)*d1		new axis origin\n"
"e1=o1+(n1-1)*d1  	new axis endpoint\n"
"n1=(e1-o1)/d1+1p	new axis count\n"
"d1=(e1-o1)/(n1-1)	new axis spacing\n"
"pad= pad1= ...		pad beyond boundary with specified value\n"
"			no number does all dimensions, default is to replicate boundary value\n"
"graph=0			if set, xgraph will show map view and vertical cross-sections\n"
"			of the input and output grids; also graph='only' option\n"
"label1=n1 ...		labels for graph axes\n"
"coremb=256		limit arrays to this much core\n"
"\n"
;

main (int argc , char **argv)
	{
	ghed igh, ogh, tgh;
	int in1, in2, in3, on1, on2, on3, i1, i2, i3;
	int nflag, oflag, dflag, eflag, wflag;
	int block3, iblock, outfd, coremb=256;
	int pflag1=0, pflag2=0, pflag3;
	float start3;
	float io1, io2, io3, id1, id2, id3, ie1, ie2, ie3, idcdp2, iocdp2, idline3, ioline3;
	float oo1, oo2, oo3, od1, od2, od3, oe1, oe2, oe3, odcdp2, oocdp2, odline3, ooline3;
	float pad1, pad2, pad3;
	string in=0, out=0, template=0, graph=0, label1=0, label2=0, label3=0;
	char command[100];
	float *indata, *outdata, *ix1, *ix2, *ix3, *ox1, *ox2, *ox3;
	FILE *gfd1, *gfd2, *gfd3;

	initargs (argc,argv);
	askdoc(1);
	getparstring ("in",&in);
	wflag = getparstring ("out",&out);
	if ((outfd = creat(out,0664)) < 0) {
		err ("cant create output file");
		}
	if (readvgrid (in,&indata,&igh) <= 0) exit(-1);
	if (rint(igh.dtype / igh.scale) != 4.) err ("input not floating point");
	in1 = rint(igh.n1 / igh.scale);
	in2 = rint(igh.n2 / igh.scale);
	in3 = rint(igh.n3 / igh.scale);
	io1 = igh.o1 / igh.scale;
	io2 = igh.o2 / igh.scale;
	io3 = igh.o3 / igh.scale;
	id1 = igh.d1 / igh.scale;
	id2 = igh.d2 / igh.scale;
	id3 = igh.d3 / igh.scale;
	ie1 = io1 + (in1 - 1) * id1;
	ie2 = io2 + (in2 - 1) * id2;
	ie3 = io3 + (in3 - 1) * id3;
	idcdp2 = igh.dcdp2 / igh.scale;
	iocdp2 = igh.ocdp2 / igh.scale;
	idline3 = igh.dline3 / igh.scale;
	ioline3 = igh.oline3 / igh.scale;

	if (getparstring ("template",&template)) {
		readvgrid (template,0,&tgh);
		on1 = rint(tgh.n1 / tgh.scale);
		on2 = rint(tgh.n2 / tgh.scale);
		on3 = rint(tgh.n3 / tgh.scale);
		oo1 = tgh.o1 / tgh.scale;
		oo2 = tgh.o2 / tgh.scale;
		oo3 = tgh.o3 / tgh.scale;
		od1 = tgh.d1 / tgh.scale;
		od2 = tgh.d2 / tgh.scale;
		od3 = tgh.d3 / tgh.scale;
		oe1 = oo1 + (on1 - 1) * od1;
		oe2 = oo2 + (on2 - 1) * od2;
		oe3 = oo3 + (on3 - 1) * od3;
		odcdp2 = tgh.dcdp2 / tgh.scale;
		oocdp2 = tgh.dcdp2 / tgh.scale;
		odline3 = tgh.dline3 / tgh.scale;
		ooline3 = tgh.dline3 / tgh.scale;
		}
	else	{
		on1 = in1;
		on2 = in2;
		on3 = in3;
		od1 = id1;
		od2 = id2;
		od3 = id3;
		oo1 = io1;
		oo2 = io2;
		oo3 = io3;
		oe1 = ie1;
		oe2 = ie2;
		oe3 = ie3;
		odcdp2 = idcdp2;
		oocdp2 = iocdp2;
		odline3 = idline3;
		ooline3 = ioline3;
		}

	/* padding */
	if (getparfloat ("pad",&pad1) > 0) {
		pflag1 = 1;
		pflag2 = 1;
		pflag3 = 1;
		pad2 = pad1;
		pad3 = pad1;
		}
	else	{
		pflag1 = getparfloat ("pad1",&pad1);
		pflag2 = getparfloat ("pad2",&pad2);
		pflag3 = getparfloat ("pad3",&pad3);
		}

	/* axis1 */
	oflag = getparfloat ("o1",&oo1);
	eflag = getparfloat ("e1",&oe1);
	dflag = getparfloat ("d1",&od1);
	nflag = getparint ("n1",&on1);
	switch (nflag+2*dflag+4*eflag+8*oflag) {
	case 0:
		break;
	case 1:
	case 5:
	case 9:
	case 13:
		od1 = (oe1 - oo1) / (on1 - 1);
		break;
	case 2:
	case 10:
	case 14:
		on1 = rint((oe1 - oo1) / od1 + 1);
		oe1 = oo1 + (on1 - 1) * od1;
		break;
	case 3:
	case 11:
		oe1 = oo1 + (on1 - 1) * od1;
		break;
	case 4:
	case 8:
	case 12:
		on1 = rint((oe1 - oo1) / od1 + 1);
		od1 = (oe1 - oo1) / (on1 - 1);
		break;
	case 6:
		on1 = rint((oe1 - oo1) / od1 + 1);
		oo1 = oe1 - (on1 - 1) * od1;
		break;
	case 7:
		oo1 = oe1 - (on1 - 1) * od1;
		break;
	case 15:
		if (oe1 != oo1 + (on1 - 1) * od1) {
			err ("four specified o1, e1, n1, and d1 quantities inconsistent");
			}
		break;
		}
	if (oo1 < io1 && oe1 < io1) err ("no overlap for axis1 input and output ranges");
	if (oo1 > ie1 && oe1 > ie1) err ("no overlap for axis1 input and output ranges");
	if (oo1 < oe1 && oo1 < io1) {
		if (pflag1) fprintf (stderr,"warning: output padding beyond o1 with %g\n",pad1);
		else fprintf (stderr,"warning: output padding beyond o1 with boundary value\n");
		}
	if (oo1 < oe1 && oe1 > ie1)  {
		if (pflag1) fprintf (stderr,"warning: output padding beyond e1 with %g\n",pad1);
		else fprintf (stderr,"warning: output padding beyond e1 with boundary value\n");
		}
	if (oo1 > oe1 && oe1 < io1) {
		if (pflag1) fprintf (stderr,"warning: output padding beyond o1 with %g\n",pad1);
		else fprintf (stderr,"warning: output padding beyond o1 with boundary value\n");
		}
	if (oo1 > oe1 && oe1 < io1)  {
		if (pflag1) fprintf (stderr,"warning: output padding beyond e1 with %g\n",pad1);
		else fprintf (stderr,"warning: output padding beyond e1 with boundary value\n");
		}

	/* axis2 */
	oflag = getparfloat ("o2",&oo2);
	eflag = getparfloat ("e2",&oe2);
	dflag = getparfloat ("d2",&od2);
	nflag = getparint ("n2",&on2);
	switch (nflag+2*dflag+4*eflag+8*oflag) {
	case 0:
		break;
	case 1:
	case 5:
	case 9:
	case 13:
		od2 = (oe2 - oo2) / (on2 - 1);
		break;
	case 2:
	case 10:
	case 14:
		on2 = rint((oe2 - oo2) / od2 + 1);
		oe2 = oo2 + (on2 - 1) * od2;
		break;
	case 3:
	case 11:
		oe2 = oo2 + (on2 - 1) * od2;
		break;
	case 4:
	case 8:
	case 12:
		on2 = rint((oe2 - oo2) / od2 + 1);
		od2 = (oe2 - oo2) / (on2 - 1);
		break;
	case 6:
		on2 = rint((oe2 - oo2) / od2 + 1);
		oo2 = oe2 - (on2 - 1) * od2;
		break;
	case 7:
		oo2 = oe2 - (on2 - 1) * od2;
		break;
	case 15:
		if (oe2 != oo2 + (on2 - 1) * od2) {
			err ("four specified o2, e2, n2, and d2 quantities inconsistent");
			}
		break;
		}
	if (oo2 < io2 && oe2 < io2) err ("no overlap for axis2 input and output ranges");
	if (oo2 > ie2 && oe2 > ie2) err ("no overlap for axis2 input and output ranges");
	if (oo2 < oe2 && oo2 < io2) {
		if (pflag2) fprintf (stderr,"warning: output padding beyond o2 with %g\n",pad2);
		else fprintf (stderr,"warning: output padding beyond o2 with boundary value\n");
		}
	if (oo2 < oe2 && oe2 > ie2)  {
		if (pflag2) fprintf (stderr,"warning: output padding beyond e2 with %g\n",pad2);
		else fprintf (stderr,"warning: output padding beyond e2 with boundary value\n");
		}
	if (oo2 > oe2 && oe2 < io2) {
		if (pflag2) fprintf (stderr,"warning: output padding beyond o2 with %g\n",pad2);
		else fprintf (stderr,"warning: output padding beyond o2 with boundary value\n");
		}
	if (oo2 > oe2 && oe2 < io2)  {
		if (pflag2) fprintf (stderr,"warning: output padding beyond e2 with %g\n",pad2);
		else fprintf (stderr,"warning: output padding beyond e2 with boundary value\n");
		}

	/* axis3 */
	oflag = getparfloat ("o3",&oo3);
	eflag = getparfloat ("e3",&oe3);
	dflag = getparfloat ("d3",&od3);
	nflag = getparint ("n3",&on3);
	switch (nflag+2*dflag+4*eflag+8*oflag) {
	case 0:
		break;
	case 1:
	case 5:
	case 9:
	case 13:
		od3 = (oe3 - oo3) / (on3 - 1);
		break;
	case 2:
	case 10:
	case 14:
		on3 = rint((oe3 - oo3) / od3 + 1);
		oe3 = oo3 + (on3 - 1) * od3;
		break;
	case 3:
	case 11:
		oe3 = oo3 + (on3 - 1) * od3;
		break;
	case 4:
	case 8:
	case 12:
		on3 = rint((oe3 - oo3) / od3 + 1);
		od3 = (oe3 - oo3) / (on3 - 1);
		break;
	case 6:
		on3 = rint((oe3 - oo3) / od3 + 1);
		oo3 = oe3 - (on3 - 1) * od3;
		break;
	case 7:
		oo3 = oe3 - (on3 - 1) * od3;
		break;
	case 15:
		if (oe3 != oo3 + (on3 - 1) * od3) {
			err ("four specified o3, e3, n3, and d3 quantities inconsistent");
			}
		break;
		}
	if (oo3 < io3 && oe3 < io3) err ("no overlap for axis3 input and output ranges");
	if (oo3 > ie3 && oe3 > ie3) err ("no overlap for axis3 input and output ranges");
	if (oo3 < oe3 && oo3 < io3) {
		if (pflag3) fprintf (stderr,"warning: output padding beyond o3 with %g\n",pad3);
		else fprintf (stderr,"warning: output padding beyond o3 with boundary value\n");
		}
	if (oo3 < oe3 && oe3 > ie3)  {
		if (pflag3) fprintf (stderr,"warning: output padding beyond e3 with %g\n",pad3);
		else fprintf (stderr,"warning: output padding beyond e3 with boundary value\n");
		}
	if (oo3 > oe3 && oe3 < io3) {
		if (pflag3) fprintf (stderr,"warning: output padding beyond o3 with %g\n",pad3);
		else fprintf (stderr,"warning: output padding beyond o3 with boundary value\n");
		}
	if (oo3 > oe3 && oe3 < io3) {
		if (pflag3) fprintf (stderr,"warning: output padding beyond e3 with %g\n",pad3);
		else fprintf (stderr,"warning: output padding beyond e3 with boundary value\n");
		}

	/* other numbers */
	oocdp2 = iocdp2 + (oo2 - io2) / id2 * idcdp2;
	odcdp2 = idcdp2 * od2 / id2;
	ooline3 = ioline3 + (oo3 - io3) / id3 * idline3;
	odline3 = idline3 * od3 / id3;
	getparint ("coremb",&coremb);

	fprintf (stderr,"in=%s size=%d\n",in,in1*in2*in3);
	fprintf (stderr,"o1=%-11g e1=%-11g d1=%-11g n1=%-11d\n",io1,ie1,id1,in1);
	fprintf (stderr,"o2=%-11g e2=%-11g d2=%-11g n2=%-11d\n",io2,ie2,id2,in2);
	fprintf (stderr,"o3=%-11g e3=%-11g d3=%-11g n3=%-11d\n",io3,ie3,id3,in3);
	fprintf (stderr,"ocdp2=%-8g dcdp2=%-8g oline3=%-7g dline3=%-7g\n",iocdp2,idcdp2,ioline3,idline3);
	fprintf (stderr,"out=%s size=%d\n",out,on1*on2*on3);
	fprintf (stderr,"o1=%-11g e1=%-11g d1=%-11g n1=%-11d pad1=%g\n",oo1,oe1,od1,on1,pad1);
	fprintf (stderr,"o2=%-11g e2=%-11g d2=%-11g n2=%-11d pad2=%g\n",oo2,oe2,od2,on2,pad2);
	fprintf (stderr,"o3=%-11g e3=%-11g d3=%-11g n3=%-11d pad3=%g\n",oo3,oe3,od3,on3,pad3);
	fprintf (stderr,"ocdp2=%-8g dcdp2=%-8g oline3=%-7g dline3=%-7g\n",oocdp2,odcdp2,ooline3,odline3);

	/* graph option */
#define DPLOT 5
	if (getparstring ("graph",&graph) > 0 && atoi(graph) != 0) {
		label1 = "n1";
		getparstring ("label1",&label1);
		label2 = "n2";
		getparstring ("label2",&label2);
		label3 = "n3";
		getparstring ("label3",&label3);
		ix1 = (float*)malloc(in1*4);
		ix2 = (float*)malloc(in2*4);
		ix3 = (float*)malloc(in3*4);
		ox1 = (float*)malloc(on1*4);
		ox2 = (float*)malloc(on2*4);
		ox3 = (float*)malloc(on3*4);
		for (i1=0; i1<in1; i1++) ix1[i1] = io1 + i1 * id1;
		for (i2=0; i2<in2; i2++) ix2[i2] = io2 + i2 * id2;
		for (i3=0; i3<in3; i3++) ix3[i3] = io3 + i3 * id3;
		for (i1=0; i1<on1; i1++) ox1[i1] = oo1 + i1 * od1;
		for (i2=0; i2<on2; i2++) ox2[i2] = oo2 + i2 * od2;
		for (i3=0; i3<on3; i3++) ox3[i3] = oo3 + i3 * od3;

		sprintf (command, "xgraph n=%d,%d color=1,2 mark=1,2 marksize=5 linewidth=0 label1=%s label2=%s title=\"MAP VIEW (constant %s cross-section)\" height=600 width=600",in2*in3,on2*on3,label2,label3,label1);
		fprintf (stderr,"drawing map view cross-section on screen ...\n");
		gfd1 = popen (command,"w");
		for (i2=0; i2<in2; i2+=DPLOT) {
			for (i3=0; i3<in3; i3+=DPLOT) {
				fwrite (ix2+i2,1,4,gfd1);
				fwrite (ix3+i3,1,4,gfd1);
				}
			fwrite (ix2+i2,1,4,gfd1);
			fwrite (ix3+in3-1,1,4,gfd1);
			}
		fwrite (ix2+in2-1,1,4,gfd1);
		fwrite (ix3+in3-1,1,4,gfd1);
		for (i2=0; i2<on2; i2+=DPLOT) {
			for (i3=0; i3<on3; i3+=DPLOT) {
				fwrite (ox2+i2,1,4,gfd1);
				fwrite (ox3+i3,1,4,gfd1);
				}
			fwrite (ox2+i2,1,4,gfd1);
			fwrite (ox3+on3-1,1,4,gfd1);
			}
		fwrite (ox2+on2-1,1,4,gfd1);
		fwrite (ox3+on3-1,1,4,gfd1);
		fflush (gfd1);
		pclose (gfd1);

		sprintf (command, "xgraph n=%d,%d color=1,2 mark=1,2 marksize=5 linewidth=0 label1=%s label2=%s title=\"%s VERTICAL (constant %s) CROSS-SECTION\" height=600 width=600",in1*in2,on1*on2,label1,label2,label3);
		fprintf (stderr,"drawing vertical cross-section on screen ...\n");
		gfd3 = popen (command,"w");
		for (i1=0; i1<in1; i1+=DPLOT) {
			for (i2=0; i2<in2; i2+=DPLOT) {
				fwrite (ix1+i1,1,4,gfd3);
				fwrite (ix2+i2,1,4,gfd3);
				}
			fwrite (ix1+i1,1,4,gfd3);
			fwrite (ix2+in2-1,1,4,gfd3);
			}
		fwrite (ix1+in1-1,1,4,gfd3);
		fwrite (ix2+in2-1,1,4,gfd3);
		for (i1=0; i1<on1; i1+=DPLOT) {
			for (i2=0; i2<on2; i2+=DPLOT) {
				fwrite (ox1+i1,1,4,gfd3);
				fwrite (ox2+i2,1,4,gfd3);
				}
			fwrite (ox1+i1,1,4,gfd3);
			fwrite (ox2+on2-1,1,4,gfd3);
			}
		fwrite (ox1+on1-1,1,4,gfd3);
		fwrite (ox2+on2-1,1,4,gfd3);
		fflush (gfd3);
		pclose (gfd3);

		free (ix1);
		free (ix2);
		free (ix3);
		free (ox1);
		free (ox2);
		free (ox3);
		if (!strcmp (graph,"only")) exit(0);
		}
	if (!wflag) exit(1);
	block3 = (coremb*1024*256 - in1*in2*in3) / (on1 *on2);
	block3 = block3 > 2 ? block3 : 2;
	block3 = block3 < on3 ? block3 : on3;
	while (block3 > 0 && (outdata = (float*) malloc(on1*on2*block3*4)) == 0) block3--;
	if (block3 == 0) err ("cant malloc space for output grid");
	for (iblock=0; iblock<on3; iblock+=block3) {
		block3 = (iblock+block3) < on3 ? block3 : (on3-iblock);
		start3 = oo3 + iblock * od3;
		regrid3d_ (indata,&in1,&in2,&in3,&io1,&io2,&io3,&id1,&id2,&id3,
			outdata,&on1,&on2,&block3,&oo1,&oo2,&start3,&od1,&od2,&od3,
			&pflag1,&pflag2,&pflag3,&pad1,&pad2,&pad3);
		write (outfd,outdata,on1*on2*block3*4);
		}
	ogh = igh;
	ogh.n1 = on1 * ogh.scale;
	ogh.n2 = on2 * ogh.scale;
	ogh.n3 = on3 * ogh.scale;
	ogh.d1 = od1 * ogh.scale;
	ogh.d2 = od2 * ogh.scale;
	ogh.d3 = od3 * ogh.scale;
	ogh.o1 = oo1 * ogh.scale;
	ogh.o2 = oo2 * ogh.scale;
	ogh.o3 = oo3 * ogh.scale;
	ogh.ocdp2 = oocdp2 * ogh.scale;
	ogh.dcdp2 = odcdp2 * ogh.scale;
	ogh.oline3 = ooline3 * ogh.scale;
	ogh.dline3 = odline3 * ogh.scale;
	write (outfd,&ogh,sizeof(ogh));
	}
