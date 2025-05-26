
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
 
#include <stdio.h>

/*===================================================================================
  Defines for dimensions an error codes
  ========================================================-----===========================*/

#define STL     132
#define MERRORS  50
#define CBUFF  1000

#define ISCOMMENT 100
#define ISTAG     102
#define ISENDTAG  103
#define ISKEYA    110
#define ISKEYB    111
#define ISKEYC    112
#define ISKEYD    113
#define ISKEYF    114
#define ISKEYI    115
#define ISKEYU    116  /* unsigned char */

/*
  static char *str_void;
  static char *str_bool;
  static char *str_byte;
  static char *str_int;
  static char *str_float;
  static char *str_double;
  static char *str_char; 

  static char *str_array;

  static char *str_initbin;
  static char *str_initasc;
*/
  
/*========================================================================================
  Methods to open and close files
  ========================================================================================*/

  void initRoffRead();
  void release();
  int  openFile (char *sname,char* mode);
  int  closeFile(); 
  int  readNextItem( int* itype, char* stype
                    ,char*** cd,int** id, unsigned char** ud,float** fd,double** dd,int* nd);

/*========================================================================================
  getLastErrorMessage: returns pointer to the last error message as null terminated string
  ========================================================================================*/

  char* getLastErrorMessage();

/*=======================================================================================
  getLastError: returns the error value (0 implies no error)
  =======================================================================================*/

  int   getLastError       ();

/*=======================================================================================
  getErrorMessage: gievn error value, returns error description 
  =======================================================================================
  ierror - input - integer error value (returned by soffFile members or getLastError
  =======================================================================================*/
  
  char* getErrorMessage(int ierror);
  
/*========================================================================================
  All remaining methods are private and not user callable
  ========================================================================================*/
 int getArgS(char* s,int ms);
 int getArgI(int*    ia);
 int getString(char* s   ,int* ns,int ms);

 int readtoNonDelim(char* c);

 int readtoChar    (char  c   ,char* s,int* ns,int ms);
 int readtoDelim   (           char* s,int* ns,int ms);

 int setError(int ierror);

 int breadi1 (unsigned char* uvals,int nitem);
 int breadi4 (int*    ivals,int nitem);
 int breadf4 (float*  fvals,int nitem);
 int breadd8 (double* dvals,int nitem);

 int readDataB(unsigned char* data,int number);
 int readDataC(char**  data,int number);
 int readDataD(double* data,int number);
 int readDataI(int*    data,int number);
 int readDataU(unsigned char* data,int number);
 int readDataF(float*  data,int number);

 int readIsBinary(int* binary);

 int isThisVoid  (char* name);
 int isThisBool  (char* name);
 int isThisChar  (char* name);
 int isThisFloat (char* name);
 int isThisInt   (char* name);
 int isThisDouble(char* name);
 int isThisByte  (char* name);
  
 int  compare(char* a,char* b);

 int   nErrors_;               /* Number of error codes                      */
 int   errorFlag_  [MERRORS];  /* List of error codes set by soffFile        */
 char* errorString_[MERRORS];  /* List of associated error messages          */

 int   fileIsOpen_;
 int   inTag_;
 int   nvalues_;
 char  cvalues_[STL];
 int   nwritten_;

 int   binary_;
 char  fname_[STL];

 int   lastError_;
 char  lastErrorMessage_[STL];

 FILE* file_;

 int     ncbuf_;
 char**  cbuf_;
 int*    ibuf_;
 float*  fbuf_;
 double* dbuf_;
 unsigned char *ubuf_;
 
 char *str_void   = "void";
 char *str_bool   = "bool";
 char *str_byte   = "byte";
 char *str_int    = "int";
 char *str_float  = "float";
 char *str_double = "double";
 char *str_char   = "char";

 char *str_array  = "array";

/*
 char *str_initbin = "soff-bin";
 char *str_initasc = "soff-asc";
*/
 char *str_initbin = "roff-bin";
 char *str_initasc = "roff-asc";


  /* RoffInfo structure */
  typedef struct {
    int nx;           /* # cells in X direction */
    int ny;           /* # cells in Y direction */
    int nz;           /* # cells in Z direction */
    float xo;         /* X-offset */
    float yo;         /* Y-offset */
    float zo;         /* Z-offset */
    float xscale;     /* X-scale */
    float yscale;     /* Y-scale */
    float zscale;     /* Z-scale */
    long ocL;         /* byte offset of cL array */
    int ncL;          /* # floats in cL array */
    long ocP;         /* byte offset of cP array */
    int ncP;          /* # floats in cP array */
    long osplitEnz;   /* byte offset of splitEnz array */
    int nsplitEnz;    /* # bytes in splitEnz array */
    long oactive;     /* byte offset of active array */
    int nactive;      /* # bytes in active array */
    int nProp;        /* # properties */
  } RoffInfo;
