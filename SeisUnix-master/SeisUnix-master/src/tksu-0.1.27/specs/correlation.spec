# correlation.spec --
#
#	Tksu correlation category modules.
#	See README.spec for a description of the spec file format.
#
# CVS: $id$

[suacor]

Cat:	Correlation
Desc:	perform autocorrelation
Port:	stdin su r req 		trace input
Port:	stdout su w req 	trace output
Port:	par par r -		optional par file input
Param:	ntout int 1<= 101	number of time samples to output (odd)
Param:	norm int 0<=1 1		if 1, normalize max abs output to 1.0
Param:	sym int 0<=1 1		if 1, produce symmetric output

[suvibro]

Cat:	Synthetics
Desc:	generate a Vibroseis sweep
Port:	stdout su w req		output vibroseis traces
Port:	par par r -		optional par file input
Param:	dt float 0< .004	time sampling interval (sec)
Param:	sweep enum-sweep - 1	sweep type
Param:	swconst float - 0.	sweep constant for sweep=3, 4, or 5
Param:	f1 float - 10.		starting sweep frequency (Hz)
Param:	f2 float - 60.		ending sweep frequency (Hz)
Param:	tv float 0< 10.		sweep length (sec)
Param:	phz float - 0.		initial phase (degrees)
Param:	fseg float-list 0<= 10.,60.	frequency segments when sweep = 2
Param:  tseg float-list 0<= 0.,10.	time segments when sweep = 2
Param:	t1 float - 1.		starting taper length (sec); if 0, no taper
Param:	t2 float - 1.		ending taper length (sec); if 0, no taper
Param:	taper enum-taper - 1	taper type

[enum-sweep]

Desc:	Vibroseis sweep type
1	linear sweep
2	linear-segment
3	decibel per octave
4	decibel per Hertz
5 	t-power

[enum-taper]

Desc:	Vibroseis taper type
1	linear taper
2	sine taper
3	cosine taper
4	gaussian taper (+/-3.8)
5	gaussian taper (+/-2.0)

