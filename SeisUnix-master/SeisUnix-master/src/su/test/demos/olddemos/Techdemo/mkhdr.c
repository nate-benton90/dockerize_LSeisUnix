/* mkhdr - makes hdr.h file from output of mkprehdr.sh and mkoffs.sh
 *
 * Credits:
 *	CWP: Jack, Shuki
 *
 * This program belongs to the Center for Wave Phenomena
 * Colorado School of Mines
 *
 * $Author: jkc $
 * $Source: /src/su/include/RCS/mkhdr.c,v $
 * $Revision: 1.18 $ ; $Date: 88/11/19 22:35:10 $
*/

/* Embed Revision Control System identifier strings */
static char progid[] =
	"   $Source: /src/su/include/RCS/mkhdr.c,v $";
static char revid[] =
	"   $Revision: 1.18 $ ; $Date: 88/11/19 22:35:10 $";

#include "segy.h"
#include "cwp.h"
#include "prehdr.h"

segy tr;

main()
{
	int i;
	char buf[16];	/* buf is used to align the output fields */
	static STRICT_ALIGN _aligned = 0;  /* for offset macro */
	int SY_NK = DIM(hdr);
	int MAXSEGY = sizeof(segy);
	int HDRBYTES = MAXSEGY - SY_NDAT*sizeof(float);


	/* hdr[].offs has been initialized to zero by the prehdr shell */

#include "offsets.h"

	/* Print out hdr.h on stdout */
	printf("/*\n * hdr.h - include file for segy offset array\n");
	printf(" * THIS HEADER FILE IS GENERATED AUTOMATICALLY - \n");
	printf(" * see the makefile in this directory\n */\n\n");
	printf("#ifndef HDR_H\n");
	printf("#define HDR_H\n\n");
	printf("#define SY_NK\t\t%d\t/* Number of key header words */\n",
			SY_NK);
	printf("#define HDRBYTES\t%d\t/* Bytes in the trace header */\n",
			HDRBYTES);
	printf("#define	MAXSEGY\t\t%d\n\n", MAXSEGY);
	printf("static struct {\n");
	printf("\tchar *key;\tchar *type;\tint offs;\n");
	printf("} hdr[] = {\n");
	buf[0] = '"';
	for (i = 0; i < SY_NK; i++) {
		strcpy(buf+1, hdr[i].key);
		strcat(buf,"\"");
		printf("\t%10s,\t\t\"%s\",\t\t%d,\n",
			buf, hdr[i].type, hdr[i].offs);
	}
	printf("};\n");
	printf("#endif\n");
}
