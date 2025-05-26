balclip(data,n,c)
float *data;
int n;
float c;
{
	float oc;
	oc = 1.0/c;
	while(n--) {
		*data *= oc;
		if( *data > 1.0 ) *data = 1.0;
		else if( *data < -1.0 ) *data = -1.0;
		data++;
	}
} 
