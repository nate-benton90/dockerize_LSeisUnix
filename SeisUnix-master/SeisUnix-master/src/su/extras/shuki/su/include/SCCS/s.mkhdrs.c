h09556
s 00000/00000/00065
d D 1.2 88/11/15 14:01:20 shuki 2 1
c 
e
s 00065/00000/00000
d D 1.1 88/04/14 14:05:21 shuki 1 0
c date and time created 88/04/14 14:05:21 by shuki
e
u
U
f e 0
t
T
I 1
/* mkhdrs - makes hdrs.h file from output of prehdr and getoffs shells */

#include "su.h"

main()
{
	int i;
	int HDRBYTES = sizeof(Sutrace) - sizeof(float*);
	char buff[16];
	Sutrace tr;
	Subhed bh;
	int TR_NK,BH_NK;
	struct s_hdr {
		char key[64],type[16];
		int offs;
	} *trhdr,*bhdr;

	trhdr = (struct s_hdr*) malloc(HDRBYTES*sizeof(struct s_hdr));

#include "trhdr.h"

	/* Print out hdr.h on stdout */
	printf("/*\n * hdrs.h - include file for su header arrays\n */\n\n");
	printf("#ifndef INCLUDE_HDR_H\n");
	printf("#define INCLUDE_HDR_H\n\n");
	printf("#define HDRBYTES\t%d\t/* Bytes in the trace header */\n",
								HDRBYTES);
	printf("#define TR_NK\t\t%d\t/* Number of key trace header words */\n",
									TR_NK);
	printf("static struct trhdr_tag {\n");
	printf("\tchar *key;\tchar *type;\tint offs;\n");
	printf("} trhdr[] = {\n");
	buff[0] = '"';			/* buff is for alligned output */
	for (i = 0; i < TR_NK; i++) {
		buff[1] = (char)0;
		strcpy(buff+1,trhdr[i].key);
		strcat(buff,"\"");
		printf("\t%10s,\t\t\"%c\",\t\t%d,\n",
			buff, trhdr[i].type[0], trhdr[i].offs);
	}
	printf("};\n");

	bhdr = trhdr;		/* PATCH */

#include "bhdr.h"

	printf("#define BH_NK\t\t%d\t/* Number of key binary header words */\n",
									BH_NK);
	printf("static struct bhdr_tag {\n");
	printf("\tchar *key;\tchar *type;\tint offs;\n");
	printf("} bhdr[] = {\n");
	buff[0] = '"';			/* buff is for alligned output */
	for (i = 0; i < BH_NK; i++) {
		buff[1] = (char)0;
		strcpy(buff+1,bhdr[i].key);
		strcat(buff,"\"");
		printf("\t%10s,\t\t\"%c\",\t\t%d,\n",
			buff, bhdr[i].type[0], bhdr[i].offs);
	}
	printf("};\n");

	printf("#endif INCLUDE_HDR_H\n");

	exit(0);
}
E 1
