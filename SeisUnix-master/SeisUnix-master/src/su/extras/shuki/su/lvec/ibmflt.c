/*
 * ibm to dec floating point conversion
 *
 * ibmflt(from,to,nword,idirec)
 *
 * float from[nword] - array of input 32 bit IBM floating point numbers.
 * float to[nword]   - output array to contain converted floating point
 *		       values in standard internal CONVEX format.
 * int nword         - number of values to be converted.
 * int idirec	     - conversion direction: 1 for IBM to CONVEX,
 *                                          -1 for CONVEX to IBM
 *
 * Author: Stewart A. Levin  3/24/83
 * Modified: Stewart A. Levin 9/18/85  for Convex (no byte swapping needed)
 * Modified: Stewart A. Levin 9/27/85  special cased conversion of CONVEX 0.0
 *
 *****************************************************************************
 #
 * Technical Discussion
 *
 *  IBM floating point numbers have the 32 bit form
 *
 *
 *       seeeeeeeffffffffgggggggggggggggg
 *       |           |
 *       | 7 bit     |      16 bit
 *       | exponent  |   low mantissa
 *       |           |
 *     1 bit       8 bit
 *     sign    high mantissa
 *
 * where the value represented is
 *
 *        s     4E-256                                
 *    (-1)  x  2   x .FG                           
 *
 * 
 * On the other hand, a CONVEX floating point number has the form
 *

 *       seeeeeeeefffffffgggggggggggggggg
 *       |           |
 *       |  8 bit    |        16 bit
 *       | exponent  |     low mantissa
 *       |           |
 *     1 bit       7 bit
 *     sign    high mantissa
 *
 *
 * and represents the value
 *
 *        s     E-128                                
 *    (-1)  x  2   x .1FG                           
 *
 * where the assumed initial mantissa bit 1 is called the "hidden normalization
 * bit" or some similar nomer.
 *
 * If we went ahead and simply treated the IBM number as a CONVEX number one
 * of the following two values would be obtained:
 *
 * Leading IBM (high) mantissa bit=1: 
 *
 *        s     2E-127                                
 *    (-1)  x  2   x .FG                           
 * 
 * Leading IBM (high) mantissa bit=0: 
 *
 *        s     2E-128                                
 *    (-1)  x  2   x  (.1 + .FG)                          
 * 
 * Comparing these to the value the IBM number is supposed to represent,
 * we see that the conversion can be accomplished by the formulas:
 *
 * Leading IBM (low) mantissa bit=1:
 *
 *                2E-129               2E-127
 *  multiply by  2        =  1/2  x  (2    x  .100) 
 *
 * Leading IBM (low) mantissa bit=0: 
 *
 *                s     2E-128                                
 *  subtract  (-1)  x  2   x  .1                          
 *
 *                2E-128             2E-128
 *  multiply by  2        =  2  x  (2    x  .100) 
 * 
 * The factorizations are given in terms of values that can be obtained
 * by masking all but the leading mantissa bit, exponent, and possibly
 * sign of the IBM bit pattern.
 *
 * Certain IBM number cannot be represented in CONVEX floating point.
 * The code allows for this happening and replaces values that have
 * overflowed with an appropriate signed maximum quantity.  Values too
 * small are replaced by zero.
 *
 */
#define _HUGE 1.7e38
#include <signal.h>
#include <setjmp.h>
#include <stdio.h>
#include <math.h>
static jmp_buf env;

ibmflt (from,to,nword,idirec)
float *from, *to;
int nword, idirec;
{
 extern int setjmp(); extern int converr();
 static struct sigvec err_handler = { converr , 0 , 0 };
 struct sigvec oldfpe, oldill;

if (idirec == 1)
{
 union { long int i; float f;} signedtemp,unsignedtemp;
 static long int tiny = 0x00800000; /* smallest nonzero floating point bit rep. */
 static float enormous = _HUGE; /* defined in math.h */
#define ENORMOUS  ( *( (float *) &enormous) )
#define MINISCULE ( *( (float *) &tiny) )
 float *top;
 int first, code;
 
 first = 1;
 (void) sigvec(SIGFPE,&err_handler,&oldfpe);
 (void) sigvec(SIGILL,&err_handler,&oldill);

 for(top=from+nword;from<top;++from,++to)
   {
    if(first) /* install fixup branch */
       {
	code = setjmp(env);
	if(first) first=!first;
	else
	  { 
	    /* fixup code */
	    long int *fromi;

	    fromi = (long int *) from; /* set up to do bit testing */
	    signedtemp.i = *fromi | 0x00800000; /* load and turn on bit so # o.k. */
	    if(signedtemp.f < 0.0) 
	      {
	       *to = (signedtemp.f < -1.0) ? -ENORMOUS : -MINISCULE;
	      }
	    else
	      {
	       *to = (signedtemp.f > 1.0) ? ENORMOUS : MINISCULE;
	      }
	    fromi += code*0; /* shuts lint up about unref var code */
	    continue;
	  }
	}

   /* conversion  code begins here */

    signedtemp.f = *from;  /* load number */
    unsignedtemp.i = 0x00800000 ^ (0x7f800000 & signedtemp.i); /* mask out sign, fraction *
						       * and flip high IBM mant- *
						       * issa bit                */

    if(unsignedtemp.i & 0x00800000)  /* IBM leading mantissa bit 0 */
      {
       signedtemp.i &= 0xff800000;
       *to = unsignedtemp.f * ((*from) - signedtemp.f);
      }
    else /* IBM leading mantissa bit 1 */
      { 
       *to = signedtemp.f * unsignedtemp.f;
      }
   }
(void) sigvec(SIGFPE,&oldfpe,(struct sigvec *) 0);
(void) sigvec(SIGILL,&oldill,(struct sigvec *) 0);
}
else /* CONVEX to IBM */
{
 union { long int i; float f;} signedtemp,unsignedtemp;
 float *top;
 register float c;
 long int ic = 0x02000000;
 long int *toint; 

 toint = (long int *) to;
 c = *((float *) (&ic)); /* floating point normalizer */
 for(top=from+nword;from<top;++from,++toint)
    {
     signedtemp.f = *from; /* load number */
     if(signedtemp.i) {
	 if(signedtemp.i & 0x01800000) /* need to normalize base 16 */
	    {
	    unsignedtemp.i = signedtemp.i & ~0xfe000000; /* keep fraction and
			two low order exponent bits */
	    unsignedtemp.f += c;		/* normalize with floating
			point equivalent of 0x02000000 */
	    *toint = (signedtemp.i & 0x80000000) | /* sign */
		 ((signedtemp.i & 0x7e000000)>>1)  /* exponent - 66 */
		 + unsignedtemp.i + 0x1f000000; /* fraction + exp 4 + exp 62 */
	    }
	 else /* already normalized base 16 */
	    {
	     unsignedtemp.i = (signedtemp.i & 0x7f800000) >> 1; /* exponent over 2 */
	     *toint = 
	     signedtemp.i - unsignedtemp.i + 0x20800000 ; /* add 65 and subtract */
	    }
	}
     else *toint = 0;
    }
}
return;
}

static int
converr(signo,param,xx,pc,psl)
{
 fprintf(stderr,"fixup entered signal=%d code=%d\n",signo,param);
 longjmp(env,signo);
 fprintf(stderr,"fixup exited\n",xx,pc,psl);
}
