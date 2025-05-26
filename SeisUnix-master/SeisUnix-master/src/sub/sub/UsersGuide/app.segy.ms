.LP
.TS H
box;
c s s s
c c n l.
\fBSEGY Trace Record Members\fP
_
type	name	length	meaning
=
.TH
integer	tracl	1	trace sequence number within line
integer	tracr	1	trace sequence number within reel
integer	fldr	1	field record number
integer	tracf	1	trace number within field record
integer	ep	1	energy source point number
integer	cdp	1	CDP ensemble number
integer	cdpt	1	trace number within CDP ensemble
short	trid	1	trace type identification code
short	nvs	1	number of vertically summed traces
short	nhs	1	number of horizontally summed traces
short	duse	1	data use: production (1) or test (2)
integer	offset	1	distance from source to receiver group
integer	gelev	1	receiver group elevation from sea level
integer	selev	1	source elevation from sea level
integer	sdepth	1	source depth (positive)
integer	gdel	1	datum elevation at receiver group
integer	sdel	1	datum elevation at source
integer	swdep	1	water depth at source
integer	gwdep	1	water depth at receiver group
short	scalel	1	scale factor for previous 7 entries
short	scalco	1	scale factor for next 4 entries
integer	sx	1	X source coordinate
integer	sy	1	Y source coordinate
integer	gx	1	X group coordinate
integer	gy	1	Y source coordinate
short	counit	1	coord units code for previous four entries
short	wevel	1	weathering velocity
short	swevel	1	subweathering velocity
short	sut	1	uphole time at source
short	gut	1	uphole time at receiver group
short	sstat	1	source static correction
short	gstat	1	group static correction
short	tstat	1	total static applied
short	laga	1	lag time A (ms)
short	lagb	1	lag time B (ms)
short	delrt	1	delay recording time (ms)
short	muts	1	mute time--start
short	mute	1	mute time--end
unsigned short	ns	1	number of samples in this trace
unsigned short	dt	1	sample interval, in micro-seconds
short	gain	1	gain type of field instruments code
short	igc	1	instrument gain constant
short	igi	1	instrument early or initial gain
short	corr	1	correlated 1 (no) or 2 (yes)
short	sfs	1	sweep frequency at start
short	sfe	1	sweep frequency at end
short	slen	1	sweep length in ms
short	styp	1	sweep type code
short	stas	1	sweep trace length at start in ms
short	stae	1	sweep trace length at end in ms
short	tatyp	1	taper type: 1=linear, 2=cos^2, 3=other
short	afilf	1	alias filter frequency if used
short	afils	1	alias filter slope
short	nofilf	1	notch filter frequency if used
short	nofils	1	notch filter slope
short	lcf	1	low cut frequency if used
short	hcf	1	high cut frequncy if used
short	lcs	1	low cut slope
short	hcs	1	high cut slope
short	year	1	year data recorded
short	day	1	day of year
short	hour	1	hour of day (24 hour clock)
short	minute	1	minute of hour
short	sec	1	second of minute
short	timbas	1	time basis code local(1), GMT(2), other(3)
short	trwf	1	trace weighting factor
short	grnors	1	geophone group number of roll sw posn one
short	grnofr	1	geophone group number of trace one (orig)
short	grnlof	1	geophone group number of last trace(orig)
short	gaps	1	gap size (total number of groups dropped)
short	otrav	1	overtravel taper code
float	d1	1	CWP: sample spacing for non-seismic data
float	f1	1	CWP: first sample loc for non-seismic data
float	d2	1	sample spacing between traces
float	f2	1	first trace location
float	ungpow	1	negative of power used for compress
float	unscale	1	reciprocal of range scaling factor
short	mark	1	mark selected traces
short array	extra	17	unassigned
float array	trace	?	trace samples
.TE
.LP
