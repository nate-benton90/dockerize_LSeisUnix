{VERSION 1 0 "NEXTSTEP" "1.0"}{GLOBALS 1 0}{FONT 0 "Times-Bold" "
Times" "Times-Bold" 8 24 64 "Times-Bold" 24}{FONT 1 "Times-Bold" 
"Times" "Times-Bold" 8 18 64 "Times-Bold" 18}{FONT 2 "Ohlfs" "Ohl
fs" "Courier" 4 11 192 "Ohlfs" 11}{FONT 3 "Times-Bold" "Times" "T
imes-Bold" 8 14 64 "Times-Bold" 14}{FONT 4 "Courier" "Courier" "C
ourier" 4 12 192 "Courier" 12}{FONT 5 "Times-Roman" "Times" "Time
s-Roman" 4 12 64 "Times-Roman" 12}{SCP_R 1 0 32{COM_R 2 0{TEXT 0 
34 "Snell's Law and Other Applications"}{NXRTF 0 347 "\{\\rtf0\\a
nsi\{\\fonttbl\\f0\\fnil Times-Roman;\\f1\\fmodern Courier;\}\012
\\margl40\012\\margr40\012\{\\colortbl;\\red255\\green255\\blue25
5;\}\012\\cb1\\pard\\tx0\\tx1580\\tx3160\\tx4740\\tx6320\\tx7900\
\tx9480\\tx11060\\tx12640\\tx14220\\tx15800\\tx17380\\tx18960\\tx
20540\\tx22120\\tx23700\\tx25280\\tx26860\\tx28440\\tx30020\\f0\\
b\\i0\\ulnone\\qc\\fs48\\fc0\\cf0 Snell's Law and Other Applicati
ons\012\}\012"}}{COM_R 3 0{TEXT 1 10 "Exercise 3"}{NXRTF 0 318 "\
{\\rtf0\\ansi\{\\fonttbl\\f0\\fnil Times-Roman;\\f1\\fmodern Cour
ier;\}\012\\margl40\012\\margr40\012\{\\colortbl;\\red255\\green2
55\\blue255;\}\012\\cb1\\pard\\tx0\\tx1180\\tx2360\\tx3540\\tx472
0\\tx5900\\tx7080\\tx8260\\tx9440\\tx10620\\tx11800\\tx12980\\tx1
4160\\tx15340\\tx16520\\tx17700\\tx18880\\tx20060\\tx21240\\tx224
20\\f0\\b\\i0\\ulnone\\fs36\\fc0\\cf0 Exercise 3\012\}\012"}}
{INP_R 4 0 "> "{TEXT 2 46 "f := x -> 3*x^4 - 24*x^3 + 51*x^2 - 32
*x + 64;"}}{INP_R 5 0 "> "{TEXT 2 17 "plot(f, -10..10);"}}{INP_R 
6 0 "> "{TEXT 2 14 "plot(f, 3..6);"}}{INP_R 7 0 "> "{TEXT 2 137 "
signif := 6: niter := 8: xn := 3.4:\012printlevel := 0:\012for k 
to niter do\012   xn := xn - f(xn) / D(f)(xn):\012   lprint( eval
f(xn, signif) )\012od;"}}{INP_R 8 0 "> "{TEXT 2 78 "a := 3.4:\012
tangent := f(a) + D(f)(a)*(x -a ):\012plot( \{f(x), tangent\}, x 
= 3..4);"}}{COM_R 9 0{TEXT 1 10 "Exercise 4"}{NXRTF 0 318 "\{\\rt
f0\\ansi\{\\fonttbl\\f0\\fnil Times-Roman;\\f1\\fmodern Courier;\
}\012\\margl40\012\\margr40\012\{\\colortbl;\\red255\\green255\\b
lue255;\}\012\\cb1\\pard\\tx0\\tx1180\\tx2360\\tx3540\\tx4720\\tx
5900\\tx7080\\tx8260\\tx9440\\tx10620\\tx11800\\tx12980\\tx14160\
\tx15340\\tx16520\\tx17700\\tx18880\\tx20060\\tx21240\\tx22420\\f
0\\b\\i0\\ulnone\\fs36\\fc0\\cf0 Exercise 4\012\}\012"}}{INP_R 10
 0 "> "{TEXT 2 143 "a := 'a': b:= 'b': L := 'L': c1 := 'c1': c2 :
= 'c2':\012d1 := sqrt(a^2 + x^2):\012d2 := sqrt((L-x)^2 + b^2):\0
12eqn := (c2*x*d2)^2 - (c1*(L - x)*d1)^2; "}}{INP_R 11 0 "> "
{TEXT 2 10 "expand(\");"}}{INP_R 12 0 "> "{TEXT 2 25 "eqn := 4 * 
collect(\", x);"}}{COM_R 13 0{TEXT 3 31 "Check on equation for ex
ample 3"}{NXRTF 0 335 "\{\\rtf0\\ansi\{\\fonttbl\\f0\\fnil Times-
Roman;\\f1\\fmodern Courier;\}\012\\margl40\012\\margr40\012\{\\c
olortbl;\\red255\\green255\\blue255;\}\012\\cb1\\pard\\tx0\\tx900
\\tx1800\\tx2700\\tx3600\\tx4500\\tx5400\\tx6300\\tx7200\\tx8100\
\tx9000\\tx9900\\tx10800\\tx11700\\tx12600\\tx13500\\tx14400\\tx1
5300\\tx16200\\tx17100\\f0\\b\\i0\\ulnone\\fs28\\fc0\\cf0 Check o
n equation for example 3\012\}\012"}}{INP_R 14 0 "> "{TEXT 2 42 "
a := 1: b:= 1: L := 4: c1 := 1: c2 := 1/2:"}}{INP_R 15 0 "> "
{TEXT 2 11 "evalf(eqn);"}}{INP_R 16 0 "> "{TEXT 2 10 "sort(\",x);
"}}{COM_R 17 0{TEXT 1 10 "Exercise 5"}{NXRTF 0 318 "\{\\rtf0\\ans
i\{\\fonttbl\\f0\\fnil Times-Roman;\\f1\\fmodern Courier;\}\012\\
margl40\012\\margr40\012\{\\colortbl;\\red255\\green255\\blue255;
\}\012\\cb1\\pard\\tx0\\tx1180\\tx2360\\tx3540\\tx4720\\tx5900\\t
x7080\\tx8260\\tx9440\\tx10620\\tx11800\\tx12980\\tx14160\\tx1534
0\\tx16520\\tx17700\\tx18880\\tx20060\\tx21240\\tx22420\\f0\\b\\i
0\\ulnone\\fs36\\fc0\\cf0 Exercise 5\012\}\012"}}{INP_R 18 0 "> "
{TEXT 2 77 "s := 400: h := 1: R := 80000:\012F := x -> cos(s/(R +
 x)) - (R + x)/(R + x + h);"}}{COM_R 19 0{TEXT 4 53 "Note:  make 
sure your have plenty of digits accuracy:"}{NXRTF 0 265 "\{\\rtf0
\\ansi\{\\fonttbl\\f1\\fmodern Courier;\}\012\\margl40\012\\margr
40\012\{\\colortbl;\\red255\\green255\\blue255;\}\012\\cb1\\pard\
\tx1152\\tx2304\\tx3456\\tx4608\\tx5760\\tx6912\\tx8064\\tx9216\\
tx10368\\tx11520\\f1\\b0\\i0\\ulnone\\fs24\\fc0\\cf0 Note:  make 
sure your have plenty of digits accuracy:\012\}\012"}}{INP_R 20 0
 "> "{TEXT 2 49 "Digits := 16:\012signif := 4: niter := 8: xn := 
1.0:"}}{INP_R 21 0 "> "{TEXT 2 84 "for k to niter do\012   xn := 
xn - F(xn) / D(F)(xn):\012   lprint( evalf(xn, signif) )\012od;"}
}{COM_R 22 0{TEXT 1 10 "Exercise 6"}{NXRTF 0 318 "\{\\rtf0\\ansi\
{\\fonttbl\\f0\\fnil Times-Roman;\\f1\\fmodern Courier;\}\012\\ma
rgl40\012\\margr40\012\{\\colortbl;\\red255\\green255\\blue255;\}
\012\\cb1\\pard\\tx0\\tx1180\\tx2360\\tx3540\\tx4720\\tx5900\\tx7
080\\tx8260\\tx9440\\tx10620\\tx11800\\tx12980\\tx14160\\tx15340\
\tx16520\\tx17700\\tx18880\\tx20060\\tx21240\\tx22420\\f0\\b\\i0\
\ulnone\\fs36\\fc0\\cf0 Exercise 6\012\}\012"}}{INP_R 23 0 "> "
{TEXT 2 50 "s := 400: h := 1:\012f := r -> cos(s/r) - r/(r + h); 
"}}{INP_R 24 0 "> "{TEXT 2 17 "plot(f, 10..400);"}}{INP_R 25 0 ">
 "{TEXT 2 5 "f(x);"}}{INP_R 26 0 "> "{TEXT 2 16 "plot(f, 60..70);
"}}{INP_R 27 0 "> "{TEXT 2 123 "signif := 6:  niter := 4:  xn := 
62.0:\012for k to niter do\012   xn := xn - f(xn) / D(f)(xn):\012
   lprint( evalf(xn, signif) )\012od;"}}{INP_R 28 0 "> "{TEXT 2 
123 "signif := 6:  niter := 4:  xn := 65.5:\012for k to niter do\
012   xn := xn - f(xn) / D(f)(xn):\012   lprint( evalf(xn, signif
) )\012od;"}}{COM_R 29 0{TEXT 5 41 "Note: if r = 65.5, then circu
mference is:"}{NXRTF 0 344 "\{\\rtf0\\ansi\{\\fonttbl\\f0\\fnil T
imes-Roman;\\f1\\fmodern Courier;\}\012\\margl40\012\\margr40\012
\{\\colortbl;\\red255\\green255\\blue255;\}\012\\cb1\\pard\\tx0\\
tx720\\tx1440\\tx2160\\tx2880\\tx3600\\tx4320\\tx5040\\tx5760\\tx
6480\\tx7200\\tx7920\\tx8640\\tx9360\\tx10080\\tx10800\\tx11520\\
tx12240\\tx12960\\tx13680\\f0\\b0\\i0\\ulnone\\fs24\\fc0\\cf0 Not
e: if r = 65.5, then circumference is:\012\}\012"}}{INP_R 30 0 ">
 "{TEXT 2 21 "evalf(2 * Pi * 65.5);"}}{COM_R 31 0{TEXT 4 97 "And 
sure enough, in walking 400m the traveling astronaut is only 11.5
m BEHIND the starting point."}{NXRTF 0 309 "\{\\rtf0\\ansi\{\\fon
ttbl\\f1\\fmodern Courier;\}\012\\margl40\012\\margr40\012\{\\col
ortbl;\\red255\\green255\\blue255;\}\012\\cb1\\pard\\tx1152\\tx23
04\\tx3456\\tx4608\\tx5760\\tx6912\\tx8064\\tx9216\\tx10368\\tx11
520\\f1\\b0\\i0\\ulnone\\fs24\\fc0\\cf0 And sure enough, in walki
ng 400m the traveling astronaut is only 11.5m BEHIND the starting
 point.\012\}\012"}}{INP_R 32 0 "> "{TEXT 2 21 "evalf(2 * Pi * 61
.9);"}}{COM_R 33 0{TEXT 4 82 "And now she has tramped all the way
 around the asteroid PLUS an additional 11.1 m!"}{NXRTF 0 294 "\{
\\rtf0\\ansi\{\\fonttbl\\f1\\fmodern Courier;\}\012\\margl40\012\
\margr40\012\{\\colortbl;\\red255\\green255\\blue255;\}\012\\cb1\
\pard\\tx1152\\tx2304\\tx3456\\tx4608\\tx5760\\tx6912\\tx8064\\tx
9216\\tx10368\\tx11520\\f1\\b0\\i0\\ulnone\\fs24\\fc0\\cf0 And no
w she has tramped all the way around the asteroid PLUS an additio
nal 11.1 m!\012\}\012"}}}{END}
