# enum.spec --
#
#	This file contains enumeration definitions essential for the proper
#	working of tksu.  See file README.spec for a definitive description
#	of the format of tksu module specification (.spec) files.
#
# Copyright (c) 2002 Henry Thorson Consulting.  All Rights Reserved.
# CVS: $id$

#-------------------------------------------------------------------------------
# Enumerations
#
# An enumeration list is defined by the special section `[enum-<name>]'
# where <name> is the name of the enumeration.  The list should begin with
# one description line:
#
# Desc:    "a short description of the enumeration"
#
# Subsequent lines have the format:
#
# <value>  "a short description of the enumeration value"
#
# If a parameter type is specified as an enumeration, the possible values the
# parameter may take are defined by the enumeration.  An enumeration need not
# be defined in just one section -- all enumeration sections with the same
# name are concatenated together to form the list of possible values.
#
# The special enumerations `type' and `file' defined below are essential to
# tksu.  They should not be modified, but they may be appended to, as new
# file types are supported.
#
# The special `color' enumeration is a list of key/value pairs that assigns
# port colors to data types.  For each element in the `file' enumeration,
# there should be a matching element in the `color' enumeration.
#-------------------------------------------------------------------------------

[enum-type]

Desc:	Known parameter types
int	integer value
float	floating point value
string	arbitrary string
enum-X	one of a list of values in [enum-X] enumeration list

[enum-file]

Desc:	Known file/data types
su	SU trace data
segy	SEGY trace file
ahed	SEGY character header in ASCII
bhed	SEGY binary header
thed	SEGY trace headers
par	SU par file containing key=value parameters
bin	arrays of floats, possibly with Fortran record structure
text	ASCII text file
dt1	GPR dt1 data format
segd	SEG-D tape format
eps	Encapsulated PostScript file format
epsi	Encapsulated PostScript file with bitmap format
ps	PostScript file format
xdr	XDR data format
hz	tetrahedral model format
tri	triangulated model format

[enum-color]

Desc:	Port colors (#RRGGBB) to assign to file types
su	#0000ff
segy	#00ffff
segd	#00ffff
ahed	#ffc000
bhed	#ffff00
thed	#c0ff00
par	#ff0000
bin	#60c060
text	#00c080
dt1	#00ffc0
ps	#c08000
eps	#c08088
epsi	#ca0000
xdr	#00ff80
hz	#c060d0
tri	#c080a0

#-------------------------------------------------------------------------------
# The enumerations for the SEGY/SU binary and trace header fields are in the
# following format:
#
#   <name> <type> <byte-position> <description>
#-------------------------------------------------------------------------------

[enum-bhed]

Desc:	SEGY binary header fields

jobid	int	001-004	job identification number
lino	int	005-008	line number (only one line per reel)
reno	int	009-012	reel number
ntrpr	short	013-014	number of data traces per record
nart	short	015-016	number of auxiliary traces per record
hdt	short	017-018	sample interval in microseconds for this reel
dto	short	019-020	same for original field recording
hns	short	021-022	number of samples per trace for this reel
nso	short	023-024	same for original field recording
format	short	025-026	data sample format code
fold	short	027-028	CDP fold expected per CDP ensemble
tsort	short	029-030	trace sorting code
vscode	short	031-032	vertical sum code
hsfs	short	033-034	sweep frequency at start
hsfe	short	035-036	sweep frequency at end
hslen	short	037-038	sweep length (ms)
hstyp	short	039-040	sweep type code
schn	short	041-042	trace number of sweep channel
hstas	short	043-044	sweep trace taper length at start
hstae	short	045-046	sweep trace taper length at end
htatyp	short	047-048	sweep trace taper type code
hcorr	short	049-050	correlated data traces code
bgrcv	short	051-052	binary gain recovered code
rcvm	short	053-054	amplitude recovery method code
mfeet	short	055-056	measurement system code
polyt	short	057-058	impulse signal polarity code
vpol	short	059-060	vibratory polarity code
unass	short	061-400	unassigned

[enum-thed]

Desc:	SEGY/SU trace header fields

tracl	int	001-004	trace sequence number in line
tracr	int	005-008	trace sequence number within reel
fldr	int	009-012	field record number
tracf	int	013-016	trace number within field record
ep	int	017-020	energy source point number
cdp	int	021-024	CDP ensemble number
cdpt	int	025-028	trace number within CDP ensemble
trid	short	029-030	trace identification code
nvs	short	031-032	number of vertically summed traces
nhs	short	033-034	number of horizontally summed traces
duse	short	035-036	data use
offset	int	037-040	distance from source point to receiver
gelev	int	041-044	receiver group elevation from sea level
selev	int	045-048	source elevation from sea level
sdepth	int	049-052	source depth (positive)
gdel	int	053-056	datum elevation at receiver group
sdel	int	057-060	datum elevation at source
swdep	int	061-064	water depth at source
gwdep	int	065-068	water depth at receiver group
scalel	short	069-070	scale factor for previous 7 entries
scalco	short	071-072	scale factor for next 4 entries
sx	int	073-076	X source coordinate
sy	int	077-080	Y source coordinate
gx	int	081-084	X group coordinate
gy	int	085-088	Y group coordinate
counit	short	089-090	coordinate units code
wevel	short	091-092	weathering velocity
swevel	short	093-094	subweathering velocity
sut	short	095-096	uphole time at source
gut	short	097-098	uphole time at receiver group
sstat	short	099-100	source static correction
gstat	short	101-102	group static correction
tstat	short	103-104	total static applied
laga	short	105-106	lag time A
lagb	short	107-108	lag time B
delrt	short	109-110	delay recording time
muts	short	111-112	mute time start
mute	short	113-114	mute time end
ns	short	115-116	number of samples in this trace
dt	short	117-118	sample interval in microseconds
gain	short	119-120	gain type of field instruments code
igc	short	121-122	instrument gain constant
igi	short	123-124	instrument early or initial gain
corr	short	125-126	correlated flag
sfs	short	127-128	sweep frequency at start
sfe	short	129-130	sweep frequency at end
slen	short	131-132	sweep length in ms
styp	short	133-134	sweep type code
stas	short	135-136	sweep trace length at start in ms
stae	short	137-138	sweep trace length at end in ms
tatyp	short	139-140	taper type: 1=linear, 2=cos^2, 3=other
afilf	short	141-142	alias filter frequency if used
afils	short	143-144	alias filter slope
nofilf	short	145-146	notch filter frequency if used
nofils	short	147-148	notch filter slope
lcf	short	149-150	low cut frequency if used
hcf	short	151-152	high cut frequncy if used
lcs	short	153-154	low cut slope
hcs	short	155-156	high cut slope
year	short	157-158	year data recorded
day	short	159-160	day of year
hour	short	161-162	hour of day (24 hour clock)
minute	short	163-164	minute of hour
sec	short	165-166	second of minute
timbas	short	167-168	time basis code
trwf	short	169-170	trace weighting factor, defined as 1/2^N
grnors	short	171-172	geophone group number of roll switch
grnofr	short	173-174	geophone group number of trace one in field record
grnlof	short	175-176	geophone group number of last trace
gaps	short	177-178	gap size (total number of groups dropped)
otrav	short	179-180	overtravel taper code
d1	float	181-184	sample spacing for non-seismic data
f1	float	185-188	first sample location for non-seismic data
d2	float	189-192	sample spacing between traces
f2	float	193-196	first trace location
ungpow	float	197-200	negative of power used for dynamic range compression
unscale	float	201-204	reciprocal of scaling factor to normalize range
ntr	int 	205-208	number of traces
mark	short	209-210	mark selected traces
unass	short	211-240	unassigned
