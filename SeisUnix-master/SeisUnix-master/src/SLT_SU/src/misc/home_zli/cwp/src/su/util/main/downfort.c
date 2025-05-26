/*
 * DWONFORT - change Fortran programs to lower case, preserving strings
 *
 * Author: Brian Sumner
 */

#include <stdio.h>
#include <ctype.h>

main()
{

	register int c;
	while ((c = getchar()) != EOF) {
		if (c=='\n') putchar(c);
		else if (c=='*' || c=='C') {
			putchar(c);
			while((c = getchar()) != '\n') putchar(c);
			putchar(c);
		} else {
			putchar(c);
			while ((c = getchar()) != '\n') {
				if (c != '\'') putchar(isupper(c) ? tolower(c)
								  : c);
				else {
					putchar(c);
					if ((c = getchar()) != '(') {
						putchar(c);
						while((c = getchar()) != '\'')
							putchar(c);
						putchar(c);
					} else {
						putchar(c);
						while((c = getchar()) != '\'')
				 			putchar(isupper(c)
								   ? tolower(c)
								   : c);
						putchar(c);
					}
				}
			}
			putchar(c);
		}
	}
	fclose(stdout);
	exit(0);
}

