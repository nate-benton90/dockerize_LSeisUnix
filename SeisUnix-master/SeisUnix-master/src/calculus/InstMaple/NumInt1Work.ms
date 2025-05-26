{VERSION 1 0 "NEXTSTEP" "1.0"}{GLOBALS 1 0}{FONT 0 "Times-Bold" "
Times" "Times-Bold" 8 24 64 "Times-Bold" 24}{FONT 1 "Times-Bold" 
"Times" "Times-Bold" 8 18 64 "Times-Bold" 18}{FONT 2 "Ohlfs" "Ohl
fs" "Courier" 4 11 192 "Ohlfs" 11}{SCP_R 1 0 17{COM_R 2 0{TEXT 0 
23 "Numerical Integration I"}{NXRTF 0 336 "\{\\rtf0\\ansi\{\\font
tbl\\f0\\fnil Times-Roman;\\f1\\fmodern Courier;\}\012\\margl40\0
12\\margr40\012\{\\colortbl;\\red255\\green255\\blue255;\}\012\\c
b1\\pard\\tx0\\tx1580\\tx3160\\tx4740\\tx6320\\tx7900\\tx9480\\tx
11060\\tx12640\\tx14220\\tx15800\\tx17380\\tx18960\\tx20540\\tx22
120\\tx23700\\tx25280\\tx26860\\tx28440\\tx30020\\f0\\b\\i0\\ulno
ne\\qc\\fs48\\fc0\\cf0 Numerical Integration I\012\}\012"}}{COM_R
 3 0{TEXT 1 10 "Exercise 2"}{NXRTF 0 318 "\{\\rtf0\\ansi\{\\fontt
bl\\f0\\fnil Times-Roman;\\f1\\fmodern Courier;\}\012\\margl40\01
2\\margr40\012\{\\colortbl;\\red255\\green255\\blue255;\}\012\\cb
1\\pard\\tx0\\tx1180\\tx2360\\tx3540\\tx4720\\tx5900\\tx7080\\tx8
260\\tx9440\\tx10620\\tx11800\\tx12980\\tx14160\\tx15340\\tx16520
\\tx17700\\tx18880\\tx20060\\tx21240\\tx22420\\f0\\b\\i0\\ulnone\
\fs36\\fc0\\cf0 Exercise 2\012\}\012"}}{INP_R 4 0 "> "{TEXT 2 18 
"f := x -> x^4 + 1:"}}{INP_R 5 0 "> "{TEXT 2 20 "int(f(x), x = 0.
.5);"}}{COM_R 6 0{TEXT 1 10 "Exercise 4"}{NXRTF 0 318 "\{\\rtf0\\
ansi\{\\fonttbl\\f0\\fnil Times-Roman;\\f1\\fmodern Courier;\}\01
2\\margl40\012\\margr40\012\{\\colortbl;\\red255\\green255\\blue2
55;\}\012\\cb1\\pard\\tx0\\tx1180\\tx2360\\tx3540\\tx4720\\tx5900
\\tx7080\\tx8260\\tx9440\\tx10620\\tx11800\\tx12980\\tx14160\\tx1
5340\\tx16520\\tx17700\\tx18880\\tx20060\\tx21240\\tx22420\\f0\\b
\\i0\\ulnone\\fs36\\fc0\\cf0 Exercise 4\012\}\012"}}{INP_R 7 0 ">
 "{TEXT 2 93 "a := 0: b := 5.0:\012n := 5:  delx := (b - a)/n:\01
2Rsum :=  delx * sum(f(a + i*delx), i = 0..n-1);"}}{INP_R 8 0 "> 
"{TEXT 2 76 "n := 10:  delx := (b - a)/n:\012Rsum :=  delx * sum(
f(a + i*delx), i = 0..n-1);"}}{INP_R 9 0 "> "{TEXT 2 77 "n := 100
:  delx := (b - a)/n:\012Rsum :=  delx * sum(f(a + i*delx), i = 0
..n-1);"}}{COM_R 10 0{TEXT 1 10 "Exercise 5"}{NXRTF 0 318 "\{\\rt
f0\\ansi\{\\fonttbl\\f0\\fnil Times-Roman;\\f1\\fmodern Courier;\
}\012\\margl40\012\\margr40\012\{\\colortbl;\\red255\\green255\\b
lue255;\}\012\\cb1\\pard\\tx0\\tx1180\\tx2360\\tx3540\\tx4720\\tx
5900\\tx7080\\tx8260\\tx9440\\tx10620\\tx11800\\tx12980\\tx14160\
\tx15340\\tx16520\\tx17700\\tx18880\\tx20060\\tx21240\\tx22420\\f
0\\b\\i0\\ulnone\\fs36\\fc0\\cf0 Exercise 5\012\}\012"}}{INP_R 11
 0 "> "{TEXT 2 104 "exact := 630:\012n := 10:  delx := (b - a)/n:
\012p := 1.0:\012Rsum :=  delx * sum(f(a + (i+p)*delx), i = 0..n-
1);"}}{INP_R 12 0 "> "{TEXT 2 91 "n := 100:  delx := (b - a)/n:\0
12p := 1.0:\012Rsum :=  delx * sum(f(a + (i+p)*delx), i = 0..n-1)
;"}}{COM_R 13 0{TEXT 1 10 "Exercise 6"}{NXRTF 0 318 "\{\\rtf0\\an
si\{\\fonttbl\\f0\\fnil Times-Roman;\\f1\\fmodern Courier;\}\012\
\margl40\012\\margr40\012\{\\colortbl;\\red255\\green255\\blue255
;\}\012\\cb1\\pard\\tx0\\tx1180\\tx2360\\tx3540\\tx4720\\tx5900\\
tx7080\\tx8260\\tx9440\\tx10620\\tx11800\\tx12980\\tx14160\\tx153
40\\tx16520\\tx17700\\tx18880\\tx20060\\tx21240\\tx22420\\f0\\b\\
i0\\ulnone\\fs36\\fc0\\cf0 Exercise 6\012\}\012"}}{INP_R 14 0 "> 
"{TEXT 2 52 "lprint(`left-endpoint   midpoint   right-endpoint`);
"}}{INP_R 15 0 "> "{TEXT 2 346 "a := 0:  b := 5.0:\012printlevel 
:= 0:\012lprint(`n   left-endpoint   midpoint   right-endpoint`);
\012for k to 3 do\012  n := 10^k:  delx := (b - a)/n:\012  lprint
( n,\012          exact - delx * sum(f(a + (i)    *delx), i = 0..
n-1),\012          exact - delx * sum(f(a + (i+0.5)*delx), i = 0.
.n-1),\012          exact - delx * sum(f(a + (i+1)  *delx), i = 0
..n-1) )\012od;"}}{COM_R 16 0{TEXT 1 10 "Exercise 7"}{NXRTF 0 318
 "\{\\rtf0\\ansi\{\\fonttbl\\f0\\fnil Times-Roman;\\f1\\fmodern C
ourier;\}\012\\margl40\012\\margr40\012\{\\colortbl;\\red255\\gre
en255\\blue255;\}\012\\cb1\\pard\\tx0\\tx1180\\tx2360\\tx3540\\tx
4720\\tx5900\\tx7080\\tx8260\\tx9440\\tx10620\\tx11800\\tx12980\\
tx14160\\tx15340\\tx16520\\tx17700\\tx18880\\tx20060\\tx21240\\tx
22420\\f0\\b\\i0\\ulnone\\fs36\\fc0\\cf0 Exercise 7\012\}\012"}}
{INP_R 17 0 "> "{TEXT 2 70 "f := x -> sqrt(100 - x^2):\012a := 0:
  b := 10.0:\012exact := evalf(25.*Pi);"}}{INP_R 18 0 "> "{TEXT 2
 310 "lprint(`n   left-endpoint   midpoint   right-endpoint`);\01
2for k to 3 do\012  n := 10^k:  delx := (b - a)/n:\012  lprint( n
,\012          exact - delx * sum(f(a + (i)    *delx), i = 0..n-1
),\012          exact - delx * sum(f(a + (i+0.5)*delx), i = 0..n-
1),\012          exact - delx * sum(f(a + (i+1)  *delx), i = 0..n
-1) )\012od;"}}}{END}
