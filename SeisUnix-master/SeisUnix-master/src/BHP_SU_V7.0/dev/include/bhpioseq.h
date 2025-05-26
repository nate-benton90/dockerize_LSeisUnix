
/*

LICENSE FOR BHP SU Suite of Programs

The following is the license that applies to the copy of the software hereby
provided to Licensee. BHP's Software Manager may be contacted at the following
address:

Colorado School of Mines
1500 Illinois Street
Golden, Colorado 80401
Attention: John Stockwell
e-mail: john@dix.mines.edu
Telephone: 303-273-3049

Copyright 2001 BHP Software. All rights reserved.

Permission is hereby granted, free of charge, to any person obtaining a copy
of this software and associated documentation files (the "Software") to deal
in the Software, without restriction, except as hereinafter provided,
including without limitation the rights to use, copy, modify merge,
publish, and distribute the Software and to permit persons
to whom the Software is furnished to do so, provided that the above
copyright notice and this permission notice appear in all copies of the
Software and that both the above copyright notice and this permission
notice appear in supporting documentation. No charge may be made for
any redistribution of the Software, including modified or merged versions
of the Software. The complete source code must be included
in any distribution. For an executable file, complete source code means the
source code for all modules it contains.

Modified or merged versions of the Software must be provided to the Software
Manager, regardless of whether such modified or merged versions are
distributed to others.

THE SOFTWARE IS PROVIDED 'AS IS" WITHOUT WARRANTY OF ANY KIND,
EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF
MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND
NONINFRINGMENT OF THIRD PARTY RIGHTS. IN NO EVENT SHALL THE
COPYRIGHT HOLDER INCLUDED IN THIS NOTICE BE LIABLE FOR ANY CLAIM OR
ANY SPECIAL INDIRECT OR CONSEQUENTIAL DAMAGES, OR ANY DAMAGES
WHATSOEVER RESULTING FROM LOSS OF USE, DATA OR PROFITS, WHETHER
IN AN ACTION OF CONTRACT, NEGLIGENCE OR OTHER TORTIOUS ACTION,
ARISING OUT OF OR IN CONNECTION WITH THE USE OF PERFORMANCE OF
THIS SOFTWARE.

The name of the copyright holder shall not be used in advertising or
otherwise to promote the use or other dealings in this Software, without
prior written consent of the copyright holder.

*/
/* structures */
/* Header limits */
typedef struct {
  char bhp_hdr_name[8];  /* Header name */
  int bhp_hdr_min;       /* Minimum key value for a header */
  int bhp_hdr_max;       /* Maximum key value for a header */
  int bhp_hdr_inc;       /* Key value increment(R) or nominal increment(I) */
  int bhp_hdr_scalar;    /* Scalar which needs to be applied to get actual values */
  int bhp_hdr_num;       /* Number of key values written */
  int bhp_hdr_order;     /* Write order; -1=random, 0=constant, 1=incr, 2=decr */
  int bhp_hdr_type;      /* Regular(1) or Irregular(0) */
  char bhp_hdr_data[2];  /* Header data type; i=int,f=float,h=short,u=unsigned short */
  int bhp_hdr_vlen;      /* Length of corresponding header key vector (floats) */
  int bhp_hdr_vloc;      /* Byte address in vector file of this key vector */
  } bhp_hdr_limits;

/* Globals */
bhp_hdr_limits *hdr_limits;    /* Header-limits table */

/* Prototypes */
int read_hdr_limits(char *path, int index, char *key, int verbose);
void get_trace(FILE **fp, segy *trace, int k1, int k2, int *v1, int vlen1, int *v2, int vlen2,
      int nsamp, int endian_in, int endian, int verbose);
void shutdown(char *errmsg, char **names, int *vals, char *file, int count, FILE *fp,
              char *cpath, int verbose, int debug);
