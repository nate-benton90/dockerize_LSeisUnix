{VERSION 1 0 "NEXTSTEP" "1.0"}{GLOBALS 1 0}{FONT 0 "Courier" "Cou
rier" "Courier" 4 12 192 "Courier" 12}{FONT 1 "Ohlfs" "Ohlfs" "Co
urier" 4 11 192 "Ohlfs" 11}{FONT 2 "Courier-Bold" "Courier" "Cour
ier-Bold" 8 12 192 "Courier-Bold" 12}{FONT 3 "Times-Bold" "Times"
 "Times-Bold" 8 18 64 "Times-Bold" 18}{SCP_R 1 0 36{COM_R 2 0
{TEXT 0 35 "Approximation by Taylor Polynomials"}{NXRTF 0 328 "\{
\\rtf0\\ansi\{\\fonttbl\\f1\\fmodern Courier;\}\012\\margl40\012\
\margr40\012\{\\colortbl;\\red255\\green255\\blue255;\}\012\\cb1\
\pard\\tx0\\tx1580\\tx3160\\tx4740\\tx6320\\tx7900\\tx9480\\tx110
60\\tx12640\\tx14220\\tx15800\\tx17380\\tx18960\\tx20540\\tx22120
\\tx23700\\tx25280\\tx26860\\tx28440\\tx30020\\f1\\b0\\i0\\ulnone
\\qc\\fs24\\fc0\\cf0 Approximation by Taylor Polynomials\012\}\01
2"}}{COM_R 3 0{TEXT 0 28 "Taylor Series in Mathematica"}{NXRTF 0 
316 "\{\\rtf0\\ansi\{\\fonttbl\\f1\\fmodern Courier;\}\012\\margl
40\012\\margr40\012\{\\colortbl;\\red255\\green255\\blue255;\}\01
2\\cb1\\pard\\tx0\\tx1180\\tx2360\\tx3540\\tx4720\\tx5900\\tx7080
\\tx8260\\tx9440\\tx10620\\tx11800\\tx12980\\tx14160\\tx15340\\tx
16520\\tx17700\\tx18880\\tx20060\\tx21240\\tx22420\\f1\\b0\\i0\\u
lnone\\fs24\\fc0\\cf0 Taylor Series in Mathematica\012\}\012"}}
{INP_R 4 0 "> "{TEXT 1 40 "f := x -> x * cos(2*x):\012a := 0:  n 
:= 6:"}}{INP_R 5 0 "> "{TEXT 1 37 "ourSeries := taylor( f(x), x =
 0, n);"}}{INP_R 6 0 "> "{TEXT 1 34 "Pn := convert(ourSeries, pol
ynom);"}}{INP_R 7 0 "> "{TEXT 1 29 "plot( \{f(x), Pn\}, x = -1..1
);"}}{INP_R 8 0 "> "{TEXT 1 28 "plot( f(x) - Pn, x = -1..1);"}}
{COM_R 9 0{TEXT 0 10 "Exercise 1"}{NXRTF 0 298 "\{\\rtf0\\ansi\{\
\fonttbl\\f1\\fmodern Courier;\}\012\\margl40\012\\margr40\012\{\
\colortbl;\\red255\\green255\\blue255;\}\012\\cb1\\pard\\tx0\\tx1
180\\tx2360\\tx3540\\tx4720\\tx5900\\tx7080\\tx8260\\tx9440\\tx10
620\\tx11800\\tx12980\\tx14160\\tx15340\\tx16520\\tx17700\\tx1888
0\\tx20060\\tx21240\\tx22420\\f1\\b0\\i0\\ulnone\\fs24\\fc0\\cf0 
Exercise 1\012\}\012"}}{INP_R 10 0 "> "{TEXT 1 59 "f := x -> exp(
x)*sin(x): \012seq( diff( f(x), x$n), n = 1..4);"}}{OUT_R 11 0 10
{DAG ,5+5*5(3n3\`exp`,2n3\`x`j2x0001(3n3\`sin`p6p9p9*5p3p9(3n3\`c
os`p6p9p9+3p11j2x0002+5p11p1Cp2i2x0002+3p2i2x0004}}{COM_R 12 0
{TEXT 0 10 "Exercise 2"}{NXRTF 0 298 "\{\\rtf0\\ansi\{\\fonttbl\\
f1\\fmodern Courier;\}\012\\margl40\012\\margr40\012\{\\colortbl;
\\red255\\green255\\blue255;\}\012\\cb1\\pard\\tx0\\tx1180\\tx236
0\\tx3540\\tx4720\\tx5900\\tx7080\\tx8260\\tx9440\\tx10620\\tx118
00\\tx12980\\tx14160\\tx15340\\tx16520\\tx17700\\tx18880\\tx20060
\\tx21240\\tx22420\\f1\\b0\\i0\\ulnone\\fs24\\fc0\\cf0 Exercise 2
\012\}\012"}}{INP_R 13 0 "> "{TEXT 1 31 "u := taylor( sin(x), x =
 0, 4);"}}{INP_R 14 0 "> "{TEXT 1 26 "taylor( sin(u), x = 0, 4);"
}}{COM_R 15 0{TEXT 0 10 "Exercise 3"}{NXRTF 0 298 "\{\\rtf0\\ansi
\{\\fonttbl\\f1\\fmodern Courier;\}\012\\margl40\012\\margr40\012
\{\\colortbl;\\red255\\green255\\blue255;\}\012\\cb1\\pard\\tx0\\
tx1180\\tx2360\\tx3540\\tx4720\\tx5900\\tx7080\\tx8260\\tx9440\\t
x10620\\tx11800\\tx12980\\tx14160\\tx15340\\tx16520\\tx17700\\tx1
8880\\tx20060\\tx21240\\tx22420\\f1\\b0\\i0\\ulnone\\fs24\\fc0\\c
f0 Exercise 3\012\}\012"}}{INP_R 16 0 "> "{TEXT 1 147 "f := x -> 
x*cos(2*x):\012a = 0:\012for n from 5 by 2 to 11 do\012   Pn := c
onvert( taylor ( f(x), x = a, n ), polynom ):\012   plot( \{f(x),
 Pn\}, x = -1..1)\012od;"}}{INP_R 17 0 "> "{TEXT 1 146 "f := x ->
 x*cos(2*x):\012a = 0:\012for n from 5 by 2 to 11 do\012   Pn := 
convert( taylor ( f(x), x = a, n ), polynom ):\012   plot( f(x) -
 Pn, x = -1..1)\012od;"}}{COM_R 18 0{TEXT 0 10 "Exercise 4"}
{NXRTF 0 298 "\{\\rtf0\\ansi\{\\fonttbl\\f1\\fmodern Courier;\}\0
12\\margl40\012\\margr40\012\{\\colortbl;\\red255\\green255\\blue
255;\}\012\\cb1\\pard\\tx0\\tx1180\\tx2360\\tx3540\\tx4720\\tx590
0\\tx7080\\tx8260\\tx9440\\tx10620\\tx11800\\tx12980\\tx14160\\tx
15340\\tx16520\\tx17700\\tx18880\\tx20060\\tx21240\\tx22420\\f1\\
b0\\i0\\ulnone\\fs24\\fc0\\cf0 Exercise 4\012\}\012"}}{INP_R 19 0
 "> "{TEXT 1 92 "f := x -> exp(x)*sin(x):\012a := 0:  n := 4:\012
Pn := convert( taylor( f(x), x = a, n), polynom );"}}{INP_R 20 0 
"> "{TEXT 1 29 "plot( f(x) - Pn, x = -1..1);\012"}}{INP_R 21 0 ">
 "{TEXT 1 34 "subs( x = 1, f(x) - Pn):\012evalf(\");"}}{INP_R 22 
0 "> "{TEXT 1 90 "g := x -> sin(sin(x)):\012a := 0:  n := 4:\012P
n := convert( taylor( g(x), x = a, n), polynom );"}}{INP_R 23 0 "
> "{TEXT 1 34 "plot( \{g(x), Pn\}, x = -.5.. 0.5);\012"}}{INP_R 
24 0 "> "{TEXT 1 30 "plot( \{g(x), Pn\}, x = -1..1);\012"}}{COM_R
 25 0{TEXT 0 10 "Exercise 5"}{NXRTF 0 298 "\{\\rtf0\\ansi\{\\font
tbl\\f1\\fmodern Courier;\}\012\\margl40\012\\margr40\012\{\\colo
rtbl;\\red255\\green255\\blue255;\}\012\\cb1\\pard\\tx0\\tx1180\\
tx2360\\tx3540\\tx4720\\tx5900\\tx7080\\tx8260\\tx9440\\tx10620\\
tx11800\\tx12980\\tx14160\\tx15340\\tx16520\\tx17700\\tx18880\\tx
20060\\tx21240\\tx22420\\f1\\b0\\i0\\ulnone\\fs24\\fc0\\cf0 Exerc
ise 5\012\}\012"}}{INP_R 26 0 "> "{TEXT 1 64 "taylor( sin(tan(x))
, x = 0, 8);\012taylor( tan(sin(x)), x = 0, 8);\012"}}{INP_R 27 0
 "> "{TEXT 1 47 "plot( \{sin(tan(x)), tan(sin(x))\}, x = 0..Pi/4)
;"}}{INP_R 28 0 "> "{TEXT 1 45 "plot( \{sin(tan(x)), tan(sin(x))\
}, x = 0..Pi);"}}{COM_R 29 0{TEXT 0 10 "Exercise 6"}{NXRTF 0 298 
"\{\\rtf0\\ansi\{\\fonttbl\\f1\\fmodern Courier;\}\012\\margl40\0
12\\margr40\012\{\\colortbl;\\red255\\green255\\blue255;\}\012\\c
b1\\pard\\tx0\\tx1180\\tx2360\\tx3540\\tx4720\\tx5900\\tx7080\\tx
8260\\tx9440\\tx10620\\tx11800\\tx12980\\tx14160\\tx15340\\tx1652
0\\tx17700\\tx18880\\tx20060\\tx21240\\tx22420\\f1\\b0\\i0\\ulnon
e\\fs24\\fc0\\cf0 Exercise 6\012\}\012"}}{INP_R 30 0 "> "{TEXT 1 
28 "g := x -> exp(-x^2)*sin(x); "}}{INP_R 31 0 "> "{TEXT 1 13 "in
t(g(x), x);"}}{INP_R 32 0 "> "{TEXT 1 117 "for n from 4 by 2 to 1
2 do\012   gn := convert( taylor( g(x), x = 0, n), polynom ):\012
   plot( \{g(x), gn\}, x = -1..1 )\012od;"}}{INP_R 33 0 "> "{TEXT
 1 49 "y0 := 0.3:  yp0 := -0.1:\012yp := int( gn, x) + yp0;"}}
{INP_R 34 0 "> "{TEXT 1 22 "y := int( yp, x) + y0;"}}{INP_R 35 0 
"> "{TEXT 1 19 "plot(y, x = -1..1);"}}{COM_R 36 0{TEXT 3 14 "Anim
ation Code"}{NXRTF 0 322 "\{\\rtf0\\ansi\{\\fonttbl\\f0\\fnil Tim
es-Roman;\\f1\\fmodern Courier;\}\012\\margl40\012\\margr40\012\{
\\colortbl;\\red255\\green255\\blue255;\}\012\\cb1\\pard\\tx0\\tx
1180\\tx2360\\tx3540\\tx4720\\tx5900\\tx7080\\tx8260\\tx9440\\tx1
0620\\tx11800\\tx12980\\tx14160\\tx15340\\tx16520\\tx17700\\tx188
80\\tx20060\\tx21240\\tx22420\\f0\\b\\i0\\ulnone\\fs36\\fc0\\cf0 
Animation Code\012\}\012"}}{INP_R 37 0 "> "{TEXT 1 145 "n := 11: 
 f := x -> cos(x):\012for k from 1 by 2 to n do\012   fn := conve
rt( taylor( f(x), x = 0, k), polynom) :\012   plot( \{fn, f(x)\},
 x = -Pi..Pi)\012od;"}}}{END}
