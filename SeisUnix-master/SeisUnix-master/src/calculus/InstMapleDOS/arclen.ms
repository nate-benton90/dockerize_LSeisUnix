{VERSION 1 0 "NEXTSTEP" "1.0"}{GLOBALS 1 0}{FONT 0 "Times-Bold" "
Times" "Times-Bold" 8 24 64 "Times-Bold" 24}{FONT 1 "Times-Bold" 
"Times" "Times-Bold" 8 18 64 "Times-Bold" 18}{FONT 2 "Ohlfs" "Ohl
fs" "Courier" 4 11 192 "Ohlfs" 11}{SCP_R 1 0 38{COM_R 2 0{TEXT 0 
18 "ArcLength Problems"}{NXRTF 0 331 "\{\\rtf0\\ansi\{\\fonttbl\\
f0\\fnil Times-Roman;\\f1\\fmodern Courier;\}\012\\margl40\012\\m
argr40\012\{\\colortbl;\\red255\\green255\\blue255;\}\012\\cb1\\p
ard\\tx0\\tx1580\\tx3160\\tx4740\\tx6320\\tx7900\\tx9480\\tx11060
\\tx12640\\tx14220\\tx15800\\tx17380\\tx18960\\tx20540\\tx22120\\
tx23700\\tx25280\\tx26860\\tx28440\\tx30020\\f0\\b\\i0\\ulnone\\q
c\\fs48\\fc0\\cf0 ArcLength Problems\012\}\012"}}{COM_R 3 0{TEXT 
1 12 "Introduction"}{NXRTF 0 320 "\{\\rtf0\\ansi\{\\fonttbl\\f0\\
fnil Times-Roman;\\f1\\fmodern Courier;\}\012\\margl40\012\\margr
40\012\{\\colortbl;\\red255\\green255\\blue255;\}\012\\cb1\\pard\
\tx0\\tx1180\\tx2360\\tx3540\\tx4720\\tx5900\\tx7080\\tx8260\\tx9
440\\tx10620\\tx11800\\tx12980\\tx14160\\tx15340\\tx16520\\tx1770
0\\tx18880\\tx20060\\tx21240\\tx22420\\f0\\b\\i0\\ulnone\\fs36\\f
c0\\cf0 Introduction\012\}\012"}}{INP_R 4 0 "> "{TEXT 2 35 "ds :=
 (y,x) -> sqrt(1 + D(y)(x)^2);"}}{INP_R 5 0 "> "{TEXT 2 14 "y := 
x -> x^2:"}}{INP_R 6 0 "> "{TEXT 2 8 "ds(y,x);"}}{INP_R 7 0 "> "
{TEXT 2 22 "dsParabola := ds(y,x);"}}{INP_R 8 0 "> "{TEXT 2 32 "s
Parabola := int(dsParabola, x);"}}{INP_R 9 0 "> "{TEXT 2 44 "subs
(x=1, sParabola) - subs(x=0, sParabola);"}}{INP_R 10 0 "> "{TEXT 
2 9 "evalf(\");"}}{INP_R 11 0 "> "{TEXT 2 16 "int(ds(y,x), x);"}}
{INP_R 12 0 "> "{TEXT 2 44 "y := x -> sin(x):\012ds(y,x):\012int(
 ds(y,x), x);"}}{COM_R 13 0{TEXT 1 10 "Exercise 1"}{NXRTF 0 318 "
\{\\rtf0\\ansi\{\\fonttbl\\f0\\fnil Times-Roman;\\f1\\fmodern Cou
rier;\}\012\\margl40\012\\margr40\012\{\\colortbl;\\red255\\green
255\\blue255;\}\012\\cb1\\pard\\tx0\\tx1180\\tx2360\\tx3540\\tx47
20\\tx5900\\tx7080\\tx8260\\tx9440\\tx10620\\tx11800\\tx12980\\tx
14160\\tx15340\\tx16520\\tx17700\\tx18880\\tx20060\\tx21240\\tx22
420\\f0\\b\\i0\\ulnone\\fs36\\fc0\\cf0 Exercise 1\012\}\012"}}
{INP_R 14 0 "> "{TEXT 2 23 "y := x -> x^p:\012ds(y,x);"}}{INP_R 
15 0 "> "{TEXT 2 47 "for n from -2 to 2 do\012   solve(2*p - 2 = 
n)\012od;"}}{COM_R 16 0{TEXT 1 10 "Exercise 2"}{NXRTF 0 318 "\{\\
rtf0\\ansi\{\\fonttbl\\f0\\fnil Times-Roman;\\f1\\fmodern Courier
;\}\012\\margl40\012\\margr40\012\{\\colortbl;\\red255\\green255\
\blue255;\}\012\\cb1\\pard\\tx0\\tx1180\\tx2360\\tx3540\\tx4720\\
tx5900\\tx7080\\tx8260\\tx9440\\tx10620\\tx11800\\tx12980\\tx1416
0\\tx15340\\tx16520\\tx17700\\tx18880\\tx20060\\tx21240\\tx22420\
\f0\\b\\i0\\ulnone\\fs36\\fc0\\cf0 Exercise 2\012\}\012"}}{INP_R 
17 0 "> "{TEXT 2 48 "y := x -> x^3/3 + 1/(4*x):\012ds(y,x)^2;\012
expand(\");"}}{INP_R 18 0 "> "{TEXT 2 46 "y := x -> ln(x) - x^2/8
;\012ds(y,x)^2;\012expand(\");"}}{COM_R 19 0{TEXT 1 10 "Exercise 
3"}{NXRTF 0 318 "\{\\rtf0\\ansi\{\\fonttbl\\f0\\fnil Times-Roman;
\\f1\\fmodern Courier;\}\012\\margl40\012\\margr40\012\{\\colortb
l;\\red255\\green255\\blue255;\}\012\\cb1\\pard\\tx0\\tx1180\\tx2
360\\tx3540\\tx4720\\tx5900\\tx7080\\tx8260\\tx9440\\tx10620\\tx1
1800\\tx12980\\tx14160\\tx15340\\tx16520\\tx17700\\tx18880\\tx200
60\\tx21240\\tx22420\\f0\\b\\i0\\ulnone\\fs36\\fc0\\cf0 Exercise 
3\012\}\012"}}{INP_R 20 0 "> "{TEXT 2 68 "f := x -> x^2:\012int(f
(x) - 1/(4*f(x)), x);\012int(f(x) + 1/(4*f(x)), x);"}}{INP_R 21 0
 "> "{TEXT 2 68 "f := x -> 1/x:\012int(f(x) - 1/(4*f(x)), x);\012
int(f(x) + 1/(4*f(x)), x);"}}{COM_R 22 0{TEXT 1 18 "Arch of Sine 
Curve"}{NXRTF 0 326 "\{\\rtf0\\ansi\{\\fonttbl\\f0\\fnil Times-Ro
man;\\f1\\fmodern Courier;\}\012\\margl40\012\\margr40\012\{\\col
ortbl;\\red255\\green255\\blue255;\}\012\\cb1\\pard\\tx0\\tx1180\
\tx2360\\tx3540\\tx4720\\tx5900\\tx7080\\tx8260\\tx9440\\tx10620\
\tx11800\\tx12980\\tx14160\\tx15340\\tx16520\\tx17700\\tx18880\\t
x20060\\tx21240\\tx22420\\f0\\b\\i0\\ulnone\\fs36\\fc0\\cf0 Arch 
of Sine Curve\012\}\012"}}{INP_R 23 0 "> "{TEXT 2 25 "y := x -> s
in(x)\012ds(y,x);"}}{INP_R 24 0 "> "{TEXT 2 16 "int(ds(y,x), x);"
}}{INP_R 25 0 "> "{TEXT 2 1 ":"}}{COM_R 26 0{TEXT 1 10 "Exercise 
5"}{NXRTF 0 318 "\{\\rtf0\\ansi\{\\fonttbl\\f0\\fnil Times-Roman;
\\f1\\fmodern Courier;\}\012\\margl40\012\\margr40\012\{\\colortb
l;\\red255\\green255\\blue255;\}\012\\cb1\\pard\\tx0\\tx1180\\tx2
360\\tx3540\\tx4720\\tx5900\\tx7080\\tx8260\\tx9440\\tx10620\\tx1
1800\\tx12980\\tx14160\\tx15340\\tx16520\\tx17700\\tx18880\\tx200
60\\tx21240\\tx22420\\f0\\b\\i0\\ulnone\\fs36\\fc0\\cf0 Exercise 
5\012\}\012"}}{INP_R 27 0 "> "{TEXT 2 32 "y := x -> x^4:\012int( 
ds(y,x), x);"}}{INP_R 28 0 "> "{TEXT 2 36 "y := x -> sqrt(x):\012
int( ds(y,x), x);"}}{INP_R 29 0 "> "{TEXT 2 36 "y := x -> x^(1/4)
:\012int( ds(y,x), x);"}}{COM_R 30 0{TEXT 1 10 "Exercise 6"}
{NXRTF 0 318 "\{\\rtf0\\ansi\{\\fonttbl\\f0\\fnil Times-Roman;\\f
1\\fmodern Courier;\}\012\\margl40\012\\margr40\012\{\\colortbl;\
\red255\\green255\\blue255;\}\012\\cb1\\pard\\tx0\\tx1180\\tx2360
\\tx3540\\tx4720\\tx5900\\tx7080\\tx8260\\tx9440\\tx10620\\tx1180
0\\tx12980\\tx14160\\tx15340\\tx16520\\tx17700\\tx18880\\tx20060\
\tx21240\\tx22420\\f0\\b\\i0\\ulnone\\fs36\\fc0\\cf0 Exercise 6\0
12\}\012"}}{INP_R 31 0 "> "{TEXT 2 44 "y := x -> x^3/3 + 1/(4*x):
\012int( ds(y,x), x);"}}{INP_R 32 0 "> "{TEXT 2 42 "y := x -> ln(
x) - x^2/8:\012int( ds(y,x), x);"}}{COM_R 33 0{TEXT 1 10 "Exercis
e 7"}{NXRTF 0 318 "\{\\rtf0\\ansi\{\\fonttbl\\f0\\fnil Times-Roma
n;\\f1\\fmodern Courier;\}\012\\margl40\012\\margr40\012\{\\color
tbl;\\red255\\green255\\blue255;\}\012\\cb1\\pard\\tx0\\tx1180\\t
x2360\\tx3540\\tx4720\\tx5900\\tx7080\\tx8260\\tx9440\\tx10620\\t
x11800\\tx12980\\tx14160\\tx15340\\tx16520\\tx17700\\tx18880\\tx2
0060\\tx21240\\tx22420\\f0\\b\\i0\\ulnone\\fs36\\fc0\\cf0 Exercis
e 7\012\}\012"}}{INP_R 34 0 "> "{TEXT 2 53 "y := x -> sin(x):\012
int( ds(y,x), x = 0..Pi);\012evalf(\");"}}{COM_R 35 0{TEXT 1 10 "
Exercise 8"}{NXRTF 0 318 "\{\\rtf0\\ansi\{\\fonttbl\\f0\\fnil Tim
es-Roman;\\f1\\fmodern Courier;\}\012\\margl40\012\\margr40\012\{
\\colortbl;\\red255\\green255\\blue255;\}\012\\cb1\\pard\\tx0\\tx
1180\\tx2360\\tx3540\\tx4720\\tx5900\\tx7080\\tx8260\\tx9440\\tx1
0620\\tx11800\\tx12980\\tx14160\\tx15340\\tx16520\\tx17700\\tx188
80\\tx20060\\tx21240\\tx22420\\f0\\b\\i0\\ulnone\\fs36\\fc0\\cf0 
Exercise 8\012\}\012"}}{INP_R 36 0 "> "{TEXT 2 135 "printlevel :=
 0:\012y := x -> sin(x):\012for k from 0 to 8 do\012    b := eval
f( k*(Pi/8) ):\012    lprint( b, evalf( int( ds(y,x), x = 0..b)))
\012od;"}}{COM_R 37 0{TEXT 1 10 "Exercise 9"}{NXRTF 0 318 "\{\\rt
f0\\ansi\{\\fonttbl\\f0\\fnil Times-Roman;\\f1\\fmodern Courier;\
}\012\\margl40\012\\margr40\012\{\\colortbl;\\red255\\green255\\b
lue255;\}\012\\cb1\\pard\\tx0\\tx1180\\tx2360\\tx3540\\tx4720\\tx
5900\\tx7080\\tx8260\\tx9440\\tx10620\\tx11800\\tx12980\\tx14160\
\tx15340\\tx16520\\tx17700\\tx18880\\tx20060\\tx21240\\tx22420\\f
0\\b\\i0\\ulnone\\fs36\\fc0\\cf0 Exercise 9\012\}\012"}}{INP_R 38
 0 "> "{TEXT 2 61 "b := 'b';\012plot( evalf( int( ds(y,x), x = 0.
.b)), b = 0...Pi);"}}{INP_R 39 0 "> "{TEXT 2 32 "evalf( int( ds(y
,x), x = 0..b) )"}}}{END}
