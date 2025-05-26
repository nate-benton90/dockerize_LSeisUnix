# sorting.spec --
#
#	Tksu sorting category modules.
#	See README.spec for a description of the spec file format.
#
# CVS: $id$

[susort]

Cat:	Sorting
Desc:	sort on any segy header keywords
Trans:	SortArgs
Port:	stdin su r req 		trace input
LDesc:	Trace input and trace output cannot both be piped.
LDesc:	At least one must be to/from a disk file.
Port:	stdout su w req 	trace output
LDesc:	Trace input and trace output cannot both be piped.
LDesc:	At least one must be to/from a disk file.
Port:	par par r -		optional par file input
Param:	enum-thed enum-plusminus - -	header key to sort on
LDesc:	To sort in increasing order, set key=+.
LDesc:	To sort in decreasing order, set key=-.

[enum-plusminus]
Desc:	Choose between `+' or `-' for sorting
+	sort in ascending order
-	sort in descending order

[susorty]

Cat:	Sorting
Desc:	create 2-D common shot off-end data set to show geometry for sorting
Port:	stdout su w req		output traces
Port:	par par r -		optional par file input
Param:	nt int 0< 100		number of time samples
Param:	nshot int 0< 10		number of shots
Param:	dshot float - 10	shot interval (m)
Param:	noff int 0< 20		number of offsets
Param:	doff float - 20		offset increment (m)

