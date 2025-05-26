# dmo.spec --
#
#	Tksu DMO category modules.
#	See README.spec for a description of the spec file format.
#
# CVS: $id$

[sudmofk]

Cat:	DMO
Desc:	DMO via F-K domain (log-stretch) method
Port:	stdin su r req 		trace input (common-offset gathers)
Port:	stdout su w req 	trace output
Port:	par par r -		optional par file input
Param:	cdpmin int - req	minimum cdp for which to apply DMO
Param:	cdpmax int - req	maximum cdp for which to apply DMO
Param:	dxcdp float 0< req	distance between adjacent cdp bins (m)
Param:	noffmix int 1<= req	number of offsets to mix
Param:	tdmo float-list 0<= 0	array of times (in seconds) matching vdmo
Param:	vdmo float-list 0< 1500	array of RMS velocities (m/s)
Param:	sdmo float 0<= 1	DMO stretch factor, typically 0.6
Param:	fmax float 0<= 0.5*dt	max frequency in input traces (Hz)
Param:	verbose int 0<=1 0	if 1, report diagnostics
Param:	tmpdir string - -	directory for storing temporary files
LDef:	CWP_TMPDIR environment variable if set, otherwise use tmpfile()

[sudmofkcw]

Cat:	DMO
Desc:	converted-wave DMO via F-K domain (log-stretch) method
Port:	stdin su r req 		trace input (common-offset gathers)
Port:	stdout su w req 	trace output
Port:	par par r -		optional par file input
Param:	cdpmin int - req	minimum cdp for which to apply DMO
Param:	cdpmax int - req	maximum cdp for which to apply DMO
Param:	dxcdp float 0<= req	distance between adjacent cdp bins (m)
Param:	noffmix int 1<= req	number of offsets to mix
Param:	tdmo float-list 0<= 0	array of times matching vdmo (sec)
Param:	vdmo float-list 0< 1500	array of RMS velocities (m/s)
Param:	sdmo float 0<= 1	DMO stretch factor, typically 0.6
Param:	fmax float 0<= 0.5*dt	max frequency in input traces (Hz)
Param:	gamma float 0< 0.5	velocity ratio, upgoing/downgoing
Param:	ntable int 1<= 1000	number of tabulated z/h and b/h (see help)
Param:	flip int 0<=1 0		if 1, do negative shifts and exchange s1, s2
Param:	verbose int 0<=1 0	if 1, report diagnostics

[sudmotivz]

Cat:	DMO
Desc:	DMO for transversely isotropic v(z) media
Port:	stdin su r req		trace input (common-offset gathers)
Port:	stdout su w req		trace output
Port:	vnfile bin r -		file of NMO interval velocities (m/s)
Port:	vfile bin r - 		file of interval velocities (m/s)
Port:	etafile bin r -		file of eta interval values (m/s)
Port:	par par r -		optional par file input
Param:	cdpmin int - req	minimum cdp for which to apply DMO
Param:	cdpmax int - req	maximum cdp for which to apply DMO
Param:	dxcdp float 0<= req	distance between adjacent cdp bins (m)
Param:	noffmix int 1<= req	number of offsets to mix
Param:	tdmo float-list	0<= 0	list of times t(k) in the velocity function
Param:	vndmo float-list 0< 1500 list of NMO velocities vn(k)
Param:	vdmo float-list 0< vndmo list of interval velocities v(k)
Param:	etadmo float-list - 1500 list of eta interval values eta(k)
Param:	fmax float 0< 0.5/dt	max frequency in input traces (Hz)
Param:	smute float 0<= 1.5	stretch mute used for NMO correction
Param:	speed float - 1.	turn this knob for speed (versus aliasing)
Param:	verbose int 0<=1 0	if 1, report diagnostics

[sudmotx]

Cat:	DMO
Desc:	DMO via T-X domain (Kirchhoff) method
Port:	stdin su r req		trace input (common-offset gathers)
Port:	stdout su w req 	trace output
Port:	par par r -		optional par file input
Param:	cdpmin int - req	minimum cdp for which to apply DMO
Param:	cdpmax int - req	maximum cdp for which to apply DMO
Param:	dxcdp float 0<= req	distance between adjacent cdp bins (m)
Param:	noffmix int 1<= req	number of offsets to mix
Param:	offmax float 0<= 3000	maximum offset
Param:	tmute float 0<= 2.0	mute time at maximum offset (sec)
Param:	vrms float 0< 1500	RMS velocity at mute time
Param:	verbose int 0<=1 0	if 1, report diagnostics
Param:	tmpdir string - -	directory for storing temporary files
LDef:	CWP_TMPDIR environment variable if set, otherwise use tmpfile()

[sudmovz]

Cat:	DMO
Desc:	DMO for v(z) media
Port:	stdin su r req		trace input (common-offset gathers)
Port:	stdout su w req 	trace output
Port:	vfile bin r -		file of interval velocities (m/s)
Port:	par par r -		optional par file input
Param:	cdpmin int - req	minimum cdp for which to apply DMO
Param:	cdpmax int - req	maximum cdp for which to apply DMO
Param:	dxcdp float 0<= req	distance between adjacent cdp bins (m)
Param:	noffmix int 1<= req	number of offsets to mix
Param:	tdmo float-list 0<= 0	array of times t(k) in velocity function
Param:	vdmo float-list 0< 1500	array of interval velocities v(k)
Param:	fmax float 0<= 0.5*dt	max frequency in input traces (Hz)
Param:	smute float 0<= 1.5	stretch mute used for NMO correction
Param:	speed float - 1.0	turn this knob for speed (versus aliasing)
Param:	verbose int 0<=1 0	if 1, report diagnostics

[suocext]

Cat:	DMO
Desc:	small offset extrapolation via offset continuation method
Port:	stdin su r req		trace input (common-offset gathers)
Port:	stdout su w req		trace output
Port:	par par r - 		optional par file input
Param:	cdpmin int - req	minimum cdp number for which to apply DMO
Param:	cdpmax int - req	maximum cdp number for which to apply DMO
Param:	dxcdp float - req	distance between adjacent cdp bins (m)
Param:	noffmix int - req	number of offsets to mix
Param:	offextr float - req	offset to extrapolate
Param:	tdmo float-list 0<= 0	array of times t(k) in velocity function
Param:	vdmo float-list 0< 1500	array of RMS velocities vrms(k)
Param:	sdmo float 0<= 1	DMO stretch factor, typically 0.6
Param:	fmax float 0<= 0.5/dt	maximum frequency in input traces (Hz)
Param:	verbose int 0<=1 0	if 1, report diagnostics
Param:	tmpdir string - -	directory for storing temporary files
LDef:	CWP_TMPDIR environment variable if set, otherwise use tmpfile()

