
#if __STD_C
ssize_t sfvaprints(char** sp, const char* form, va_list args)
#else
ssize_t sfvaprints(sp, form, args)
char**	sp;
char*	form;
va_list	args;
#endif
{
	char	*s;
	ssize_t	n;

	if(!sp || !(s = sfvprints(form,args)) )
		return -1;
	else
	{	if(!(*sp = (char*)malloc(n = strlen(s)+1)) )
			return -1;
		memcpy(*sp, s, n);
		return n-1;
	}
}
