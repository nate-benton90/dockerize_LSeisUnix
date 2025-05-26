{VERSION 1 0 "NEXTSTEP" "1.0"}{GLOBALS 1 0}{FONT 0 "Times-Bold" "
Times" "Times-Bold" 8 24 64 "Times-Bold" 24}{FONT 1 "Times-Bold" 
"Times" "Times-Bold" 8 18 64 "Times-Bold" 18}{FONT 2 "Ohlfs" "Ohl
fs" "Courier" 4 11 192 "Ohlfs" 11}{FONT 3 "Times-Roman" "Times" "
Times-Roman" 4 12 64 "Times-Roman" 12}{FONT 4 "Times-Bold" "Times
" "Times-Bold" 8 14 64 "Times-Bold" 14}{SCP_R 1 0 26{COM_R 2 0
{TEXT 0 24 "Force Applications in 3D"}{NXRTF 0 337 "\{\\rtf0\\ans
i\{\\fonttbl\\f0\\fnil Times-Roman;\\f1\\fmodern Courier;\}\012\\
margl40\012\\margr40\012\{\\colortbl;\\red255\\green255\\blue255;
\}\012\\cb1\\pard\\tx0\\tx1580\\tx3160\\tx4740\\tx6320\\tx7900\\t
x9480\\tx11060\\tx12640\\tx14220\\tx15800\\tx17380\\tx18960\\tx20
540\\tx22120\\tx23700\\tx25280\\tx26860\\tx28440\\tx30020\\f0\\b\
\i0\\ulnone\\qc\\fs48\\fc0\\cf0 Force Applications in 3D\012\}\01
2"}}{COM_R 3 0{TEXT 1 17 "Fpoint definition"}{NXRTF 0 325 "\{\\rt
f0\\ansi\{\\fonttbl\\f0\\fnil Times-Roman;\\f1\\fmodern Courier;\
}\012\\margl40\012\\margr40\012\{\\colortbl;\\red255\\green255\\b
lue255;\}\012\\cb1\\pard\\tx0\\tx1180\\tx2360\\tx3540\\tx4720\\tx
5900\\tx7080\\tx8260\\tx9440\\tx10620\\tx11800\\tx12980\\tx14160\
\tx15340\\tx16520\\tx17700\\tx18880\\tx20060\\tx21240\\tx22420\\f
0\\b\\i0\\ulnone\\fs36\\fc0\\cf0 Fpoint definition\012\}\012"}}
{INP_R 4 0 "> "{TEXT 2 21 "Fpoint := -G*M*m/q^2:"}}{COM_R 5 0
{TEXT 1 10 "Exercise 6"}{NXRTF 0 318 "\{\\rtf0\\ansi\{\\fonttbl\\
f0\\fnil Times-Roman;\\f1\\fmodern Courier;\}\012\\margl40\012\\m
argr40\012\{\\colortbl;\\red255\\green255\\blue255;\}\012\\cb1\\p
ard\\tx0\\tx1180\\tx2360\\tx3540\\tx4720\\tx5900\\tx7080\\tx8260\
\tx9440\\tx10620\\tx11800\\tx12980\\tx14160\\tx15340\\tx16520\\tx
17700\\tx18880\\tx20060\\tx21240\\tx22420\\f0\\b\\i0\\ulnone\\fs3
6\\fc0\\cf0 Exercise 6\012\}\012"}}{COM_R 6 0{TEXT 3 168 "Comment
:   Notice that evenness-oddness considerations imply that the z-
term integrates to zero in all 3 of these.  That's the mathematic
al implication of the symmetry."}{NXRTF 0 471 "\{\\rtf0\\ansi\{\\
fonttbl\\f0\\fnil Times-Roman;\\f1\\fmodern Courier;\}\012\\margl
40\012\\margr40\012\{\\colortbl;\\red255\\green255\\blue255;\}\01
2\\cb1\\pard\\tx0\\tx720\\tx1440\\tx2160\\tx2880\\tx3600\\tx4320\
\tx5040\\tx5760\\tx6480\\tx7200\\tx7920\\tx8640\\tx9360\\tx10080\
\tx10800\\tx11520\\tx12240\\tx12960\\tx13680\\f0\\b0\\i0\\ulnone\
\fs24\\fc0\\cf0 Comment:   Notice that evenness-oddness considera
tions imply that the z-term integrates to zero in all 3 of these.
  That's the mathematical implication of the symmetry.\012\}\012"
}}{COM_R 7 0{TEXT 4 11 "Rectangular"}{NXRTF 0 315 "\{\\rtf0\\ansi
\{\\fonttbl\\f0\\fnil Times-Roman;\\f1\\fmodern Courier;\}\012\\m
argl40\012\\margr40\012\{\\colortbl;\\red255\\green255\\blue255;\
}\012\\cb1\\pard\\tx0\\tx900\\tx1800\\tx2700\\tx3600\\tx4500\\tx5
400\\tx6300\\tx7200\\tx8100\\tx9000\\tx9900\\tx10800\\tx11700\\tx
12600\\tx13500\\tx14400\\tx15300\\tx16200\\tx17100\\f0\\b\\i0\\ul
none\\fs28\\fc0\\cf0 Rectangular\012\}\012"}}{INP_R 8 0 "> "{TEXT
 2 173 "rho*g* int( int( int( z+h+R, \012          x = -sqrt(R^2 
- y^2 - z^2)..sqrt(R^2 - y^2 - z^2) ),\012          y = -sqrt(R^2
 - z^2)..sqrt(R^2 - z^2) ),\012          z = -R..R):        "}}
{INP_R 9 0 "> "{TEXT 2 12 "simplify(\");"}}{COM_R 10 0{TEXT 4 14 
"Cross sections"}{NXRTF 0 318 "\{\\rtf0\\ansi\{\\fonttbl\\f0\\fni
l Times-Roman;\\f1\\fmodern Courier;\}\012\\margl40\012\\margr40\
012\{\\colortbl;\\red255\\green255\\blue255;\}\012\\cb1\\pard\\tx
0\\tx900\\tx1800\\tx2700\\tx3600\\tx4500\\tx5400\\tx6300\\tx7200\
\tx8100\\tx9000\\tx9900\\tx10800\\tx11700\\tx12600\\tx13500\\tx14
400\\tx15300\\tx16200\\tx17100\\f0\\b\\i0\\ulnone\\fs28\\fc0\\cf0
 Cross sections\012\}\012"}}{INP_R 11 0 "> "{TEXT 2 51 "Pi*rho*g*
 int( (z + h + R)*(R^2 - z^2), z = -R..R);"}}{INP_R 12 0 "> "
{TEXT 2 12 "simplify(\");"}}{COM_R 13 0{TEXT 4 9 "Spherical"}
{NXRTF 0 313 "\{\\rtf0\\ansi\{\\fonttbl\\f0\\fnil Times-Roman;\\f
1\\fmodern Courier;\}\012\\margl40\012\\margr40\012\{\\colortbl;\
\red255\\green255\\blue255;\}\012\\cb1\\pard\\tx0\\tx900\\tx1800\
\tx2700\\tx3600\\tx4500\\tx5400\\tx6300\\tx7200\\tx8100\\tx9000\\
tx9900\\tx10800\\tx11700\\tx12600\\tx13500\\tx14400\\tx15300\\tx1
6200\\tx17100\\f0\\b\\i0\\ulnone\\fs28\\fc0\\cf0 Spherical\012\}\
012"}}{INP_R 14 0 "> "{TEXT 2 121 "z := r*cos(phi):\012rho*g* int
( int( int( (z + h + R)*r^2*sin(phi),\012           theta = 0..2*
Pi ), phi = 0..Pi ), r = 0..R); "}}{INP_R 15 0 "> "{TEXT 2 12 "si
mplify(\");"}}{COM_R 16 0{TEXT 1 25 "Expansions about infinity"}
{NXRTF 0 333 "\{\\rtf0\\ansi\{\\fonttbl\\f0\\fnil Times-Roman;\\f
1\\fmodern Courier;\}\012\\margl40\012\\margr40\012\{\\colortbl;\
\red255\\green255\\blue255;\}\012\\cb1\\pard\\tx0\\tx1180\\tx2360
\\tx3540\\tx4720\\tx5900\\tx7080\\tx8260\\tx9440\\tx10620\\tx1180
0\\tx12980\\tx14160\\tx15340\\tx16520\\tx17700\\tx18880\\tx20060\
\tx21240\\tx22420\\f0\\b\\i0\\ulnone\\fs36\\fc0\\cf0 Expansions a
bout infinity\012\}\012"}}{INP_R 17 0 "> "{TEXT 2 43 "series( a /
 sqrt(a^2 + q^2), q = infinity);"}}{COM_R 18 0{TEXT 1 24 "Exercis
e 8: Square plate"}{NXRTF 0 332 "\{\\rtf0\\ansi\{\\fonttbl\\f0\\f
nil Times-Roman;\\f1\\fmodern Courier;\}\012\\margl40\012\\margr4
0\012\{\\colortbl;\\red255\\green255\\blue255;\}\012\\cb1\\pard\\
tx0\\tx1180\\tx2360\\tx3540\\tx4720\\tx5900\\tx7080\\tx8260\\tx94
40\\tx10620\\tx11800\\tx12980\\tx14160\\tx15340\\tx16520\\tx17700
\\tx18880\\tx20060\\tx21240\\tx22420\\f0\\b\\i0\\ulnone\\fs36\\fc
0\\cf0 Exercise 8: Square plate\012\}\012"}}{INP_R 19 0 "> "{TEXT
 2 96 "R := sqrt(x^2 + y^2 + q^2):\012F := -G*M*m*q/a^2* int( int
( 1/R^3, x = -a/2..a/2 ), y = -a/2..a/2);"}}{INP_R 20 0 "> "{TEXT
 2 24 "series(F, q = infinity);"}}{INP_R 21 0 "> "{TEXT 2 7 "Fpoi
nt;"}}{INP_R 22 0 "> "{TEXT 2 120 "printlevel := 0:\012for k from
 0 to 8 do\012   ratio := evalf( subs( \{a = 1, q = 2^k\}, F/Fpoi
nt) ):\012   print( 2^k, ratio )\012od;"}}{COM_R 23 0{TEXT 1 25 "
Exercise 9: Circular disk"}{NXRTF 0 333 "\{\\rtf0\\ansi\{\\fonttb
l\\f0\\fnil Times-Roman;\\f1\\fmodern Courier;\}\012\\margl40\012
\\margr40\012\{\\colortbl;\\red255\\green255\\blue255;\}\012\\cb1
\\pard\\tx0\\tx1180\\tx2360\\tx3540\\tx4720\\tx5900\\tx7080\\tx82
60\\tx9440\\tx10620\\tx11800\\tx12980\\tx14160\\tx15340\\tx16520\
\tx17700\\tx18880\\tx20060\\tx21240\\tx22420\\f0\\b\\i0\\ulnone\\
fs36\\fc0\\cf0 Exercise 9: Circular disk\012\}\012"}}{INP_R 24 0 
"> "{TEXT 2 20 "R := 'R':  F := 'F':"}}{INP_R 25 0 "> "{TEXT 2 93
 "R := sqrt(r^2 + q^2):\012F := -G*M*m*q/(Pi*a^2)* int( int( r/R^
3, theta = 0..2*Pi ), r = 0..a );"}}{INP_R 26 0 "> "{TEXT 2 24 "s
eries(F, q = infinity);"}}{INP_R 27 0 "> "{TEXT 2 103 "for k from
 0 to 8 do\012   ratio := evalf( subs( \{a = 1, q = 2^k\}, F/Fpoi
nt) ):\012   print( 2^k, ratio )\012od;"}}}{END}
