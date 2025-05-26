{VERSION 1 0 "NEXTSTEP" "1.0"}{GLOBALS 1 0}{FONT 0 "Times-Bold" "
Times" "Times-Bold" 8 24 64 "Times-Bold" 24}{FONT 1 "Times-Bold" 
"Times" "Times-Bold" 8 18 64 "Times-Bold" 18}{FONT 2 "Ohlfs" "Ohl
fs" "Courier" 4 11 192 "Ohlfs" 11}{FONT 3 "Courier-Bold" "Courier
" "Courier-Bold" 8 12 192 "Courier-Bold" 12}{FONT 4 "Courier" "Co
urier" "Courier" 4 12 192 "Courier" 12}{SCP_R 1 0 48{COM_R 2 0
{TEXT 0 14 "Center of Mass"}{NXRTF 0 327 "\{\\rtf0\\ansi\{\\fontt
bl\\f1\\fnil Times-Roman;\\f0\\fmodern Courier;\}\012\\margl40\01
2\\margr40\012\{\\colortbl;\\red255\\green255\\blue255;\}\012\\cb
1\\pard\\tx0\\tx1580\\tx3160\\tx4740\\tx6320\\tx7900\\tx9480\\tx1
1060\\tx12640\\tx14220\\tx15800\\tx17380\\tx18960\\tx20540\\tx221
20\\tx23700\\tx25280\\tx26860\\tx28440\\tx30020\\f1\\b\\i0\\ulnon
e\\qc\\fs48\\fc0\\cf0 Center of Mass\012\}\012"}}{COM_R 3 0{TEXT 
1 12 "Introduction"}{NXRTF 0 320 "\{\\rtf0\\ansi\{\\fonttbl\\f1\\
fnil Times-Roman;\\f0\\fmodern Courier;\}\012\\margl40\012\\margr
40\012\{\\colortbl;\\red255\\green255\\blue255;\}\012\\cb1\\pard\
\tx0\\tx1180\\tx2360\\tx3540\\tx4720\\tx5900\\tx7080\\tx8260\\tx9
440\\tx10620\\tx11800\\tx12980\\tx14160\\tx15340\\tx16520\\tx1770
0\\tx18880\\tx20060\\tx21240\\tx22420\\f1\\b\\i0\\ulnone\\fs36\\f
c0\\cf0 Introduction\012\}\012"}}{INP_R 4 0 "> "{TEXT 2 62 "a := 
1:\012plot3d( a - x - y, x = 0..a, y = 0..a, axes = boxed);\012"}
}{INP_R 5 0 "> "{TEXT 2 77 "a := 'a':\012volume := int( int( int(
 1, z = 0..a-x-y ), y = 0..a-x), x = 0..a);"}}{OUT_R 6 0 5{DAG :3
n4\`volume`+3*3n3\`a`j2x0003/3j2x0001j2x0006}}{INP_R 7 0 "> "
{TEXT 2 80 "cmassX := int( int( int(x, z = 0..a-x-y ), y = 0..a-x
 ), x = 0..a )\012   / volume;"}}{OUT_R 8 0 7{DAG :3n4\`cmassX`+3
n3\`a`/3j2x0001j2x0004}}{COM_R 9 0{TEXT 4 24 "And similar for y a
nd z."}{NXRTF 0 236 "\{\\rtf0\\ansi\{\\fonttbl\\f0\\fmodern Couri
er;\}\012\\margl40\012\\margr40\012\{\\colortbl;\\red255\\green25
5\\blue255;\}\012\\cb1\\pard\\tx1152\\tx2304\\tx3456\\tx4608\\tx5
760\\tx6912\\tx8064\\tx9216\\tx10368\\tx11520\\f0\\b0\\i0\\ulnone
\\fs24\\fc0\\cf0 And similar for y and z.\012\}\012"}}{COM_R 10 0
{TEXT 1 10 "Exercise 3"}{NXRTF 0 318 "\{\\rtf0\\ansi\{\\fonttbl\\
f1\\fnil Times-Roman;\\f0\\fmodern Courier;\}\012\\margl40\012\\m
argr40\012\{\\colortbl;\\red255\\green255\\blue255;\}\012\\cb1\\p
ard\\tx0\\tx1180\\tx2360\\tx3540\\tx4720\\tx5900\\tx7080\\tx8260\
\tx9440\\tx10620\\tx11800\\tx12980\\tx14160\\tx15340\\tx16520\\tx
17700\\tx18880\\tx20060\\tx21240\\tx22420\\f1\\b\\i0\\ulnone\\fs3
6\\fc0\\cf0 Exercise 3\012\}\012"}}{INP_R 11 0 "> "{TEXT 2 69 "vo
lume := int( int( int( 1, z = a-x-y..a ), y = 0..a-x ), x = 0..a 
);"}}{OUT_R 12 0 11{DAG :3n4\`volume`+3*3n3\`a`j2x0003/3j2x0001p8
}}{INP_R 13 0 "> "{TEXT 2 80 "cmassX := int( int( int(x, z = a-x-
y..a ), y = 0..a-x ), x = 0..a )\012   / volume;"}}{OUT_R 14 0 13
{DAG :3n4\`cmassX`+3n3\`a`/3j2x0003j2x0008}}{INP_R 15 0 "> "{TEXT
 2 80 "cmassY := int( int( int(y, z = a-x-y..a ), y = 0..a-x ), x
 = 0..a )\012   / volume;"}}{OUT_R 16 0 15{DAG :3n4\`cmassY`+3n3\
`a`/3j2x0003j2x0008}}{INP_R 17 0 "> "{TEXT 2 80 "cmassZ := int( i
nt( int(z, z = a-x-y..a ), y = 0..a-x ), x = 0..a )\012   / volum
e;"}}{OUT_R 18 0 17{DAG :3n4\`cmassZ`+3n3\`a`/3j2x0005j2x0008}}
{COM_R 19 0{TEXT 1 16 "Exercise 4  (2D)"}{NXRTF 0 324 "\{\\rtf0\\
ansi\{\\fonttbl\\f1\\fnil Times-Roman;\\f0\\fmodern Courier;\}\01
2\\margl40\012\\margr40\012\{\\colortbl;\\red255\\green255\\blue2
55;\}\012\\cb1\\pard\\tx0\\tx1180\\tx2360\\tx3540\\tx4720\\tx5900
\\tx7080\\tx8260\\tx9440\\tx10620\\tx11800\\tx12980\\tx14160\\tx1
5340\\tx16520\\tx17700\\tx18880\\tx20060\\tx21240\\tx22420\\f1\\b
\\i0\\ulnone\\fs36\\fc0\\cf0 Exercise 4  (2D)\012\}\012"}}{INP_R 
20 0 "> "{TEXT 2 63 "area :=  int( int( 1, y = 0..b*sqrt(1 - (x/a
)^2) ), x = 0..a );"}}{OUT_R 21 0 20{DAG :3n4\`area`+3*7n3\`a`j2x
0001n3\`Pi`p8n3\`b`p8/3p8j2x0004}}{INP_R 22 0 "> "{TEXT 2 72 "cma
ssX :=  int( int( x, y = 0..b*sqrt(1 - (x/a)^2) ), x = 0..a ) / a
rea;"}}{OUT_R 23 0 22{DAG :3n4\`cmassX`+3*5n3\`a`j2x0001n3\`Pi`i2
x0001/3j2x0004j2x0003}}{INP_R 24 0 "> "{TEXT 2 72 "cmassY :=  int
( int( y, y = 0..b*sqrt(1 - (x/a)^2) ), x = 0..a ) / area;"}}
{OUT_R 25 0 24{DAG :3n4\`cmassY`+3*5n3\`b`j2x0001n3\`Pi`i2x0001/3
j2x0004j2x0003}}{COM_R 26 0{TEXT 1 16 "Exercise 5  (3D)"}{NXRTF 0
 324 "\{\\rtf0\\ansi\{\\fonttbl\\f1\\fnil Times-Roman;\\f0\\fmode
rn Courier;\}\012\\margl40\012\\margr40\012\{\\colortbl;\\red255\
\green255\\blue255;\}\012\\cb1\\pard\\tx0\\tx1180\\tx2360\\tx3540
\\tx4720\\tx5900\\tx7080\\tx8260\\tx9440\\tx10620\\tx11800\\tx129
80\\tx14160\\tx15340\\tx16520\\tx17700\\tx18880\\tx20060\\tx21240
\\tx22420\\f1\\b\\i0\\ulnone\\fs36\\fc0\\cf0 Exercise 5  (3D)\012
\}\012"}}{INP_R 27 0 "> "{TEXT 2 122 "volume := int( int( int( 1,
 z = 0..c*sqrt(1 - (x/a)^2 -(y/b)^2) ),\012              y = 0..b
*sqrt(1 - (x/a)^2) ), x = 0..a );"}}{OUT_R 28 0 27{DAG :3n4\`volu
me`(3n3\`int`,3+3*Bn3\`b`j2x0001(3n4\`arcsin`,2*7pApC+5*3n3\`a`j2
x0002pC*3n3\`x`p1Ai2x0001/3pCp1A+5*5p18p1ApAp1ApC*5p1Ep1ApAp1Ap21
/3p21p1ApCp16pCn3\`c`pCp18i2x0002p23=3p1E~3j2x0000p18}}{INP_R 29 
0 "> "{TEXT 2 27 "volume := simplify(volume);"}}{OUT_R 30 0 29
{DAG :3n4\`volume`+3*9n3\`b`j2x0001n3\`Pi`p8n3\`c`p8n3\`a`p8/3p8j
2x0006}}{INP_R 31 0 "> "{TEXT 2 131 "cmassX := int( int( int( x, 
z = 0..c*sqrt(1 - (x/a)^2 -(y/b)^2) ),\012              y = 0..b*
sqrt(1 - (x/a)^2) ), x = 0..a ) / volume;"}}{OUT_R 32 0 31{DAG :3
n4\`cmassX`+3*B(3n3\`int`,3+3*Dn3\`b`j2x0001(3n4\`arcsin`,2*7pCpE
+5*3n3\`a`j2x0002pE*3n3\`x`p1Ci2x0001/3pEp1C+5*5p1Ap1CpCp1CpE*5p2
0p1CpCp1Cp23/3p23p1CpEp18pEp20pEn3\`c`pEp1Ai2x0002p25=3p20~3j2x00
00p1ApEpCp23n3\`Pi`p23p3Dp23p1Ap23j2x0006}}{INP_R 33 0 "> "{TEXT 
2 17 "simplify(cmassX);"}}{OUT_R 34 0 33{DAG +3n3\`a`/3j2x0003j2x
0008}}{INP_R 35 0 "> "{TEXT 2 131 "cmassY := int( int( int( y, z 
= 0..c*sqrt(1 - (x/a)^2 -(y/b)^2) ),\012              y = 0..b*sq
rt(1 - (x/a)^2) ), x = 0..a ) / volume;"}}{OUT_R 36 0 35{DAG :3n4
\`cmassY`+3n3\`b`/3j2x0003j2x0008}}{INP_R 37 0 "> "{TEXT 2 131 "c
massZ := int( int( int( z, z = 0..c*sqrt(1 - (x/a)^2 -(y/b)^2) ),
\012              y = 0..b*sqrt(1 - (x/a)^2) ), x = 0..a ) / volu
me;"}}{OUT_R 38 0 37{DAG :3n4\`cmassZ`+3n3\`c`/3j2x0003j2x0008}}
{COM_R 39 0{TEXT 1 16 "Exercise 6  (4D)"}{NXRTF 0 324 "\{\\rtf0\\
ansi\{\\fonttbl\\f1\\fnil Times-Roman;\\f0\\fmodern Courier;\}\01
2\\margl40\012\\margr40\012\{\\colortbl;\\red255\\green255\\blue2
55;\}\012\\cb1\\pard\\tx0\\tx1180\\tx2360\\tx3540\\tx4720\\tx5900
\\tx7080\\tx8260\\tx9440\\tx10620\\tx11800\\tx12980\\tx14160\\tx1
5340\\tx16520\\tx17700\\tx18880\\tx20060\\tx21240\\tx22420\\f1\\b
\\i0\\ulnone\\fs36\\fc0\\cf0 Exercise 6  (4D)\012\}\012"}}{INP_R 
40 0 "> "{TEXT 2 205 "volume := int( int( int( int(1, \012       
       w = 0..d*sqrt(1 - (x/a)^2 -(y/b)^2 - (z/c)^2 )), \012     
         z = 0..c*sqrt(1 - (x/a)^2 -(y/b)^2) ),\012              
y = 0..b*sqrt(1 - (x/a)^2) ), x = 0..a );"}}{OUT_R 41 0 40{DAG :3
n4\`volume`(3n3\`int`,3(3p5,3+3*Dn3\`c`j2x0001(3n4\`arcsin`,2*7pD
pF+7*5n3\`a`j2x0002n3\`b`p1DpF*5n3\`x`p1Dp1Fp1Di2x0001*5n3\`y`p1D
p1Bp1Dp29/3pFp1D+7*7p1Bp1Dp1Fp1DpDp1DpF*7p24p1Dp1Fp1DpDp1Dp29*7p2
Cp1Dp1Bp1DpDp1Dp29/3p29p1DpF+7p1Ap29p23pFp2BpFpFn3\`d`pFp1Bi2x000
2p1Fp5Ep4E=3p2C~3j2x0000*5p1FpF+5pFpF*5p24p1Dp1Bp5Ep29p32=3p24~3p
66p1B}}{INP_R 42 0 "> "{TEXT 2 22 "volume := simplify(\");"}}
{OUT_R 43 0 42{DAG :3n4\`volume`+3*Bn3\`a`j2x0001n3\`Pi`j2x0002n3
\`c`p8n3\`d`p8n3\`b`p8/3p8j2x0032}}{INP_R 44 0 "> "{TEXT 2 214 "c
massX := int( int( int( int(x, \012              w = 0..d*sqrt(1 
- (x/a)^2 -(y/b)^2 - (z/c)^2 )), \012              z = 0..c*sqrt(
1 - (x/a)^2 -(y/b)^2) ),\012              y = 0..b*sqrt(1 - (x/a)
^2) ), x = 0..a ) / volume;"}}{OUT_R 45 0 44{DAG :3n4\`cmassX`+3*
D(3n3\`int`,3(3p7,3+3*Fn3\`c`j2x0001(3n4\`arcsin`,2*7pFp11+7*5n3\
`a`j2x0002n3\`b`p1Fp11*5n3\`x`p1Fp21p1Fi2x0001*5n3\`y`p1Fp1Dp1Fp2
B/3p11p1F+7*7p1Dp1Fp21p1FpFp1Fp11*7p26p1Fp21p1FpFp1Fp2B*7p2Ep1Fp1
Dp1FpFp1Fp2B/3p2Bp1Fp11+7p1Cp2Bp25p11p2Dp11p11p26p11n3\`d`p11p1Di
2x0002p21p62p50=3p2E~3j2x0000*5p21p11+5p11p11*5p26p1Fp1Dp62p2Bp34
=3p26~3p6Ap1Dp11p1Dp2Bn3\`Pi`p62pFp2Bp5Ep2Bp21p2Bj2x0032}}{INP_R 
46 0 "> "{TEXT 2 22 "cmassX := simplify(\");"}}{OUT_R 47 0 46{DAG
 :3n4\`cmassX`+3*5n3\`a`j2x0001n3\`Pi`i2x0001/3j2x0016j2x0015}}
{COM_R 48 0{TEXT 4 46 "For cmassY a -> b, etc. for cmassZ and cma
ssW."}{NXRTF 0 258 "\{\\rtf0\\ansi\{\\fonttbl\\f0\\fmodern Courie
r;\}\012\\margl40\012\\margr40\012\{\\colortbl;\\red255\\green255
\\blue255;\}\012\\cb1\\pard\\tx1152\\tx2304\\tx3456\\tx4608\\tx57
60\\tx6912\\tx8064\\tx9216\\tx10368\\tx11520\\f0\\b0\\i0\\ulnone\
\fs24\\fc0\\cf0 For cmassY a -> b, etc. for cmassZ and cmassW.\01
2\}\012"}}{INP_R 49 0 "> "{TEXT 2 0 ""}}}{END}
