#! /bin/sh
# Constant-velocity stack of a range of cmp gathers
# Authors: Jack, Ken
# NOTE: Comment lines preceeding user input start with  #!#
set -x

#!# Set input/output file names and data parameters
input=cdp371to380
stackdata=cvstack
cdpmin=371 cdpmax=380
fold=30
space=6		# 6 null traces between panels

#!# Determine velocity sampling.
vmin=1500   vmax=4000   dv=250


### Determine ns and dt from data (for sunull)
nt=`sugethw ns <$input | sed 1q | sed 's/.*ns=//'`
dt=`sugethw dt <$input | sed 1q | sed 's/.*dt=//'`
### Convert dt to seconds from header value in microseconds
dt=`bc -l <<-END
	$dt / 1000000
END`


### Do the velocity analyses.
>$stackdata  # zero output file
v=$vmin
while [ $v -le $vmax ]
do
	cdp=$cdpmin
	while [ $cdp -le $cdpmax ]
	do
		suwind <$input key=cdp min=$cdp max=$cdp count=$fold |
		sunmo cdp=$cdp vnmo=$v tnmo=0.0 |
		sustack >>$stackdata
		cdp=`expr $cdp + 1`
	done
	sunull ntr=$space nt=$nt dt=$dt >>$stackdata
	v=`expr $v + $dv`
done


### Plot the common velocity stacked data
ncdp=`expr $cdpmax - $cdpmin + 1`
f2=$vmin
d2=`bc -l <<-END
	$dv/($ncdp + $space)
END`

sugain <$stackdata tpow=2.0 gpow=.5 |
suximage perc=99 f2=$f2 d2=$d2 \
	title="File: $input  Constant-Velocity Stack " \
	label1="Time (s)"  label2="Velocity (m/s)" & 
