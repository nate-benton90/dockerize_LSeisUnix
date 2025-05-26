Here are some suggestion and scripst how to process 3D seismic data.

Step. 1 Read shotgathers and put them into a folder and assign header words

In: files from tape/disk + observer report/survey information Out: shot gathers with headers completed
Example: 01_header_a2b and 02_load_headers

This is probably the most time consuming and boring part. The shot gathers from tape/disk need to be converted to su files and header words assigned for each. The shost gathers are stored in a folder as individual files. For 3D processing the following parameters are put into the given header words. In most of the programs these are hard coded.

# Header assignments
# field record #        ->      fldr
# shot point #          ->      ep
# receiver point #      ->      sdepth
# receiver x coord      ->      swdep
# receiver y coord      ->      gwdep
# receiver Z elev       ->      gelev
# shot x coord          ->      sx
# shot y coord          ->      sy
# shot z elev           ->      selev
# offset                ->      offset
# azimuth		->      otrav
# multiplier for gelev selev sdepth gdel sdel swdep gwdep is in scalel
# scalel                ->      -1
# multiplier for sx, sy, gx, gy in scaleco
# scaleco               ->      -1
# counit                ->      1

The easiest is to create ascii headers for each shot than convert them to binary and load them into the header. 

The scripts 01_header_a2b and 02_load_headers shows examples of how to do this. The shot gathers with the header completed are stored in a separate folder. Bad receiver locations and shots can be filtered out by putting their number into text files and using suwind to filter them out from the stream.

Step. 2 Start processing

In: Shot gathers with header completed 	Out: Surface consistent decon operators
Example: 03_pc-1

First part of the processing is to correct for spherical divergence, limit offsets, assign bin numbers and run surface consistent deconvolution/designature.
With su3dbin the CDP bin numbers are assigned. In order to have a unique bin number for each CDP a composite number is created using the formula binx*1000+biny. For example bin(1,1) will have a number of 1001 bin(10,2) would be 10002 etc. This scheme works for surveys where biny is smaller than 1000. I do not know a better way of  

The CDP X and Y coordinates are computed and assigned to
#cdp x		 -> 		gx
#cdp y		 -< 		gy

The surface consistent decon modules solve for surface consistent effects for every shot point,receiver point, offset,azimuth CDP, and for the average component. The shot range and the azimuth range has to be "binned", the same way as one would do it in a histogram.
There is no output of this stream only the above solution. 


Step. 3  Apply surface consistent solution and spectral whitening

In: Shot gathers with header completed Out: Processed shot gathers
Example: 04_pc-2

The beginning of the script is the same as 03_pc-1 then the surface consistent decon is applied and the spectral whitening is done on the gathers.

 
