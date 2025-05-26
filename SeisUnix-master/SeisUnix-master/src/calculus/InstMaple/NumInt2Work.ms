{VERSION 1 0 "NEXTSTEP" "1.0"}{GLOBALS 1 0}{FONT 0 "Times-Bold" "
Times" "Times-Bold" 8 24 64 "Times-Bold" 24}{FONT 1 "Times-Bold" 
"Times" "Times-Bold" 8 18 64 "Times-Bold" 18}{FONT 2 "Ohlfs" "Ohl
fs" "Courier" 4 11 192 "Ohlfs" 11}{FONT 3 "Times-Roman" "Times" "
Times-Roman" 4 14 64 "Times-Roman" 14}{FONT 4 "Courier" "Courier"
 "Courier" 4 12 192 "Courier" 12}{SCP_R 1 0 40{COM_R 2 0{TEXT 0 
31 "Area & Numerical Integration II"}{NXRTF 0 344 "\{\\rtf0\\ansi
\{\\fonttbl\\f0\\fnil Times-Roman;\\f1\\fmodern Courier;\}\012\\m
argl40\012\\margr40\012\{\\colortbl;\\red255\\green255\\blue255;\
}\012\\cb1\\pard\\tx0\\tx1580\\tx3160\\tx4740\\tx6320\\tx7900\\tx
9480\\tx11060\\tx12640\\tx14220\\tx15800\\tx17380\\tx18960\\tx205
40\\tx22120\\tx23700\\tx25280\\tx26860\\tx28440\\tx30020\\f0\\b\\
i0\\ulnone\\qc\\fs48\\fc0\\cf0 Area & Numerical Integration II\01
2\}\012"}}{COM_R 3 0{TEXT 1 23 "Material for Exercise 2"}{NXRTF 0
 331 "\{\\rtf0\\ansi\{\\fonttbl\\f0\\fnil Times-Roman;\\f1\\fmode
rn Courier;\}\012\\margl40\012\\margr40\012\{\\colortbl;\\red255\
\green255\\blue255;\}\012\\cb1\\pard\\tx0\\tx1180\\tx2360\\tx3540
\\tx4720\\tx5900\\tx7080\\tx8260\\tx9440\\tx10620\\tx11800\\tx129
80\\tx14160\\tx15340\\tx16520\\tx17700\\tx18880\\tx20060\\tx21240
\\tx22420\\f0\\b\\i0\\ulnone\\fs36\\fc0\\cf0 Material for Exercis
e 2\012\}\012"}}{INP_R 4 0 "> "{TEXT 2 30 "A := x -> int(f(t), t 
= a..x);"}}{INP_R 5 0 "> "{TEXT 2 5 "A(x);"}}{INP_R 6 0 "> "{TEXT
 2 14 "diff(A(x), x);"}}{INP_R 7 0 "> "{TEXT 2 8 "D(A)(x);"}}
{INP_R 8 0 "> "{TEXT 2 20 "f := x -> 5;\012a := 1:"}}{INP_R 9 0 "
> "{TEXT 2 5 "A(x);"}}{COM_R 10 0{TEXT 1 38 "Simpson by hand -- n
ot used in project"}{NXRTF 0 346 "\{\\rtf0\\ansi\{\\fonttbl\\f0\\
fnil Times-Roman;\\f1\\fmodern Courier;\}\012\\margl40\012\\margr
40\012\{\\colortbl;\\red255\\green255\\blue255;\}\012\\cb1\\pard\
\tx0\\tx1180\\tx2360\\tx3540\\tx4720\\tx5900\\tx7080\\tx8260\\tx9
440\\tx10620\\tx11800\\tx12980\\tx14160\\tx15340\\tx16520\\tx1770
0\\tx18880\\tx20060\\tx21240\\tx22420\\f0\\b\\i0\\ulnone\\fs36\\f
c0\\cf0 Simpson by hand -- not used in project\012\}\012"}}{INP_R
 11 0 "> "{TEXT 2 20 "f := 'f':  a := 'a':"}}{COM_R 12 0{TEXT 1 
25 "Basic integration modules"}{NXRTF 0 333 "\{\\rtf0\\ansi\{\\fo
nttbl\\f0\\fnil Times-Roman;\\f1\\fmodern Courier;\}\012\\margl40
\012\\margr40\012\{\\colortbl;\\red255\\green255\\blue255;\}\012\
\cb1\\pard\\tx0\\tx1180\\tx2360\\tx3540\\tx4720\\tx5900\\tx7080\\
tx8260\\tx9440\\tx10620\\tx11800\\tx12980\\tx14160\\tx15340\\tx16
520\\tx17700\\tx18880\\tx20060\\tx21240\\tx22420\\f0\\b\\i0\\ulno
ne\\fs36\\fc0\\cf0 Basic integration modules\012\}\012"}}{INP_R 
13 0 "> "{TEXT 2 99 "Rsum := proc(f, a, b, n, p)\012   delx := (b
-a)/n;\012   delx * sum ( f(a + (i+p)*delx), i = 0..n-1)\012end;"
}}{INP_R 14 0 "> "{TEXT 2 53 "Mid := proc(f, a, b, n)\012   Rsum(
f, a, b, n, 0.5)\012end;"}}{INP_R 15 0 "> "{TEXT 2 100 "Trap := p
roc(f, a, b, n)\012    0.5 * ( Rsum(f, a, b, n, 0) + Rsum(f, a, b
, n, 1) );\012    evalf(\") \012end;"}}{INP_R 16 0 "> "{TEXT 2 82
 "f := x -> sin(x):\012a := 0.0: b := evalf(Pi):\012n := 5:  p :=
 .5:\012Rsum(f, a, b, n, p);"}}{INP_R 17 0 "> "{TEXT 2 20 "Trap(s
in, a, b, 10);"}}{INP_R 18 0 "> "{TEXT 2 57 "f := x -> x:\012a :=
 0:  b := 1.0:\012n := 5:\012Trap(f, a, b, n);"}}{INP_R 19 0 "> "
{TEXT 2 86 "Simp := proc(f, a, b, n)\012    ( Trap(f, a, b, n, 0)
 + 2*Mid(f, a, b, n, 1) ) / 3.0\012end;"}}{INP_R 20 0 "> "{TEXT 2
 69 "f := x -> sin(x):\012a := 0.0: b := evalf(Pi):\012n := 5:\01
2Simp(f, a, b, n);"}}{COM_R 21 0{TEXT 1 10 "Exercise 5"}{NXRTF 0 
318 "\{\\rtf0\\ansi\{\\fonttbl\\f0\\fnil Times-Roman;\\f1\\fmoder
n Courier;\}\012\\margl40\012\\margr40\012\{\\colortbl;\\red255\\
green255\\blue255;\}\012\\cb1\\pard\\tx0\\tx1180\\tx2360\\tx3540\
\tx4720\\tx5900\\tx7080\\tx8260\\tx9440\\tx10620\\tx11800\\tx1298
0\\tx14160\\tx15340\\tx16520\\tx17700\\tx18880\\tx20060\\tx21240\
\tx22420\\f0\\b\\i0\\ulnone\\fs36\\fc0\\cf0 Exercise 5\012\}\012"
}}{INP_R 22 0 "> "{TEXT 2 141 "A := x -> int(f(t), t = a..x);\012
f := x -> sin(x):\012a := 0.0: b := evalf(Pi/2):\012u := f(x):  y
 := A(x):  z := D(A)(x):\012plot(\{u, y, z\}, x = a..b);"}}{COM_R
 23 0{TEXT 1 18 "Exercises 6 and 10"}{NXRTF 0 326 "\{\\rtf0\\ansi
\{\\fonttbl\\f0\\fnil Times-Roman;\\f1\\fmodern Courier;\}\012\\m
argl40\012\\margr40\012\{\\colortbl;\\red255\\green255\\blue255;\
}\012\\cb1\\pard\\tx0\\tx1180\\tx2360\\tx3540\\tx4720\\tx5900\\tx
7080\\tx8260\\tx9440\\tx10620\\tx11800\\tx12980\\tx14160\\tx15340
\\tx16520\\tx17700\\tx18880\\tx20060\\tx21240\\tx22420\\f0\\b\\i0
\\ulnone\\fs36\\fc0\\cf0 Exercises 6 and 10\012\}\012"}}{INP_R 24
 0 "> "{TEXT 2 57 "f := x -> sin(x):\012a := 0.0: b := evalf(Pi):
\012exact := 2.0:"}}{INP_R 25 0 "> "{TEXT 2 52 "lprint( `x       
MidE         TrapE         SimpE`);"}}{INP_R 26 0 "> "{TEXT 2 239
 "Digits := 10:\012printlevel := 0:\012lprint( `x       MidE     
    TrapE         SimpE`);\012for k to 7 do\012   n := 2^k:\012  
 lprint( n,\012           exact - Mid(f, a, b, n),\012           
exact - Trap(f, a, b, n),\012           exact - Simp(f, a, b, n) 
)\012od;"}}{COM_R 27 0{TEXT 3 21 "Compare last two rows"}{NXRTF 0
 326 "\{\\rtf0\\ansi\{\\fonttbl\\f0\\fnil Times-Roman;\\f1\\fmode
rn Courier;\}\012\\margl40\012\\margr40\012\{\\colortbl;\\red255\
\green255\\blue255;\}\012\\cb1\\pard\\tx0\\tx840\\tx1680\\tx2520\
\tx3360\\tx4200\\tx5040\\tx5880\\tx6720\\tx7560\\tx8400\\tx9240\\
tx10080\\tx10920\\tx11760\\tx12600\\tx13440\\tx14280\\tx15120\\tx
15960\\f0\\b0\\i0\\ulnone\\fs28\\fc0\\cf0 Compare last two rows\0
12\}\012"}}{INP_R 28 0 "> "{TEXT 2 71 "pMid := evalf( log[2](2008
12/50226) );\012cMid := -.0000502003 * 128^pMid;"}}{INP_R 29 0 ">
 "{TEXT 2 74 "pTrap := evalf( log[2](4016/1004) );\012cTrap := .1
00374*10^(-3) * 128^pTrap;"}}{COM_R 30 0{TEXT 4 102 "Since for la
rge n we are getting within the machine tolerance,\012we compare 
the ratio for n = 16 and 32:"}{NXRTF 0 315 "\{\\rtf0\\ansi\{\\fon
ttbl\\f1\\fmodern Courier;\}\012\\margl40\012\\margr40\012\{\\col
ortbl;\\red255\\green255\\blue255;\}\012\\cb1\\pard\\tx1152\\tx23
04\\tx3456\\tx4608\\tx5760\\tx6912\\tx8064\\tx9216\\tx10368\\tx11
520\\f1\\b0\\i0\\ulnone\\fs24\\fc0\\cf0 Since for large n we are 
getting within the machine tolerance,\\\012we compare the ratio f
or n = 16 and 32:\012\}\012"}}{INP_R 31 0 "> "{TEXT 2 71 "pSimp :
= evalf( log[2](1034/63) );\012cSimp := 26.0 * 10^(-9) * 128^pSim
p;"}}{COM_R 32 0{TEXT 1 18 "Exercises 7 and 11"}{NXRTF 0 326 "\{\
\rtf0\\ansi\{\\fonttbl\\f0\\fnil Times-Roman;\\f1\\fmodern Courie
r;\}\012\\margl40\012\\margr40\012\{\\colortbl;\\red255\\green255
\\blue255;\}\012\\cb1\\pard\\tx0\\tx1180\\tx2360\\tx3540\\tx4720\
\tx5900\\tx7080\\tx8260\\tx9440\\tx10620\\tx11800\\tx12980\\tx141
60\\tx15340\\tx16520\\tx17700\\tx18880\\tx20060\\tx21240\\tx22420
\\f0\\b\\i0\\ulnone\\fs36\\fc0\\cf0 Exercises 7 and 11\012\}\012"
}}{INP_R 33 0 "> "{TEXT 2 68 "f := x -> sin(sin(x)):\012a := 0:  
b := evalf(Pi):\012int(f(x), x = a..b);"}}{INP_R 34 0 "> "{TEXT 2
 18 "exact := evalf(\");"}}{INP_R 35 0 "> "{TEXT 2 208 "lprint( `
x       MidE         TrapE         SimpE`);\012for k to 7 do\012 
  n := 2^k:\012   lprint( n,\012           exact - Mid(f, a, b, n
),\012           exact - Trap(f, a, b, n),\012           exact - 
Simp(f, a, b, n) )\012od;"}}{INP_R 36 0 "> "{TEXT 2 195 "pMid := 
evalf( log[2](8036/2008) );\012cMid := -.0002008 * 64^pMid;\012pT
rap := evalf( log[2](1607/402) );\012cTrap := .000402 * 64^pTrap;
\012pSimp := evalf( log[2](130/9) );\012cSimp := 9 * 10^(-9) * 64
^pSimp;"}}{COM_R 37 0{TEXT 1 18 "Exercises 8 and 12"}{NXRTF 0 326
 "\{\\rtf0\\ansi\{\\fonttbl\\f0\\fnil Times-Roman;\\f1\\fmodern C
ourier;\}\012\\margl40\012\\margr40\012\{\\colortbl;\\red255\\gre
en255\\blue255;\}\012\\cb1\\pard\\tx0\\tx1180\\tx2360\\tx3540\\tx
4720\\tx5900\\tx7080\\tx8260\\tx9440\\tx10620\\tx11800\\tx12980\\
tx14160\\tx15340\\tx16520\\tx17700\\tx18880\\tx20060\\tx21240\\tx
22420\\f0\\b\\i0\\ulnone\\fs36\\fc0\\cf0 Exercises 8 and 12\012\}
\012"}}{INP_R 38 0 "> "{TEXT 2 84 "f := x -> sqrt(4 - x^2):\012a 
:= 0:  b := 2.0:\012exact := evalf(Pi);\012int(f(x), x = a..b);"}
}{INP_R 39 0 "> "{TEXT 2 208 "lprint( `x       MidE         TrapE
         SimpE`);\012for k to 5 do\012   n := 2^k:\012   lprint( 
n,\012           exact - Mid(f, a, b, n),\012           exact - T
rap(f, a, b, n),\012           exact - Simp(f, a, b, n) )\012od;"
}}{INP_R 40 0 "> "{TEXT 2 197 "pMid := evalf( log[2](5359/1899) )
;\012cMid := -.001899 * 32^pMid;\012pTrap := evalf( log[2](1834/6
49) );\012cTrap := .006490402 * 32^pTrap;\012pSimp := evalf( log[
2](2540/898) );\012cSimp := .0008976 * 32^pSimp;"}}{INP_R 41 0 ">
 "{TEXT 2 0 ""}}}{END}
