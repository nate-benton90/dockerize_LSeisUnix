{VERSION 1 0 "NEXTSTEP" "1.0"}{GLOBALS 1 0}{FONT 0 "Times-Bold" "
Times" "Times-Bold" 8 24 64 "Times-Bold" 24}{FONT 1 "Times-Bold" 
"Times" "Times-Bold" 8 18 64 "Times-Bold" 18}{FONT 2 "Times-Roman
" "Times" "Times-Roman" 4 12 64 "Times-Roman" 12}{FONT 3 "Ohlfs" 
"Ohlfs" "Courier" 4 11 192 "Ohlfs" 11}{FONT 4 "Courier" "Courier"
 "Courier" 4 12 192 "Courier" 12}{FONT 5 "Times-Bold" "Times" "Ti
mes-Bold" 8 14 64 "Times-Bold" 14}{SCP_R 1 0 38{COM_R 2 0{TEXT 0 
21 "Newton's Method in 2D"}{NXRTF 0 334 "\{\\rtf0\\ansi\{\\fonttb
l\\f1\\fnil Times-Roman;\\f0\\fmodern Courier;\}\012\\margl40\012
\\margr40\012\{\\colortbl;\\red255\\green255\\blue255;\}\012\\cb1
\\pard\\tx0\\tx1580\\tx3160\\tx4740\\tx6320\\tx7900\\tx9480\\tx11
060\\tx12640\\tx14220\\tx15800\\tx17380\\tx18960\\tx20540\\tx2212
0\\tx23700\\tx25280\\tx26860\\tx28440\\tx30020\\f1\\b\\i0\\ulnone
\\qc\\fs48\\fc0\\cf0 Newton's Method in 2D\012\}\012"}}{COM_R 3 0
{TEXT 1 45 "Derivation of Normal Equations for Paraboloid"}{NXRTF
 0 353 "\{\\rtf0\\ansi\{\\fonttbl\\f1\\fnil Times-Roman;\\f0\\fmo
dern Courier;\}\012\\margl40\012\\margr40\012\{\\colortbl;\\red25
5\\green255\\blue255;\}\012\\cb1\\pard\\tx0\\tx1180\\tx2360\\tx35
40\\tx4720\\tx5900\\tx7080\\tx8260\\tx9440\\tx10620\\tx11800\\tx1
2980\\tx14160\\tx15340\\tx16520\\tx17700\\tx18880\\tx20060\\tx212
40\\tx22420\\f1\\b\\i0\\ulnone\\fs36\\fc0\\cf0 Derivation of Norm
al Equations for Paraboloid\012\}\012"}}{COM_R 4 0{TEXT 2 134 "Th
is is just a check on the hand calculations.  We consider the par
aboloid, z = x^2 + 4 y^2.  Normal at x=a, y=b, is n = [2a, 8b, -1
]."}{NXRTF 0 447 "\{\\rtf0\\ansi\{\\fonttbl\\f1\\fnil Times-Roman
;\\f0\\fmodern Courier;\}\012\\margl40\012\\margr40\012\{\\colort
bl;\\red255\\green255\\blue255;\}\012\\cb1\\pard\\tx0\\tx720\\tx1
440\\tx2160\\tx2880\\tx3600\\tx4320\\tx5040\\tx5760\\tx6480\\tx72
00\\tx7920\\tx8640\\tx9360\\tx10080\\tx10800\\tx11520\\tx12240\\t
x12960\\tx13680\\f1\\b0\\i0\\ulnone\\fs24\\fc0\\cf0 This is just 
a check on the hand calculations\012\\f0 .  \012\\f1 We consider 
the paraboloid, z = x^2 + 4 y^2.  Normal at x=a, y=b, is n = [2a,
 8b, -1].\012\}\012"}}{INP_R 5 0 "> "{TEXT 3 96 "lhs1 := a*(1 + 2
*t):  lhs2 := b*(1 + 8*t):\012subs( t = a^2 + 4*b^2 -r, lhs1 ):\0
12left1 := expand(\");"}}{INP_R 6 0 "> "{TEXT 3 53 "left2 := expa
nd( subs( t = a^2 + 4*b^2 -r, lhs2 ) );\012"}}{INP_R 7 0 "> "
{TEXT 3 37 "eqn1 := left1 = p;\012eqn2 := left2 = q;"}}{COM_R 8 0
{TEXT 2 121 "As a check, here is our old friend from the Graphica
l Equation Solving Project.\012Really a 1-D problem (show analyti
cally)."}{NXRTF 0 425 "\{\\rtf0\\ansi\{\\fonttbl\\f1\\fnil Times-
Roman;\\f0\\fmodern Courier;\}\012\\margl40\012\\margr40\012\{\\c
olortbl;\\red255\\green255\\blue255;\}\012\\cb1\\pard\\tx0\\tx720
\\tx1440\\tx2160\\tx2880\\tx3600\\tx4320\\tx5040\\tx5760\\tx6480\
\tx7200\\tx7920\\tx8640\\tx9360\\tx10080\\tx10800\\tx11520\\tx122
40\\tx12960\\tx13680\\f1\\b0\\i0\\ulnone\\fs24\\fc0\\cf0 As a che
ck, here is our old friend from the Graphical Equation Solving Pr
oject.\\\012Really a 1-D problem (show analytically).\012\}\012"}
}{INP_R 9 0 "> "{TEXT 3 55 "p := 2:  r := 0:\012fsolve( 2*a^3 + a
 - 2 = 0, a = 0..1 );"}}{INP_R 10 0 "> "{TEXT 3 38 "q := 0:\012fs
olve( \{eqn1, eqn2\}, \{a,b\} );"}}{COM_R 11 0{TEXT 1 20 "Illustr
ative Example"}{NXRTF 0 328 "\{\\rtf0\\ansi\{\\fonttbl\\f1\\fnil 
Times-Roman;\\f0\\fmodern Courier;\}\012\\margl40\012\\margr40\01
2\{\\colortbl;\\red255\\green255\\blue255;\}\012\\cb1\\pard\\tx0\
\tx1180\\tx2360\\tx3540\\tx4720\\tx5900\\tx7080\\tx8260\\tx9440\\
tx10620\\tx11800\\tx12980\\tx14160\\tx15340\\tx16520\\tx17700\\tx
18880\\tx20060\\tx21240\\tx22420\\f1\\b\\i0\\ulnone\\fs36\\fc0\\c
f0 Illustrative Example\012\}\012"}}{INP_R 12 0 "> "{TEXT 3 123 "
p := 4:  q := 1:\012r := 0:\012f := (a,b) -> 2*a^3 + 8*a*b^2 + (1
 - 2*r)*a - p:\012g := (a,b) -> 32*b^3 + 8*b*a^2 + (1 - 8*r)*b - 
q:"}}{INP_R 13 0 "> "{TEXT 3 113 "with(plots):\012contourplot( \{
f(a,b), g(a,b)\}, a = 0..2, b = -1..1, axes = NORMAL, \012       
      view = -0.1..0.1 );"}}{INP_R 14 0 "> "{TEXT 3 120 "dfx := (
a,b) -> D[1](f)(a,b):\012dfy := (a,b) -> D[2](f)(a,b):\012dgx := 
(a,b) -> D[1](g)(a,b):\012dgy := (a,b) -> D[2](g)(a,b):\012"}}
{INP_R 15 0 "> "{TEXT 3 330 "printlevel := 0:\012an := 1.1:  bn :
= .1:\012for n to 8 do\012   an := an - ( f(an,bn)*dgy(an,bn) - g
(an,bn)*dfy(an,bn) ) /\012              ( dfx(an,bn)*dgy(an,bn) -
 dgx(an,bn)*dfy(an,bn) ):\012   bn := bn - ( g(an,bn)*dfx(an,bn) 
- f(an,bn)*dgx(an,bn) ) /\012              ( dfx(an,bn)*dgy(an,bn
) - dgx(an,bn)*dfy(an,bn) ):\012   lprint( an, bn )\012od;"}}
{COM_R 16 0{TEXT 4 14 "Check answers:"}{NXRTF 0 226 "\{\\rtf0\\an
si\{\\fonttbl\\f0\\fmodern Courier;\}\012\\margl40\012\\margr40\0
12\{\\colortbl;\\red255\\green255\\blue255;\}\012\\cb1\\pard\\tx1
140\\tx2300\\tx3440\\tx4600\\tx5760\\tx6900\\tx8060\\tx9200\\tx10
360\\tx11520\\f0\\b0\\i0\\ulnone\\fs24\\fc0\\cf0 Check answers:\0
12\}\012"}}{INP_R 17 0 "> "{TEXT 3 45 "subs( \{a = an, b = bn\}, 
\{ f(a,b), g(a,b) \} );"}}{COM_R 18 0{TEXT 1 14 "Exercises 2, 3"}
{NXRTF 0 322 "\{\\rtf0\\ansi\{\\fonttbl\\f1\\fnil Times-Roman;\\f
0\\fmodern Courier;\}\012\\margl40\012\\margr40\012\{\\colortbl;\
\red255\\green255\\blue255;\}\012\\cb1\\pard\\tx0\\tx1180\\tx2360
\\tx3540\\tx4720\\tx5900\\tx7080\\tx8260\\tx9440\\tx10620\\tx1180
0\\tx12980\\tx14160\\tx15340\\tx16520\\tx17700\\tx18880\\tx20060\
\tx21240\\tx22420\\f1\\b\\i0\\ulnone\\fs36\\fc0\\cf0 Exercises 2,
 3\012\}\012"}}{COM_R 19 0{TEXT 2 69 "The point is outside the pa
raboloid and \"low\", so expect unique root."}{NXRTF 0 372 "\{\\r
tf0\\ansi\{\\fonttbl\\f1\\fnil Times-Roman;\\f0\\fmodern Courier;
\}\012\\margl40\012\\margr40\012\{\\colortbl;\\red255\\green255\\
blue255;\}\012\\cb1\\pard\\tx0\\tx720\\tx1440\\tx2160\\tx2880\\tx
3600\\tx4320\\tx5040\\tx5760\\tx6480\\tx7200\\tx7920\\tx8640\\tx9
360\\tx10080\\tx10800\\tx11520\\tx12240\\tx12960\\tx13680\\f1\\b0
\\i0\\ulnone\\fs24\\fc0\\cf0 The point is outside the paraboloid 
and \"low\", so expect unique root.\012\}\012"}}{INP_R 20 0 "> "
{TEXT 3 164 "p := 2:  q := 2:  r := 0:\012f := (a,b) -> 2*a^3 + 8
*a*b^2 + (1 - 2*r)*a - p:\012g := (a,b) -> 32*b^3 + 8*b*a^2 + (1 
- 8*r)*b - q:\012eqn1 := f(a,b) = 0;\012eqn2 := g(a,b) = 0;"}}
{INP_R 21 0 "> "{TEXT 3 68 "contourplot( \{f(a,b), g(a,b)\}, a = 
0..2, b = -1..1, axes = NORMAL );"}}{COM_R 22 0{TEXT 5 15 "Solve 
by fsolve"}{NXRTF 0 319 "\{\\rtf0\\ansi\{\\fonttbl\\f1\\fnil Time
s-Roman;\\f0\\fmodern Courier;\}\012\\margl40\012\\margr40\012\{\
\colortbl;\\red255\\green255\\blue255;\}\012\\cb1\\pard\\tx0\\tx9
00\\tx1800\\tx2700\\tx3600\\tx4500\\tx5400\\tx6300\\tx7200\\tx810
0\\tx9000\\tx9900\\tx10800\\tx11700\\tx12600\\tx13500\\tx14400\\t
x15300\\tx16200\\tx17100\\f1\\b\\i0\\ulnone\\fs28\\fc0\\cf0 Solve
 by fsolve\012\}\012"}}{INP_R 23 0 "> "{TEXT 3 30 "fsolve( \{eqn1
, eqn2\}, \{a,b\} );"}}{COM_R 24 0{TEXT 5 18 "Solve by iteration"
}{NXRTF 0 322 "\{\\rtf0\\ansi\{\\fonttbl\\f1\\fnil Times-Roman;\\
f0\\fmodern Courier;\}\012\\margl40\012\\margr40\012\{\\colortbl;
\\red255\\green255\\blue255;\}\012\\cb1\\pard\\tx0\\tx900\\tx1800
\\tx2700\\tx3600\\tx4500\\tx5400\\tx6300\\tx7200\\tx8100\\tx9000\
\tx9900\\tx10800\\tx11700\\tx12600\\tx13500\\tx14400\\tx15300\\tx
16200\\tx17100\\f1\\b\\i0\\ulnone\\fs28\\fc0\\cf0 Solve by iterat
ion\012\}\012"}}{INP_R 25 0 "> "{TEXT 3 119 "dfx := (a,b) -> D[1]
(f)(a,b):\012dfy := (a,b) -> D[2](f)(a,b):\012dgx := (a,b) -> D[1
](g)(a,b):\012dgy := (a,b) -> D[2](g)(a,b):"}}{INP_R 26 0 "> "
{TEXT 3 312 "an := .8:  bn := .3:\012for n to 8 do\012   an := an
 - ( f(an,bn)*dgy(an,bn) - g(an,bn)*dfy(an,bn) ) /\012           
   ( dfx(an,bn)*dgy(an,bn) - dgx(an,bn)*dfy(an,bn) ):\012   bn :=
 bn - ( g(an,bn)*dfx(an,bn) - f(an,bn)*dgx(an,bn) ) /\012        
      ( dfx(an,bn)*dgy(an,bn) - dgx(an,bn)*dfy(an,bn) ):\012   lp
rint( an, bn )\012od;"}}{COM_R 27 0{TEXT 4 9 "Checking:"}{NXRTF 0
 221 "\{\\rtf0\\ansi\{\\fonttbl\\f0\\fmodern Courier;\}\012\\marg
l40\012\\margr40\012\{\\colortbl;\\red255\\green255\\blue255;\}\0
12\\cb1\\pard\\tx1140\\tx2300\\tx3440\\tx4600\\tx5760\\tx6900\\tx
8060\\tx9200\\tx10360\\tx11520\\f0\\b0\\i0\\ulnone\\fs24\\fc0\\cf
0 Checking:\012\}\012"}}{INP_R 28 0 "> "{TEXT 3 45 "subs( \{a = a
n, b = bn\}, \{ f(a,b), g(a,b) \} );"}}{COM_R 29 0{TEXT 5 25 "The
 distance (Exercise 3)"}{NXRTF 0 329 "\{\\rtf0\\ansi\{\\fonttbl\\
f1\\fnil Times-Roman;\\f0\\fmodern Courier;\}\012\\margl40\012\\m
argr40\012\{\\colortbl;\\red255\\green255\\blue255;\}\012\\cb1\\p
ard\\tx0\\tx900\\tx1800\\tx2700\\tx3600\\tx4500\\tx5400\\tx6300\\
tx7200\\tx8100\\tx9000\\tx9900\\tx10800\\tx11700\\tx12600\\tx1350
0\\tx14400\\tx15300\\tx16200\\tx17100\\f1\\b\\i0\\ulnone\\fs28\\f
c0\\cf0 The distance (Exercise 3)\012\}\012"}}{INP_R 30 0 "> "
{TEXT 3 74 "h := (x,y) -> x^2 + 4*y^2:\012sqrt( (an-p)^2 + (bn-q)
^2 + (h(an,bn) - r)^2 );"}}{COM_R 31 0{TEXT 4 44 "Checking nearby
, confirms the minimun.  E.g."}{NXRTF 0 256 "\{\\rtf0\\ansi\{\\fo
nttbl\\f0\\fmodern Courier;\}\012\\margl40\012\\margr40\012\{\\co
lortbl;\\red255\\green255\\blue255;\}\012\\cb1\\pard\\tx1140\\tx2
300\\tx3440\\tx4600\\tx5760\\tx6900\\tx8060\\tx9200\\tx10360\\tx1
1520\\f0\\b0\\i0\\ulnone\\fs24\\fc0\\cf0 Checking nearby, confirm
s the minimun.  E.g.\012\}\012"}}{INP_R 32 0 "> "{TEXT 3 75 "eps 
:= .01:\012sqrt( (an+eps-p)^2 + (bn+eps-q)^2 + (h(an+eps,bn+eps) 
- r)^2 );"}}{COM_R 33 0{TEXT 1 14 "Exercises 4, 5"}{NXRTF 0 322 "
\{\\rtf0\\ansi\{\\fonttbl\\f1\\fnil Times-Roman;\\f0\\fmodern Cou
rier;\}\012\\margl40\012\\margr40\012\{\\colortbl;\\red255\\green
255\\blue255;\}\012\\cb1\\pard\\tx0\\tx1180\\tx2360\\tx3540\\tx47
20\\tx5900\\tx7080\\tx8260\\tx9440\\tx10620\\tx11800\\tx12980\\tx
14160\\tx15340\\tx16520\\tx17700\\tx18880\\tx20060\\tx21240\\tx22
420\\f1\\b\\i0\\ulnone\\fs36\\fc0\\cf0 Exercises 4, 5\012\}\012"}
}{COM_R 34 0{TEXT 2 72 "The point is outside the paraboloid and \
"high\", so expect several roots."}{NXRTF 0 375 "\{\\rtf0\\ansi\{
\\fonttbl\\f1\\fnil Times-Roman;\\f0\\fmodern Courier;\}\012\\mar
gl40\012\\margr40\012\{\\colortbl;\\red255\\green255\\blue255;\}\
012\\cb1\\pard\\tx0\\tx720\\tx1440\\tx2160\\tx2880\\tx3600\\tx432
0\\tx5040\\tx5760\\tx6480\\tx7200\\tx7920\\tx8640\\tx9360\\tx1008
0\\tx10800\\tx11520\\tx12240\\tx12960\\tx13680\\f1\\b0\\i0\\ulnon
e\\fs24\\fc0\\cf0 The point is outside the paraboloid and \"high\
", so expect several roots.\012\}\012"}}{INP_R 35 0 "> "{TEXT 3 
24 "p := 4:  q := 1:  r = 4:"}}{COM_R 36 0{TEXT 5 26 "Getting the
 starting value"}{NXRTF 0 330 "\{\\rtf0\\ansi\{\\fonttbl\\f1\\fni
l Times-Roman;\\f0\\fmodern Courier;\}\012\\margl40\012\\margr40\
012\{\\colortbl;\\red255\\green255\\blue255;\}\012\\cb1\\pard\\tx
0\\tx900\\tx1800\\tx2700\\tx3600\\tx4500\\tx5400\\tx6300\\tx7200\
\tx8100\\tx9000\\tx9900\\tx10800\\tx11700\\tx12600\\tx13500\\tx14
400\\tx15300\\tx16200\\tx17100\\f1\\b\\i0\\ulnone\\fs28\\fc0\\cf0
 Getting the starting value\012\}\012"}}{INP_R 37 0 "> "{TEXT 3 
138 "f := (a,b) -> 2*a^3 + 8*a*b^2 + (1 - 2*r)*a - p:\012g := (a,
b) -> 32*b^3 + 8*b*a^2 + (1 - 8*r)*b - q:\012eqn1 := f(a,b) = 0:\
012eqn2 := g(a,b) = 0:"}}{INP_R 38 0 "> "{TEXT 3 72 "contourplot(
 f(a,b), a = -3..3, b = -2..2, axes=NORMAL, view=-.1..0.1 );"}}
{INP_R 39 0 "> "{TEXT 3 72 "contourplot( f(a,b), a = -3..0, b = -
2..2, axes=NORMAL, view=-.1..0.1 );"}}}{END}
