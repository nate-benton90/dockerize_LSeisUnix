# display.spec --
#
#	Tksu display category modules.
#	See README.spec for a description of the spec file format.
#
# CVS: $id$

# The shared sections holds parameters shared by many of the display modules.

[shared-axes1]

Param:	xbox int - 50		x position of upper left corner of window
Param:	ybox int - 50		y position of upper left corner of window
Param:	wbox int 0< 550		width of window in pixels
Param:	hbox int 0< 700		height of window in pixels

Param:	x1beg float - "min x1"	axis 1 beginning value
Param:	x1end float - "max x1"	axis 1 ending value
Param:	d1num float 0<= 0	axis 1 numbered tic interval (0 for auto)
Param:	f1num float - "min x1"	axis 1 first numbered tic (if not auto)
Param:	n1tic int 1<= 1		axis 1 number of tics per numbered tic
Param:	grid1 enum-grid - none	axis 1 choice of grid lines
Param:	label1 string - none	axis 1 label

Param:	x2beg float - "min x2"	axis 2 beginning value
Param:	x2end float - "max x2"	axis 2 ending value
Param:	d2num float 0<= 0	axis 2 numbered tic interval (0 for auto)
Param:	f2num float - "min x2"	axis 2 first numbered tic (if not auto)
Param:	n2tic int 1<= 1		axis 2 number of tics per numbered tic
Param:	grid2 enum-grid - none	axis 2 choice of grid lines
Param:	label2 string - none	axis 2 label

Param:	gridcolor string - blue		color of grid lines
Param:	labelfont string - Erg14	X-windows font name for axis labels
Param:	labelcolor string - blue	color of axis labels
Param:	title string - -		title of plot
Param:	titlefont string - Rom22	X-windows font name for title
Param:	titlecolor string - red		color of title
Param:	windowtitle string - -		title in window frame

[shared-axes2]

Param:	width int 0< "from DB"		width of window in pixels
Param:	height int 0< "from DB"		height of window in pixels
Param:	nTic1 int 1<= "from DB"		axis 1 no. of tics per numbered tic
Param:	grid1 enum-grid - "from DB"	axis 1 choice of grid lines
Param:	label1 string - "from DB"	axis 1 label
Param:	nTic2 int 1<= "from DB"		axis 2 no. of tics per numbered tic
Param:	grid2 enum-grid - "from DB"	axis 2 choice of grid lines
Param:	label2 string - "from DB"	axis 2 label
Param:	labelFont string - "from DB"	X-windows font name for axes labels
Param:	title string - "from DB"	title of plot
Param:	titleFont string - "from DB"	X-windows font name for title
Param:	titleColor string - "from DB"	color of title
Param:	axesColor string - "from DB"	color of axes
Param:	gridColor string - "from DB"	color of grid lines
Param:	style enum-xwigb - "from DB"	plot style

[shared-d1]

Param:	d1 float - 1.0		sampling interval in 1st dimension
Param:	f1 float - 0.0		first sample in 1st dimension
Param:	d2 float - 1.0		sampling interval in 2nd dimension
Param:	f2 float - 0.0		first sample in 2nd dimension
Param:	verbose int 0<=1 0	if 1, report diagnostics

[shared-d2]

Param:	d1 float - 1.0		sampling interval in 1st dimension
Param:	f1 float - d1		first sample in 1st dimension
Param:	d2 float - 1.0		sampling interval in 2nd dimension
Param:	f2 float - d2		first sample in 2nd dimension
Param:	verbose int 0<=1 0	if 1, report diagnostics

[shared-sud1]

Param:	d1 float - "from header" sampling interval in 1st dimension
Param:	f1 float - "from header" first sample in 1st dimension
Param:	d2 float - "from header" sampling interval in trace dimension
Param:	f2 float - "from header" first sample in trace dimension
Param:	verbose int 0<=1 0	if 1, report diagnostics
Param:	tmpdir string - -	directory for storing temporary files
LDef:	CWP_TMPDIR environment variable if set, otherwise use tmpfile()

[shared-curve]

Param:	curve string-list - -	files containing points to draw curves
Param:	npair int-list - -	number of pairs in each curve file
Param:	curvecolor string-list - -	colors for curves

[shared-contour]

Param:	nc int 1<= 5			number of contours
Param:	dc float - (zmax-zmin)/nc	contour interval
Param:	fc float - zmin+dc		first contour
Param:	c float-list - fc,fc+dc,...	array of contour values
Param:	cwidth float-list - 1,...	array of contour line widths
Param:	ccolor string-list - none,...	array of contour colors
Param:	cdash float-list - 0,...	array of dash spacings (0 = solid)
Param:	labelcf int 1<= 1		first labeled contour
Param:	labelcper int 1<= 1		label every labelcper-th contour
Param:	nlabelc int 0<= nc		number of labeled contours (may be 0)

[shared-graph]

Param:	pairs int-list 0<=1 1,...	array of input pair flags (see help)
Param:	linewidth int-list 0<= 1,...	line widths in pixels
Param:	linecolor int-list 0<= 2,3,...	line colors
Param:	mark int-list 0<= 0,1,...	indices of marks for plotted points
Param:	marksize int-list 0<= 0,0,...	size of marks in pixels
Param:	x1beg float - "min x1"	value at which axis 1 begins
Param:	x1end float - "max x1"	value at which axis 1 ends
Param:	x2beg float - "min x2"	value at which axis 2 begins
Param:	x2end float - "max x2"	value at which axis 2 ends
Param:	windowtitle string - -	title on window

[shared-image]

Param:  perc float 0<=100 100	percentile for determining clip
Param:	clip float 0<= "from perc" amplitude at which to clip trace
Param:	bperc float 0<=100 perc	percentile of black (positive) clip
Param:	wperc float 0<=100 100-perc	percentile of white (negative) clip
Param:	bclip float - clip	positive excursion clip amplitude
Param:	wclip float - -clip	negative excursion clip amplitude
Param:	blank float 0<=1 0	what fraction of the lower range to blank out
Param:	cmap enum-cmap - rgb0	initial color map to display
Param:	legend int 0<=1 0	if 1, display the color scale
Param:	units string - -	legend units label
Param:	legendfont string - times_roman10	legend font
Param:	lwidth int 0<= 16	legend width in pixels
Param:	lheight int 0<= hbox/3	legend height in pixels
Param:	lx int - 3		legend x-position in pixels
Param:	ly int - (hbox-lheight)/3	legend y-position in pixels

[shared-movie]

Param:  perc float 0<=100 100	percentile for determining clip
Param:	clip float 0<= "from perc" amplitude at which to clip trace
Param:	bperc float 0<=100 perc	percentile of black (positive) clip
Param:	wperc float 0<=100 100-perc	percentile of white (negative) clip
Param:	bclip float - clip	positive excursion clip amplitude
Param:	wclip float - -clip	negative excursion clip amplitude
Param:	x1beg float - "min x1"	value at which axis 1 begins
Param:	x1end float - "max x1"	value at which axis 1 ends
Param:	x2beg float - "min x2"	value at which axis 2 begins
Param:	x2end float - "max x2"	value at which axis 2 ends
Param:	fframe int 1<= 1	value corresponding to first frame
Param:	dframe int 1<= 1	frame sampling interval
Param:	loop int 0<=2 0		0 = no loop, 1 = circular, 2 = back & forth
Param:	interp int 0<=1 0	If 1, interpolate image
Param:	cmap enum-cmovie - gray	type of color map
Param:	bhue int 0<=360 0	color hue mapped to bclip
Param:	whue int 0<=360 240	color hue mapped to wclip
Param:	sat float 0<=1 1	color saturation
Param:	bright float 0<=1 1	color brightness
Param:	white float - (bclip+wclip)/2	data value mapped to white

[shared-wigb]

Param:	bias float - 0		amplitude assigned to trace baseline on axis 2
Param:  perc float 0<=100 100	percentile for determining clip
Param:	clip float 0< "from perc" amplitude at which to clip trace
Param:	xcur float - 1.0	maximum wiggle excursion of trace at clip amp
Param:	wt int 0<=1 1		if 1, draw wiggle-trace line
Param:	va int 0<=1 1		if 1, do variable-area fill

[xgraph]

Cat:	Display
Desc:	graph n[i] pairs of (x,y) points, for i = 1 to nplot
Port:	stdin bin r req		arrays of (x,y) points (floats)
Port:	par par r -		optional par file input
Param:	n int-list 1<= req	array containing number of points per plot
Param:	nplot int 1<= "number of n's" number of plots
Param:	d1 float-list - 0,...	x sampling intervals (0 if x coords are input)
Param:	f1 float-list - 0,...	first x values (unused if x coords are input)
Param:	d2 float-list - 0,...	y sampling intervals (0 if y coords are input)
Param:	f2 float-list - 0,...	first y values (unused if y coords are input)
Shared:	shared-graph
Shared:	shared-axes2

[suxgraph]

Cat:	Display
Desc:	X-windows graph plot of SU data set
Port:	stdin su r req		trace input
Port:	par par r -		optional par file input
Param:	nplot int 1<= -		number of traces
Shared:	shared-sud1
Shared:	shared-graph
Shared:	shared-axes2

[suxmax]

Cat:	Display
Desc:	X-windows graph of the max, min or abs max value on each SU trace
Port:	stdin su r req		trace input
Port:	par par r -		optional par file input
Port:	outpar par w -		optional par file output
Param:	mode enum-xmax - max	plot mode
Param:	n int-list 1<= req	array containing number of points per plot
Param:	nplot int 1<= "number of n's" number of plots
Shared:	shared-sud1
Shared:	shared-graph
Shared:	shared-axes2

[enum-xmax]
Desc:	Plot mode
max	maximum value
min	minimum value
abs	absolute maximum value

[xcontour]

Cat:	Display
Desc:	X contour plot of 2D array via vector plot calls
Port:	stdin bin r req		2D array of n1 by n2 floats
Port:	stdout ps w -		optional postscript output
Port:	mpicks text w /dev/tty	file to save mouse picks in
Port:	par par r -		optional par file input
Param:	n1 int 1<= req		number of samples in 1st (fast) dimension
Param:	n2 int 1<= "all samples" number of samples in 2nd (slow) dimension
Param:	x1 float-list - -	array of sampled values in 1st dimension
Param:	x2 float-list - -	array of sampled values in 2nd dimension
Shared:	shared-d2
Shared:	shared-contour
Shared:	shared-axes1
Param:	labelccolor string - black	color of contour labels
Param:	labelcfont string - fixed	font name for contour labels
Param:	style enum-xwigb - seismic	plot style

[suxcontour]

Cat:	Display
Desc:	X contour plot of SU traces
Port:	stdin su r req		trace input
Port:	stdout ps w -		optional postscript output
Port:	mpicks text w /dev/tty	file to save mouse picks in
Port:	par par r -		optional par file input
Param:	n1 int 1<= "from header"  number of samples in 1st (fast) dimension
Param:	n2 int 1<= "from header"	 number of traces
Param:	x1 float-list - -	array of sampled values in 1st dimension
Param:	x2 float-list - -	array of sampled trace values
Param:	key enum-thed - -	set values of x2 from this header field
Shared:	shared-sud1
Shared:	shared-contour
Shared:	shared-axes1
Param:	labelccolor string - black	color of contour labels
Param:	labelcfont string - fixed	font name for contour labels
Param:	style enum-xwigb - seismic	plot style

[ximage]

Cat:	Display
Desc:	X image plot of uniformly sampled 2D array
Port:	stdin bin r req		2D array of n1 by n2 floats
Port:	stdout ps w -		optional postscript output
Port:	mpicks text w /dev/tty	file to save mouse picks in
Port:	par par r -		optional par file input

Param:	n1 int 1<= req		number of samples in 1st (fast) dimension
Param:	n2 int 1<= "all samples" number of samples in 2nd (slow) dimension
Shared:	shared-d1
Shared:	shared-image
Shared:	shared-axes1
Param:	style enum-xwigb - seismic	plot style
Shared:	shared-curve

[suximage]

Cat:	Display
Desc:	X image plot of SU traces
Port:	stdin su r req		trace input
Port:	stdout ps w -		optional postscript output
Port:	mpicks text w /dev/tty	file to save mouse picks in
Port:	par par r -		optional par file input

Param:	n1 int 1<= "from header" number of samples in 1st (fast) dimension
Param:	n2 int 1<= "from header" number of traces
Shared:	shared-sud1
Shared:	shared-image
Shared:	shared-axes1
Param:	style enum-suxwigb - seismic	plot style
Shared:	shared-curve

[xmovie]

Cat:	Display
Desc:	image one or more frames of a uniformly sampled 2D array
Port:	stdin bin r req		uniformly sampled array of floats
Port:	par par r -		optional par file input
Param:	n1 int 1<= req		number of samples in 1st (fast) dimension
Param:	n2 int 1<= req		number of samples in 2nd (slow) dimension
Shared:	shared-d1
Shared:	shared-movie
Shared:	shared-axes2

[suxmovie]

Cat:	Display
Desc:	image one or more frames of an SU data set
Port:	stdin su r req		trace input
Port:	par par r -		optional par file input
Param:	n1 int 1<= "from header" number of samples in 1st (fast) dimension
Param:	n2 int 1<= "from header" number of traces
Shared:	shared-sud1
Shared:	shared-movie
Shared:	shared-axes2

[enum-cmovie]
Desc:		Color map choices for xmovie
gray		gray scale
hue		hue scale
saturation	saturation scale
default		default scale

[xpicker]

Cat:	Display
Desc:	X wiggle-trace plot of binary file via bitmap with picking
Port:	stdin bin r req		2D array of n1 by n2 floats
Port:	mpicks text rw pick_file	name of pick file
Port:	x2file text r -		file of `acceptable' x2 values
Port:	par par r -		optional par file input

Param:	n1 int 1<= req		number of samples in 1st (fast) dimension
Param:	n2 int 1<= "all samples" number of samples in 2nd (slow) dimension
Param:	x2 float-list - -	array of sampled values in 2nd dimension
Shared:	shared-d2
Shared:	shared-wigb
Shared:	shared-axes1
Param:	style enum-xwigb - seismic	plot style
Param:	endian int 0<=1 automatic	set to 0 for little-endian display
Param:	interp int 0<=1 0	if 1, use 8 point sinc interpolation on trace
Param:	x1x2 int 0<=1 1		order to save picks in: 0=(x1,x2), 1=(x2,x1)

[suxpicker]

Cat:	Display
Desc:	X wiggle-trace plot of SU traces via bitmap with picking
Port:	stdin su r req		trace input
Port:	mpicks text rw pick_file	name of pick file
Port:	x2file text r -		file of `acceptable' x2 values
Port:	par par r -		optional par file input

Param:	n1 int 1<= "from header"  number of samples in 1st (fast) dimension
Param:	n2 int 1<= "from header"	 number of traces
Param:	x2 float-list - -	array of sampled trace values
Param:	key enum-thed - -	set values of x2 from this header field
Shared:	shared-sud1
Shared:	shared-wigb
Shared:	shared-axes1
Param:	style enum-suxwigb - seismic	plot style
Param:	endian int 0<=1 automatic	set to 0 for little-endian display
Param:	interp int 0<=1 0	if 1, use 8 point sinc interpolation on trace
Param:	x1x2 int 0<=1 1		order to save picks in: 0=(x1,x2), 1=(x2,x1)

[xwigb]

Cat:	Display
Desc:	X wiggle-trace plot of binary file via bitmap
Port:	stdin bin r req		2D array of n1 by n2 floats
Port:	stdout ps w -		optional postscript output
Port:	mpicks text w /dev/tty	file to save mouse picks in
Port:	par par r -		optional par file input

Param:	n1 int 1<= req		number of samples in 1st (fast) dimension
Param:	n2 int 1<= "all samples" number of samples in 2nd (slow) dimension
Param:	x2 float-list - -	array of sampled values in 2nd dimension
Shared:	shared-d1
Shared:	shared-wigb
Shared:	shared-axes1
Param:	style enum-xwigb - seismic	plot style
Param:	endian int 0<=1 automatic	set to 0 for little-endian display
Param:	interp int 0<=1 0	If 1, use 8 point sinc interpolation on trace
Param:	wigclip int 0<=1 0	If 1, don't expand window for wiggles at edge
Shared:	shared-curve

[suxwigb]

Cat:	Display
Desc:	X wiggle-trace plot of SU traces via bitmap
Port:	stdin su r req		trace input
Port:	stdout ps w -		optional postscript output
Port:	mpicks text w /dev/tty	file to save mouse picks in
Port:	par par r -		optional par file input

Param:	n1 int 1<= "from header" number of samples per trace
Param:	n2 int 1<= "from header" number of traces
Param:	x2 float-list - -	array of sampled trace values
Param:	key enum-thed - -	set values of x2 from this header field
Shared:	shared-sud1
Shared:	shared-wigb
Shared:	shared-axes1
Param:	style enum-suxwigb - seismic	plot style
Param:	endian int 0<=1 automatic	set to 0 for little-endian display
Param:	interp int 0<=1 0	If 1, use 8 point sinc interpolation on trace
Param:	wigclip int 0<=1 0	If 1, don't expand window for wiggles at edge
Shared:	shared-curve

[enum-suxwigb]
Desc:	suxwigb plot style
normal	plot axis 1 horizontal and axis 2 vertical
seismic	plot axis 1 vertical, top down, and axis 2 horizontal
vsp	plot axis 1 horizontal and axis 2 vertical, reversed

[enum-xwigb]
Desc:	xwigb plot style
normal	plot axis 1 horizontal and axis 2 vertical
seismic	plot axis 1 vertical, top down, and axis 2 horizontal

[enum-grid]
Desc:	choice of style for grid lines
none	no grid line
dot	dotted grid line
dash	dashed grid line
solid	solid grid line

[enum-cmap]
Desc:	color map choice for ximage
rgb0	gray scale
rgb1
rgb2
rgb3
rgb4
rgb5
rgb6
rgb7
rgb8
rgb9
rgb10
rgb11
hsv0	hue scale
hsv1
hsv2
hsv3
hsv4
hsv5
hsv6
hsv7
hsv8
hsv9
hsv10
hsv11
hsv12
hsv13

[su3dchart]

Cat:	Display
Desc:	plot x-midpoints vs. y-midpoints for 3D data
Port:	stdin su r req 		trace input
Port:	stdout bin w req 	(x,y) coordinates for psgraph
Port:	outpar par w -		optional par file for psgraph
Port:	par par r -		optional par file input
Param:	degree int 0<=1 0	if 1, convert seconds of arc to degrees

[suchart]

Cat:	Display
Desc:	prepare data for x vs. y plot
Port:	stdin su r req 		trace input
Port:	stdout bin w req 	(x,y) coordinates for psgraph
Port:	outpar par w -		optional par file for psgraph
Port:	par par r -		optional par file input
Param:	key1 enum-thed - sx	header field holding abscissa
Param:	key2 enum-thed - gx	header field holding ordinate

[sxplot]

Cat:	Display
Desc:	X-window plot of a triangulated sloth function s(x1,x2)
Port:	stdin tri r req		triangulated sloth model
Port:	par par r -		optional par file input
Param:	edgecolor string - cyan	color to draw fixed edges of triangles
Param:	tricolor string - yellow color to draw non-fixed edges of triangles
Param:	bclip float 0< "minimum sloth" sloth value corresponding to black
Param:	wclip float 0< "maximum sloth" sloth value corresponding to white
Param:	x1beg float - "min x1"	value at which axis 1 begins
Param:	x1end float - "max x1"	value at which axis 1 ends
Param:	x2beg float - "min x2"	value at which axis 2 begins
Param:	x2end float - "max x2"	value at which axis 2 ends
Param:	cmap enum-cmap - gray	initial color map to display
Shared:	shared-axes2

[trip]

Cat:	Display
Desc:	tri-plane 3D data viewer (Mesa program)
Port:	stdin bin r req		3D float array
Port:	par par r -		optional par file input
Param:	n1 int 1<= req		number of samples in 1st (fast) dimension
Param:	n2 int 1<= req		number of samples in 2nd (mid) dimension
Param:	n3 int 1<= req		number of samples in 3rd (slow) dimension
Param:	n1s int 1<= 1		dimension 1 stride
Param:	n2s int 1<= 1		dimension 2 stride
Param:	n3s int 1<= 1		dimension 3 stride
Param:	cx int 0<= n2/n2s/2	x-intercept of view plane facing x-axis
Param:	cy int 0<= n3/n3s/2	y-intercept of view plane facing y-axis
Param:	cz int 0<= n1/n1s/2	z-intercept of view plane facing z-axis
Param:	hue int 0<=1 1		hue flag: 0 = black & white, 1 = color
Param:	q float-list - -.6,.06,-.06,.8 quaternion definition
Param:	tbs float 0< 0.8	trackball size: larger number slows drag rate
Param:	verbose int 0<=1 0	if 1, print some useful information

[xrects]

Cat:	Display
Desc:	X-windows plot of rectangles on a 2D grid
Port:	stdin bin r req		input file of rectangles to plot
Port:	par par r -		optional par file input
Param:	x1min float - req	minimum x1 coordinate for plot area
Param:	x1max float - req	maximum x1 coordinate for plot area
Param:	x2min float - req	minimum x2 coordinate for plot area
Param:	x2max float - req	maximum x2 coordinate for plot area
Param:	color string - red	color for rectangles
Shared:	shared-axes2

[viewer3]

Cat:	Display
Desc:	3D tetrahedral model viewer (Mesa program)
Port:	stdin hz r req		tetrahedral model input (see tetramod)
Port:	rayfile hz r -		raypath file from sutetraray
Port:	wffile hz r -		wavefront file from sutetraray
Port:	sttfile hz r -		surface traveltime file from sutetraray
Port:	par par r -		optional par file input
Param:	hue int 0<=1 1		hue flag: 0 = black & white, 1 = color
Param:	q float-list - -.6,.06,-.06,.8 quaternion definition
Param:	tbs float 0< 0.8	trackball size: larger number slows drag rate
Param:	verbose int 0<=1 0	if 1, print some useful information

