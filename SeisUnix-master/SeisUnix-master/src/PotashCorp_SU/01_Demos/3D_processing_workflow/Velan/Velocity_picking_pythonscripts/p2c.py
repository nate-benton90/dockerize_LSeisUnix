#! /usr/bin/env python
# Python script to compute moveout curve from picks Pick2Curve

from string import split,atoi,atof
from math import sqrt
import sys

# This script reads the stdin and writes to files
# Input a series of t,v pairs of floats representig time and velocity picks
# Output digitized moveout curve for each velocity pick sampled at given offset
# intervall -> files mc1.....mc4
# also the zero offset time lines are output t0 -1.0e10, t0 1.0e10
# -> c1.....c4
# Name of the moveout curve files -> moveoutc.txt
# Number of ponts in each -> npairsc.txt
# Name of tline files -> tlines.txt
#Number of ponts in each -> npairst.txt
def f_nmo(minx,maxx,dx,v0,t0):
	t02 = t0*t0
	ov2 = 1.0/(v0*v0)
	n = (maxx-minx)/dx
	list=''
	for x in xrange(minx,maxx,dx) :
		t = sqrt( t02+ov2*x*x)
		list=list+str('%10.3f'%t)+str('%10.3f\n'%x)
	return(list,n)

def f(t0):
	t=t0+toff
	list=str('%10.3f'%t)+' 0.0\n'+str('%10.3f'%t)+str('%10.3f'%maxx)
	return(list,2)
		
dx=50
minx=0
maxx=2000
toff=0.01
i=0
files=''
npairs=''
filesn=''
npairsn=''
# Read stdin
while 1:
	try:
		l = raw_input()	 # read a line
		i=i+1
	except EOFError, e: break
	list = split(l)
	t = list[0]
	v = list[1]

	lout,n=f_nmo(minx,maxx,dx,atof(v),atof(t))
	loutn,nn=f(atof(t))

	fname='mvc'+str(i)
	fnamen='t'+str(i)

	fp=open(fname,'w')
	fp.write(lout)
	fp.close()

	fp=open(fnamen,'w')
	fp.write(loutn)
	fp.close()

	if files=='': 
		files=fname
		npairs=str(n)
	else:
		files=files+','+fname
		npairs=npairs+','+str(n)

	if filesn=='': 
		filesn=fnamen
		npairsn=str(nn)
	else:
		filesn=filesn+','+fnamen
		npairsn=npairsn+','+str(nn)
		
if i==0 : sys.exit(1)
fp=open('moveoutc.txt','w')
fp.write(files)
fp.close()

fp=open('npairsc.txt','w')
fp.write(npairs)
fp.close()

fp=open('tlines.txt','w')
fp.write(filesn)
fp.close()

fp=open('npairst.txt','w')
fp.write(npairsn)
fp.close()
