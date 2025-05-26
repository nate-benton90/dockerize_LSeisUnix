{VERSION 1 0 "NEXTSTEP" "1.0"}{GLOBALS 1 0}{FONT 0 "Times-Bold" "
Times" "Times-Bold" 8 24 64 "Times-Bold" 24}{FONT 1 "Times-Bold" 
"Times" "Times-Bold" 8 18 64 "Times-Bold" 18}{FONT 2 "Ohlfs" "Ohl
fs" "Courier" 4 11 192 "Ohlfs" 11}{FONT 3 "Courier-Bold" "Courier
" "Courier-Bold" 8 12 192 "Courier-Bold" 12}{FONT 4 "Times-Roman"
 "Times" "Times-Roman" 4 12 64 "Times-Roman" 12}{FONT 5 "Courier"
 "Courier" "Courier" 4 12 192 "Courier" 12}{SCP_R 1 0 78{COM_R 2 
0{TEXT 0 30 "Solving Differential Equations"}{NXRTF 0 343 "\{\\rt
f0\\ansi\{\\fonttbl\\f0\\fnil Times-Roman;\\f1\\fmodern Courier;\
}\012\\margl40\012\\margr40\012\{\\colortbl;\\red255\\green255\\b
lue255;\}\012\\cb1\\pard\\tx0\\tx1580\\tx3160\\tx4740\\tx6320\\tx
7900\\tx9480\\tx11060\\tx12640\\tx14220\\tx15800\\tx17380\\tx1896
0\\tx20540\\tx22120\\tx23700\\tx25280\\tx26860\\tx28440\\tx30020\
\f0\\b\\i0\\ulnone\\qc\\fs48\\fc0\\cf0 Solving Differential Equat
ions\012\}\012"}}{COM_R 3 0{TEXT 1 23 "Exercise 1 (with Maple)"}
{NXRTF 0 364 "\{\\rtf0\\ansi\{\\fonttbl\\f0\\fnil Times-Roman;\\f
1\\fmodern Courier;\}\012\\margl40\012\\margr40\012\{\\colortbl;\
\red255\\green255\\blue255;\\red0\\green0\\blue0;\}\012\\cb1\\par
d\\tx0\\tx1180\\tx2360\\tx3540\\tx4720\\tx5900\\tx7080\\tx8260\\t
x9440\\tx10620\\tx11800\\tx12980\\tx14160\\tx15340\\tx16520\\tx17
700\\tx18880\\tx20060\\tx21240\\tx22420\\f0\\b\\i0\\ulnone\\fs36\
\fc2\\cf2 Exercise 1 (with \012\\b0\\i Maple\012\\b\\i0 )\012\}\0
12"}}{INP_R 4 0 "> "{TEXT 2 74 "ypp := 3*x - 5*x^3:\012yp := beta
 + int( ypp, x ):\012y := alpha + int ( yp, x);"}}{OUT_R 5 0 4
{DAG :3n3\`y`+9n4\`alpha`j2x0001*5n4\`beta`p7n3\`x`p7p7*3pEj2x000
3/3p7j2x0002*3pEj2x0005/3i2x0001j2x0004}}{INP_R 6 0 "> "{TEXT 2 
69 "ypp := sin(x):\012yp := beta + int( ypp, x ):\012y := alpha +
 int ( yp, x);"}}{OUT_R 7 0 6{DAG :3n3\`y`+7n4\`alpha`j2x0001*5n4
\`beta`p7n3\`x`p7p7(3n3\`sin`,2pEi2x0001}}{INP_R 8 0 "> "{TEXT 2 
110 "ypp := convert( taylor( sin(x^2), x=0, 16 ), polynom );\012y
p := beta + int( ypp, x ):\012y := alpha + int ( yp, x);"}}{OUT_R
 9 0 8{DAG :3n3\`ypp`+9*3n3\`x`j2x0002j2x0001*3p5j2x0006/3i2x0001
pD*3p5j2x0010/3p9j2x0120*3p5j2x0014/3p10j2x5040}}{OUT_R 10 0 8
{DAG :3n3\`y`+Dn4\`alpha`j2x0001*5n4\`beta`p7n3\`x`p7p7*3pEj2x000
4/3p7j2x0012*3pEj2x0008/3i2x0001j2x0336*3pEp18/3p7j3x00015840*3pE
j2x0016/3p1Fj3x01209600}}{COM_R 11 0{TEXT 1 24 "Exercise 2  (with
 Maple)"}{NXRTF 0 365 "\{\\rtf0\\ansi\{\\fonttbl\\f0\\fnil Times-
Roman;\\f1\\fmodern Courier;\}\012\\margl40\012\\margr40\012\{\\c
olortbl;\\red255\\green255\\blue255;\\red0\\green0\\blue0;\}\012\
\cb1\\pard\\tx0\\tx1180\\tx2360\\tx3540\\tx4720\\tx5900\\tx7080\\
tx8260\\tx9440\\tx10620\\tx11800\\tx12980\\tx14160\\tx15340\\tx16
520\\tx17700\\tx18880\\tx20060\\tx21240\\tx22420\\f0\\b\\i0\\ulno
ne\\fs36\\fc2\\cf2 Exercise 2  (with \012\\b0\\i Maple\012\\b\\i0
 )\012\}\012"}}{INP_R 12 0 "> "{TEXT 2 124 "f := x -> sin(x^2):\0
12kMax := 3:\012Pn := taylor( sin(x^2), x=0, kMax );\012for k fro
m 0 to kMax do\012   b[k] := coeff( Pn, x, k)\012od: "}}{OUT_R 13
 0 12{DAG :3n3\`Pn`_6n3\`x`j2x0001x2(3n3\`O`,2p6x6}}{INP_R 14 0 "
> "{TEXT 2 138 "a[0] := alpha:  a[1] := beta:\012for k from 0 to 
kMax do\012   a[k+2] := (b[k] - a[k]) / ((k+1)*(k+2))\012od; \012
yn := sum( a[n]*x^n, n = 0..kMax+2);"}}{OUT_R 15 0 14{DAG :3b3n3\
`a`,2j2x0002+3n4\`alpha`/3i2x0001p5}}{OUT_R 16 0 14{DAG :3b3n3\`a
`,2j2x0003+3n4\`beta`/3i2x0001j2x0006}}{OUT_R 17 0 14{DAG :3b3n3\
`a`,2j2x0004+5/3j2x0001j2x0012p9n4\`alpha`/3p9j2x0024}}{OUT_R 18 
0 14{DAG :3b3n3\`a`,2j2x0005+3n4\`beta`/3j2x0001j2x0120}}{OUT_R 
19 0 14{DAG :3n3\`yn`+Dn4\`alpha`j2x0001*5n4\`beta`p7n3\`x`p7p7*5
p4p7pEj2x0002/3i2x0001p16*5pAp7pEj2x0003/3p19j2x0006*5+5/3p7j2x00
12p7p4/3p7j2x0024p7pEj2x0004p7*5pAp7pEj2x0005/3p7j2x0120}}{COM_R 
20 0{TEXT 1 10 "Exercise 3"}{NXRTF 0 318 "\{\\rtf0\\ansi\{\\fontt
bl\\f0\\fnil Times-Roman;\\f1\\fmodern Courier;\}\012\\margl40\01
2\\margr40\012\{\\colortbl;\\red255\\green255\\blue255;\}\012\\cb
1\\pard\\tx0\\tx1180\\tx2360\\tx3540\\tx4720\\tx5900\\tx7080\\tx8
260\\tx9440\\tx10620\\tx11800\\tx12980\\tx14160\\tx15340\\tx16520
\\tx17700\\tx18880\\tx20060\\tx21240\\tx22420\\f0\\b\\i0\\ulnone\
\fs36\\fc0\\cf0 Exercise 3\012\}\012"}}{COM_R 21 0{TEXT 4 118 "He
re is the code for kMax = 11.  Note the approximation is failing 
at the end of the interval.\012kMax = 15 does the job."}{NXRTF 0 
428 "\{\\rtf0\\ansi\{\\fonttbl\\f0\\fnil Times-Roman;\\f1\\fmoder
n Courier;\}\012\\margl40\012\\margr40\012\{\\colortbl;\\red255\\
green255\\blue255;\}\012\\cb1\\pard\\tx0\\tx1180\\tx2360\\tx3540\
\tx4720\\tx5900\\tx7080\\tx8260\\tx9440\\tx10620\\tx11800\\tx1298
0\\tx14160\\tx15340\\tx16520\\tx17700\\tx18880\\tx20060\\tx21240\
\tx22420\\f0\\b0\\i0\\ulnone\\fs24\\fc0\\cf0 Here is the code for
 kMax = 11.  Note the approximation is failing at the end of the 
interval.\\\012kMax = 15 does the job.\012\}\012"}}{INP_R 22 0 ">
 "{TEXT 2 255 "f := x -> 3*cos(2*x):\012kMax := 11:\012Pn := tayl
or( f(x), x=0, kMax );\012for k from 0 to kMax do\012   b[k] := c
oeff( Pn, x, k)\012od: \012a[0] := 2:  a[1] := 0:\012for k from 0
 to kMax do\012   a[k+2] := (b[k] - a[k]) / ((k+1)*(k+2))\012od: 
\012yn := sum( a[n]*x^n, n = 0..kMax+1);"}}{OUT_R 23 0 22{DAG :3n
3\`Pn`_10n3\`x`j2x0003x0i2x0006x2j2x0002x4/3i2x0004j2x0015x6/3pCj
2x0105x8/3p10j2x4725xA(3n3\`O`,2j2x0001xB}}{OUT_R 24 0 22{DAG :3n
3\`yn`+Fj2x0002j2x0001*3n3\`x`p4/3p6p4*3p9j2x0004/3i2x0013j2x0024
*3p9j2x0006/3j2x0061j2x0720*3p9j2x0008/3i2x0253j3x00040320*3p9j2x
0010/3j2x1021j3x03628800*3p9j2x0012/3i2x4093j4x000479001600}}
{INP_R 25 0 "> "{TEXT 2 47 "plot( \{yn, 3*cos(x) - cos(2*x)\}, x 
= -Pi..Pi );"}}{COM_R 26 0{TEXT 4 32 "14 terms does the job out t
o pi:"}{NXRTF 0 335 "\{\\rtf0\\ansi\{\\fonttbl\\f0\\fnil Times-Ro
man;\\f1\\fmodern Courier;\}\012\\margl40\012\\margr40\012\{\\col
ortbl;\\red255\\green255\\blue255;\}\012\\cb1\\pard\\tx0\\tx720\\
tx1440\\tx2160\\tx2880\\tx3600\\tx4320\\tx5040\\tx5760\\tx6480\\t
x7200\\tx7920\\tx8640\\tx9360\\tx10080\\tx10800\\tx11520\\tx12240
\\tx12960\\tx13680\\f0\\b0\\i0\\ulnone\\fs24\\fc0\\cf0 14 terms d
oes the job out to pi:\012\}\012"}}{INP_R 27 0 "> "{TEXT 2 233 "k
Max := 15:\012Pn := taylor( f(x), x=0, kMax );\012for k from 0 to
 kMax do\012   b[k] := coeff( Pn, x, k)\012od: \012a[0] := 2:  a[
1] := 0:\012for k from 0 to kMax do\012   a[k+2] := (b[k] - a[k])
 / ((k+1)*(k+2))\012od: \012yn := sum( a[n]*x^n, n = 0..kMax+1);"
}}{OUT_R 28 0 27{DAG :3n3\`Pn`_14n3\`x`j2x0003x0i2x0006x2j2x0002x
4/3i2x0004j2x0015x6/3pCj2x0105x8/3p10j2x4725xA/3j2x0004j3x0015592
5xC/3i2x0008j3x14189175xE(3n3\`O`,2j2x0001xF}}{OUT_R 29 0 27{DAG 
:3n3\`yn`+13j2x0002j2x0001*3n3\`x`p4/3p6p4*3p9j2x0004/3i2x0013j2x
0024*3p9j2x0006/3j2x0061j2x0720*3p9j2x0008/3i2x0253j3x00040320*3p
9j2x0010/3j2x1021j3x03628800*3p9j2x0012/3i2x4093j4x000479001600*3
p9j2x0014/3j3x00016381j4x087178291200*3p9j2x0016/3i2x5041j5x00016
09445376000}}{INP_R 30 0 "> "{TEXT 2 47 "plot( \{yn, 3*cos(x) - c
os(2*x)\}, x = -Pi..Pi );"}}{COM_R 31 0{TEXT 1 10 "Exercise 4"}
{NXRTF 0 318 "\{\\rtf0\\ansi\{\\fonttbl\\f0\\fnil Times-Roman;\\f
1\\fmodern Courier;\}\012\\margl40\012\\margr40\012\{\\colortbl;\
\red255\\green255\\blue255;\}\012\\cb1\\pard\\tx0\\tx1180\\tx2360
\\tx3540\\tx4720\\tx5900\\tx7080\\tx8260\\tx9440\\tx10620\\tx1180
0\\tx12980\\tx14160\\tx15340\\tx16520\\tx17700\\tx18880\\tx20060\
\tx21240\\tx22420\\f0\\b\\i0\\ulnone\\fs36\\fc0\\cf0 Exercise 4\0
12\}\012"}}{INP_R 32 0 "> "{TEXT 2 180 "f := x -> ln(x+1)*cos(x):
\012kMax := 8:\012Pn := convert( taylor( f(x), x = 0, kMax), poly
nom);\012for k from 0 to kMax-1 do\012   b[k] := coeff( Pn, x, k)
\012od:\012plot( \{f(x), Pn\}, x = 0..Pi/4 );"}}{OUT_R 33 0 32
{DAG :3n3\`Pn`+Dn3\`x`j2x0001*3p4j2x0002/3i2x0001pA*3p4j2x0003/3p
Dj2x0006*3p4j2x0005/3p12j2x0040*3p4p16/3pDj2x0016*3p4j2x0007/3j2x
0031j2x0560}}{INP_R 34 0 "> "{TEXT 2 35 "plot( \{f(x), Pn\}, x = 
Pi/4..Pi/2 );"}}{INP_R 35 0 "> "{TEXT 2 33 "plot( \{f(x) - Pn\}, 
x = 0..Pi/4 );"}}{INP_R 36 0 "> "{TEXT 2 37 "evalf( subs( x = Pi/
4, f(x) - Pn ) );"}}{OUT_R 37 0 36{DAG e3i4x000430778999i2x0011}}
{COM_R 38 0{TEXT 5 52 "This is the worst error on [0, Pi/4].\012O
n to the D.E."}{NXRTF 0 265 "\{\\rtf0\\ansi\{\\fonttbl\\f1\\fmode
rn Courier;\}\012\\margl40\012\\margr40\012\{\\colortbl;\\red255\
\green255\\blue255;\}\012\\cb1\\pard\\tx1152\\tx2304\\tx3456\\tx4
608\\tx5760\\tx6912\\tx8064\\tx9216\\tx10368\\tx11520\\f1\\b0\\i0
\\ulnone\\fs24\\fc0\\cf0 This is the worst error on [0, Pi/4].\\\
012On to the D.E.\012\}\012"}}{INP_R 39 0 "> "{TEXT 2 131 "a[0] :
= 0:  a[1] := 0:\012for k from 0 to kMax do\012   a[k+2] := (b[k]
 - a[k]) / ((k+1)*(k+2))\012od: \012yn := sum( a[n]*x^n, n = 0..k
Max+1);"}}{OUT_R 40 0 39{DAG :3n3\`yn`+F*3n3\`x`j2x0003/3j2x0001j
2x0006*3p5j2x0004/3i2x0001j2x0024*3p5j2x0005/3p13j2x0060*3p5pC/3p
Aj2x0720*3p5j2x0007/3j2x0011j2x5040*3p5j2x0008/3i2x0023j3x0002016
0*3p5j2x0009/3j2x0067j3x00090720}}{INP_R 41 0 "> "{TEXT 2 24 "plo
t( yn, x = 0..Pi/4 );"}}{COM_R 42 0{TEXT 4 72 "Now let's check th
e accuracy of yn using variation-of-constants formula:"}{NXRTF 0 
375 "\{\\rtf0\\ansi\{\\fonttbl\\f0\\fnil Times-Roman;\\f1\\fmoder
n Courier;\}\012\\margl40\012\\margr40\012\{\\colortbl;\\red255\\
green255\\blue255;\}\012\\cb1\\pard\\tx0\\tx720\\tx1440\\tx2160\\
tx2880\\tx3600\\tx4320\\tx5040\\tx5760\\tx6480\\tx7200\\tx7920\\t
x8640\\tx9360\\tx10080\\tx10800\\tx11520\\tx12240\\tx12960\\tx136
80\\f0\\b0\\i0\\ulnone\\fs24\\fc0\\cf0 Now let's check the accura
cy of yn using variation-of-constants formula:\012\}\012"}}{INP_R
 43 0 "> "{TEXT 2 49 "G := x -> evalf( Int( f(t)*sin(x-t), t = 0.
.x) );"}}{OUT_R 44 0 43{DAG :3n3\`G`@7,2n3\`x`,1,3n5\`operator`n4
\`arrow`p7(3n4\`evalf`,2(3n3\`Int`,3*5(3n3\`f`,2n3\`t`j2x0001(3n3
\`sin`,2+5a2x0001p21p1Fi2x0001p21=3p1F~3j2x0000p28p7}}{INP_R 45 0
 "> "{TEXT 2 33 "plot( \{G(x) - yn\}, x = 0..Pi/4 );"}}{INP_R 46 
0 "> "{TEXT 2 47 "error := evalf( subs( x = Pi/4, yn - G(x) ) ); 
"}}{OUT_R 47 0 46{DAG :3n4\`error`e3j3x03124428i2x0011}}{INP_R 48
 0 "> "{TEXT 2 26 "relerror := error/G(Pi/4);"}}{OUT_R 49 0 48
{DAG :3n5\`relerror`e3j4x005162102678i2x0013}}{COM_R 50 0{TEXT 1 
14 "Elegant Method"}{NXRTF 0 322 "\{\\rtf0\\ansi\{\\fonttbl\\f0\\
fnil Times-Roman;\\f1\\fmodern Courier;\}\012\\margl40\012\\margr
40\012\{\\colortbl;\\red255\\green255\\blue255;\}\012\\cb1\\pard\
\tx0\\tx1180\\tx2360\\tx3540\\tx4720\\tx5900\\tx7080\\tx8260\\tx9
440\\tx10620\\tx11800\\tx12980\\tx14160\\tx15340\\tx16520\\tx1770
0\\tx18880\\tx20060\\tx21240\\tx22420\\f0\\b\\i0\\ulnone\\fs36\\f
c0\\cf0 Elegant Method\012\}\012"}}{COM_R 51 0{TEXT 5 64 "We expa
nd the right side out; the uses Maples dsolve for the DE."}{NXRTF
 0 276 "\{\\rtf0\\ansi\{\\fonttbl\\f1\\fmodern Courier;\}\012\\ma
rgl40\012\\margr40\012\{\\colortbl;\\red255\\green255\\blue255;\}
\012\\cb1\\pard\\tx1152\\tx2304\\tx3456\\tx4608\\tx5760\\tx6912\\
tx8064\\tx9216\\tx10368\\tx11520\\f1\\b0\\i0\\ulnone\\fs24\\fc0\\
cf0 We expand the right side out; the uses Maples dsolve for the 
DE.\012\}\012"}}{INP_R 52 0 "> "{TEXT 2 86 "f := x -> 3*cos(2*x):
\012kMax := 13:\012Pn := convert( taylor( f(x), x=0, kMax ), poly
nom );"}}{OUT_R 53 0 52{DAG :3n3\`Pn`+Fj2x0003j2x0001*3n3\`x`j2x0
002i2x0006*3p9j2x0004pB*3p9j2x0006/3i2x0004j2x0015*3p9j2x0008/3pB
j2x0105*3p9j2x0010/3p19j2x4725*3p9j2x0012/3p11j3x00155925}}{INP_R
 54 0 "> "{TEXT 2 127 "y := 'y':\012diffequ := diff(y(x), x$2) + 
y(x) = Pn:\012Order := 13:\012yseries := dsolve( \{diffequ, y(0)=
2, D(y)(0)= 0\}, y(x), series);"}}{OUT_R 55 0 54{DAG :3n4\`yserie
s`=3(3n3\`y`,2n3\`x`_12p9j2x0002x0/3j2x0001pDx2/3i2x0013j2x0024x4
/3j2x0061j2x0720x6/3i2x0253j3x00040320x8/3j2x1021j3x03628800xA/3i
2x4093j4x000479001600xC(3n3\`O`,2p11xD}}{COM_R 56 0{TEXT 1 26 "Ex
ercise 5 (same as above)"}{NXRTF 0 334 "\{\\rtf0\\ansi\{\\fonttbl
\\f0\\fnil Times-Roman;\\f1\\fmodern Courier;\}\012\\margl40\012\
\margr40\012\{\\colortbl;\\red255\\green255\\blue255;\}\012\\cb1\
\pard\\tx0\\tx1180\\tx2360\\tx3540\\tx4720\\tx5900\\tx7080\\tx826
0\\tx9440\\tx10620\\tx11800\\tx12980\\tx14160\\tx15340\\tx16520\\
tx17700\\tx18880\\tx20060\\tx21240\\tx22420\\f0\\b\\i0\\ulnone\\f
s36\\fc0\\cf0 Exercise 5 (same as above)\012\}\012"}}{COM_R 57 0
{TEXT 1 44 "Numerical Solution of Differential Equations"}{NXRTF 
0 352 "\{\\rtf0\\ansi\{\\fonttbl\\f0\\fnil Times-Roman;\\f1\\fmod
ern Courier;\}\012\\margl40\012\\margr40\012\{\\colortbl;\\red255
\\green255\\blue255;\}\012\\cb1\\pard\\tx0\\tx1180\\tx2360\\tx354
0\\tx4720\\tx5900\\tx7080\\tx8260\\tx9440\\tx10620\\tx11800\\tx12
980\\tx14160\\tx15340\\tx16520\\tx17700\\tx18880\\tx20060\\tx2124
0\\tx22420\\f0\\b\\i0\\ulnone\\fs36\\fc0\\cf0 Numerical Solution 
of Differential Equations\012\}\012"}}{INP_R 58 0 "> "{TEXT 2 127
 "f := x -> 3*cos(2*x):\012diffequ := diff(y(x), x$2) + y(x) = f(
x):\012fnum := dsolve( \{diffequ, y(0)=2, D(y)(0)= 0\}, y(x), num
eric );"}}{OUT_R 59 0 58{DAG :3n4\`fnum`@7,2n3\`x`,1p8p8(3n8\`dso
lve/numeric/result2`,4a2x0001j3x02276852[2,2j2x0002p8}{TEXT 3 61 
"fnum := proc(x) `dsolve/numeric/result2`(x,2276852,[2]) end\012\
012"}}{INP_R 60 0 "> "{TEXT 2 9 "fnum(Pi);"}}{OUT_R 61 0 60{DAG %
2,3=3(3n3\`y`,2n3\`x`e3i4x004000000013i2x0009=3p7e3j4x00314159265
4pE}}{INP_R 62 0 "> "{TEXT 2 9 "fnum(Pi);"}}{OUT_R 63 0 62{DAG %2
,3=3(3n3\`y`,2n3\`x`e3i4x004000000013i2x0009=3p7e3j4x003141592654
pE}}{INP_R 64 0 "> "{TEXT 2 56 "yexact := x -> 3*cos(x) - cos(2*x
):\012evalf( yexact(Pi) );"}}{OUT_R 65 0 64{DAG e3i2x0004j2x0000}
}{INP_R 66 0 "> "{TEXT 2 11 "fnum(Pi/2);"}}{OUT_R 67 0 66{DAG %2,
3=3n3\`x`e3j4x001570796327i2x0009=3(3n3\`y`,2p3e3j4x001000000242p
A}}{INP_R 68 0 "> "{TEXT 2 22 "evalf( yexact(Pi/2) );"}}{OUT_R 69
 0 68{DAG e3j2x0001j2x0000}}{INP_R 70 0 "> "{TEXT 2 82 "So it loo
ks like our numerical solution, fnum, is good to 7 places.\012Let
's plot it:"}}{INP_R 71 0 "> "{TEXT 2 49 "with(plots):\012odeplot
( fnum, [x, y(x)], -Pi..Pi );"}}{COM_R 72 0{TEXT 1 10 "Exercise 6
"}{NXRTF 0 318 "\{\\rtf0\\ansi\{\\fonttbl\\f0\\fnil Times-Roman;\
\f1\\fmodern Courier;\}\012\\margl40\012\\margr40\012\{\\colortbl
;\\red255\\green255\\blue255;\}\012\\cb1\\pard\\tx0\\tx1180\\tx23
60\\tx3540\\tx4720\\tx5900\\tx7080\\tx8260\\tx9440\\tx10620\\tx11
800\\tx12980\\tx14160\\tx15340\\tx16520\\tx17700\\tx18880\\tx2006
0\\tx21240\\tx22420\\f0\\b\\i0\\ulnone\\fs36\\fc0\\cf0 Exercise 6
\012\}\012"}}{INP_R 73 0 "> "{TEXT 2 133 "f := x -> ln(1+x)*cos(2
*x):\012diffequ := diff(y(x), x$2) + y(x) = f(x):\012fnum := dsol
ve( \{diffequ, y(0)=0, D(y)(0)= 0\}, y(x), numeric );"}}{OUT_R 74
 0 73{DAG :3n4\`fnum`@7,2n3\`x`,1p8p8(3n8\`dsolve/numeric/result2
`,4a2x0001j3x00721448[2,2j2x0002p8}{TEXT 3 60 "fnum := proc(x) `d
solve/numeric/result2`(x,721448,[2]) end\012\012"}}{INP_R 75 0 ">
 "{TEXT 2 11 "fnum(Pi/4);"}}{OUT_R 76 0 75{DAG %2,3=3(3n3\`y`,2n3
\`x`e3j4x004475689163i2x0011=3p7e3j4x007853981634i2x0010}}{INP_R 
77 0 "> "{TEXT 2 12 "with(plots):"}}{INP_R 78 0 "> "{TEXT 2 40 "o
deplot( fnum, [x, y(x)], -Pi/4..Pi/4 );"}}{INP_R 79 0 "> "{TEXT 2
 0 ""}}}{END}
