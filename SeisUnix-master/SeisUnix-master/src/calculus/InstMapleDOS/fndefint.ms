{VERSION 1 0 "NEXTSTEP" "1.0"}{GLOBALS 1 0}{FONT 0 "Times-Bold" "
Times" "Times-Bold" 8 24 64 "Times-Bold" 24}{FONT 1 "Times-Bold" 
"Times" "Times-Bold" 8 18 64 "Times-Bold" 18}{FONT 2 "Ohlfs" "Ohl
fs" "Courier" 4 11 192 "Ohlfs" 11}{FONT 3 "Courier-Bold" "Courier
" "Courier-Bold" 8 12 192 "Courier-Bold" 12}{SCP_R 1 0 20{COM_R 2
 0{TEXT 0 30 "Functions Defined by Integrals"}{NXRTF 0 343 "\{\\r
tf0\\ansi\{\\fonttbl\\f1\\fnil Times-Roman;\\f0\\fmodern Courier;
\}\012\\margl40\012\\margr40\012\{\\colortbl;\\red255\\green255\\
blue255;\}\012\\cb1\\pard\\tx0\\tx1580\\tx3160\\tx4740\\tx6320\\t
x7900\\tx9480\\tx11060\\tx12640\\tx14220\\tx15800\\tx17380\\tx189
60\\tx20540\\tx22120\\tx23700\\tx25280\\tx26860\\tx28440\\tx30020
\\f1\\b\\i0\\ulnone\\qc\\fs48\\fc0\\cf0 Functions Defined by Inte
grals\012\}\012"}}{COM_R 3 0{TEXT 1 12 "Introduction"}{NXRTF 0 
320 "\{\\rtf0\\ansi\{\\fonttbl\\f1\\fnil Times-Roman;\\f0\\fmoder
n Courier;\}\012\\margl40\012\\margr40\012\{\\colortbl;\\red255\\
green255\\blue255;\}\012\\cb1\\pard\\tx0\\tx1180\\tx2360\\tx3540\
\tx4720\\tx5900\\tx7080\\tx8260\\tx9440\\tx10620\\tx11800\\tx1298
0\\tx14160\\tx15340\\tx16520\\tx17700\\tx18880\\tx20060\\tx21240\
\tx22420\\f1\\b\\i0\\ulnone\\fs36\\fc0\\cf0 Introduction\012\}\01
2"}}{INP_R 4 0 "> "{TEXT 2 29 "myLog := int( 1/x, x = 1..z);"}}
{INP_R 5 0 "> "{TEXT 2 46 "P := sqrt(1/Pi) * int( exp(-x^2/2), x 
= 0..z);"}}{INP_R 6 0 "> "{TEXT 2 48 "MyErf := 2/sqrt(Pi) * int( 
exp(-t^2), t = 0..z);"}}{OUT_R 7 0 6{DAG :3n4\`MyErf`(3n3\`erf`,2
n3\`z`}}{COM_R 8 0{TEXT 1 10 "Exercise 6"}{NXRTF 0 318 "\{\\rtf0\
\ansi\{\\fonttbl\\f1\\fnil Times-Roman;\\f0\\fmodern Courier;\}\0
12\\margl40\012\\margr40\012\{\\colortbl;\\red255\\green255\\blue
255;\}\012\\cb1\\pard\\tx0\\tx1180\\tx2360\\tx3540\\tx4720\\tx590
0\\tx7080\\tx8260\\tx9440\\tx10620\\tx11800\\tx12980\\tx14160\\tx
15340\\tx16520\\tx17700\\tx18880\\tx20060\\tx21240\\tx22420\\f1\\
b\\i0\\ulnone\\fs36\\fc0\\cf0 Exercise 6\012\}\012"}}{INP_R 9 0 "
> "{TEXT 2 33 "ln(1);\012diff( ln(x), z);\012D(ln)(z);"}}{COM_R 
10 0{TEXT 1 10 "Exercise 7"}{NXRTF 0 318 "\{\\rtf0\\ansi\{\\fontt
bl\\f1\\fnil Times-Roman;\\f0\\fmodern Courier;\}\012\\margl40\01
2\\margr40\012\{\\colortbl;\\red255\\green255\\blue255;\}\012\\cb
1\\pard\\tx0\\tx1180\\tx2360\\tx3540\\tx4720\\tx5900\\tx7080\\tx8
260\\tx9440\\tx10620\\tx11800\\tx12980\\tx14160\\tx15340\\tx16520
\\tx17700\\tx18880\\tx20060\\tx21240\\tx22420\\f1\\b\\i0\\ulnone\
\fs36\\fc0\\cf0 Exercise 7\012\}\012"}}{INP_R 11 0 "> "{TEXT 2 61
 "for z from 1 by .1 to 2 do\012   lprint( z, myLog(z), ln(z))\01
2od;"}}{COM_R 12 0{TEXT 1 10 "Exercise 8"}{NXRTF 0 318 "\{\\rtf0\
\ansi\{\\fonttbl\\f1\\fnil Times-Roman;\\f0\\fmodern Courier;\}\0
12\\margl40\012\\margr40\012\{\\colortbl;\\red255\\green255\\blue
255;\}\012\\cb1\\pard\\tx0\\tx1180\\tx2360\\tx3540\\tx4720\\tx590
0\\tx7080\\tx8260\\tx9440\\tx10620\\tx11800\\tx12980\\tx14160\\tx
15340\\tx16520\\tx17700\\tx18880\\tx20060\\tx21240\\tx22420\\f1\\
b\\i0\\ulnone\\fs36\\fc0\\cf0 Exercise 8\012\}\012"}}{INP_R 13 0 
"> "{TEXT 2 69 "x := 'x':\012erfseries := convert( taylor( erf(x)
, x = 0, 7 ), polynom);"}}{COM_R 14 0{TEXT 1 10 "Exercise 9"}
{NXRTF 0 318 "\{\\rtf0\\ansi\{\\fonttbl\\f1\\fnil Times-Roman;\\f
0\\fmodern Courier;\}\012\\margl40\012\\margr40\012\{\\colortbl;\
\red255\\green255\\blue255;\}\012\\cb1\\pard\\tx0\\tx1180\\tx2360
\\tx3540\\tx4720\\tx5900\\tx7080\\tx8260\\tx9440\\tx10620\\tx1180
0\\tx12980\\tx14160\\tx15340\\tx16520\\tx17700\\tx18880\\tx20060\
\tx21240\\tx22420\\f1\\b\\i0\\ulnone\\fs36\\fc0\\cf0 Exercise 9\0
12\}\012"}}{INP_R 15 0 "> "{TEXT 2 92 "z := 'z':\012erfseries := 
taylor( erf(z), z = 0, 7):\012erfApprox := convert( erfseries, po
lynom);"}}{INP_R 16 0 "> "{TEXT 2 89 "z := 'z':\012for z from 0 b
y .25 to 2 do\012   lprint( z, MyErf, erf(z), evalf( erfApprox))\
012od;"}}{COM_R 17 0{TEXT 1 11 "Exercise 10"}{NXRTF 0 319 "\{\\rt
f0\\ansi\{\\fonttbl\\f1\\fnil Times-Roman;\\f0\\fmodern Courier;\
}\012\\margl40\012\\margr40\012\{\\colortbl;\\red255\\green255\\b
lue255;\}\012\\cb1\\pard\\tx0\\tx1180\\tx2360\\tx3540\\tx4720\\tx
5900\\tx7080\\tx8260\\tx9440\\tx10620\\tx11800\\tx12980\\tx14160\
\tx15340\\tx16520\\tx17700\\tx18880\\tx20060\\tx21240\\tx22420\\f
1\\b\\i0\\ulnone\\fs36\\fc0\\cf0 Exercise 10\012\}\012"}}{INP_R 
18 0 "> "{TEXT 2 52 "subs( z = infinity, MyErf);\012evalf(\");\01
2erf(infinity);"}}{COM_R 19 0{TEXT 1 11 "Exercise 11"}{NXRTF 0 
319 "\{\\rtf0\\ansi\{\\fonttbl\\f1\\fnil Times-Roman;\\f0\\fmoder
n Courier;\}\012\\margl40\012\\margr40\012\{\\colortbl;\\red255\\
green255\\blue255;\}\012\\cb1\\pard\\tx0\\tx1180\\tx2360\\tx3540\
\tx4720\\tx5900\\tx7080\\tx8260\\tx9440\\tx10620\\tx11800\\tx1298
0\\tx14160\\tx15340\\tx16520\\tx17700\\tx18880\\tx20060\\tx21240\
\tx22420\\f1\\b\\i0\\ulnone\\fs36\\fc0\\cf0 Exercise 11\012\}\012
"}}{INP_R 20 0 "> "{TEXT 2 24 "plot( MyErf, z = -5..5);"}}{INP_R 
21 0 "> "{TEXT 2 0 ""}}}{END}
