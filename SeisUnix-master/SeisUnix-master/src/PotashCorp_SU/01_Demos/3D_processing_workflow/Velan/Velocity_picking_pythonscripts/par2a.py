#! /usr/bin/env python
# Python script to convert back parfile format to ascii
# Read stdin
from string import split,atof
from re import sub

i=0
while 1:
	try:
		t = raw_input()	 # read a line
		v = raw_input()	 # read a line
	except EOFError, e: break
t=sub('tnmo=','',t)
t=sub(',',' ',t)
times = split(t)
v=sub('vnmo=','',v)
v=sub(',',' ',v)
vels = split(v)
i=0
for vel in  vels:
	print times[i],vel
	i=i+1
