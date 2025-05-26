{VERSION 1 0 "NEXTSTEP" "1.0"}{GLOBALS 1 0}{FONT 0 "Times-Bold" "
Times" "Times-Bold" 8 24 64 "Times-Bold" 24}{FONT 1 "Times-Bold" 
"Times" "Times-Bold" 8 18 64 "Times-Bold" 18}{FONT 2 "Times-Bold"
 "Times" "Times-Bold" 8 14 64 "Times-Bold" 14}{FONT 3 "Ohlfs" "Oh
lfs" "Courier" 4 11 192 "Ohlfs" 11}{FONT 4 "Courier-Bold" "Courie
r" "Courier-Bold" 8 10 192 "Courier-Bold" 10}{FONT 5 "Courier" "C
ourier" "Courier" 4 12 192 "Courier" 12}{FONT 6 "Times-Bold" "Tim
es" "Times-Bold" 8 12 64 "Times-Bold" 12}{SCP_R 1 0 70{COM_R 2 0
{TEXT 0 26 "Integration in Mathematica"}{NXRTF 0 337 "\{\\rtf0\\a
nsi\{\\fonttbl\\f0\\fnil Times-Roman;\\f1\\fmodern Ohlfs;\}\012\\
margl40\012\\margr40\012\{\\colortbl;\\red255\\green255\\blue255;
\}\012\\cb1\\pard\\tx0\\tx1580\\tx3160\\tx4740\\tx6320\\tx7900\\t
x9480\\tx11060\\tx12640\\tx14220\\tx15800\\tx17380\\tx18960\\tx20
540\\tx22120\\tx23700\\tx25280\\tx26860\\tx28440\\tx30020\\f0\\b\
\i0\\ulnone\\qc\\fs48\\fc0\\cf0 Integration in Mathematica\012\}\
012"}}{COM_R 3 0{TEXT 1 8 "Overview"}{NXRTF 0 314 "\{\\rtf0\\ansi
\{\\fonttbl\\f0\\fnil Times-Roman;\\f1\\fmodern Ohlfs;\}\012\\mar
gl40\012\\margr40\012\{\\colortbl;\\red255\\green255\\blue255;\}\
012\\cb1\\pard\\tx0\\tx1180\\tx2360\\tx3540\\tx4720\\tx5900\\tx70
80\\tx8260\\tx9440\\tx10620\\tx11800\\tx12980\\tx14160\\tx15340\\
tx16520\\tx17700\\tx18880\\tx20060\\tx21240\\tx22420\\f0\\b\\i0\\
ulnone\\fs36\\fc0\\cf0 Overview\012\}\012"}}{COM_R 4 0{TEXT 2 23 
"The Indefinite Integral"}{NXRTF 0 325 "\{\\rtf0\\ansi\{\\fonttbl
\\f0\\fnil Times-Roman;\\f1\\fmodern Ohlfs;\}\012\\margl40\012\\m
argr40\012\{\\colortbl;\\red255\\green255\\blue255;\}\012\\cb1\\p
ard\\tx0\\tx900\\tx1800\\tx2700\\tx3600\\tx4500\\tx5400\\tx6300\\
tx7200\\tx8100\\tx9000\\tx9900\\tx10800\\tx11700\\tx12600\\tx1350
0\\tx14400\\tx15300\\tx16200\\tx17100\\f0\\b\\i0\\ulnone\\fs28\\f
c0\\cf0 The Indefinite Integral\012\}\012"}}{INP_R 5 0 "> "{TEXT 
3 12 "int(x^3, x);"}}{OUT_R 6 0 5{DAG +3*3n3\`x`j2x0004/3j2x0001p
4}}{INP_R 7 0 "> "{TEXT 3 31 "f := x -> sin(x):\012int(f(x), x);"
}}{OUT_R 8 0 7{DAG +3(3n3\`cos`,2n3\`x`i2x0001}}{COM_R 9 0{TEXT 2
 46 "The Definite Integral -- Antiderivative Exists"}{NXRTF 0 348
 "\{\\rtf0\\ansi\{\\fonttbl\\f0\\fnil Times-Roman;\\f1\\fmodern O
hlfs;\}\012\\margl40\012\\margr40\012\{\\colortbl;\\red255\\green
255\\blue255;\}\012\\cb1\\pard\\tx0\\tx900\\tx1800\\tx2700\\tx360
0\\tx4500\\tx5400\\tx6300\\tx7200\\tx8100\\tx9000\\tx9900\\tx1080
0\\tx11700\\tx12600\\tx13500\\tx14400\\tx15300\\tx16200\\tx17100\
\f0\\b\\i0\\ulnone\\fs28\\fc0\\cf0 The Definite Integral -- Antid
erivative Exists\012\}\012"}}{INP_R 10 0 "> "{TEXT 3 21 "int(x^5,
 x = 10..20);"}}{OUT_R 11 0 10{DAG j3x10500000}}{INP_R 12 0 "> "
{TEXT 3 19 "int(x^5, x = a..b);"}}{OUT_R 13 0 12{DAG +5*3n3\`b`j2
x0006/3j2x0001p4*3n3\`a`p4/3i2x0001p4}}{INP_R 14 0 "> "{TEXT 3 28
 "int( arcsin(x), x = 0..1/2);"}}{OUT_R 15 0 14{DAG +7n3\`Pi`/3j2
x0001j2x0012*3j2x0003/3p4j2x0002pBi2x0001p4}}{COM_R 16 0{TEXT 5 
58 "To get a numerical evaluation of an integral one can e.g.:"}
{NXRTF 0 288 "\{\\rtf0\\ansi\{\\fonttbl\\f2\\fmodern Courier;\\f1
\\fmodern Ohlfs;\}\012\\margl40\012\\margr40\012\{\\colortbl;\\re
d255\\green255\\blue255;\}\012\\cb1\\pard\\tx1140\\tx2300\\tx3440
\\tx4600\\tx5760\\tx6900\\tx8060\\tx9200\\tx10360\\tx11520\\f2\\b
0\\i0\\ulnone\\fs24\\fc0\\cf0 To get a numerical evaluation of an
 integral one can e.g.:\012\}\012"}}{INP_R 17 0 "> "{TEXT 3 38 "e
valf( int( arcsin(x), x = 0..1/2) ); "}}{OUT_R 18 0 17{DAG e3j4x0
00127824792i2x0009}}{COM_R 19 0{TEXT 5 120 "However, this means  
int  must first do a symbolic integration.\012Sometimes this is i
mpossible; so using  Int  is the way:"}{NXRTF 0 351 "\{\\rtf0\\an
si\{\\fonttbl\\f2\\fmodern Courier;\\f1\\fmodern Ohlfs;\}\012\\ma
rgl40\012\\margr40\012\{\\colortbl;\\red255\\green255\\blue255;\}
\012\\cb1\\pard\\tx1140\\tx2300\\tx3440\\tx4600\\tx5760\\tx6900\\
tx8060\\tx9200\\tx10360\\tx11520\\f2\\b0\\i0\\ulnone\\fs24\\fc0\\
cf0 However, this means  int  must first do a symbolic integratio
n.\\\012Sometimes this is impossible; so using  Int  is the way:\
012\}\012"}}{INP_R 20 0 "> "{TEXT 3 38 "evalf( Int( arcsin(x), x 
= 0..1/2) ); "}}{OUT_R 21 0 20{DAG e3j4x001278247916i2x0010}}
{INP_R 22 0 "> "{TEXT 3 49 "MyArcSin := x -> int( 1/sqrt(1 - t^2)
, t = 0..x);"}}{OUT_R 23 0 22{DAG :3n5\`MyArcSin`@7,2n3\`x`,1,3n5
\`operator`n4\`arrow`p9(3n3\`int`,3*3(3n4\`sqrt`,2+5j2x0001p1E*3n
3\`t`j2x0002i2x0001p26=3p22~3j2x0000a2x0001p9}}{INP_R 24 0 "> "
{TEXT 3 26 "plot(MyArcSin, -0.9..0.9);"}}{COM_R 25 0{TEXT 5 103 "
This was slow because for each plot-point the integration was don
e.\012Here is one way to speed things up:"}{NXRTF 0 334 "\{\\rtf0
\\ansi\{\\fonttbl\\f2\\fmodern Courier;\\f1\\fmodern Ohlfs;\}\012
\\margl40\012\\margr40\012\{\\colortbl;\\red255\\green255\\blue25
5;\}\012\\cb1\\pard\\tx1140\\tx2300\\tx3440\\tx4600\\tx5760\\tx69
00\\tx8060\\tx9200\\tx10360\\tx11520\\f2\\b0\\i0\\ulnone\\fs24\\f
c0\\cf0 This was slow because for each plot-point the integration
 was done.\\\012Here is one way to speed things up:\012\}\012"}}
{INP_R 26 0 "> "{TEXT 3 38 "y := int( 1/sqrt(1 - t^2), t = 0..x);
 "}}{OUT_R 27 0 26{DAG :3n3\`y`(3n4\`arcsin`,2n3\`x`}}{INP_R 28 0
 "> "{TEXT 3 23 "plot(y, x = -0.9..0.9);"}}{COM_R 29 0{TEXT 5 177
 "In this case the integration was done once, before plotting.  S
o defining a formal function (like MyArcSin) `delays' the evaluat
ion of the integral, in this case with a penalty."}{NXRTF 0 407 "
\{\\rtf0\\ansi\{\\fonttbl\\f2\\fmodern Courier;\\f1\\fmodern Ohlf
s;\}\012\\margl40\012\\margr40\012\{\\colortbl;\\red255\\green255
\\blue255;\}\012\\cb1\\pard\\tx1140\\tx2300\\tx3440\\tx4600\\tx57
60\\tx6900\\tx8060\\tx9200\\tx10360\\tx11520\\f2\\b0\\i0\\ulnone\
\fs24\\fc0\\cf0 In this case the integration was done once, befor
e plotting.  So defining a formal function (like MyArcSin) `delay
s' the evaluation of the integral, in this case with a penalty.\0
12\}\012"}}{COM_R 30 0{TEXT 2 42 "The Definite Integral -- No Ant
iderivative"}{NXRTF 0 344 "\{\\rtf0\\ansi\{\\fonttbl\\f0\\fnil Ti
mes-Roman;\\f1\\fmodern Ohlfs;\}\012\\margl40\012\\margr40\012\{\
\colortbl;\\red255\\green255\\blue255;\}\012\\cb1\\pard\\tx0\\tx9
00\\tx1800\\tx2700\\tx3600\\tx4500\\tx5400\\tx6300\\tx7200\\tx810
0\\tx9000\\tx9900\\tx10800\\tx11700\\tx12600\\tx13500\\tx14400\\t
x15300\\tx16200\\tx17100\\f0\\b\\i0\\ulnone\\fs28\\fc0\\cf0 The D
efinite Integral -- No Antiderivative\012\}\012"}}{COM_R 31 0
{TEXT 6 11 "SuperArcSin"}{NXRTF 0 312 "\{\\rtf0\\ansi\{\\fonttbl\
\f0\\fnil Times-Roman;\\f1\\fmodern Ohlfs;\}\012\\margl40\012\\ma
rgr40\012\{\\colortbl;\\red255\\green255\\blue255;\}\012\\cb1\\pa
rd\\tx0\\tx780\\tx1560\\tx2340\\tx3120\\tx3900\\tx4680\\tx5460\\t
x6240\\tx7020\\tx7800\\tx8580\\tx9360\\tx10140\\tx10920\\tx11700\
\tx12480\\tx13260\\tx14040\\tx14820\\f0\\b\\i0\\ulnone\\fs24\\fc0
\\cf0 SuperArcSin\012\}\012"}}{INP_R 32 0 "> "{TEXT 3 52 "SuperAr
cSin := x -> int( 1/sqrt(1 - t^4), t = 0..x);"}}{OUT_R 33 0 32
{DAG :3n5\`SuperArcSin`@7,2n3\`x`,1,3n5\`operator`n4\`arrow`p9(3n
3\`int`,3*3(3n4\`sqrt`,2+5j2x0001p1E*3n3\`t`j2x0004i2x0001p26=3p2
2~3j2x0000a2x0001p9}}{INP_R 34 0 "> "{TEXT 3 41 "plot( SuperArcSi
n, -0.9..0.9, y = -1..1);"}}{COM_R 35 0{TEXT 6 8 "Supersin"}
{NXRTF 0 309 "\{\\rtf0\\ansi\{\\fonttbl\\f0\\fnil Times-Roman;\\f
1\\fmodern Ohlfs;\}\012\\margl40\012\\margr40\012\{\\colortbl;\\r
ed255\\green255\\blue255;\}\012\\cb1\\pard\\tx0\\tx780\\tx1560\\t
x2340\\tx3120\\tx3900\\tx4680\\tx5460\\tx6240\\tx7020\\tx7800\\tx
8580\\tx9360\\tx10140\\tx10920\\tx11700\\tx12480\\tx13260\\tx1404
0\\tx14820\\f0\\b\\i0\\ulnone\\fs24\\fc0\\cf0 Supersin\012\}\012"
}}{INP_R 36 0 "> "{TEXT 3 45 "SuperSin := x -> int( sin(sin(t)), 
t = 0..x);"}}{OUT_R 37 0 36{DAG :3n5\`SuperSin`@7,2n3\`x`,1,3n5\`
operator`n4\`arrow`p9(3n3\`int`,3(3n3\`sin`,2(3p18,2n3\`t`=3p1E~3
j2x0000a2x0001p9}}{INP_R 38 0 "> "{TEXT 3 29 "plot( SuperSin, -3*
Pi..3*Pi);"}}{INP_R 39 0 "> "{TEXT 3 15 "D(SuperSin)(x);"}}{OUT_R
 40 0 39{DAG (3n3\`sin`,2(3p1,2n3\`x`}}{COM_R 41 0{TEXT 5 33 "Exe
rcise 5 using Newton's method:"}{NXRTF 0 263 "\{\\rtf0\\ansi\{\\f
onttbl\\f2\\fmodern Courier;\\f1\\fmodern Ohlfs;\}\012\\margl40\0
12\\margr40\012\{\\colortbl;\\red255\\green255\\blue255;\}\012\\c
b1\\pard\\tx1140\\tx2300\\tx3440\\tx4600\\tx5760\\tx6900\\tx8060\
\tx9200\\tx10360\\tx11520\\f2\\b0\\i0\\ulnone\\fs24\\fc0\\cf0 Exe
rcise 5 using Newton's method:\012\}\012"}}{INP_R 42 0 "> "{TEXT 
3 28 "F := x -> SuperSin(x) - 1.0;"}}{OUT_R 43 0 42{DAG :3n3\`F`@
7,2n3\`x`,1,3n5\`operator`n4\`arrow`p7+5(3n5\`SuperSin`,2a2x0001j
2x0001e3i2x0010i2x0001p1Ap7}}{COM_R 44 0{TEXT 5 100 "We use the f
act that F'(x) = sin(sin(x)) in Newton's method.\012Also the eval
f is often necessary here."}{NXRTF 0 331 "\{\\rtf0\\ansi\{\\fontt
bl\\f2\\fmodern Courier;\\f1\\fmodern Ohlfs;\}\012\\margl40\012\\
margr40\012\{\\colortbl;\\red255\\green255\\blue255;\}\012\\cb1\\
pard\\tx1140\\tx2300\\tx3440\\tx4600\\tx5760\\tx6900\\tx8060\\tx9
200\\tx10360\\tx11520\\f2\\b0\\i0\\ulnone\\fs24\\fc0\\cf0 We use 
the fact that F'(x) = sin(sin(x)) in Newton's method.\\\012Also t
he evalf is often necessary here.\012\}\012"}}{INP_R 45 0 "> "
{TEXT 3 68 "xn := 2:\012for k to 5 do\012   xn := evalf( xn - F(x
n)/sin(sin(xn)) )\012od;"}}{OUT_R 46 0 45{DAG :3n3\`xn`e3j4x00168
6903159i2x0009}}{OUT_R 47 0 45{DAG :3n3\`xn`e3j4x001697880563i2x0
009}}{OUT_R 48 0 45{DAG :3n3\`xn`e3j4x001697885261i2x0009}}{OUT_R
 49 0 45{DAG :3n3\`xn`e3j4x001697885261i2x0009}}{OUT_R 50 0 45
{DAG :3n3\`xn`e3j4x001697885261i2x0009}}{COM_R 51 0{TEXT 1 14 "So
me Integrals"}{NXRTF 0 320 "\{\\rtf0\\ansi\{\\fonttbl\\f0\\fnil T
imes-Roman;\\f1\\fmodern Ohlfs;\}\012\\margl40\012\\margr40\012\{
\\colortbl;\\red255\\green255\\blue255;\}\012\\cb1\\pard\\tx0\\tx
1180\\tx2360\\tx3540\\tx4720\\tx5900\\tx7080\\tx8260\\tx9440\\tx1
0620\\tx11800\\tx12980\\tx14160\\tx15340\\tx16520\\tx17700\\tx188
80\\tx20060\\tx21240\\tx22420\\f0\\b\\i0\\ulnone\\fs36\\fc0\\cf0 
Some Integrals\012\}\012"}}{INP_R 52 0 "> "{TEXT 3 39 "y := x^11 
* (x^4 + 1)^(1/3):\012int(y, x);"}}{OUT_R 53 0 52{DAG +3*5+9j2x00
09j2x0001*3n3\`x`j2x0004i2x0003*3p8j2x0008j2x0002*3p8j2x0012j2x00
14p5+5p7p5p5p5/3p5j2x0003/3p22j2x0560}}{INP_R 54 0 "> "{TEXT 3 46
 "y := 1/sqrt( exp(4*x) - 1):\012yint := int(y, x);"}}{OUT_R 55 0
 54{DAG :3n4\`yint`+3(3n4\`arctan`,2*3+5(3n3\`exp`,2+3n3\`x`j2x00
04j2x0001i2x0001p15/3p15j2x0002p1A}}{INP_R 56 0 "> "{TEXT 3 45 "p
lot(\{2*yint, arcsec( exp(2*x) )\}, x = 0..1);"}}{INP_R 57 0 "> "
{TEXT 3 37 "y := x^5*exp(a*x):\012yint := int(y, x);"}}{OUT_R 58 
0 57{DAG :3n4\`yint`*5+D*7n3\`a`j2x0005n3\`x`p9(3n3\`exp`,2*5p7j2
x0001pBp14p14p14*7p7j2x0004pBp1CpEp14i2x0005*7p7j2x0003pBp26pEp14
j2x0020*7p7j2x0002pBp30pEp14i2x0060*7p7p14pBp14pEp14j2x0120pEi2x0
120p14p7i2x0006}}{INP_R 59 0 "> "{TEXT 3 37 "y := x^3*cos(a*x):\0
12yint := int(y, x);"}}{OUT_R 60 0 59{DAG :3n4\`yint`*5+9*7n3\`a`
j2x0003n3\`x`p9(3n3\`sin`,2*5p7j2x0001pBp14p14p14*7p7j2x0002pBp1C
(3n3\`cos`p11p14p9p20i2x0006*7p7p14pBp14pEp14p27p14p7i2x0004}}
{INP_R 61 0 "> "{TEXT 3 37 "y := x^3*sin(a*x):\012yint := int(y, 
x);"}}{OUT_R 62 0 61{DAG :3n4\`yint`*5+9*7n3\`a`j2x0003n3\`x`p9(3
n3\`cos`,2*5p7j2x0001pBp14p14i2x0001*7p7j2x0002pBp1D(3n3\`sin`p11
p14p9p21i2x0006*7p7p14pBp14pEp14j2x0006p14p7i2x0004}}{INP_R 63 0 
"> "{TEXT 3 22 "y := x^3 * exp(I*a*x);"}}{OUT_R 64 0 63{DAG :3n3\
`y`*5n3\`x`j2x0003(3n3\`exp`,2*7n3\`I`j2x0001n3\`a`pFp4pFpF}}
{INP_R 65 0 "> "{TEXT 3 23 "complexInt := int(y,x);"}}{OUT_R 66 0
 65{DAG :3n5\`complexInt`*5+9*9n3\`I`j2x0001n3\`a`j2x0003n3\`x`pE
(3n3\`exp`,2*7p8pApCpAp10pApAi2x0001*7pCj2x0002p10p23p13pApE*9p8p
ApCpAp10pAp13pAj2x0006p13i2x0006pApCi2x0004}}{INP_R 67 0 "> "
{TEXT 3 34 "cosInt := evalc( Re(complexInt) );"}}{OUT_R 68 0 67
{DAG :3n4\`cosInt`*5+9*7n3\`a`j2x0003n3\`x`p9(3n3\`sin`,2*5p7j2x0
001pBp14p14p14*7p7j2x0002pBp1C(3n3\`cos`p11p14p9*7p7p14pBp14pEp14
i2x0006p20p2Dp14p7i2x0004}}{INP_R 69 0 "> "{TEXT 3 36 "sinInt := 
evalc( Im((complexInt)) );"}}{OUT_R 70 0 69{DAG :3n4\`sinInt`*5+9
*7n3\`a`j2x0003n3\`x`p9(3n3\`cos`,2*5p7j2x0001pBp14p14i2x0001*7p7
j2x0002pBp1D(3n3\`sin`p11p14p9*7p7p14pBp14pEp14j2x0006p21i2x0006p
14p7i2x0004}}{INP_R 71 0 "> "{TEXT 3 0 ""}}}{END}
