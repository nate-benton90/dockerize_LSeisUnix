{VERSION 1 0 "NEXTSTEP" "1.0"}{GLOBALS 1 0}{FONT 0 "Times-Bold" "
Times" "Times-Bold" 8 24 64 "Times-Bold" 24}{FONT 1 "Times-Bold" 
"Times" "Times-Bold" 8 18 64 "Times-Bold" 18}{FONT 2 "Ohlfs" "Ohl
fs" "Courier" 4 11 192 "Ohlfs" 11}{FONT 3 "Courier-Bold" "Courier
" "Courier-Bold" 8 12 192 "Courier-Bold" 12}{FONT 4 "Courier" "Co
urier" "Courier" 4 12 192 "Courier" 12}{SCP_R 1 0 15{COM_R 2 0
{TEXT 0 22 "Simple Harmonic Motion"}{NXRTF 0 335 "\{\\rtf0\\ansi\
{\\fonttbl\\f0\\fnil Times-Roman;\\f1\\fmodern Courier;\}\012\\ma
rgl40\012\\margr40\012\{\\colortbl;\\red255\\green255\\blue255;\}
\012\\cb1\\pard\\tx0\\tx1580\\tx3160\\tx4740\\tx6320\\tx7900\\tx9
480\\tx11060\\tx12640\\tx14220\\tx15800\\tx17380\\tx18960\\tx2054
0\\tx22120\\tx23700\\tx25280\\tx26860\\tx28440\\tx30020\\f0\\b\\i
0\\ulnone\\qc\\fs48\\fc0\\cf0 Simple Harmonic Motion\012\}\012"}}
{COM_R 3 0{TEXT 1 17 "Exercises 2 and 3"}{NXRTF 0 325 "\{\\rtf0\\
ansi\{\\fonttbl\\f0\\fnil Times-Roman;\\f1\\fmodern Courier;\}\01
2\\margl40\012\\margr40\012\{\\colortbl;\\red255\\green255\\blue2
55;\}\012\\cb1\\pard\\tx0\\tx1180\\tx2360\\tx3540\\tx4720\\tx5900
\\tx7080\\tx8260\\tx9440\\tx10620\\tx11800\\tx12980\\tx14160\\tx1
5340\\tx16520\\tx17700\\tx18880\\tx20060\\tx21240\\tx22420\\f0\\b
\\i0\\ulnone\\fs36\\fc0\\cf0 Exercises 2 and 3\012\}\012"}}{INP_R
 4 0 "> "{TEXT 2 44 "f := x -> cos(x) - sin(x):\012plot(f, -Pi..P
i);"}}{INP_R 5 0 "> "{TEXT 2 23 "plot(f, -Pi/4..3*Pi/4);"}}{INP_R
 6 0 "> "{TEXT 2 52 "rt2 := sqrt(2):\012finv := x -> arccos(x/rt2
) - Pi/4.0;"}}{OUT_R 7 0 6{DAG :3n4\`finv`@7,2n3\`x`,1,3n5\`opera
tor`n4\`arrow`p8+5(3n4\`arccos`,2*5a2x0001j2x0001n3\`rt2`i2x0001p
1Bn3\`Pi`e3i4x002500000000i2x0010p8}}{INP_R 8 0 "> "{TEXT 2 22 "p
lot(finv, -rt2..rt2);"}}{INP_R 9 0 "> "{TEXT 2 99 "with(plots):\0
12p2 := plot(f, -Pi/4..3*Pi/4):\012p3 := plot(finv, -rt2..rt2):\0
12p4 := plot(x, x = -2..rt2):"}}{INP_R 10 0 "> "{TEXT 2 20 "displ
ay([p2,p3,p4]);"}}{COM_R 11 0{TEXT 1 17 "Exercises 4 and 5"}
{NXRTF 0 325 "\{\\rtf0\\ansi\{\\fonttbl\\f0\\fnil Times-Roman;\\f
1\\fmodern Courier;\}\012\\margl40\012\\margr40\012\{\\colortbl;\
\red255\\green255\\blue255;\}\012\\cb1\\pard\\tx0\\tx1180\\tx2360
\\tx3540\\tx4720\\tx5900\\tx7080\\tx8260\\tx9440\\tx10620\\tx1180
0\\tx12980\\tx14160\\tx15340\\tx16520\\tx17700\\tx18880\\tx20060\
\tx21240\\tx22420\\f0\\b\\i0\\ulnone\\fs36\\fc0\\cf0 Exercises 4 
and 5\012\}\012"}}{INP_R 12 0 "> "{TEXT 2 64 "A := 5:  B := A:\01
2y := sin(A*x) + sin(B*x):\012plot(y, x = 0..4*Pi);"}}{COM_R 13 0
{TEXT 4 95 "Clearly the amplitude is 2 and the frequency is 5 rad
/sec  (assuming seconds is units of time)."}{NXRTF 0 307 "\{\\rtf
0\\ansi\{\\fonttbl\\f1\\fmodern Courier;\}\012\\margl40\012\\marg
r40\012\{\\colortbl;\\red255\\green255\\blue255;\}\012\\cb1\\pard
\\tx1152\\tx2304\\tx3456\\tx4608\\tx5760\\tx6912\\tx8064\\tx9216\
\tx10368\\tx11520\\f1\\b0\\i0\\ulnone\\fs24\\fc0\\cf0 Clearly the
 amplitude is 2 and the frequency is 5 rad/sec  (assuming seconds
 is units of time).\012\}\012"}}{INP_R 14 0 "> "{TEXT 2 63 "B := 
1.05 * A:\012y := sin(A*x) + sin(B*x):\012plot(y, x = 0..20*Pi);"
}}{COM_R 15 0{TEXT 4 275 "Careful measuring shows the new `fast' 
frequency is slightly bigger than 5.0.  The `slow' frequency is a
 bit confusing graphically.\012It takes about 26 sec for the amp.
 to repeat, suggesting a freq. of\012about 2Pi/26 or 1/4 rad/sec.
  Actually this freq. is 1/8 as in \012Exercise 5."}{NXRTF 0 490 
"\{\\rtf0\\ansi\{\\fonttbl\\f1\\fmodern Courier;\}\012\\margl40\0
12\\margr40\012\{\\colortbl;\\red255\\green255\\blue255;\}\012\\c
b1\\pard\\tx1152\\tx2304\\tx3456\\tx4608\\tx5760\\tx6912\\tx8064\
\tx9216\\tx10368\\tx11520\\f1\\b0\\i0\\ulnone\\fs24\\fc0\\cf0 Car
eful measuring shows the new `fast' frequency is slightly bigger 
than 5.0.  The `slow' frequency is a bit confusing graphically.\\
\012It takes about 26 sec for the amp. to repeat, suggesting a fr
eq. of\\\012about 2Pi/26 or 1/4 rad/sec.  Actually this freq. is 
1/8 as in \\\012Exercise 5.\012\}\012"}}{INP_R 16 0 "> "{TEXT 2 0
 ""}}}{END}
