# plotting.spec --
#
#	Tksu plotting category modules.
#	See README.spec for a description of the spec file format.
#
# CVS: $id$
#
# The shared sections holds parameters shared by many of the plotting modules.

[shared-psaxes1]

Param:	xbox float - 1.5	offset of left side of axes box (in)
Param:	ybox float - 1.5	offset of bottom side of axes box (in)
Param:	wbox float 0< 6.0	width of axes box (in)
Param:	hbox float 0< 8.0	height of axes box (in)

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

Param:	axescolor string - black	color of axes
Param:	axeswidth float - 1 		width of axes (points)
Param:	ticwidth float - axeswidth	width of tic marks (points)
Param:	gridcolor string - black	color of grid lines
Param:	gridwidth float - axeswidth	width in points of grid lines
Param:	labelfont string - Helvetica	X-windows font name for axis labels
Param:	labelsize float - 18		font size of axis labels (points)
Param:	title string - -		title of plot
Param:	titlefont string - Helvetica-Bold X-windows font name for title
Param:	titlesize float - 24		font size of title (points)
Param:	titlecolor string - black	color for title

[shared-psaxes3]

Param:	xbox float - 1.5	offset of left side of axes box (in)
Param:	ybox float - 1.5	offset of bottom side of axes box (in)

Param:	x1end float - "max x1"	axis 1 ending value
Param:	d1num float 0<= 0	axis 1 numbered tic interval (0 for auto)
Param:	f1num float - "min x1"	axis 1 first numbered tic (if not auto)
Param:	n1tic int 1<= 1		axis 1 number of tics per numbered tic
Param:	grid1 enum-grid - none	axis 1 choice of grid lines
Param:	label1 string - none	axis 1 label

Param:	x2beg float - "min x2"	axis 2 beginning value
Param:	d2num float 0<= 0	axis 2 numbered tic interval (0 for auto)
Param:	f2num float - "min x2"	axis 2 first numbered tic (if not auto)
Param:	n2tic int 1<= 1		axis 2 number of tics per numbered tic
Param:	grid2 enum-grid - none	axis 2 choice of grid lines
Param:	label2 string - none	axis 2 label

Param:	x3end float - "max x3"	axis 3 ending value
Param:	d3num float 0<= 0	axis 3 numbered tic interval (0 for auto)
Param:	f3num float - "min x3"	axis 3 first numbered tic (if not auto)
Param:	n3tic int 1<= 1		axis 3 number of tics per numbered tic
Param:	grid3 enum-grid - none	axis 3 choice of grid lines
Param:	label3 string - none	axis 3 label

Param:	axescolor string - black	color of axes
Param:	gridcolor string - black	color of grid lines
Param:	labelfont string - Helvetica	X-windows font name for axes labels
Param:	labelsize float - 18		font size of axes labels (points)
Param:	title string - -		title of plot
Param:	titlefont string - Helvetica-Bold X-windows font name for title
Param:	titlesize float - 24		font size of title (points)
Param:	titlecolor string - black	color of title

[shared-labelc]

Param:	labelcfont string - Helvetica-Bold  font name for contour labels
Param:	labelcsize float - 6		font size of contour labels (points)
Param:	labelccolor string - black	color of contour labels

[shared-psd1]

Param:	d1 float - 1.0		sampling interval in 1st dimension
Param:	f1 float - 0.0		first sample in 1st dimension
Param:	d2 float - 1.0		sampling interval in 2nd dimension
Param:	f2 float - 0.0		first sample in 2nd dimension
Param:	verbose int 0<=1 0	if 1, report diagnostics

[shared-psd2]

Param:	d1 float - 1.0		sampling interval in 1st dimension
Param:	f1 float - d1		first sample in 1st dimension
Param:	d2 float - 1.0		sampling interval in 2nd dimension
Param:	f2 float - d2		first sample in 2nd dimension
Param:	verbose int 0<=1 0	if 1, report diagnostics

[shared-supsd1]

Param:	d1 float - "from header" sampling interval in 1st dimension
Param:	f1 float - "from header" first sample in 1st dimension
Param:	d2 float - "from header" sampling interval in trace dimension
Param:	f2 float - "from header" first sample in trace dimension
Param:	verbose int 0<=1 0	if 1, report diagnostics
Param:	tmpdir string - -	directory for storing temporary files
LDef:	CWP_TMPDIR environment variable if set, otherwise use tmpfile()

[shared-pscurve]

Param:	curve string-list - curve1,curve2,...	filenames for curve points
Param:	npair int-list - n1,n2,...	numbers of pairs in each curve file
Param:	curvecolor string-list - black  colors of curves
Param:	curvewidth float - axeswidth	widths of curves (points)

[shared-pscontour]

Param:	nc int 1<= 5			number of contours
Param:	dc float - (zmax-zmin)/nc	contour interval
Param:	fc float - zmin+dc		first contour
Param:	c float-list - fc,fc+dc,...	array of contour values
Param:	cwidth float-list - 1,...	array of contour line widths
Param:	cgray float-list - 0,... array of contour grays (0=black, 1=white)
Param:	ccolor string-list - "use cgray" array of contour colors
Param:	cdash float-list - 0,...	array of dash spacings (0 = solid)
Param:	labelcf int 1<= 1		first labeled contour
Param:	labelcper int 1<= 1		label every labelcper-th contour
Param:	nlabelc int 0<= nc		number of labeled contours (may be 0)

[shared-pscube]

Param:  perc float 0<=100 100		percentile for determining clip
Param:	clip float 0<= "from perc"	amplitude at which to clip trace
Param:	bperc float 0<=100 perc		percentile of black (positive) clip
Param:	wperc float 0<=100 100-perc	percentile of white (negative) clip
Param:	bclip float - clip		positive excursion clip amplitude
Param:	wclip float - -clip		negative excursion clip amplitude
Param:	brgb float-list - 0,0,0		RGB values corresponding to black
Param:	wrgb float-list - 1,1,1		RGB values corresponding to white
Param:	bhls float-list - 0,0,0		HLS values corresponding to black
Param:	whls float-list - 0,1,0		HLS values corresponding to white
Param:	bps int 12<=24 12	bits per sample for color plots (12 or 24)
Param:	d1s float - 1.		scale factor for d1 before imaging
Param:	d2s float - 1.		scale factor for d2 before imaging
Param:	d3s float - 1.		scale factor for d3 before imaging
Param:	size1 float 0<= 4.0	size of 1st (vertical) axis (in)
Param:	size2 float 0<= 4.0	size of 2nd (horizontal) axis (in)
Param:	size3 float 0<= 3.0	size of 3rd (projected) axis (in)
Param:	angle float 0<=90 45	projection angle of cube (between axes 2 & 3)

[shared-psgraph]

Param:	pairs int-list 0<=1 1,...	array of input pair flags (see help)
Param:	linewidth int-list 0<= 1,...	line widths in pixels; =0. for no lines
Param:  linegray int-list 0<=1 0,...	line gray levels (0 = black, 1 = white)
Param:	linecolor string-list - none	line colors (if none, use linegray)
Param:	lineon float-list 0<= 1		length of dashed segments in points
Param:	lineoff float-list 0<= 0	spacing between dashes (0 = solid line)
Param:	mark int-list 0<= 0,1,...	indices of marks for plotted points
Param:	marksize int-list 0<= 0,0,...	size of marks in pixels

[shared-psimage]

Param:  perc float 0<=100 100		percentile for determining clip
Param:	clip float 0<= "from perc" 	amplitude at which to clip trace
Param:	bperc float 0<=100 perc		percentile of black (positive) clip
Param:	wperc float 0<=100 100-perc	percentile of white (negative) clip
Param:	bclip float - clip		positive excursion clip amplitude
Param:	wclip float - -clip		negative excursion clip amplitude
Param:	threecolor int 0<=1 1	supply 3 color values instead of only 2
Param:	brgb float-list - 0,0,0		RGB values corresponding to black
Param:	grgb float-list - 1,1,1		RGB values corresponding to gray
Param:	wrgb float-list - 1,1,1		RGB values corresponding to white
Param:	bhls float-list - 0,0,0		HLS values corresponding to black
Param:	ghls float-list - 0,1,0		HLS values corresponding to gray
Param:	whls float-list - 0,1,0		HLS values corresponding to white
Param:	bps int 12<=24 12	bits per sample for color plots (12 or 24)
Param:	d1s float - 1.			scale factor for d1 before imaging
Param:	d2s float - 1.			scale factor for d2 before imaging
Param:	legend int 0<=1 0		if 1, display the legend (color scale)
Param:	lstyle enum-lstyle - vertleft	axis label location
Param:	legendfont string - times_roman10	legend font
Param:	units string - -		legend units label
Param:	lwidth int 0<= 1.2		legend width (in) 
Param:	lheight int 0<= height/3	legend height (in)
Param:	lx float - 1.			legend x-position (in)
Param:	ly float - (height-lheight)/2*xybox   legend y-position (in)
Param:	lbeg float - "see help"		value at which legend axis begins
Param:	lend float - "see help" 	value at which legend axis ends
Param:	ldnum float 0<= 0.	legend axis numbered tic interval (0 = auto)
Param:	lfnum float - lmin	1st numbered tic on legend axis (ldnum > 0)
Param:	lntic int - 1		number of tics per numbered tic on legend axis
Param:	lgrid enum-grid - none	grid lines on legend axis

[shared-psmovie]

Param:  perc float 0<=100 100	percentile for determining clip
Param:	clip float 0<= "from perc" amplitude at which to clip trace
Param:	bperc float 0<=100 perc	percentile of black (positive) clip
Param:	wperc float 0<=100 100-perc	percentile of white (negative) clip
Param:	bclip float - clip	positive excursion clip amplitude
Param:	wclip float - -clip	negative excursion clip amplitude
Param:	d1s float - 1.		scale factor for d1 before imaging
Param:	d2s float - 1.		scale factor for d2 before imaging

[shared-pswigb]

Param:	bias float - 0		amplitude assigned to trace baseline on axis 2
Param:  perc float 0<=100 100	percentile for determining clip
Param:	clip float 0< "from perc" amplitude at which to clip trace
Param:	xcur float - 1.0	maximum wiggle excursion of trace at clip amp
Param:	wt int 0<=1 1		if 1, draw wiggle-trace line
Param:	va int 0<=1 1		if 1, do variable-area fill
Param:	nbpi int 0< 72		number of bits/inch at which to rasterize

[shared-pswigp]

Param:	bias float - 0		amplitude assigned to trace baseline on axis 2
Param:  perc float 0<=100 100	percentile for determining clip
Param:	clip float 0< "from perc" amplitude at which to clip trace
Param:	xcur float - 1.0	maximum wiggle excursion of trace at clip amp
Param:	fill int -1<=1 1	0 = no fill, -1 = neg fill, 1 = pos fill
Param:	linewidth float 0<= 1.	line width in points (0 = thinnest line)
Param:	tracecolor string - black	color of traces
Param:	backcolor string - none		color of background

[enum-lstyle]

Desc:		Choice of location for axis legend
vertleft	vertical, axis label on left side
vertright	vertical, axis label on right side
horibottom	horizontal, axis label on bottom

[merge2]

Cat:	Plotting
Desc:	merge 2 PostScript files onto one 8.5 x 11 in page (landscape default)
Trans:	PositionalArgs
Port:	file-1 ps r req		first input file
Port:	file-2 ps r req		second input file
Port:	stdout ps w req		output merged PostScript file
Param:	mode string - landscape orientation: portrait or landscape
Param:	aspect string - true	aspect ratio: true or max (to expand to page)
Param:	stacked string - true	true (overlay) or false (side-by-side)

[merge2v]

Cat:	Plotting
Desc:	merge 2 PostScript files onto one 8.5 x 11 in page (portrait default)
Trans:	PositionalArgs
Port:	file-1 ps r req		first input file
Port:	file-2 ps r req		second input file
Port:	stdout ps w req		output merged PostScript file
Param:	mode string - portrait	orientation: portrait or landscape
Param:	aspect string - max	aspect ratio: true or max (to expand to page)
Param:	stacked string - true	true (overlay) or false (side-by-side)

[merge4]

Cat:	Plotting
Desc:	merge 4 PostScript files onto one 8.5 x 11 inch page
Trans:	PositionalArgs
Port:	file-1 ps r req		upper-left figure
Port:	file-2 ps r req		upper-right figure
Port:	file-3 ps r req		lower-left figure
Port:	file-4 ps r req		lower-right figure
Port:	stdout ps w req		output PostScript file

[prplot]

Cat:	Plotting
Desc:	printer plot of 1-D arrays f(x1) from a 2-D function f(x1,x2)
Port:	stdin bin r req		input of 2-D function
Port:	stdout text w req	output printer plot
Port:	par par r -		optional par file input
Param:	n1 int 0< all		number of samples in 1st dimension
Param:	n2 int 0< all		number of samples in 2nd dimension
Shared:	shared-psd2
Param:	label2 string - Trace	label for 2nd dimension

[psbbox]

Cat:	Plotting
Desc:	change bounding box of existing PostScript file
Port:	stdin ps r req		input PostScript file
Port:	stdout ps w req		output PostScript file
Port:	par par r -		optional par file input
Param:	llx int - - 		new lower left x
Param:	lly int - - 		new lower left y
Param:	urx int - -		new upper right x
Param:	ury int - -		new upper right y
Param:	verbose int 0<=1 1	if 1, report diagnostics

[pscontour]

Cat:	Plotting
Desc:	PostScript contouring of a 2D function f(x1,x2)
Port:	stdin bin r req		input binary file
Port:	stdout ps w req		output PostScript file
Port:	par par r -		optional par file input
Param:	n1 int 0< req		number of samples in 1st (fast) dimension
Shared: shared-psd2
Param:	x1 float-list - f1,f1+d1,... array of monotonic 1st dimension samples
Param:	n2 int 0< all		number of samples in 2nd (slow) dimension
Param:	x2 float-list - f2,f2+d2,... array of monotonic 2nd dimension samples
Shared: shared-pscontour
Shared: shared-psaxes1
Shared: shared-labelc
Param:	style enum-xwigb - seismic	plot style

[pscube]

Cat:	Plotting
Desc:	PostScript image plot of a data cube
Port:	stdin bin r -		input data
LDesc:	input contains either entire cube of data (faces=0)
LDesc:	or just front, side, and top faces (faces=1)
Port:	front bin r -		file containing front face if not using stdin
Port:	side bin r -		file containing side face if not using stdin
Port:	top bin r -		file containing top face if not using stdin
Port:	stdout ps w req		PostScript output
Port:	par par r -		optional par file input
Param:	n1 int 0< req		number of samples in 1st (fast) dimension
Param:	n2 int 0< req		number of samples in 2nd (mid) dimension
Param:	n3 int 0< req		number of samples in 3rd (slow) dimension
Param:	faces int 0<=1 0	0 = entire cube input, 1 = face panels input
Shared: shared-psd1
Param:	d3 float 0< 1.		sampling interval in 3rd dimension
Param:	f3 float - 0.		first sample in 3rd dimension
Shared: shared-pscube
Shared: shared-psaxes3

[pscubecontour]

Cat:	Plotting
Desc:	PostScript contour plot of a data cube
Port:	stdin bin r req		input data
LDesc:	input contains either entire cube of data (faces=0)
LDesc:	or just front, side, and top faces (faces=1)
Port:	front bin r -		file containing front face if not using stdin
Port:	side bin r -		file containing side face if not using stdin
Port:	top bin r -		file containing top face if not using stdin
Port:	stdout ps w req		PostScript output
Port:	par par r -		optional par file input
Param:	n1 int 0< req		number of samples in 1st (fast) dimension
Param:	n2 int 0< req		number of samples in 2nd (mid) dimension
Param:	n3 int 0< req		number of samples in 3rd (slow) dimension
Param:	faces int 0<=1 0	0 = entire cube input, 1 = face panels input
Shared: shared-psd1
Param:	d3 float 0< 1.		sampling interval in 3rd dimension
Param:	f3 float - 0.		first sample in 3rd dimension
Param:	d1s float - 1.		scale factor for d1 before imaging
Param:	d2s float - 1.		scale factor for d2 before imaging
Param:	d3s float - 1.		scale factor for d3 before imaging
Shared:	shared-pscontour
Param:	size1 float - 4.	size in inches of 1st axis (vertical)
Param:	size2 float - 4.	size in inches of 2nd axis (horizontal)
Param:	size3 float - 4.	size in inches of 3rd axis (projected)
Param:	angle float 0<=90 45	projection angle of cube (between axes 2 & 3)
Shared:	shared-psaxes3
Shared: shared-labelc

[psepsi]

Cat:	Plotting
Desc:	add an epsi-formatted preview bitmap to an eps file
Port:	stdin eps r req		input eps file (see help)
Port:	stdout epsi w req	output merged epsi file

[psgraph]

Cat:	Plotting
Desc:	graph n[i] pairs of (x,y) points, for i = 1 to nplot
Port:	stdin bin r req		arrays of (x,y) points (floats)
Port:	stdout ps w req		PostScript output
Port:	par par r -		optional par file input
Param:	n int-list 1<= req	array containing number of points per plot
Param:	nplot int 1<=5000 "number of n's" number of plots
Param:	d1 float-list - 0,...	x sampling intervals (0 if x coords are input)
Param:	f1 float-list - 0,...	first x values (unused if x coords are input)
Param:	d2 float-list - 0,...	y sampling intervals (0 if y coords are input)
Param:	f2 float-list - 0,...	first y values (unused if y coords are input)
Shared: shared-psgraph
Shared:	shared-psaxes1
Param:	style enum-xwigb - normal	plot style

[psimage]

Cat:	Plotting
Desc:	PostScript image plot of a uniformly-samples function f(x1,x2)
Port:	stdin bin r req		binary input
Port:	stdout ps w req		PostScript output
Port:	par par r -		optional par file input
Param:	n1 int 0< req		number of samples in 1st (fast) dimension
Param:	n2 int 0< all		number of samples in 2nd (slow) dimension
Shared: shared-psd1
Shared: shared-psimage
Shared: shared-psaxes1
Param:	style enum-xwigb - seismic	plot style
Shared: shared-pscurve

[pslabel]

Cat:	Plotting
Desc:	output PostScript file of single text string on specified background
Port:	stdout eps - req	output epsfile of text label
Port:	par par r -		optional par file input
Param:	dup-t string - req	text string for output label
Param:	dup-f string - Times-Bold	text string font
Param:	size float 0< 30	size of characters in points (72 pts/in)
Param:	tcolor string - black	color of text string
Param:	bcolor string - white 	color of background box
Param:	nsub int - 0		number of chars to subtract for box size

[psmerge]

Cat:	Plotting
Desc:	merge PostScript files; for example, use to merge pslabel on a ps file
Port:	stdout ps w req		merged PostScript output
Port:	par par r -		optional par file input
Param:	dup-in string - req	input PostScript file(s) to merge
Param:	dup-origin float-list - 0.,0.   x,y origin (inches)
Param:	dup-scale float-list - 1.,1.    x,y scale factors
Param:	dup-rotate float - 0.	rotation angle (degrees)
Param:	dup-translate float-list - 0.,0.   x,y translation (inches)

[psmovie]

Cat:	Display
Desc:	PostScript movie of a uniformly sampled function f(x1,x2,x3)
Port:	stdin bin r req		uniformly sampled array of floats
Port:	stdout ps w req		PostScript output
Port:	par par r -		optional par file input
Param:	n1 int 1<= req		number of samples in 1st (fast) dimension
Param:	n2 int 1<= all		number of samples in 2nd (mid) dimension
Param:	n3 int 1<= 1		number of samples in 3rd (slow) dimension
Param:	d3 float 0< 1.0		sampling interval in 3rd dimension
Param:	f3 float - d3		first sample in 3rd dimension
Shared:	shared-psd1
Shared:	shared-psmovie
Shared:	shared-psaxes1
Param:	style enum-xwigb - seismic	plot style
Param:	title2 string - -	second title to annotate different frames
Param:	loopdsp int 1<=3 3	dimension to loop over

[pswigb]

Cat:	Plotting
Desc:	PostScript bit-mapped wiggle plot of function f(x1,x2) for many traces
Port:	stdin bin r req		input data
Port:	stdout ps w req		PostScript output
Port:	par par r -		optional par file input
Param:	n1 int 0< req		number of samples in 1st (fast) dimension
Param:  n2 int 0< all		number of samples in 2nd (slow) dimension
Param:	x2 float-list - f2,f2+d2,...  array of sampled values in 2nd dimension
Shared: shared-psd1
Shared:	shared-pswigb
Shared: shared-psaxes1
Param:	style enum-xwigb - seismic	plot style
Param:	interp int 0<=1 0	if 1, use 8 point sinc interpolation on trace
Shared:	shared-pscurve

[pswigp]

Cat:	Plotting
Desc:	PostScript polygon-filled wiggle plot of f(x1,x2) for few traces
Port:	stdin bin r req		input data
Port:	stdout ps w req		PostScript output
Port:	par par r -		optional par file input
Param:	n1 int 0< req		number of samples in 1st (fast) dimension
Param:  n2 int 0< all		number of samples in 2nd (slow) dimension
Param:	x2 float-list - f2,f2+d2,...  array of sampled values in 2nd dimension
Shared: shared-psd1
Shared:	shared-pswigp
Shared:	shared-psaxes1
Param:	style enum-xwigb - seismic	plot style
Shared: shared-pscurve

[spsplot]

Cat:	Plotting
Desc:	PostScript plot of a triangulated sloth function s(x,z)
Port:	stdin tri r req		triangulated sloth model
Port:	stdout ps w req		PostScript output
Port:	par par r -		optional par file input
Param:	gedge float 0<=1 0	gray value for drawing fixed edges
Param:	gtri float 0<=1 1	gray value for drawing non-fixed edges
Param:	gmin float 0<=1 0	min gray for shading triangles
Param:	gmax float 0<=1 1	max gray for shading triangles
Param:	sgmin float - "min s(x,z)"  s(x,z) corresponding to gmin
Param:	sgmax float - "max s(x,z)"  s(x,z) corresponding to gmax

Param:	xbox float - 1.5	offset (in) of left side of axes box
Param:	ybox float - 1.5	offset (in) of bottom side of axes box
Param:	wbox float 0< 6.0	width of axes box in inches
Param:	hbox float 0< 8.0	height of axes box in inches

Param:	xbeg float - "min x1"	value at which x-axis begins
Param:	xend float - "max x1"	value at which x-axis ends
Param:	dxnum float 0<= 0	numbered tic interval on x-axis (0 for auto)
Param:	fxnum float - "min x1"	first numbered tic on x-axis (if not auto)
Param:	nxtic int 1<= 1		number of tics per numbered tic on x-axis
Param:	gridx enum-grid - none	choice of grid lines on x-axis
Param:	labelx string - -	label on x-axis

Param:	zbeg float - "min z"	value at which z-axis begins
Param:	zend float - "max z"	value at which z-axis ends
Param:	dznum float 0<= 0	numbered tic interval on z-axis (0 for auto)
Param:	fznum float - "min x2"	first numbered tic on z-axis (if not auto)
Param:	nztic int 1<= 1		number of tics per numbered tic on z-axis
Param:	gridz enum-grid - none	choice of grid lines on z-axis
Param:	labelz string - -	label on z-axis

Param:	labelfont string - Helvetica	X-windows font name for axes labels
Param:	labelsize float - 12		font size of axes labels
Param:	title string - -		title of plot
Param:	titlefont string - Helvetica-Bold	X-windows font name for title
Param:	titlesize float - 24		font size of title
Param:	titlecolor string - black	color of title
Param:	axescolor string - black	color of axes
Param:	gridcolor string - black	color of grid lines
Param:	style enum-xwigb - seismic	plot style

[supscontour]

Cat:	Plotting
Desc:	PostScript contour plot of a 2D segy data set
Port:	stdin su r req 		input traces
Port:	stdout ps w req		output traces
Port:	par par r -		optional par file input
Param:	n1 int 1<= "from header" number of samples in 1st (fast) dimension
Param:	n2 int 1<= "from header" number of input traces
Shared:	shared-supsd1
Param:	x1 float-list - f1,f1+d1,... array of monotonic 1st dimension samples
Param:	x2 float-list - f2,f2+d2,... array of monotonic 2nd dimension samples
Shared: shared-pscontour
Shared: shared-psaxes1
Shared:	shared-labelc
Param:	style enum-xwigb - seismic	plot style

[supscube]

Cat:	Plotting
Desc:	PostScript image plot of a segy data cube
Port: 	stdin su r req		trace input
Port:	stdout ps w req		PostScript output
Port:	par par r -		optional par file input
Param:	n1 int 0< "from header"	number of samples per trace
Param:	n2 int 0< -		number of traces per frame, or total traces
Param:	n3 int 0< -		number of frames, or (total traces)/n2
Shared:	shared-supsd1
Param:	d3 float - 1.		sampling interval in 3rd dimension
Param:	f3 float - 0.		first sample in 3rd dimension
Shared: shared-pscube
Shared: shared-psaxes3

[supscubecontour]

Cat:	Plotting
Desc:	PostScript contour plot of a segy data cube
Port: 	stdin su r req		trace input
Port:	stdout ps w req		PostScript output
Port:	par par r -		optional par file input
Param:	n1 int 0< "from header"	number of samples per trace
Param:	n2 int 0< -		number of traces per frame, or total traces
Param:	n3 int 0< -		number of frames, or (total traces)/n2
Shared:	shared-supsd1
Param:	d1s float - 1.		scale factor for d1 before imaging
Param:	d2s float - 1.		scale factor for d2 before imaging
Param:	d3s float - 1.		scale factor for d3 before imaging
Shared:	shared-pscontour
Param:	size1 float - 4.	size in inches of 1st axis (vertical)
Param:	size2 float - 4.	size in inches of 2nd axis (horizontal)
Param:	size3 float - 4.	size in inches of 3rd axis (projected)
Param:	angle float 0<=90 45	projection angle of cube (between axes 2 & 3)
LDesc:	angle between 2nd and 3rd axes
Shared:	shared-psaxes3
Shared: shared-labelc

[supsgraph]

Cat:	Plotting
Desc:	PostScript graph plot of a segy data set
Port:	stdin su r req		input data
Port: 	stdout ps w req		PostScript output
Port:	par par r -		optional par file input
Param:	nplot int - - 		number of input traces
Shared:	shared-supsd1
Shared: shared-psgraph
Shared:	shared-psaxes1
Param:	style enum-xwigb - seismic	plot style

[supsmax]

Cat:	Plotting
Desc:	PostScript graph of max, min, or absolute max value on each segy trace
Port:	stdin su r req		trace input
Port:	stdout ps w req		PostScript output
Port:	par par r -		optional par file input
Port:	outpar par w -		optional par file output
Param:	mode enum-xmax - max	plot mode
Param:	n int-list 1<= req	array containing number of points per plot
Param:	nplot int 1<= "number of n's" number of plots
Shared:	shared-supsd1
Shared: shared-psgraph
Shared:	shared-psaxes1
Param:	style enum-xwigb - seismic	plot style

[supsimage]

Cat:	Plotting
Desc:	PostScript image plot of a segy data set
Port:	stdin su r req		trace input
Port:	stdout ps w req		PostScript output
Port:	par par r -		optional par file input
Param:	n1 int 1<= "from header" number of samples in 1st (fast) dimension
Param:	n2 int 1<= "from header" number of traces
Shared: shared-supsd1
Shared: shared-psimage
Shared: shared-psaxes1
Param:	style enum-xwigb - seismic	plot style
Shared: shared-pscurve

[supsmovie]

Cat:	Display
Desc:	PostScript movie of a segy data set
Port:	stdin bin r req		trace input
Port:	stdout ps w req		PostScript output
Port:	par par r -		optional par file input
Param:	n1 int 1<= "from header"  number of samples in 1st (fast) dimension
Param:	n2 int 1<= "from header"	 number of traces
Param:	n3 int 1<= all		number of samples in 3rd (slow) dimension
Param:	d3 float 0< 1.0		sampling interval in 3rd dimension
Param:	f3 float - d3		first sample in 3rd dimension
Shared:	shared-supsd1
Shared:	shared-psmovie
Shared:	shared-psaxes1
Param:	style enum-xwigb - seismic	plot style
Param:	title2 string - -	second title to annotate different frames
Param:	loopdsp int 1<=3 3	dimension to loop over

[supswigb]

Cat:	Plotting
Desc:	PostScript bit-mapped wiggle plot of a segy data set
Port:	stdin su r req		trace input
Port:	stdout ps w req		PostScript output
Port:	par par r -		optional par file input
Param:	key enum-thed - - 	set values of x2 from this header field
LDesc:	if key is not set, values must be input using parameter x2
Param:	n1 int 1<= "from header" number of samples per trace
Param:	n2 int 1<= "from header" number of traces
Param:	x2 float-list - f2,f2+d2,...  array of sampled values in 2nd dimension
Shared:	shared-supsd1
Shared:	shared-pswigb
Shared: shared-psaxes1
Param:	style enum-xwigb - seismic	plot style
Param:	interp int 0<=1 0	if 1, use 8 point sinc interpolation on trace
Shared:	shared-pscurve

[supswigp]

Cat:	Plotting
Desc:	PostScript polygon-filled wiggle plot of a segy data set
Port:	stdin su r req		trace input
Port:	stdout ps w req		PostScript output
Port:	par par r -		optional par file input
Param:	key enum-thed - - 	set values of x2 from this header field
LDesc:	if key is not set, values must be input using parameter x2
Param:	n1 int 1<= "from header" number of samples per trace
Param:	n2 int 1<= "from header" number of traces
Param:	x2 float-list - f2,f2+d2,...  array of sampled values in 2nd dimension
Shared:	shared-supsd1
Shared:	shared-pswigp
Shared:	shared-psaxes1
Param:	style enum-xwigb - seismic	plot style
Shared: shared-pscurve

[xepsb]

Cat:	Plotting
Desc:	X windows display of encapsulated Postscript as a single bitmap
Port:	stdin epsi r req	EPS input

[xepsp]

Cat:	Plotting
Desc:	X windows display of encapsulated Postscript
Port:	stdin eps r req		EPS input

[xpsp]

Cat:	Plotting
Desc:	display conforming PostScript in an X-window
Port:	stdin eps r req		EPS input

[elaps]

Cat:	Plotting
Cat:	Anisotropy
Desc:	plot a triangulated function p(x,z) from model file via PostScript
Port:	stdin bin r req		input model file
Port:	stdout ps w req		PostScript output
Param:	p int 0<=3 0		component to be plotted
LDesc:	If 0, plot vertical P-wave velocity Vpvert = sqrt(a3333).
LDesc:	If 1, plot vertical S-wave velocity Vsvert = sqrt(a1313).
LDesc:	If 2, plot (Vphorz - Vpvert)/Vpvert.
LDesc:	If 3, plot gamma = (a1212 - a1313)/(2*a1313).
Param:	gedge float <=1 0.0	gray level to draw fixed edges with
LDesc:	draw fixed edges of triangles with this gray level, or set negative
LDesc:	for no edges to be drawn
Param:	gtri float <=1 1.0	gray level to draw non-fixed edges with
LDesc:	draw non-fixed edges of triangles with this gray level, or set negative
LDesc:	for no edges to be drawn
Param:	gmin float 0<=1 0.0	min gray level for shading triangles
Param:	gmax float 0<=1 1.0	max gray level for shading triangles
Param:	pgmin float - "min p(x,z)" p(x,y) value corresponding to gmin
Param:	pgmax float - "max p(x,z)" p(x,y) value corresponding to gmax

Param:	xbox float - 1.5	offset of left side of axes box (in)
Param:	ybox float - 1.5	offset of bottom side of axes box (in)
Param:	wbox float 0< 6.0	width of axes box (in)
Param:	hbox float 0< 8.0	height of axes box (in)

Param:	zbeg float - "min x1"	axis 1 beginning value
Param:	zend float - "max x1"	axis 1 ending value
Param:	dznum float 0<= 0	axis 1 numbered tic interval (0 for auto)
Param:	fznum float - "min x1"	axis 1 first numbered tic (if not auto)
Param:	nztic int 1<= 1		axis 1 number of tics per numbered tic
Param:	gridz enum-grid - none	axis 1 choice of grid lines
Param:	labelz string - none	axis 1 label

Param:	xbeg float - "min x2"	axis 2 beginning value
Param:	xend float - "max x2"	axis 2 ending value
Param:	dxnum float 0<= 0	axis 2 numbered tic interval (0 for auto)
Param:	fxnum float - "min x2"	axis 2 first numbered tic (if not auto)
Param:	nxtic int 1<= 1		axis 2 number of tics per numbered tic
Param:	gridx enum-grid - none	axis 2 choice of grid lines
Param:	labelx string - none	axis 2 label

Param:	axescolor string - black	color of axes
Param:	gridcolor string - black	color of grid lines
Param:	labelfont string - Helvetica	X-windows font name for axis labels
Param:	labelsize float - 12		font size of axis labels (points)
Param:	title string - -		title of plot
Param:	titlefont string - Helvetica-Bold X-windows font name for title
Param:	titlesize float - 24		font size of title (points)
Param:	titlecolor string - black	color for title
Param:	style enum-xwigb - seismic	plot style

