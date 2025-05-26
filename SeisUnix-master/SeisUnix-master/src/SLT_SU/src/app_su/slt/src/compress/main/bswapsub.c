/* BSWAP subroutines */
#include "su.h"
#include "bswap.h"
#include "sucomp.h"


/* convert SU_compress header */
void cmhconvert(char *chin, char *chout) {
	int i;
	int sizecmh;

	sizecmh = 56;

	bcopy(chin,chout,sizecmh);

	bswap8(&chin[20],&chout[20]);

	for(i=0;i<6;i++) {
		bswap4(&chin[28+i*4],&chout[28+i*4]);
	}
}

/* convert binary header file */
void bhconvert(char *cin, char *cout) {
	int i, ibyte;

	ibyte = 0;
	for (i=0;i<3;i++) {
	 	bswap4(&cin[ibyte+i*4],&cout[ibyte+i*4]);
	}
	ibyte = ibyte + 3 * 4;
	for (i=0;i<24;i++) {
		bswap2(&cin[ibyte+i*2],&cout[ibyte+i*2]);
	}
	ibyte = ibyte + 24 * 2;
	/* the rest using 4-byte swap */
	for (i=0;i<85;i++) {
		bswap4(&cin[ibyte+i*4],&cout[ibyte+i*4]);
	}
}
/* convert trace header */
void trhdconvert(char *cin,char *cout) {
	int i, ibyte;

	ibyte = 0;
	/* tracl - cdpt   bytes 1-28 */
	for(i=0;i<7;i++) {
	 	bswap4(&cin[ibyte+i*4],&cout[ibyte+i*4]);
	}
	ibyte = ibyte + 7 * 4;
	/* trid - duse   bytes 29-36 */
	for(i=0;i<4;i++) {
	 	bswap2(&cin[ibyte+i*2],&cout[ibyte+i*2]);
	}
	ibyte = ibyte + 4 * 2;
	/* offset - gwdep   bytes 37-68 */
	for(i=0;i<8;i++) {
	 	bswap4(&cin[ibyte+i*4],&cout[ibyte+i*4]);
	}
	ibyte = ibyte + 8 * 4;
	/* scalel - scalco   bytes 69-72 */
	for(i=0;i<2;i++) {
	 	bswap2(&cin[ibyte+i*2],&cout[ibyte+i*2]);
	}
	ibyte = ibyte + 2 * 2;
	/* sx - gy           bytes 73-88 */
	for(i=0;i<4;i++) {
	 	bswap4(&cin[ibyte+i*4],&cout[ibyte+i*4]);
	}
	ibyte = ibyte + 4 * 4;
	/* counit - otrav    bytes 89-180 */
	for(i=0;i<46;i++) {
	 	bswap2(&cin[ibyte+i*2],&cout[ibyte+i*2]);
	}
	ibyte = ibyte + 46 * 2;
	/* d1 - unscale    bytes 181-204 */
	for(i=0;i<6;i++) {
	 	bswap4(&cin[ibyte+i*4],&cout[ibyte+i*4]);
	}
	ibyte = ibyte + 6 * 4;
	/* mark - mutb    bytes 205-208 */
	for(i=0;i<2;i++) {
	 	bswap2(&cin[ibyte+i*2],&cout[ibyte+i*2]);
	}
	ibyte = ibyte + 2 * 2;
	/* dz - fz        bytes 209-216 */
	for(i=0;i<2;i++) {
	 	bswap4(&cin[ibyte+i*4],&cout[ibyte+i*4]);
	}
	ibyte = ibyte + 2 * 4;
	/* n2 -           bytes 217-220 */
	for(i=0;i<2;i++) {
	 	bswap2(&cin[ibyte+i*2],&cout[ibyte+i*2]);
	}
	ibyte = ibyte + 2 * 2;
	/* the rest using 4-byte swap */ 
	for (i=0;i<5;i++) {
	 	bswap4(&cin[ibyte+i*4],&cout[ibyte+i*4]);
	}
}

void traceconvert(char *cin,char *cout,int nt) {
	int i;
	for (i=0;i<nt;i++) {
	 	bswap4(&cin[i*4],&cout[i*4]);
	}
}

/* byte swapping of 8-bytes interger or float */
void bswap8(char *c8i, char *c8o)  {
	c8o[0] = c8i[7];	
	c8o[1] = c8i[6];	
	c8o[2] = c8i[5];	
	c8o[3] = c8i[4];	
	c8o[4] = c8i[3];	
	c8o[5] = c8i[2];	
	c8o[6] = c8i[1];	
	c8o[7] = c8i[0];	
}

/* byte swapping of 4-bytes interger or float */
void bswap4(char *c4i, char *c4o)  {
	c4o[0] = c4i[3];	
	c4o[1] = c4i[2];	
	c4o[2] = c4i[1];	
	c4o[3] = c4i[0];	
}
/* byte swapping of 2-bytes interger */
void bswap2(char *c2i, char *c2o)  {
	c2o[0] = c2i[1];	
	c2o[1] = c2i[0];	
}

void     gather_convert(char *cbuf, int ntrace, int ns)
{
   char    *cin, *cout;
   int      j, nsegy;

   nsegy = ns * sizeof(float) + 240;

   cin = (char *) malloc(nsegy);
   cout = (char *) malloc(nsegy);

   for (j = 0; j < ntrace; j++) {
      bcopy(cbuf + j * nsegy, cin, nsegy);
      trhdconvert(cin, cout);
      traceconvert(&cin[240], &cout[240], ns);
      bcopy(cout, cbuf + j * nsegy, nsegy);
   }
   free(cin);
   free(cout);
}
size_t   fread_cmh(char *cbuf, size_t chsize, FILE * infp)
{
   size_t   nread;
   char    *cmh_in;

   if (LINUX == 1) {
      cmh_in = (char *) malloc(chsize);
      nread = fread(cmh_in, 1, chsize, infp);
      if (nread == chsize) {
         cmhconvert(cmh_in, cbuf);
      } else {
         return nread;
      }
      free(cmh_in);
   } else {
      nread = fread(cbuf, 1, chsize, infp);
   }
   return nread;
}

/*--------------------------------------------------------------------*\
   Flop the halfwords to work around the fullword byte flip embedded 
   in the SeisPact seismic_compress() function.  After the byte flip
   everything will be in the correct place.

   Note for this to work the number of traces and trace length must be
   passed to seismic_compress() as it will not be able to read the 
   header fields correctly after header_flop() has been called.
\*--------------------------------------------------------------------*/

void header_flop( unsigned short* halfword ){

   unsigned short tmp;

   
   tmp           = halfword[ 13];
   halfword[ 13] = halfword[ 12];
   halfword[ 12] = tmp;

   tmp           = halfword[ 15];
   halfword[ 15] = halfword[ 14];
   halfword[ 14] = tmp;

   tmp           = halfword[ 33];
   halfword[ 33] = halfword[ 32];
   halfword[ 32] = tmp;

   tmp           = halfword[ 43];
   halfword[ 43] = halfword[ 42];
   halfword[ 42] = tmp;

   tmp           = halfword[ 45];
   halfword[ 45] = halfword[ 44];
   halfword[ 44] = tmp;

   tmp           = halfword[ 47];
   halfword[ 47] = halfword[ 46];
   halfword[ 46] = tmp;

   tmp           = halfword[ 49];
   halfword[ 49] = halfword[ 48];
   halfword[ 48] = tmp;

   tmp           = halfword[ 51];
   halfword[ 51] = halfword[ 50];
   halfword[ 50] = tmp;

   tmp           = halfword[ 53];
   halfword[ 53] = halfword[ 52];
   halfword[ 52] = tmp;

   tmp           = halfword[ 55];
   halfword[ 55] = halfword[ 54];
   halfword[ 54] = tmp;

   tmp           = halfword[ 57];
   halfword[ 57] = halfword[ 56];
   halfword[ 56] = tmp;

   tmp           = halfword[ 59];
   halfword[ 59] = halfword[ 58];
   halfword[ 58] = tmp;

   tmp           = halfword[ 61];
   halfword[ 61] = halfword[ 60];
   halfword[ 60] = tmp;

   tmp           = halfword[ 63];
   halfword[ 63] = halfword[ 62];
   halfword[ 62] = tmp;

   tmp           = halfword[ 65];
   halfword[ 65] = halfword[ 64];
   halfword[ 64] = tmp;

   tmp           = halfword[ 67];
   halfword[ 67] = halfword[ 66];
   halfword[ 66] = tmp;

   tmp           = halfword[ 69];
   halfword[ 69] = halfword[ 68];
   halfword[ 68] = tmp;

   tmp           = halfword[ 71];
   halfword[ 71] = halfword[ 70];
   halfword[ 70] = tmp;

   tmp           = halfword[ 73];
   halfword[ 73] = halfword[ 72];
   halfword[ 72] = tmp;

   tmp           = halfword[ 75];
   halfword[ 75] = halfword[ 74];
   halfword[ 74] = tmp;

   tmp           = halfword[ 77];
   halfword[ 77] = halfword[ 76];
   halfword[ 76] = tmp;

   tmp           = halfword[ 79];
   halfword[ 79] = halfword[ 78];
   halfword[ 78] = tmp;

   tmp           = halfword[ 81];
   halfword[ 81] = halfword[ 80];
   halfword[ 80] = tmp;

   tmp           = halfword[ 83];
   halfword[ 83] = halfword[ 82];
   halfword[ 82] = tmp;

   tmp           = halfword[ 85];
   halfword[ 85] = halfword[ 84];
   halfword[ 84] = tmp;

   tmp           = halfword[ 87];
   halfword[ 87] = halfword[ 86];
   halfword[ 86] = tmp;

   tmp           = halfword[101];
   halfword[101] = halfword[100];
   halfword[100] = tmp;

   tmp           = halfword[107];
   halfword[107] = halfword[106];
   halfword[106] = tmp;


}
