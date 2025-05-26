/*
 * UPFORT - change Fortran programs to upper case, preserving strings
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
		else if (c=='*' || c=='C' || c=='c' || c=='D') {
			putchar(c);
			while((c = getchar()) != '\n') putchar(c);
			putchar(c);
		} else {
			putchar(c);
			while ((c = getchar()) != '\n') {
				if (c != '\'') putchar(islower(c) ? toupper(c)
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
				 			putchar(islower(c)
								 ? toupper(c)
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

