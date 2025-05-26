#include <su.h>

char *sdoc =
"MEDIAN3D - smooth a 3D dataset with a median filter\n\n"
"median3d in= out= [n1= n2= n3=] w1= w2= w3= dim=\n\n"
"in= out=		files, float arrays[n3][n2][n1]\n"
"n1= n2= n3=		array dimensions\n"
"w1= w2= w3=		median window dimensions, should be odd numbers\n"
"dim=			=1 w's are lines, three passes\n"
"			=2 w's are planes, three passes\n"
"			=3 w's define a sub-cube, one pass\n"
;

int main (int argc , char **argv) {
int n1, n2, n3, w1, w2, w3, dim, one=1, vgrid, infd, outfd;
float *in, *out, *buf;
char *infile, *outfile;

initargs (argc,argv);
askdoc (1);

if (getparstring ("in",&infile) == 0) err ("in= missing");
if ((infd = open (infile,0)) < 0) err ("cant open in=%s",infile);
if (getparstring ("out",&outfile) == 0) err ("out= missing");
if ((outfd = creat (outfile,0664)) < 0) err ("cant create out=%s",outfile);
if (getvgrid (infd,&n1,&n2,&n3)) {
	vgrid = 1;
	}
else	{
	vgrid = 0;
	if (getparint("n1",&n1) == 0) err ("n1= missing");
	if (getparint("n2",&n2) == 0) err ("n2= missing");
	if (getparint("n3",&n3) == 0) err ("n3= missing");
	}
if (getparint("w1",&w1) == 0) err ("w1= missing");
if (getparint("w2",&w2) == 0) err ("w2= missing");
if (getparint("w3",&w3) == 0) err ("w3= missing");
if (getparint("dim",&dim) == 0) err ("dim= missing");
if (dim < 1 || dim > 3) err ("dim=1,2,3");
in = (float*) malloc (n1*n2*n3*4);
out = (float*) malloc (n1*n2*n3*4);
buf = (float*) malloc (w1*w2*w3*4);
read (infd,in,n1*n2*n3*4);
switch (dim) {
case 1:
	medfilter_ (in,out,buf,&n1,&n2,&n3,&w1,&one,&one);
	medfilter_ (out,in,buf,&n1,&n2,&n3,&one,&w2,&one);
	medfilter_ (in,out,buf,&n1,&n2,&n3,&one,&one,&w3);
	break;
case 2:
	medfilter_ (in,out,buf,&n1,&n2,&n3,&w1,&w2,&one);
	medfilter_ (out,in,buf,&n1,&n2,&n3,&w1,&one,&w3);
	medfilter_ (in,out,buf,&n1,&n2,&n3,&one,&w2,&w3);
	break;
case 3:
	medfilter_ (in,out,buf,&n1,&n2,&n3,&w1,&w2,&w3);
	break;
	}
write (outfd,out,n1*n2*n3*4);
if (vgrid) putvgrid (outfd,n1,n2,n3);
}
