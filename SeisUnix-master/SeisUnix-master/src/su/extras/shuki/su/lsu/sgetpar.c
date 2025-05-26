int sgetpar(name, ptr)
char *name;
char **ptr;
{
	int i;		/* index of name in symbol table	*/
	int getparindex();	/* PARMS(char *name)		*/

	if (xargc == 1) return(0);
	if (first) getparinit();/* Tabulate command line and parfile */
	i = getparindex(name);	/* Get parameter index */
	if (i < 0) return(0);	/* Not there */

	/* Copy the argument */
	*ptr = argtbl[i].asciival;
	return(strlen(*ptr));
}
