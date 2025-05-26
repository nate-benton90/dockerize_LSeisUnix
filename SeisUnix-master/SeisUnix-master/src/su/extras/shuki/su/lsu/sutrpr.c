char *SccsId="%W% %G%\n";
sutrpr(atr,n)
Sutrace *atr;
int n;
{
	int i;
	fprintf(stderr,"---------------- tracl=%d ----------------\n",atr->tracl);
	for(i=0;i<n;i++) {
		fprintf(stderr,"trace[%d].data[%d] = %e\n",atr->tracl,i,atr->data[i]);
	}
}
