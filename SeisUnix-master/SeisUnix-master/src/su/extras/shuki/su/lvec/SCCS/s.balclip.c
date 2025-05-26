h24072
s 00000/00000/00014
d D 1.2 88/11/15 14:03:55 shuki 2 1
c 
e
s 00014/00000/00000
d D 1.1 88/04/14 13:49:47 shuki 1 0
c date and time created 88/04/14 13:49:47 by shuki
e
u
U
f e 0
t
T
I 1
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
E 1
