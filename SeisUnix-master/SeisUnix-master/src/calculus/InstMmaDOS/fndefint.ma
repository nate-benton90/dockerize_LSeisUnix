(*^

::[paletteColors = 128; automaticGrouping; currentKernel; 
	fontset = title, inactive, noPageBreakBelow, nohscroll, preserveAspect, groupLikeTitle, center, M7, bold, L1, e8,  24, "Times"; ;
	fontset = subtitle, inactive, noPageBreakBelow, nohscroll, preserveAspect, groupLikeTitle, center, M7, bold, L1, e6,  18, "Times"; ;
	fontset = subsubtitle, inactive, noPageBreakBelow, nohscroll, preserveAspect, groupLikeTitle, center, M7, italic, L1, e6,  14, "Times"; ;
	fontset = section, inactive, noPageBreakBelow, nohscroll, preserveAspect, groupLikeSection, grayBox, M22, bold, L1, a20,  18, "Times"; ;
	fontset = subsection, inactive, noPageBreakBelow, nohscroll, preserveAspect, groupLikeSection, blackBox, M19, bold, L1, a15,  14, "Times"; ;
	fontset = subsubsection, inactive, noPageBreakBelow, nohscroll, preserveAspect, groupLikeSection, whiteBox, M18, bold, L1, a12,  12, "Times"; ;
	fontset = text, inactive, nohscroll, noKeepOnOnePage, preserveAspect, M7, L1,  14, "Times"; ;
	fontset = smalltext, inactive, nohscroll, noKeepOnOnePage, preserveAspect, M7, L1,  12;
	fontset = input, noPageBreakBelow, nowordwrap, preserveAspect, groupLikeInput, M42, N23, bold, L1,  12, "Courier"; ;
	fontset = output, output, inactive, noPageBreakBelow, nowordwrap, preserveAspect, groupLikeOutput, M42, N23, L-5,  12, "Courier"; ;
	fontset = message, inactive, noPageBreakBelow, nowordwrap, preserveAspect, groupLikeOutput, M42, N23, L1,  12, "Courier"; ;
	fontset = print, inactive, noPageBreakBelow, nowordwrap, preserveAspect, groupLikeOutput, M42, N23, L1,  12, "Courier"; ;
	fontset = info, inactive, noPageBreakBelow, nowordwrap, preserveAspect, groupLikeOutput, M42, N23, L1,  12, "Courier"; ;
	fontset = postscript, PostScript, formatAsPostScript, output, inactive, noPageBreakBelow, nowordwrap, preserveAspect, groupLikeGraphics, M7, l34, w282, h287, L1,  12, "Courier"; ;
	fontset = name, inactive, nohscroll, noKeepOnOnePage, preserveAspect, M7, italic, L1,  10, "Times"; ;
	fontset = header, inactive, nohscroll, noKeepOnOnePage, preserveAspect, M7, L1,  12;
	fontset = Left Header, nohscroll, cellOutline,  12;
	fontset = footer, inactive, nohscroll, noKeepOnOnePage, preserveAspect, center, M7, L1,  12;
	fontset = Left Footer, cellOutline, blackBox,  12;
	fontset = help, inactive, nohscroll, noKeepOnOnePage, preserveAspect, M7, L1,  10, "Times"; ;
	fontset = clipboard, inactive, nohscroll, noKeepOnOnePage, preserveAspect, M7, L1,  12;
	fontset = completions, inactive, nohscroll, noKeepOnOnePage, preserveAspect, M7, L1,  12, "Courier"; ;
	fontset = special1, inactive, nohscroll, noKeepOnOnePage, preserveAspect, M7, L1,  12;
	fontset = special2, inactive, nohscroll, noKeepOnOnePage, preserveAspect, M7, L1,  12;
	fontset = special3, inactive, nohscroll, noKeepOnOnePage, preserveAspect, M7, L1,  12;
	fontset = special4, inactive, nohscroll, noKeepOnOnePage, preserveAspect, M7, L1,  12;
	fontset = special5, inactive, nohscroll, noKeepOnOnePage, preserveAspect, M7, L1,  12;]
:[font = title; inactive; preserveAspect; startGroup; ]
Functions Defined by Integrals
:[font = section; inactive; preserveAspect; startGroup; ]
Introduction
:[font = input; preserveAspect; ]
ln[z_] := Integrate[1/x, {x, 1, z}]
:[font = input; preserveAspect; ]
P[z_] := Sqrt[2/Pi] Integrate[Exp[-x^2/2], {x, 0, z}]
:[font = input; preserveAspect; endGroup; ]
erf[z_] := 2/Sqrt[Pi] Integrate[Exp[-t^2], {t, 0, z}]
:[font = section; inactive; preserveAspect; startGroup; ]
Exercise 6
:[font = input; preserveAspect; startGroup; ]
Log[1]
:[font = output; output; inactive; preserveAspect; endGroup; ]
0
;[o]
0
:[font = input; preserveAspect; startGroup; ]
D[Log[z], z]
:[font = output; output; inactive; preserveAspect; endGroup; ]
z^(-1)
;[o]
1
-
z
:[font = input; preserveAspect; startGroup; ]
Log'[z]
:[font = output; output; inactive; preserveAspect; endGroup; endGroup; ]
z^(-1)
;[o]
1
-
z
:[font = section; inactive; preserveAspect; startGroup; ]
Exercise 7
:[font = input; preserveAspect; startGroup; ]
Table[{z, ln[z], Log[z]}, {z, 1.0, 2.0, 0.1}] //TableForm
:[font = output; output; inactive; preserveAspect; endGroup; endGroup; ]
TableForm[{{1., 0, 0.}, 
   {1.1, 0.0953101798043249, 0.0953101798043249}, 
   {1.2, 0.1823215567939548, 0.1823215567939548}, 
   {1.3, 0.2623642644674913, 0.2623642644674913}, 
   {1.4, 0.3364722366212132, 0.3364722366212132}, 
   {1.5, 0.4054651081081647, 0.4054651081081647}, 
   {1.6, 0.4700036292457359, 0.4700036292457359}, 
   {1.700000000000001, 0.5306282510621707, 
    0.5306282510621707}, 
   {1.800000000000001, 0.5877866649021195, 
    0.5877866649021195}, 
   {1.900000000000001, 0.6418538861723953, 
    0.6418538861723953}, 
   {2.000000000000001, 0.6931471805599458, 
    0.6931471805599458}}]
;[o]
1.    0           0.

1.1   0.0953102   0.0953102

1.2   0.182322    0.182322

1.3   0.262364    0.262364

1.4   0.336472    0.336472

1.5   0.405465    0.405465

1.6   0.470004    0.470004

1.7   0.530628    0.530628

1.8   0.587787    0.587787

1.9   0.641854    0.641854

2.    0.693147    0.693147
:[font = section; inactive; preserveAspect; startGroup; ]
Exercise 8
:[font = input; preserveAspect; startGroup; ]
Erf[z] + O[z]^7
:[font = output; output; inactive; preserveAspect; endGroup; endGroup; ]
SeriesData[z, 0, {2/Pi^(1/2), 0, -2/(3*Pi^(1/2)), 0, 
   1/(5*Pi^(1/2))}, 1, 7, 1]
;[o]
                 3           5
  2 z         2 z           z            7
-------- - ---------- + ---------- + O[z]
Sqrt[Pi]   3 Sqrt[Pi]   5 Sqrt[Pi]
:[font = section; inactive; preserveAspect; startGroup; ]
Exercise 9
:[font = input; preserveAspect; startGroup; ]
p[z_] = Erf[z] + O[z]^7 //Normal//N;
Table[{z, erf[z], Erf[z], p[z]}, {z, 0.0, 2.0, 0.25}] //
TableForm
:[font = output; output; inactive; preserveAspect; endGroup; endGroup; ]
TableForm[{{0., 0., 0., 0.}, 
   {0.25, 0.2763263901682369, 0.2763263901682369, 
    0.2763280102232923}, 
   {0.5, 0.5204998778130465, 0.5204998778130465, 
    0.52069996981595}, 
   {0.75, 0.7111556336535151, 0.7111556336535151, 
    0.7143830215117392}, 
   {1., 0.842700792949715, 0.842700792949715, 
    0.865090694773226}, 
   {1.25, 0.922900128256458, 0.922900128256458, 
    1.02020609915618}, 
   {1.5, 0.966105146475311, 0.966105146475311, 
    1.280005117673972}, 
   {1.75, 0.986671671219182, 0.986671671219182, 
    1.810879600161975}, 
   {2., 0.995322265018953, 0.995322265018953, 
    2.858560556641966}}]
;[o]
0.     0.         0.         0.

0.25   0.276326   0.276326   0.276328

0.5    0.5205     0.5205     0.5207

0.75   0.711156   0.711156   0.714383

1.     0.842701   0.842701   0.865091

1.25   0.9229     0.9229     1.02021

1.5    0.966105   0.966105   1.28001

1.75   0.986672   0.986672   1.81088

2.     0.995322   0.995322   2.85856
:[font = section; inactive; preserveAspect; startGroup; ]
Exercise 10
:[font = input; preserveAspect; startGroup; ]
erf[Infinity]
:[font = output; output; inactive; preserveAspect; endGroup; ]
1
;[o]
1
:[font = input; preserveAspect; startGroup; ]
Erf[Infinity]
:[font = output; output; inactive; preserveAspect; endGroup; endGroup; ]
1
;[o]
1
:[font = section; inactive; preserveAspect; startGroup; ]
Exercise 11
:[font = input; preserveAspect; startGroup; ]
Plot[Erf[x], {x, -5, 5}]
:[font = postscript; PostScript; formatAsPostScript; output; inactive; preserveAspect; pictureLeft = 34; pictureWidth = 282; pictureHeight = 174; ]
%!
%%Creator: Mathematica
%%AspectRatio: 0.61803 
MathPictureStart
/Courier findfont 10  scalefont  setfont
% Scaling calculations
0.5 0.095238 0.309017 0.294302 [
[(-4)] 0.11905 0.30902 0 2 Msboxa
[(-2)] 0.30952 0.30902 0 2 Msboxa
[(2)] 0.69048 0.30902 0 2 Msboxa
[(4)] 0.88095 0.30902 0 2 Msboxa
[(-1)] 0.4875 0.01472 1 0 Msboxa
[(-0.5)] 0.4875 0.16187 1 0 Msboxa
[(0.5)] 0.4875 0.45617 1 0 Msboxa
[(1)] 0.4875 0.60332 1 0 Msboxa
[ -0.001 -0.001 0 0 ]
[ 1.001 0.61903 0 0 ]
] MathScale
% Start of Graphics
1 setlinecap
1 setlinejoin
newpath
%%Object: Graphics
[ ] 0 setdash
0 setgray
gsave
gsave
0.002 setlinewidth
0.11905 0.30902 moveto
0.11905 0.31527 lineto
stroke
grestore
[(-4)] 0.11905 0.30902 0 2 Mshowa
gsave
0.002 setlinewidth
0.30952 0.30902 moveto
0.30952 0.31527 lineto
stroke
grestore
[(-2)] 0.30952 0.30902 0 2 Mshowa
gsave
0.002 setlinewidth
0.69048 0.30902 moveto
0.69048 0.31527 lineto
stroke
grestore
[(2)] 0.69048 0.30902 0 2 Mshowa
gsave
0.002 setlinewidth
0.88095 0.30902 moveto
0.88095 0.31527 lineto
stroke
grestore
[(4)] 0.88095 0.30902 0 2 Mshowa
gsave
0.001 setlinewidth
0.15714 0.30902 moveto
0.15714 0.31277 lineto
stroke
grestore
gsave
0.001 setlinewidth
0.19524 0.30902 moveto
0.19524 0.31277 lineto
stroke
grestore
gsave
0.001 setlinewidth
0.23333 0.30902 moveto
0.23333 0.31277 lineto
stroke
grestore
gsave
0.001 setlinewidth
0.27143 0.30902 moveto
0.27143 0.31277 lineto
stroke
grestore
gsave
0.001 setlinewidth
0.34762 0.30902 moveto
0.34762 0.31277 lineto
stroke
grestore
gsave
0.001 setlinewidth
0.38571 0.30902 moveto
0.38571 0.31277 lineto
stroke
grestore
gsave
0.001 setlinewidth
0.42381 0.30902 moveto
0.42381 0.31277 lineto
stroke
grestore
gsave
0.001 setlinewidth
0.4619 0.30902 moveto
0.4619 0.31277 lineto
stroke
grestore
gsave
0.001 setlinewidth
0.5381 0.30902 moveto
0.5381 0.31277 lineto
stroke
grestore
gsave
0.001 setlinewidth
0.57619 0.30902 moveto
0.57619 0.31277 lineto
stroke
grestore
gsave
0.001 setlinewidth
0.61429 0.30902 moveto
0.61429 0.31277 lineto
stroke
grestore
gsave
0.001 setlinewidth
0.65238 0.30902 moveto
0.65238 0.31277 lineto
stroke
grestore
gsave
0.001 setlinewidth
0.72857 0.30902 moveto
0.72857 0.31277 lineto
stroke
grestore
gsave
0.001 setlinewidth
0.76667 0.30902 moveto
0.76667 0.31277 lineto
stroke
grestore
gsave
0.001 setlinewidth
0.80476 0.30902 moveto
0.80476 0.31277 lineto
stroke
grestore
gsave
0.001 setlinewidth
0.84286 0.30902 moveto
0.84286 0.31277 lineto
stroke
grestore
gsave
0.001 setlinewidth
0.08095 0.30902 moveto
0.08095 0.31277 lineto
stroke
grestore
gsave
0.001 setlinewidth
0.04286 0.30902 moveto
0.04286 0.31277 lineto
stroke
grestore
gsave
0.001 setlinewidth
0.00476 0.30902 moveto
0.00476 0.31277 lineto
stroke
grestore
gsave
0.001 setlinewidth
0.91905 0.30902 moveto
0.91905 0.31277 lineto
stroke
grestore
gsave
0.001 setlinewidth
0.95714 0.30902 moveto
0.95714 0.31277 lineto
stroke
grestore
gsave
0.001 setlinewidth
0.99524 0.30902 moveto
0.99524 0.31277 lineto
stroke
grestore
gsave
0.002 setlinewidth
0 0.30902 moveto
1 0.30902 lineto
stroke
grestore
gsave
0.002 setlinewidth
0.5 0.01472 moveto
0.50625 0.01472 lineto
stroke
grestore
[(-1)] 0.4875 0.01472 1 0 Mshowa
gsave
0.002 setlinewidth
0.5 0.16187 moveto
0.50625 0.16187 lineto
stroke
grestore
[(-0.5)] 0.4875 0.16187 1 0 Mshowa
gsave
0.002 setlinewidth
0.5 0.45617 moveto
0.50625 0.45617 lineto
stroke
grestore
[(0.5)] 0.4875 0.45617 1 0 Mshowa
gsave
0.002 setlinewidth
0.5 0.60332 moveto
0.50625 0.60332 lineto
stroke
grestore
[(1)] 0.4875 0.60332 1 0 Mshowa
gsave
0.001 setlinewidth
0.5 0.04415 moveto
0.50375 0.04415 lineto
stroke
grestore
gsave
0.001 setlinewidth
0.5 0.07358 moveto
0.50375 0.07358 lineto
stroke
grestore
gsave
0.001 setlinewidth
0.5 0.10301 moveto
0.50375 0.10301 lineto
stroke
grestore
gsave
0.001 setlinewidth
0.5 0.13244 moveto
0.50375 0.13244 lineto
stroke
grestore
gsave
0.001 setlinewidth
0.5 0.1913 moveto
0.50375 0.1913 lineto
stroke
grestore
gsave
0.001 setlinewidth
0.5 0.22073 moveto
0.50375 0.22073 lineto
stroke
grestore
gsave
0.001 setlinewidth
0.5 0.25016 moveto
0.50375 0.25016 lineto
stroke
grestore
gsave
0.001 setlinewidth
0.5 0.27959 moveto
0.50375 0.27959 lineto
stroke
grestore
gsave
0.001 setlinewidth
0.5 0.33845 moveto
0.50375 0.33845 lineto
stroke
grestore
gsave
0.001 setlinewidth
0.5 0.36788 moveto
0.50375 0.36788 lineto
stroke
grestore
gsave
0.001 setlinewidth
0.5 0.39731 moveto
0.50375 0.39731 lineto
stroke
grestore
gsave
0.001 setlinewidth
0.5 0.42674 moveto
0.50375 0.42674 lineto
stroke
grestore
gsave
0.001 setlinewidth
0.5 0.4856 moveto
0.50375 0.4856 lineto
stroke
grestore
gsave
0.001 setlinewidth
0.5 0.51503 moveto
0.50375 0.51503 lineto
stroke
grestore
gsave
0.001 setlinewidth
0.5 0.54446 moveto
0.50375 0.54446 lineto
stroke
grestore
gsave
0.001 setlinewidth
0.5 0.57389 moveto
0.50375 0.57389 lineto
stroke
grestore
gsave
0.002 setlinewidth
0.5 0 moveto
0.5 0.61803 lineto
stroke
grestore
grestore
0 0 moveto
1 0 lineto
1 0.61803 lineto
0 0.61803 lineto
closepath
clip
newpath
gsave
gsave
0.004 setlinewidth
0.02381 0.01472 moveto
0.02629 0.01472 lineto
0.02877 0.01472 lineto
0.03125 0.01472 lineto
0.03373 0.01472 lineto
0.03621 0.01472 lineto
0.03869 0.01472 lineto
0.04117 0.01472 lineto
0.04365 0.01472 lineto
0.04613 0.01472 lineto
0.04861 0.01472 lineto
0.05109 0.01472 lineto
0.05357 0.01472 lineto
0.05605 0.01472 lineto
0.05853 0.01472 lineto
0.06101 0.01472 lineto
0.06349 0.01472 lineto
0.06597 0.01472 lineto
0.06845 0.01472 lineto
0.07093 0.01472 lineto
0.07341 0.01472 lineto
0.07589 0.01472 lineto
0.07837 0.01472 lineto
0.08085 0.01472 lineto
0.08333 0.01472 lineto
0.08581 0.01472 lineto
0.08829 0.01472 lineto
0.09325 0.01472 lineto
0.09573 0.01472 lineto
0.09821 0.01472 lineto
0.10069 0.01472 lineto
0.10317 0.01472 lineto
0.10813 0.01472 lineto
0.11062 0.01472 lineto
0.1131 0.01472 lineto
0.11806 0.01472 lineto
0.12054 0.01472 lineto
0.12302 0.01472 lineto
0.1255 0.01472 lineto
0.12798 0.01472 lineto
0.13294 0.01472 lineto
0.13542 0.01472 lineto
0.1379 0.01472 lineto
0.14038 0.01472 lineto
0.14286 0.01472 lineto
0.14534 0.01472 lineto
0.14782 0.01472 lineto
0.1503 0.01472 lineto
0.15278 0.01472 lineto
0.15774 0.01472 lineto
Mistroke
0.16022 0.01472 lineto
0.1627 0.01472 lineto
0.16518 0.01472 lineto
0.16766 0.01472 lineto
0.17262 0.01472 lineto
0.1751 0.01472 lineto
0.17758 0.01472 lineto
0.18254 0.01472 lineto
0.1875 0.01472 lineto
0.19246 0.01472 lineto
0.19742 0.01472 lineto
0.20238 0.01472 lineto
0.20734 0.01472 lineto
0.2123 0.01472 lineto
0.21726 0.01472 lineto
0.22222 0.01473 lineto
0.22718 0.01473 lineto
0.23214 0.01474 lineto
0.2371 0.01474 lineto
0.24206 0.01475 lineto
0.24702 0.01477 lineto
0.25198 0.01478 lineto
0.25694 0.01481 lineto
0.2619 0.01483 lineto
0.26687 0.01487 lineto
0.27183 0.01492 lineto
0.27679 0.01499 lineto
0.28175 0.01507 lineto
0.28671 0.01517 lineto
0.29167 0.0153 lineto
0.29663 0.01546 lineto
0.30159 0.01566 lineto
0.31151 0.01622 lineto
0.31647 0.01661 lineto
0.32143 0.01707 lineto
0.32639 0.01764 lineto
0.33135 0.01833 lineto
0.34127 0.02014 lineto
0.35119 0.0227 lineto
0.36111 0.02624 lineto
0.37103 0.03104 lineto
0.38095 0.03741 lineto
0.39087 0.04566 lineto
0.40079 0.05613 lineto
0.42063 0.08493 lineto
0.44048 0.1256 lineto
0.46032 0.17826 lineto
0.5 0.30902 lineto
0.53968 0.43978 lineto
0.55952 0.49244 lineto
Mistroke
0.57937 0.5331 lineto
0.58929 0.5489 lineto
0.59921 0.56191 lineto
0.60913 0.57238 lineto
0.61905 0.58063 lineto
0.62897 0.58699 lineto
0.63889 0.59179 lineto
0.64881 0.59534 lineto
0.65873 0.5979 lineto
0.66865 0.59971 lineto
0.67361 0.60039 lineto
0.67857 0.60096 lineto
0.68353 0.60143 lineto
0.68849 0.60181 lineto
0.69345 0.60212 lineto
0.69841 0.60237 lineto
0.70833 0.60274 lineto
0.71329 0.60287 lineto
0.71825 0.60297 lineto
0.72321 0.60305 lineto
0.72817 0.60311 lineto
0.73313 0.60316 lineto
0.7381 0.6032 lineto
0.74306 0.60323 lineto
0.74802 0.60325 lineto
0.75298 0.60327 lineto
0.75794 0.60328 lineto
0.7629 0.60329 lineto
0.76786 0.6033 lineto
0.77282 0.6033 lineto
0.77778 0.60331 lineto
0.78274 0.60331 lineto
0.7877 0.60331 lineto
0.79266 0.60331 lineto
0.79762 0.60332 lineto
0.80258 0.60332 lineto
0.80754 0.60332 lineto
0.8125 0.60332 lineto
0.81746 0.60332 lineto
0.82242 0.60332 lineto
0.8249 0.60332 lineto
0.82738 0.60332 lineto
0.82986 0.60332 lineto
0.83234 0.60332 lineto
0.8373 0.60332 lineto
0.83978 0.60332 lineto
0.84226 0.60332 lineto
0.84474 0.60332 lineto
0.84722 0.60332 lineto
0.85218 0.60332 lineto
Mistroke
0.85466 0.60332 lineto
0.85714 0.60332 lineto
0.85962 0.60332 lineto
0.8621 0.60332 lineto
0.86706 0.60332 lineto
0.86954 0.60332 lineto
0.87202 0.60332 lineto
0.8745 0.60332 lineto
0.87698 0.60332 lineto
0.87946 0.60332 lineto
0.88194 0.60332 lineto
0.8869 0.60332 lineto
0.88938 0.60332 lineto
0.89187 0.60332 lineto
0.89435 0.60332 lineto
0.89683 0.60332 lineto
0.90179 0.60332 lineto
0.90427 0.60332 lineto
0.90675 0.60332 lineto
0.90923 0.60332 lineto
0.91171 0.60332 lineto
0.91667 0.60332 lineto
0.91915 0.60332 lineto
0.92163 0.60332 lineto
0.92411 0.60332 lineto
0.92659 0.60332 lineto
0.92907 0.60332 lineto
0.93155 0.60332 lineto
0.93403 0.60332 lineto
0.93651 0.60332 lineto
0.93899 0.60332 lineto
0.94147 0.60332 lineto
0.94395 0.60332 lineto
0.94643 0.60332 lineto
0.94891 0.60332 lineto
0.95139 0.60332 lineto
0.95387 0.60332 lineto
0.95635 0.60332 lineto
0.95883 0.60332 lineto
0.96131 0.60332 lineto
0.96379 0.60332 lineto
0.96627 0.60332 lineto
0.96875 0.60332 lineto
0.97123 0.60332 lineto
0.97371 0.60332 lineto
0.97619 0.60332 lineto
Mfstroke
grestore
grestore
% End of Graphics
MathPictureEnd
:[font = output; output; inactive; preserveAspect; endGroup; endGroup; endGroup; ]
The Unformatted text for this cell was not generated.
Use options in the Actions Settings dialog box to control
when Unformatted text is generated.
;[o]
-Graphics-
^*)