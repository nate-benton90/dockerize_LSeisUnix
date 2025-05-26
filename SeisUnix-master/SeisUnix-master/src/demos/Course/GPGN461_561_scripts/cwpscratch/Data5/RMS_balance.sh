#! /bin/sh
 
set -x

# RMS_balance - balance the amplitudes by the rms power of the first arrival on
# the nearest offset trace. 


infile=shot_gathers.su 		# input shot gathers
outfile=rmsbal_$infile		# RMS balanced 

# remove output file, temporary files
rm $outfile
rm $tempout
rm temp.ascii

# split the original data into shot gathers
# split shot data
susplit < $infile key=ep  


# loop over shot gather files
for i in `ls split_* `
do
	# apply rms power balance to each gather
	# get the RMS power of the first arrival on the nearest trace
        suwind key=offset min=-262 max=-262 tmin=.48 tmax=.56 < $i |
	sumax output=binary mode=rms | b2a n1=1 > temp.ascii
	
	# normalize each trace in the $i-th gather with the RMS value
	scale=`cat temp.ascii`
	echo $scale
	sushw key=ungpow a=$scale < $i |
	suhtmath key=ungpow op=div >> $outfile
done

## clean up
# remove shot split files
rm split*
rm temp.ascii

exit 0
