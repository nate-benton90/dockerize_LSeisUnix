{VERSION 1 0 "NEXTSTEP" "1.0"}{GLOBALS 1 0}{FONT 0 "Times-Bold" "
Times" "Times-Bold" 8 24 64 "Times-Bold" 24}{FONT 1 "Times-Bold" 
"Times" "Times-Bold" 8 18 64 "Times-Bold" 18}{FONT 2 "Ohlfs" "Ohl
fs" "Courier" 4 11 192 "Ohlfs" 11}{FONT 3 "Times-Bold" "Times" "T
imes-Bold" 8 14 64 "Times-Bold" 14}{FONT 4 "Courier" "Courier" "C
ourier" 4 12 192 "Courier" 12}{SCP_R 1 0 20{COM_R 2 0{TEXT 0 5 "C
haos"}{NXRTF 0 318 "\{\\rtf0\\ansi\{\\fonttbl\\f0\\fnil Times-Rom
an;\\f1\\fmodern Courier;\}\012\\margl40\012\\margr40\012\{\\colo
rtbl;\\red255\\green255\\blue255;\}\012\\cb1\\pard\\tx0\\tx1580\\
tx3160\\tx4740\\tx6320\\tx7900\\tx9480\\tx11060\\tx12640\\tx14220
\\tx15800\\tx17380\\tx18960\\tx20540\\tx22120\\tx23700\\tx25280\\
tx26860\\tx28440\\tx30020\\f0\\b\\i0\\ulnone\\qc\\fs48\\fc0\\cf0 
Chaos\012\}\012"}}{COM_R 3 0{TEXT 1 25 "Slow Convergence  Example
"}{NXRTF 0 333 "\{\\rtf0\\ansi\{\\fonttbl\\f0\\fnil Times-Roman;\
\f1\\fmodern Courier;\}\012\\margl40\012\\margr40\012\{\\colortbl
;\\red255\\green255\\blue255;\}\012\\cb1\\pard\\tx0\\tx1180\\tx23
60\\tx3540\\tx4720\\tx5900\\tx7080\\tx8260\\tx9440\\tx10620\\tx11
800\\tx12980\\tx14160\\tx15340\\tx16520\\tx17700\\tx18880\\tx2006
0\\tx21240\\tx22420\\f0\\b\\i0\\ulnone\\fs36\\fc0\\cf0 Slow Conve
rgence  Example\012\}\012"}}{INP_R 4 0 "> "{TEXT 2 52 "f := x -> 
expand( 27*(x-2/3)^3 *(x^2 + 1)*(x - 6) );"}}{INP_R 5 0 "> "{TEXT
 2 5 "f(x);"}}{INP_R 6 0 "> "{TEXT 2 15 "plot(f, -2..2);"}}{INP_R
 7 0 "> "{TEXT 2 90 "xn := 1.0: printlevel := 0:\012for k to 20 d
o\012   xn := xn - f(xn)/D(f)(xn):\012   lprint(xn)\012od;"}}
{COM_R 8 0{TEXT 1 5 "Chaos"}{NXRTF 0 313 "\{\\rtf0\\ansi\{\\fontt
bl\\f0\\fnil Times-Roman;\\f1\\fmodern Courier;\}\012\\margl40\01
2\\margr40\012\{\\colortbl;\\red255\\green255\\blue255;\}\012\\cb
1\\pard\\tx0\\tx1180\\tx2360\\tx3540\\tx4720\\tx5900\\tx7080\\tx8
260\\tx9440\\tx10620\\tx11800\\tx12980\\tx14160\\tx15340\\tx16520
\\tx17700\\tx18880\\tx20060\\tx21240\\tx22420\\f0\\b\\i0\\ulnone\
\fs36\\fc0\\cf0 Chaos\012\}\012"}}{COM_R 9 0{TEXT 3 27 "Basic Def
initions and Plots"}{NXRTF 0 331 "\{\\rtf0\\ansi\{\\fonttbl\\f0\\
fnil Times-Roman;\\f1\\fmodern Courier;\}\012\\margl40\012\\margr
40\012\{\\colortbl;\\red255\\green255\\blue255;\}\012\\cb1\\pard\
\tx0\\tx900\\tx1800\\tx2700\\tx3600\\tx4500\\tx5400\\tx6300\\tx72
00\\tx8100\\tx9000\\tx9900\\tx10800\\tx11700\\tx12600\\tx13500\\t
x14400\\tx15300\\tx16200\\tx17100\\f0\\b\\i0\\ulnone\\fs28\\fc0\\
cf0 Basic Definitions and Plots\012\}\012"}}{INP_R 10 0 "> "{TEXT
 2 56 "f := x -> x^3 - x:\012s3 := 1/sqrt(3.0);\012s5 := 1/sqrt(5
.0);"}}{INP_R 11 0 "> "{TEXT 2 15 "plot(f, -4..4);"}}{INP_R 12 0 
"> "{TEXT 2 30 "plot(f, -1..1, xtickmarks=20);"}}{INP_R 13 0 "> "
{TEXT 2 71 "xn := s3:\012for k to 5 do\012   xn := xn - f(xn)/D(f
)(xn):\012   lprint(xn)\012od;"}}{INP_R 14 0 "> "{TEXT 2 72 "xn :
= s5:\012for k to 10 do\012   xn := xn - f(xn)/D(f)(xn):\012   lp
rint(xn)\012od;"}}{INP_R 15 0 "> "{TEXT 2 72 "xn := 0.8:\012for k
 to 6 do\012   xn := xn - f(xn)/D(f)(xn):\012   lprint(xn)\012od;
"}}{INP_R 16 0 "> "{TEXT 2 73 "xn := 2.0:\012for k to 10 do\012  
 xn := xn - f(xn)/D(f)(xn):\012   lprint(xn)\012od;"}}{INP_R 17 0
 "> "{TEXT 2 73 "xn := 0.4:\012for k to 10 do\012   xn := xn - f(
xn)/D(f)(xn):\012   lprint(xn)\012od;"}}{COM_R 18 0{TEXT 4 40 "Se
tting up the array of `magic' numbers:"}{NXRTF 0 252 "\{\\rtf0\\a
nsi\{\\fonttbl\\f1\\fmodern Courier;\}\012\\margl40\012\\margr40\
012\{\\colortbl;\\red255\\green255\\blue255;\}\012\\cb1\\pard\\tx
1152\\tx2304\\tx3456\\tx4608\\tx5760\\tx6912\\tx8064\\tx9216\\tx1
0368\\tx11520\\f1\\b0\\i0\\ulnone\\fs24\\fc0\\cf0 Setting up the 
array of `magic' numbers:\012\}\012"}}{INP_R 19 0 "> "{TEXT 2 19 
"mag := array(1..6);"}}{INP_R 20 0 "> "{TEXT 2 120 "mag[1] := 0.4
48955: mag[2] := 0.447503: mag[3] := 0.447262:\012mag[4] := 0.447
222: mag[5] := 0.447215: mag[6] := 0.447213: "}}{INP_R 21 0 "> "
{TEXT 2 131 "for k to 6 do\012   lprint(`k = `, k);\012   xn := m
ag[k]:\012   for j to 10 do\012      xn := xn - f(xn)/D(f)(xn):\0
12      lprint(xn)\012   od;\012od;"}}}{END}
