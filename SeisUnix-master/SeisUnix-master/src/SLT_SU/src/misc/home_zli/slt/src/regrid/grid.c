#include <stdio.h>
#include "gridhd.h"
#ifdef __sgi
#define LSEEK lseek64
#else
#define LSEEK lseek
#endif

readvgrid (file,array,header)
char *file;
float **array;
ghed *header;
	{
	int fd;
	long fsize, hsize;
	double rint();

	if (!strcmp(file,"stdin")) {
		fd = 0;
		}
	else	{
		if ((fd = open (file,0)) == 0) {
			fprintf (stderr,"cant open vgrid file %s\n",file);
			return (-1);
			}
		}
	if ((fsize = lseek (fd,-sizeof(header[0]),2)) < 0) {
		fprintf (stderr,"cant read header from vgrid file %s\n",file);
		close (fd);
		return (-1);
		}
	read (fd,header,sizeof(header[0]));
	hsize = rint(header->n1 / header->scale)
	      * rint(header->n2 / header->scale)
	      * rint(header->n3 / header->scale)
	      * rint(header->n4 / header->scale)
	      * rint(header->n5 / header->scale)
	      * rint(header->dtype / header->scale);
	if (hsize != fsize) {
		fprintf (stderr, "file %s not vgrid type or inconsistent size\n", file);
		close (fd);
		return (0);
		}
	if (array) {
		if ((*array = (float*)malloc(hsize)) == 0) {
			fprintf (stderr, "cant allocate space for vgrid array %s\n", file);
			close (fd);
			return (0);
			}
		LSEEK (fd,0,0);
		if (read (fd,*array,hsize) != hsize) {
			fprintf (stderr, "cant read vgrid data from %s\n", file);
			close (fd);
			return (0);
			}
		}
	close (fd);
	return (hsize);
	}

writevgrid (file,array,header)
char *file, *array;
ghed header;
	{
	int size, fd;
	double rint();

	if (!strcmp (file,"stdout")) {
		fd = 1;
		}
	else	{
		if ((fd = creat (file,0664)) < 0) {
			fprintf (stderr, "cant create vgrid file %s\n", file);
			return (-1);
			}
		}
	size = rint(header.n1 / header.scale)
	      * rint(header.n2 / header.scale)
	      * rint(header.n3 / header.scale)
	      * rint(header.n4 / header.scale)
	      * rint(header.n5 / header.scale)
	      * rint(header.dtype / header.scale);
	if (write (fd,array,size) != size) {
		fprintf (stderr, "cant write vgrid file %s data\n", file);
		return (0);
		}
	if (write (fd,&header,sizeof(header)) != sizeof(header)) {
		fprintf (stderr, "cant write vgrid file %s data\n", file);
		return (0);
		}
	return (size);
	}

/*
main ()
	{
	float *array;
	ghed header;
	printf ("size=%d\n",readvgrid("stdin",&array,&header));
	printf ("size=%d\n",writevgrid("junk",array,header));
	}
*/
