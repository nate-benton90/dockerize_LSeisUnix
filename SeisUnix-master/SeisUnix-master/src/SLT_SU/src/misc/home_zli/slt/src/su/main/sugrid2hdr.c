#include "usgrid.h"
#include "su.h"
#include "segy.h"

/*********************** self documentation ******************************/
string sdoc =
"\n"
"SUGRID2HDR - grid to segy trace header conversion  			\n"
"\n"
"sugrid2hdr <stdin >stdout grid= key= parameters] \n"
"\n"
"Required Parameters:\n"
"stdin                      Name of input data 			\n"
"stdout                     Name of output data 		\n"
"grid                       Name of the grid (nx by ny)			\n"
"key=                       Name of trace header key word	\n"
"Optional Parameters:\n"
"nx=n1                      number of traces per line in the input		\n"
"ny=n2                      number of lines in the input		\n"
"                           nx must be the same as n1 of grid	\n"         
"                           ny must be the same as n2 of grid	\n"         
"                           there are nx*ny traces in the input \n"
"\n"
"Author:	Zhiming Li		      		7-29-96		\n"
"\n";
/**************** end self doc *******************************************/


segytrace tr;

void putval(String type, Value *val, double a);

main(int argc, char **argv)
{
	FILE *gfp, *infp=stdin, *outfp=stdout;
	char *grid;
	int n1, n2, nx, ny, ierr, nxy;
	String key="tracl", type;
	Value val;
	int indx;
	float *data;
	double a;

	usghed usgh;


	/* hook up getpar */
	initargs(argc, argv);
	askdoc(1);


	/* get required parameters */
	if (!getparstring("grid",&grid)) err(" grid missing ");
	gfp = efopen(grid,"r");
	ierr = fgetusghdr(gfp, &usgh);
	if(ierr!=0) err(" error getting grid header ");
	n1 = usgh.n1;
	n2 = usgh.n2;
	if(!getparstring("key",&key)) err(" key missing ");
	type = hdtype(key);
	indx = getindex(key);

	/* get information from the first header */
	if (!fgettr(infp,&tr)) err("can't get first trace");
	
	/* get other optional parameters */
	if (!getparint("nx",&nx)) nx = n1;
	if (!getparint("ny",&ny)) ny = n2;
	if (n1!=nx || n2!=ny) err(" check nx, ny or n1, n2"); 

	/* read in grid */
	data = (float*) emalloc(n1*n2*sizeof(float));
	efseek(gfp,0,0);
	efread(data,sizeof(float),n1*n2,gfp);

	/* loop over traces */
	nxy = 0;
	do {

		a = data[nxy];
		putval(type,&val,a);
		puthval(&tr,indx,&val);

		fputtr(outfp,&tr);
		nxy += 1;
		if(nxy>nx*ny) {
			warn(" maximum number of input traces processed %d \n",nx*ny);
			break;
		}
	} while (fgettr(infp,&tr));

	free(data);

	return EXIT_SUCCESS;
}

void putval(String type, Value *val, double a) 
{
	switch (*type) {
	case 's':
		err("can't change char header word");
	break;
	case 'h':
		val->h = a;
	break;
	case 'u':
		val->u = a;
	break;
	case 'l':
		val->l = a;
	break;
	case 'v':
		val->v = a;
	break;
	case 'i':
		val->i = a;
	break;
	case 'p':
		val->p = a;
	break;
	case 'f':
		val->f = a;
	break;
	case 'd':
		val->d = a;
	break;
	default:
		err("unknown type %s", type);
	break;
	}
}
