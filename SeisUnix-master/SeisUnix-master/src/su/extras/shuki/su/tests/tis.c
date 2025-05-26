int xargc; char **xargv;
main(ac,av)
int ac; char **av;
{
	xargc = ac; xargv = av;
	printf("%d\n",isapipe(0));
}
