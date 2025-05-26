/* valpkge - routines to handle variables of type value
 *
 * vtoi       - cast value variable as an int
 * vtol       - cast value variable as a long
 * vtof       - cast value variable as a float
 * vtoz       - cast value variable as a double
 * valcmp     - compare value variables
 * printfval  - printf a value variable
 * fprintfval - fprintf a value variable
 * scanfval   - scanf a value variable
 * printftype - printf for the type of a segy header word
 *
 * Returns:
 *	vtoi      : int
 *	vtol      : long
 *	vtof      : float
 *	vtoz      : double
 *	valcmp    : int
 *	printfval : void
 *	fprintfval: void
 *	scanfval  : void
 *	printftype: void
 *
 * Examples:
 *	scanfval(hdtype(key), &val);
 *	printfval(hdtype(key), val); putchar('\n');
 *	fval = vtof(hdtype(key), val);
 *
 * Synopsis:
 *	int vtoi(type, val);
 *	char *type;
 *	value val;
 *
 *	long vtol(type, val);
 *	char *type;
 *	value val;
 *
 *	float vtof(type, val);
 *	char *type;
 *	value val;
 *
 *	double vtoz(type, val);
 *	char *type;
 *	value val;
 *
 *	int valcmp(val1, val2)
 *	value val1, val2;
 *
 *	void printfval(type, val);
 *	char *type;
 *	value val;
 *
 *	void fprintfval(stream, type, val);
 *	FILE stream;
 *	char *type;
 *	value val;
 *
 *	void scanfval(type, valp);
 *	char *type;
 *	value *valp;
 *
 *	void printftype(key);
 *	char *key;
 *
 * Credits:
 *	CWP: Jack, Shuki
 *
*/

#include <stdio.h>
#include "../include/su.h"


int vtoi(type, val)
char *type;
value val;
{
	switch(*type) {
		case 's':
		case 'c': return( (int) val.s[0] );
		case 'h': return( (int) val.h );
		case 'u': return( (int) val.u );
		case 'l': return( (int) val.l );
		case 'v': return( (int) val.v );
		case 'i': return(       val.i );
		case 'f': return( (int) val.f );
		case 'z': return( (int) val.z );
		default: err(__FILE__,__LINE__,"vtoi: unknown type %s", type);
			 return(0);	/* for lint */
	}
}

long vtol(type, val)
char *type;
value val;
{
	switch(*type) {
		case 's':
		case 'c': return( (long) val.s[0] );
		case 'h': return( (long) val.h );
		case 'u': return( (long) val.u );
		case 'l': return(        val.l );
		case 'v': return( (long) val.v );
		case 'i': return( (long) val.i );
		case 'f': return( (long) val.f );
		case 'z': return( (long) val.z );
		default: err(__FILE__,__LINE__,"vtol: unknown type %s", type);
			 return(0L);	/* for lint */
	}
}


float vtof(type, val)
char *type;
value val;
{
	switch(*type) {
		case 's':
		case 'c': return( (float) val.s[0] );
		case 'h': return( (float) val.h );
		case 'u': return( (float) val.u );
		case 'l': return( (float) val.l );
		case 'v': return( (float) val.v );
		case 'i': return( (float) val.i );
		case 'f': return(         val.f );
		case 'z': return( (float) val.z );
		default: err(__FILE__,__LINE__,"vtof: unknown type %s", type);
			 return(0.0);	/* for lint */
	}
}

double vtoz(type, val)
char *type;
value val;
{
	switch(*type) {
		case 's':
		case 'c': return( (double) val.s[0] );
		case 'h': return( (double) val.h );
		case 'u': return( (double) val.u );
		case 'l': return( (double) val.l );
		case 'v': return( (double) val.v );
		case 'i': return( (double) val.i );
		case 'f': return( (double) val.f );
		case 'z': return(          val.z );
		default: err(__FILE__,__LINE__,"vtoz: unknown type %s", type);
			 return(0.0);	/* for lint */
	}
}


int valcmp(type, val1, val2)
char *type;
value val1, val2;
{
	switch(*type) {
		case 's':
		case 'c': return( strcmp(val1.s, val2.s) );
		case 'h':
			if      ( val1.h < val2.h ) return(-1);
			else if ( val1.h > val2.h ) return(1);
			else                        return(0);
		case 'u':
			if      ( val1.u < val2.u ) return(-1);
			else if ( val1.u > val2.u ) return(1);
			else                        return(0);
		case 'l':
			if      ( val1.l < val2.l ) return(-1);
			else if ( val1.l > val2.l ) return(1);
			else                        return(0);
		case 'v':
			if      ( val1.v < val2.v ) return(-1);
			else if ( val1.v > val2.v ) return(1);
			else                        return(0);
		case 'i':
			if      ( val1.i < val2.i ) return(-1);
			else if ( val1.i > val2.i ) return(1);
			else                        return(0);
		case 'f':
			if      ( val1.f < val2.f ) return(-1);
			else if ( val1.f > val2.f ) return(1);
			else                        return(0);
		case 'z':
			if      ( val1.z < val2.z ) return(-1);
			else if ( val1.z > val2.z ) return(1);
			else                        return(0);
		default: err(__FILE__,__LINE__,"valcmp: unknown type %s", type);
			 return(0);	/* for lint */
	}
}


void printfval(type, val)
char *type;
value val;
{
	switch(*type) {
	case 's':
	case 'c':
		printf("%s", val.s);
	break;
	case 'h':
		printf("%d", val.h);
	break;
	case 'u':
		printf("%d", val.u);
	break;
	case 'l':
		printf("%ld", val.l);
	break;
	case 'v':
		printf("%ld", val.v);
	break;
	case 'i':
		printf("%d", val.i);
	break;
	case 'f':
		printf("%f", val.f);
	break;
	case 'z':
		printf("%f", val.z);
	break;
	default:
		err(__FILE__,__LINE__,"printfval: unknown type %s", type);
	}
}


void fprintfval(stream, type, val)
FILE *stream;
char *type;
value val;
{
	switch(*type) {
	case 's':
	case 'c':
		fprintf(stream, "%s", val.s);
	break;
	case 'h':
		fprintf(stream, "%d", val.h);
	break;
	case 'u':
		fprintf(stream, "%d", val.u);
	break;
	case 'l':
		fprintf(stream, "%ld", val.l);
	break;
	case 'v':
		fprintf(stream, "%ld", val.v);
	break;
	case 'i':
		fprintf(stream, "%d", val.i);
	break;
	case 'f':
		fprintf(stream, "%f", val.f);
	break;
	case 'z':
		fprintf(stream, "%f", val.z);
	break;
	default:
		err(__FILE__,__LINE__,"fprintfval: unknown type %s", type);
	}
}


void scanfval(type, valp)
char *type;
value *valp;
{
	switch(*type) {
	case 's':
	case 'c':
		scanf("%s", valp);
	break;
	case 'h':
	case 'u':
		scanf("%hd", valp);
	break;
	case 'l':
	case 'v':
		scanf("%ld", valp);
	break;
	case 'f':
		scanf("%f", valp);
	break;
	case 'z':
		scanf("%lf", valp);
	break;
	default:
		err(__FILE__,__LINE__,"scanfval: unknown type %s", type);
	}
}


void printftype(key)
char *key;
{
	switch(*hdtype(key)) {
	case 's':
	case 'c':
		printf("char\n");
	break;
	case 'h':
		printf("short\n");
	break;
	case 'u':
		printf("unsigned short\n");
	break;
	case 'l':
		printf("long\n");
	break;
	case 'v':
		printf("unsigned long\n");
	break;
	case 'i':
		printf("int\n");
	break;
	case 'f':
		printf("float\n");
	break;
	case 'z':
		printf("double\n");
	break;
	default:
		err(__FILE__,__LINE__,"printftype: unknown type %s", hdtype(key));
	break;
	}
}
