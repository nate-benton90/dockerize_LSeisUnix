/*
*	SeisTable.c
*
*	See SeisMig2D.c
*	14 Oct 1994  Murillo: c
*/

#include "su.h"
#include "SeisTable.h"


void *GetAll(char * efile, int *size)
{
  struct stat buffer;
  void *s1;
  int fd;

  if ((fd=open(efile,O_RDONLY)) < 0) {
    fprintf(stderr, "error opening efile=%s\n",efile);
    return(NULL);
  }

  fstat(fd, &buffer);
  *size = buffer.st_size;
  s1 = (void *) malloc(*size);
  read(fd, s1, *size);
  close(fd);

  return(s1);
}/* End of GetAll() */


void sread(void* str, int size, int n, void *fp)
{
  static int i = 0;
  static int sum = 0;
  static char *ptr;

  if (i == 0) {
    ptr = (char *)fp;
    i++;
  }
  memcpy(str, ptr, size*n);
  ptr += size*n;
/*
  fprintf(stderr, "Bytes read up to now (%d)\n", sum = sum + size);
  fflush(stderr);
*/

}

eTable *ezread (void *fp)
/*****************************************************************************
read and return a pointer to an extrapolator table stored in a file
******************************************************************************
Input:
fp		file pointer to file containing extrapolator table
******************************************************************************
Author:  Dave Hale, Colorado School of Mines, 11/24/89
Modified by AEM
******************************************************************************/
{
	int nhmax,nwdxov,iop,*nh;
	float dwdxov,fwdxov,dzodx,vpo,vso,ep,del;
	complex **e;
	eTable *et;
	int	i,j;
	
	/* read table values */
	sread(&nhmax,sizeof(int),1,fp);
        swap_int_4(&nhmax);
	sread(&nwdxov,sizeof(int),1,fp);
        swap_int_4(&nwdxov);
	sread(&dwdxov,sizeof(float),1,fp);
        swap_float_4(&dwdxov);
	sread(&fwdxov,sizeof(float),1,fp);
        swap_float_4(&fwdxov);
	sread(&dzodx,sizeof(float),1,fp);
        swap_float_4(&dzodx);
	sread(&iop,sizeof(int),1,fp);
        swap_int_4(&iop);
	sread(&vpo,sizeof(float),1,fp);
        swap_float_4(&vpo);
	sread(&vso,sizeof(float),1,fp);
        swap_float_4(&vso);
	sread(&ep,sizeof(float),1,fp);
        swap_float_4(&ep);
	sread(&del,sizeof(float),1,fp);
        swap_float_4(&del);

	if ( (nh = alloc1int(nwdxov) ) == NULL) 
	  fprintf(stderr, "Error using alloc1int in sread\n");

/*
	fprintf(stderr, "Number of bytes to read in nh: (%d X %d)\n", 
		sizeof(int), nwdxov);
	fflush(stderr);
*/
	sread(nh,sizeof(int),nwdxov,fp);
	for (i = 0; i < nwdxov; i++)
		swap_int_4(&nh[i]);
	e = alloc2complex(nhmax,nwdxov);
	sread(e[0],sizeof(complex),nhmax*nwdxov,fp);
	for (i = 0; i < nwdxov; i++)
	  for (j = 0; j < nhmax; j++) {
            swap_float_4(&e[i][j].r);
            swap_float_4(&e[i][j].i);
	  }
	
	/* allocate table and set table values */
	et = (eTable*)malloc(sizeof(eTable));
	et->nhmax = nhmax;
	et->nwdxov = nwdxov;
	et->dwdxov = dwdxov;
	et->fwdxov = fwdxov;
	et->dzodx = dzodx;
	et->iop   =  iop;
	et->vpo   =  vpo;
	et->vso   =  vso;
	et->ep    =   ep;
	et->del   =  del;
	et->nh = nh;
	et->e = e;
	
	/* return pointer to table */
	return(et);
}

void pret(eTable *et)
{
	int i, j;

	fprintf(stderr, "nhmax = %d\n", et->nhmax);
	fprintf(stderr, "nwdxov = %d\n", et->nwdxov);
	fprintf(stderr, "dwdxov = %f\n", et->dwdxov);
	fprintf(stderr, "fwdxov = %f\n", et->fwdxov);
	fprintf(stderr, "dzodx = %f\n", et->dzodx);
	fprintf(stderr, "iop = %d\n", et->iop);
	fprintf(stderr, "vpo = %f\n", et->vpo);
	fprintf(stderr, "vso = %f\n", et->vso);
	fprintf(stderr, "ep = %d\n", et->ep);
	fprintf(stderr, "del = %d\n", et->del);
	
	for (i = 0; i < et -> nwdxov; i++) 
	  fprintf(stderr, "nh[%d] = %d\n", i, et-> nh[i]);
	for (i = 0; i < et -> nwdxov; i++) 
	  for (j = 0; j < et -> nhmax; j++) {
	    fprintf(stderr, "e[%d,%d].r) = %f\n", i, j, et-> e[i][j].r);
	    fprintf(stderr, "e[%d,%d].i) = %f\n", i, j, et-> e[i][j].i);
	  }
	fflush(stderr);
}
