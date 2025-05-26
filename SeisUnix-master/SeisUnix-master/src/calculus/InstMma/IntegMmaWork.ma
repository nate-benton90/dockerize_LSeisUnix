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
Integration in Mathematica
:[font = section; inactive; Cclosed; preserveAspect; startGroup; ]
Overview
:[font = subsection; inactive; Cclosed; preserveAspect; startGroup; ]
The Indefinite Integral
:[font = input; preserveAspect; startGroup; ]
Integrate[x^3, x]
:[font = output; output; inactive; preserveAspect; endGroup; ]
x^4/4
;[o]
 4
x
--
4
:[font = input; preserveAspect; startGroup; ]
f[x_] := Sin[x]
Integrate[f[x], x]
:[font = output; output; inactive; preserveAspect; endGroup; endGroup; ]
-Cos[x]
;[o]
-Cos[x]
:[font = subsection; inactive; Cclosed; preserveAspect; startGroup; ]
The Definite Integral -- Antiderivative Exists
:[font = input; preserveAspect; startGroup; ]
Integrate[x^5, {x, 10, 20}]
:[font = output; output; inactive; preserveAspect; endGroup; ]
10500000
;[o]
10500000
:[font = input; preserveAspect; startGroup; ]
Integrate[x^5, {x, a, b}]
:[font = output; output; inactive; preserveAspect; endGroup; ]
-a^6/6 + b^6/6
;[o]
  6    6
-a    b
--- + --
 6    6
:[font = input; preserveAspect; startGroup; ]
Integrate[ArcSin[x], {x, 0, 1/2}]
:[font = output; output; inactive; preserveAspect; endGroup; ]
-1 + 3^(1/2)/2 + Pi/12
;[o]
     Sqrt[3]   Pi
-1 + ------- + --
        2      12
:[font = input; preserveAspect; startGroup; ]
N@Integrate[ArcSin[x], {x, 0, 1/2}]
:[font = output; output; inactive; preserveAspect; endGroup; ]
0.1278247915835878
;[o]
0.127825
:[font = input; preserveAspect; startGroup; ]
NIntegrate[ArcSin[x], {x, 0, 1/2}]
:[font = output; output; inactive; preserveAspect; endGroup; ]
0.1278247915835881
;[o]
0.127825
:[font = input; preserveAspect; ]
MyArcSin[x_] := Integrate[1/Sqrt[1 - t^2], {t, 0, x}]
:[font = input; preserveAspect; startGroup; ]
Plot[MyArcSin[x], {x, -0.9, 0.9}] //Timing
:[font = postscript; PostScript; formatAsPostScript; output; inactive; preserveAspect; pictureLeft = 34; pictureWidth = 282; pictureHeight = 174; ]
%!
%%Creator: Mathematica
%%AspectRatio: 0.61803 
MathPictureStart
/Courier findfont 10  scalefont  setfont
% Scaling calculations
0.5 0.529101 0.309017 0.262824 [
[(-0.75)] 0.10317 0.30902 0 2 Msboxa
[(-0.5)] 0.23545 0.30902 0 2 Msboxa
[(-0.25)] 0.36772 0.30902 0 2 Msboxa
[(0.25)] 0.63228 0.30902 0 2 Msboxa
[(0.5)] 0.76455 0.30902 0 2 Msboxa
[(0.75)] 0.89683 0.30902 0 2 Msboxa
[(-1)] 0.4875 0.04619 1 0 Msboxa
[(-0.5)] 0.4875 0.17761 1 0 Msboxa
[(0.5)] 0.4875 0.44043 1 0 Msboxa
[(1)] 0.4875 0.57184 1 0 Msboxa
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
0.10317 0.30902 moveto
0.10317 0.31527 lineto
stroke
grestore
[(-0.75)] 0.10317 0.30902 0 2 Mshowa
gsave
0.002 setlinewidth
0.23545 0.30902 moveto
0.23545 0.31527 lineto
stroke
grestore
[(-0.5)] 0.23545 0.30902 0 2 Mshowa
gsave
0.002 setlinewidth
0.36772 0.30902 moveto
0.36772 0.31527 lineto
stroke
grestore
[(-0.25)] 0.36772 0.30902 0 2 Mshowa
gsave
0.002 setlinewidth
0.63228 0.30902 moveto
0.63228 0.31527 lineto
stroke
grestore
[(0.25)] 0.63228 0.30902 0 2 Mshowa
gsave
0.002 setlinewidth
0.76455 0.30902 moveto
0.76455 0.31527 lineto
stroke
grestore
[(0.5)] 0.76455 0.30902 0 2 Mshowa
gsave
0.002 setlinewidth
0.89683 0.30902 moveto
0.89683 0.31527 lineto
stroke
grestore
[(0.75)] 0.89683 0.30902 0 2 Mshowa
gsave
0.001 setlinewidth
0.12963 0.30902 moveto
0.12963 0.31277 lineto
stroke
grestore
gsave
0.001 setlinewidth
0.15608 0.30902 moveto
0.15608 0.31277 lineto
stroke
grestore
gsave
0.001 setlinewidth
0.18254 0.30902 moveto
0.18254 0.31277 lineto
stroke
grestore
gsave
0.001 setlinewidth
0.20899 0.30902 moveto
0.20899 0.31277 lineto
stroke
grestore
gsave
0.001 setlinewidth
0.2619 0.30902 moveto
0.2619 0.31277 lineto
stroke
grestore
gsave
0.001 setlinewidth
0.28836 0.30902 moveto
0.28836 0.31277 lineto
stroke
grestore
gsave
0.001 setlinewidth
0.31481 0.30902 moveto
0.31481 0.31277 lineto
stroke
grestore
gsave
0.001 setlinewidth
0.34127 0.30902 moveto
0.34127 0.31277 lineto
stroke
grestore
gsave
0.001 setlinewidth
0.39418 0.30902 moveto
0.39418 0.31277 lineto
stroke
grestore
gsave
0.001 setlinewidth
0.42063 0.30902 moveto
0.42063 0.31277 lineto
stroke
grestore
gsave
0.001 setlinewidth
0.44709 0.30902 moveto
0.44709 0.31277 lineto
stroke
grestore
gsave
0.001 setlinewidth
0.47354 0.30902 moveto
0.47354 0.31277 lineto
stroke
grestore
gsave
0.001 setlinewidth
0.52646 0.30902 moveto
0.52646 0.31277 lineto
stroke
grestore
gsave
0.001 setlinewidth
0.55291 0.30902 moveto
0.55291 0.31277 lineto
stroke
grestore
gsave
0.001 setlinewidth
0.57937 0.30902 moveto
0.57937 0.31277 lineto
stroke
grestore
gsave
0.001 setlinewidth
0.60582 0.30902 moveto
0.60582 0.31277 lineto
stroke
grestore
gsave
0.001 setlinewidth
0.65873 0.30902 moveto
0.65873 0.31277 lineto
stroke
grestore
gsave
0.001 setlinewidth
0.68519 0.30902 moveto
0.68519 0.31277 lineto
stroke
grestore
gsave
0.001 setlinewidth
0.71164 0.30902 moveto
0.71164 0.31277 lineto
stroke
grestore
gsave
0.001 setlinewidth
0.7381 0.30902 moveto
0.7381 0.31277 lineto
stroke
grestore
gsave
0.001 setlinewidth
0.79101 0.30902 moveto
0.79101 0.31277 lineto
stroke
grestore
gsave
0.001 setlinewidth
0.81746 0.30902 moveto
0.81746 0.31277 lineto
stroke
grestore
gsave
0.001 setlinewidth
0.84392 0.30902 moveto
0.84392 0.31277 lineto
stroke
grestore
gsave
0.001 setlinewidth
0.87037 0.30902 moveto
0.87037 0.31277 lineto
stroke
grestore
gsave
0.001 setlinewidth
0.07672 0.30902 moveto
0.07672 0.31277 lineto
stroke
grestore
gsave
0.001 setlinewidth
0.05026 0.30902 moveto
0.05026 0.31277 lineto
stroke
grestore
gsave
0.001 setlinewidth
0.02381 0.30902 moveto
0.02381 0.31277 lineto
stroke
grestore
gsave
0.001 setlinewidth
0.92328 0.30902 moveto
0.92328 0.31277 lineto
stroke
grestore
gsave
0.001 setlinewidth
0.94974 0.30902 moveto
0.94974 0.31277 lineto
stroke
grestore
gsave
0.001 setlinewidth
0.97619 0.30902 moveto
0.97619 0.31277 lineto
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
0.5 0.04619 moveto
0.50625 0.04619 lineto
stroke
grestore
[(-1)] 0.4875 0.04619 1 0 Mshowa
gsave
0.002 setlinewidth
0.5 0.17761 moveto
0.50625 0.17761 lineto
stroke
grestore
[(-0.5)] 0.4875 0.17761 1 0 Mshowa
gsave
0.002 setlinewidth
0.5 0.44043 moveto
0.50625 0.44043 lineto
stroke
grestore
[(0.5)] 0.4875 0.44043 1 0 Mshowa
gsave
0.002 setlinewidth
0.5 0.57184 moveto
0.50625 0.57184 lineto
stroke
grestore
[(1)] 0.4875 0.57184 1 0 Mshowa
gsave
0.001 setlinewidth
0.5 0.07248 moveto
0.50375 0.07248 lineto
stroke
grestore
gsave
0.001 setlinewidth
0.5 0.09876 moveto
0.50375 0.09876 lineto
stroke
grestore
gsave
0.001 setlinewidth
0.5 0.12504 moveto
0.50375 0.12504 lineto
stroke
grestore
gsave
0.001 setlinewidth
0.5 0.15132 moveto
0.50375 0.15132 lineto
stroke
grestore
gsave
0.001 setlinewidth
0.5 0.20389 moveto
0.50375 0.20389 lineto
stroke
grestore
gsave
0.001 setlinewidth
0.5 0.23017 moveto
0.50375 0.23017 lineto
stroke
grestore
gsave
0.001 setlinewidth
0.5 0.25645 moveto
0.50375 0.25645 lineto
stroke
grestore
gsave
0.001 setlinewidth
0.5 0.28273 moveto
0.50375 0.28273 lineto
stroke
grestore
gsave
0.001 setlinewidth
0.5 0.3353 moveto
0.50375 0.3353 lineto
stroke
grestore
gsave
0.001 setlinewidth
0.5 0.36158 moveto
0.50375 0.36158 lineto
stroke
grestore
gsave
0.001 setlinewidth
0.5 0.38786 moveto
0.50375 0.38786 lineto
stroke
grestore
gsave
0.001 setlinewidth
0.5 0.41415 moveto
0.50375 0.41415 lineto
stroke
grestore
gsave
0.001 setlinewidth
0.5 0.46671 moveto
0.50375 0.46671 lineto
stroke
grestore
gsave
0.001 setlinewidth
0.5 0.49299 moveto
0.50375 0.49299 lineto
stroke
grestore
gsave
0.001 setlinewidth
0.5 0.51928 moveto
0.50375 0.51928 lineto
stroke
grestore
gsave
0.001 setlinewidth
0.5 0.54556 moveto
0.50375 0.54556 lineto
stroke
grestore
gsave
0.001 setlinewidth
0.5 0.01991 moveto
0.50375 0.01991 lineto
stroke
grestore
gsave
0.001 setlinewidth
0.5 0.59812 moveto
0.50375 0.59812 lineto
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
0.06349 0.05402 lineto
0.10317 0.08613 lineto
0.14286 0.11427 lineto
0.18254 0.13989 lineto
0.22222 0.16375 lineto
0.2619 0.18634 lineto
0.30159 0.20799 lineto
0.34127 0.22894 lineto
0.38095 0.24937 lineto
0.42063 0.26944 lineto
0.46032 0.28929 lineto
0.5 0.30902 lineto
0.53968 0.32875 lineto
0.57937 0.34859 lineto
0.61905 0.36866 lineto
0.65873 0.3891 lineto
0.69841 0.41005 lineto
0.7381 0.43169 lineto
0.77778 0.45428 lineto
0.81746 0.47814 lineto
0.85714 0.50376 lineto
0.89683 0.53191 lineto
0.93651 0.56401 lineto
0.97619 0.60332 lineto
stroke
grestore
grestore
% End of Graphics
MathPictureEnd
:[font = output; output; inactive; preserveAspect; endGroup; ]
{21.08333333333333*Second, 
  Graphics[{{Line[{{-0.9, -1.119769514998634}, 
       {-0.825, -0.970202199928846}, 
       {-0.75, -0.848062078981481}, 
       {-0.675, -0.7409647022030201}, 
       {-0.6000000000000001, -0.6435011087932845}, 
       {-0.525, -0.5527151130967832}, 
       {-0.4500000000000001, -0.4667653390472965}, 
       {-0.375, -0.3843967744956391}, 
       {-0.3, -0.3046926540153976}, 
       {-0.225, -0.22694303617852}, 
       {-0.15, -0.1505682727766861}, 
       {-0.07500000000000006, -0.0750704910767166}, 
       {-(5.551115123125783*10^-17), 
        -(5.551115123125783*10^-17)}, 
       {0.07499999999999994, 0.07507049107671648}, 
       {0.1499999999999999, 0.150568272776686}, 
       {0.2249999999999999, 0.2269430361785199}, 
       {0.2999999999999999, 0.3046926540153974}, 
       {0.375, 0.384396774495639}, 
       {0.45, 0.4667653390472964}, 
       {0.525, 0.5527151130967832}, 
       {0.5999999999999999, 0.6435011087932843}, 
       {0.675, 0.74096470220302}, 
       {0.75, 0.848062078981481}, 
       {0.825, 0.970202199928846}, 
       {0.9, 1.119769514998634}}]}}, 
   {PlotRange -> Automatic, 
    AspectRatio -> GoldenRatio^(-1), 
    DisplayFunction :> $DisplayFunction, 
    ColorOutput -> Automatic, Axes -> Automatic, 
    AxesOrigin -> Automatic, PlotLabel -> None, 
    AxesLabel -> None, Ticks -> Automatic, 
    GridLines -> None, Prolog -> {}, Epilog -> {}, 
    AxesStyle -> Automatic, Background -> Automatic, 
    DefaultColor -> Automatic, 
    DefaultFont :> $DefaultFont, RotateLabel -> True, 
    Frame -> False, FrameStyle -> Automatic, 
    FrameTicks -> Automatic, FrameLabel -> None, 
    PlotRegion -> Automatic}]}
;[o]
{21.0833 Second, -Graphics-}
:[font = input; preserveAspect; startGroup; ]
MyBetterArcSin[x_] = Integrate[1/Sqrt[1 - t^2], {t, 0, x}]
:[font = output; output; inactive; preserveAspect; endGroup; ]
ArcSin[x]
;[o]
ArcSin[x]
:[font = input; preserveAspect; startGroup; ]
Plot[MyBetterArcSin[x], {x, -0.9, 0.9}] //Timing
:[font = postscript; PostScript; formatAsPostScript; output; inactive; preserveAspect; pictureLeft = 34; pictureWidth = 282; pictureHeight = 174; ]
%!
%%Creator: Mathematica
%%AspectRatio: 0.61803 
MathPictureStart
/Courier findfont 10  scalefont  setfont
% Scaling calculations
0.5 0.529101 0.309017 0.262824 [
[(-0.75)] 0.10317 0.30902 0 2 Msboxa
[(-0.5)] 0.23545 0.30902 0 2 Msboxa
[(-0.25)] 0.36772 0.30902 0 2 Msboxa
[(0.25)] 0.63228 0.30902 0 2 Msboxa
[(0.5)] 0.76455 0.30902 0 2 Msboxa
[(0.75)] 0.89683 0.30902 0 2 Msboxa
[(-1)] 0.4875 0.04619 1 0 Msboxa
[(-0.5)] 0.4875 0.17761 1 0 Msboxa
[(0.5)] 0.4875 0.44043 1 0 Msboxa
[(1)] 0.4875 0.57184 1 0 Msboxa
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
0.10317 0.30902 moveto
0.10317 0.31527 lineto
stroke
grestore
[(-0.75)] 0.10317 0.30902 0 2 Mshowa
gsave
0.002 setlinewidth
0.23545 0.30902 moveto
0.23545 0.31527 lineto
stroke
grestore
[(-0.5)] 0.23545 0.30902 0 2 Mshowa
gsave
0.002 setlinewidth
0.36772 0.30902 moveto
0.36772 0.31527 lineto
stroke
grestore
[(-0.25)] 0.36772 0.30902 0 2 Mshowa
gsave
0.002 setlinewidth
0.63228 0.30902 moveto
0.63228 0.31527 lineto
stroke
grestore
[(0.25)] 0.63228 0.30902 0 2 Mshowa
gsave
0.002 setlinewidth
0.76455 0.30902 moveto
0.76455 0.31527 lineto
stroke
grestore
[(0.5)] 0.76455 0.30902 0 2 Mshowa
gsave
0.002 setlinewidth
0.89683 0.30902 moveto
0.89683 0.31527 lineto
stroke
grestore
[(0.75)] 0.89683 0.30902 0 2 Mshowa
gsave
0.001 setlinewidth
0.12963 0.30902 moveto
0.12963 0.31277 lineto
stroke
grestore
gsave
0.001 setlinewidth
0.15608 0.30902 moveto
0.15608 0.31277 lineto
stroke
grestore
gsave
0.001 setlinewidth
0.18254 0.30902 moveto
0.18254 0.31277 lineto
stroke
grestore
gsave
0.001 setlinewidth
0.20899 0.30902 moveto
0.20899 0.31277 lineto
stroke
grestore
gsave
0.001 setlinewidth
0.2619 0.30902 moveto
0.2619 0.31277 lineto
stroke
grestore
gsave
0.001 setlinewidth
0.28836 0.30902 moveto
0.28836 0.31277 lineto
stroke
grestore
gsave
0.001 setlinewidth
0.31481 0.30902 moveto
0.31481 0.31277 lineto
stroke
grestore
gsave
0.001 setlinewidth
0.34127 0.30902 moveto
0.34127 0.31277 lineto
stroke
grestore
gsave
0.001 setlinewidth
0.39418 0.30902 moveto
0.39418 0.31277 lineto
stroke
grestore
gsave
0.001 setlinewidth
0.42063 0.30902 moveto
0.42063 0.31277 lineto
stroke
grestore
gsave
0.001 setlinewidth
0.44709 0.30902 moveto
0.44709 0.31277 lineto
stroke
grestore
gsave
0.001 setlinewidth
0.47354 0.30902 moveto
0.47354 0.31277 lineto
stroke
grestore
gsave
0.001 setlinewidth
0.52646 0.30902 moveto
0.52646 0.31277 lineto
stroke
grestore
gsave
0.001 setlinewidth
0.55291 0.30902 moveto
0.55291 0.31277 lineto
stroke
grestore
gsave
0.001 setlinewidth
0.57937 0.30902 moveto
0.57937 0.31277 lineto
stroke
grestore
gsave
0.001 setlinewidth
0.60582 0.30902 moveto
0.60582 0.31277 lineto
stroke
grestore
gsave
0.001 setlinewidth
0.65873 0.30902 moveto
0.65873 0.31277 lineto
stroke
grestore
gsave
0.001 setlinewidth
0.68519 0.30902 moveto
0.68519 0.31277 lineto
stroke
grestore
gsave
0.001 setlinewidth
0.71164 0.30902 moveto
0.71164 0.31277 lineto
stroke
grestore
gsave
0.001 setlinewidth
0.7381 0.30902 moveto
0.7381 0.31277 lineto
stroke
grestore
gsave
0.001 setlinewidth
0.79101 0.30902 moveto
0.79101 0.31277 lineto
stroke
grestore
gsave
0.001 setlinewidth
0.81746 0.30902 moveto
0.81746 0.31277 lineto
stroke
grestore
gsave
0.001 setlinewidth
0.84392 0.30902 moveto
0.84392 0.31277 lineto
stroke
grestore
gsave
0.001 setlinewidth
0.87037 0.30902 moveto
0.87037 0.31277 lineto
stroke
grestore
gsave
0.001 setlinewidth
0.07672 0.30902 moveto
0.07672 0.31277 lineto
stroke
grestore
gsave
0.001 setlinewidth
0.05026 0.30902 moveto
0.05026 0.31277 lineto
stroke
grestore
gsave
0.001 setlinewidth
0.02381 0.30902 moveto
0.02381 0.31277 lineto
stroke
grestore
gsave
0.001 setlinewidth
0.92328 0.30902 moveto
0.92328 0.31277 lineto
stroke
grestore
gsave
0.001 setlinewidth
0.94974 0.30902 moveto
0.94974 0.31277 lineto
stroke
grestore
gsave
0.001 setlinewidth
0.97619 0.30902 moveto
0.97619 0.31277 lineto
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
0.5 0.04619 moveto
0.50625 0.04619 lineto
stroke
grestore
[(-1)] 0.4875 0.04619 1 0 Mshowa
gsave
0.002 setlinewidth
0.5 0.17761 moveto
0.50625 0.17761 lineto
stroke
grestore
[(-0.5)] 0.4875 0.17761 1 0 Mshowa
gsave
0.002 setlinewidth
0.5 0.44043 moveto
0.50625 0.44043 lineto
stroke
grestore
[(0.5)] 0.4875 0.44043 1 0 Mshowa
gsave
0.002 setlinewidth
0.5 0.57184 moveto
0.50625 0.57184 lineto
stroke
grestore
[(1)] 0.4875 0.57184 1 0 Mshowa
gsave
0.001 setlinewidth
0.5 0.07248 moveto
0.50375 0.07248 lineto
stroke
grestore
gsave
0.001 setlinewidth
0.5 0.09876 moveto
0.50375 0.09876 lineto
stroke
grestore
gsave
0.001 setlinewidth
0.5 0.12504 moveto
0.50375 0.12504 lineto
stroke
grestore
gsave
0.001 setlinewidth
0.5 0.15132 moveto
0.50375 0.15132 lineto
stroke
grestore
gsave
0.001 setlinewidth
0.5 0.20389 moveto
0.50375 0.20389 lineto
stroke
grestore
gsave
0.001 setlinewidth
0.5 0.23017 moveto
0.50375 0.23017 lineto
stroke
grestore
gsave
0.001 setlinewidth
0.5 0.25645 moveto
0.50375 0.25645 lineto
stroke
grestore
gsave
0.001 setlinewidth
0.5 0.28273 moveto
0.50375 0.28273 lineto
stroke
grestore
gsave
0.001 setlinewidth
0.5 0.3353 moveto
0.50375 0.3353 lineto
stroke
grestore
gsave
0.001 setlinewidth
0.5 0.36158 moveto
0.50375 0.36158 lineto
stroke
grestore
gsave
0.001 setlinewidth
0.5 0.38786 moveto
0.50375 0.38786 lineto
stroke
grestore
gsave
0.001 setlinewidth
0.5 0.41415 moveto
0.50375 0.41415 lineto
stroke
grestore
gsave
0.001 setlinewidth
0.5 0.46671 moveto
0.50375 0.46671 lineto
stroke
grestore
gsave
0.001 setlinewidth
0.5 0.49299 moveto
0.50375 0.49299 lineto
stroke
grestore
gsave
0.001 setlinewidth
0.5 0.51928 moveto
0.50375 0.51928 lineto
stroke
grestore
gsave
0.001 setlinewidth
0.5 0.54556 moveto
0.50375 0.54556 lineto
stroke
grestore
gsave
0.001 setlinewidth
0.5 0.01991 moveto
0.50375 0.01991 lineto
stroke
grestore
gsave
0.001 setlinewidth
0.5 0.59812 moveto
0.50375 0.59812 lineto
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
0.06349 0.05402 lineto
0.10317 0.08613 lineto
0.14286 0.11427 lineto
0.18254 0.13989 lineto
0.22222 0.16375 lineto
0.2619 0.18634 lineto
0.30159 0.20799 lineto
0.34127 0.22894 lineto
0.38095 0.24937 lineto
0.42063 0.26944 lineto
0.46032 0.28929 lineto
0.5 0.30902 lineto
0.53968 0.32875 lineto
0.57937 0.34859 lineto
0.61905 0.36866 lineto
0.65873 0.3891 lineto
0.69841 0.41005 lineto
0.7381 0.43169 lineto
0.77778 0.45428 lineto
0.81746 0.47814 lineto
0.85714 0.50376 lineto
0.89683 0.53191 lineto
0.93651 0.56401 lineto
0.97619 0.60332 lineto
stroke
grestore
grestore
% End of Graphics
MathPictureEnd
:[font = output; output; inactive; preserveAspect; endGroup; ]
{0.1666666666666666*Second, 
  Graphics[{{Line[{{-0.9, -1.119769514998634}, 
       {-0.825, -0.970202199928846}, 
       {-0.75, -0.848062078981481}, 
       {-0.675, -0.7409647022030201}, 
       {-0.6000000000000001, -0.6435011087932845}, 
       {-0.525, -0.5527151130967832}, 
       {-0.4500000000000001, -0.4667653390472965}, 
       {-0.375, -0.3843967744956391}, 
       {-0.3, -0.3046926540153976}, 
       {-0.225, -0.22694303617852}, 
       {-0.15, -0.1505682727766861}, 
       {-0.07500000000000006, -0.0750704910767166}, 
       {-(5.551115123125783*10^-17), 
        -(5.551115123125783*10^-17)}, 
       {0.07499999999999994, 0.07507049107671648}, 
       {0.1499999999999999, 0.150568272776686}, 
       {0.2249999999999999, 0.2269430361785199}, 
       {0.2999999999999999, 0.3046926540153974}, 
       {0.375, 0.384396774495639}, 
       {0.45, 0.4667653390472964}, 
       {0.525, 0.5527151130967832}, 
       {0.5999999999999999, 0.6435011087932843}, 
       {0.675, 0.74096470220302}, 
       {0.75, 0.848062078981481}, 
       {0.825, 0.970202199928846}, 
       {0.9, 1.119769514998634}}]}}, 
   {PlotRange -> Automatic, 
    AspectRatio -> GoldenRatio^(-1), 
    DisplayFunction :> $DisplayFunction, 
    ColorOutput -> Automatic, Axes -> Automatic, 
    AxesOrigin -> Automatic, PlotLabel -> None, 
    AxesLabel -> None, Ticks -> Automatic, 
    GridLines -> None, Prolog -> {}, Epilog -> {}, 
    AxesStyle -> Automatic, Background -> Automatic, 
    DefaultColor -> Automatic, 
    DefaultFont :> $DefaultFont, RotateLabel -> True, 
    Frame -> False, FrameStyle -> Automatic, 
    FrameTicks -> Automatic, FrameLabel -> None, 
    PlotRegion -> Automatic}]}
;[o]
{0.166667 Second, -Graphics-}
:[font = input; preserveAspect; startGroup; ]
21.0833/0.166667
:[font = output; output; inactive; preserveAspect; endGroup; endGroup; ]
126.499547000906
;[o]
126.5
:[font = subsection; inactive; Cclosed; preserveAspect; startGroup; ]
The Definite Integral -- No Antiderivative
:[font = subsubsection; inactive; Cclosed; preserveAspect; startGroup; ]
SuperArcSin
:[font = input; preserveAspect; startGroup; ]
SuperArcSin[x_] = Integrate[1/Sqrt[1 - t^4], {t, 0, x}]
:[font = output; output; inactive; preserveAspect; endGroup; ]
x*Hypergeometric2F1[1/4, 1/2, 5/4, x^4]
;[o]
                    1  1  5   4
x Hypergeometric2F1[-, -, -, x ]
                    4  2  4
:[font = input; preserveAspect; startGroup; ]
Plot[SuperArcSin[x], {x, -0.9, 0.9}]
:[font = postscript; PostScript; formatAsPostScript; output; inactive; preserveAspect; pictureLeft = 34; pictureWidth = 282; pictureHeight = 174; ]
%!
%%Creator: Mathematica
%%AspectRatio: 0.61803 
MathPictureStart
/Courier findfont 10  scalefont  setfont
% Scaling calculations
0.5 0.529101 0.309017 0.298276 [
[(-0.75)] 0.10317 0.30902 0 2 Msboxa
[(-0.5)] 0.23545 0.30902 0 2 Msboxa
[(-0.25)] 0.36772 0.30902 0 2 Msboxa
[(0.25)] 0.63228 0.30902 0 2 Msboxa
[(0.5)] 0.76455 0.30902 0 2 Msboxa
[(0.75)] 0.89683 0.30902 0 2 Msboxa
[(-1)] 0.4875 0.01074 1 0 Msboxa
[(-0.5)] 0.4875 0.15988 1 0 Msboxa
[(0.5)] 0.4875 0.45816 1 0 Msboxa
[(1)] 0.4875 0.60729 1 0 Msboxa
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
0.10317 0.30902 moveto
0.10317 0.31527 lineto
stroke
grestore
[(-0.75)] 0.10317 0.30902 0 2 Mshowa
gsave
0.002 setlinewidth
0.23545 0.30902 moveto
0.23545 0.31527 lineto
stroke
grestore
[(-0.5)] 0.23545 0.30902 0 2 Mshowa
gsave
0.002 setlinewidth
0.36772 0.30902 moveto
0.36772 0.31527 lineto
stroke
grestore
[(-0.25)] 0.36772 0.30902 0 2 Mshowa
gsave
0.002 setlinewidth
0.63228 0.30902 moveto
0.63228 0.31527 lineto
stroke
grestore
[(0.25)] 0.63228 0.30902 0 2 Mshowa
gsave
0.002 setlinewidth
0.76455 0.30902 moveto
0.76455 0.31527 lineto
stroke
grestore
[(0.5)] 0.76455 0.30902 0 2 Mshowa
gsave
0.002 setlinewidth
0.89683 0.30902 moveto
0.89683 0.31527 lineto
stroke
grestore
[(0.75)] 0.89683 0.30902 0 2 Mshowa
gsave
0.001 setlinewidth
0.12963 0.30902 moveto
0.12963 0.31277 lineto
stroke
grestore
gsave
0.001 setlinewidth
0.15608 0.30902 moveto
0.15608 0.31277 lineto
stroke
grestore
gsave
0.001 setlinewidth
0.18254 0.30902 moveto
0.18254 0.31277 lineto
stroke
grestore
gsave
0.001 setlinewidth
0.20899 0.30902 moveto
0.20899 0.31277 lineto
stroke
grestore
gsave
0.001 setlinewidth
0.2619 0.30902 moveto
0.2619 0.31277 lineto
stroke
grestore
gsave
0.001 setlinewidth
0.28836 0.30902 moveto
0.28836 0.31277 lineto
stroke
grestore
gsave
0.001 setlinewidth
0.31481 0.30902 moveto
0.31481 0.31277 lineto
stroke
grestore
gsave
0.001 setlinewidth
0.34127 0.30902 moveto
0.34127 0.31277 lineto
stroke
grestore
gsave
0.001 setlinewidth
0.39418 0.30902 moveto
0.39418 0.31277 lineto
stroke
grestore
gsave
0.001 setlinewidth
0.42063 0.30902 moveto
0.42063 0.31277 lineto
stroke
grestore
gsave
0.001 setlinewidth
0.44709 0.30902 moveto
0.44709 0.31277 lineto
stroke
grestore
gsave
0.001 setlinewidth
0.47354 0.30902 moveto
0.47354 0.31277 lineto
stroke
grestore
gsave
0.001 setlinewidth
0.52646 0.30902 moveto
0.52646 0.31277 lineto
stroke
grestore
gsave
0.001 setlinewidth
0.55291 0.30902 moveto
0.55291 0.31277 lineto
stroke
grestore
gsave
0.001 setlinewidth
0.57937 0.30902 moveto
0.57937 0.31277 lineto
stroke
grestore
gsave
0.001 setlinewidth
0.60582 0.30902 moveto
0.60582 0.31277 lineto
stroke
grestore
gsave
0.001 setlinewidth
0.65873 0.30902 moveto
0.65873 0.31277 lineto
stroke
grestore
gsave
0.001 setlinewidth
0.68519 0.30902 moveto
0.68519 0.31277 lineto
stroke
grestore
gsave
0.001 setlinewidth
0.71164 0.30902 moveto
0.71164 0.31277 lineto
stroke
grestore
gsave
0.001 setlinewidth
0.7381 0.30902 moveto
0.7381 0.31277 lineto
stroke
grestore
gsave
0.001 setlinewidth
0.79101 0.30902 moveto
0.79101 0.31277 lineto
stroke
grestore
gsave
0.001 setlinewidth
0.81746 0.30902 moveto
0.81746 0.31277 lineto
stroke
grestore
gsave
0.001 setlinewidth
0.84392 0.30902 moveto
0.84392 0.31277 lineto
stroke
grestore
gsave
0.001 setlinewidth
0.87037 0.30902 moveto
0.87037 0.31277 lineto
stroke
grestore
gsave
0.001 setlinewidth
0.07672 0.30902 moveto
0.07672 0.31277 lineto
stroke
grestore
gsave
0.001 setlinewidth
0.05026 0.30902 moveto
0.05026 0.31277 lineto
stroke
grestore
gsave
0.001 setlinewidth
0.02381 0.30902 moveto
0.02381 0.31277 lineto
stroke
grestore
gsave
0.001 setlinewidth
0.92328 0.30902 moveto
0.92328 0.31277 lineto
stroke
grestore
gsave
0.001 setlinewidth
0.94974 0.30902 moveto
0.94974 0.31277 lineto
stroke
grestore
gsave
0.001 setlinewidth
0.97619 0.30902 moveto
0.97619 0.31277 lineto
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
0.5 0.01074 moveto
0.50625 0.01074 lineto
stroke
grestore
[(-1)] 0.4875 0.01074 1 0 Mshowa
gsave
0.002 setlinewidth
0.5 0.15988 moveto
0.50625 0.15988 lineto
stroke
grestore
[(-0.5)] 0.4875 0.15988 1 0 Mshowa
gsave
0.002 setlinewidth
0.5 0.45816 moveto
0.50625 0.45816 lineto
stroke
grestore
[(0.5)] 0.4875 0.45816 1 0 Mshowa
gsave
0.002 setlinewidth
0.5 0.60729 moveto
0.50625 0.60729 lineto
stroke
grestore
[(1)] 0.4875 0.60729 1 0 Mshowa
gsave
0.001 setlinewidth
0.5 0.04057 moveto
0.50375 0.04057 lineto
stroke
grestore
gsave
0.001 setlinewidth
0.5 0.0704 moveto
0.50375 0.0704 lineto
stroke
grestore
gsave
0.001 setlinewidth
0.5 0.10022 moveto
0.50375 0.10022 lineto
stroke
grestore
gsave
0.001 setlinewidth
0.5 0.13005 moveto
0.50375 0.13005 lineto
stroke
grestore
gsave
0.001 setlinewidth
0.5 0.18971 moveto
0.50375 0.18971 lineto
stroke
grestore
gsave
0.001 setlinewidth
0.5 0.21953 moveto
0.50375 0.21953 lineto
stroke
grestore
gsave
0.001 setlinewidth
0.5 0.24936 moveto
0.50375 0.24936 lineto
stroke
grestore
gsave
0.001 setlinewidth
0.5 0.27919 moveto
0.50375 0.27919 lineto
stroke
grestore
gsave
0.001 setlinewidth
0.5 0.33884 moveto
0.50375 0.33884 lineto
stroke
grestore
gsave
0.001 setlinewidth
0.5 0.36867 moveto
0.50375 0.36867 lineto
stroke
grestore
gsave
0.001 setlinewidth
0.5 0.3985 moveto
0.50375 0.3985 lineto
stroke
grestore
gsave
0.001 setlinewidth
0.5 0.42833 moveto
0.50375 0.42833 lineto
stroke
grestore
gsave
0.001 setlinewidth
0.5 0.48798 moveto
0.50375 0.48798 lineto
stroke
grestore
gsave
0.001 setlinewidth
0.5 0.51781 moveto
0.50375 0.51781 lineto
stroke
grestore
gsave
0.001 setlinewidth
0.5 0.54764 moveto
0.50375 0.54764 lineto
stroke
grestore
gsave
0.001 setlinewidth
0.5 0.57747 moveto
0.50375 0.57747 lineto
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
0.06349 0.04847 lineto
0.10317 0.07708 lineto
0.14286 0.10309 lineto
0.18254 0.1276 lineto
0.22222 0.15119 lineto
0.2619 0.17423 lineto
0.30159 0.19694 lineto
0.34127 0.21946 lineto
0.38095 0.24189 lineto
0.42063 0.26427 lineto
0.46032 0.28665 lineto
0.5 0.30902 lineto
0.53968 0.33139 lineto
0.57937 0.35376 lineto
0.61905 0.37615 lineto
0.65873 0.39857 lineto
0.69841 0.42109 lineto
0.7381 0.4438 lineto
0.77778 0.46684 lineto
0.81746 0.49044 lineto
0.85714 0.51494 lineto
0.89683 0.54095 lineto
0.93651 0.56956 lineto
0.97619 0.60332 lineto
stroke
grestore
grestore
% End of Graphics
MathPictureEnd
:[font = output; output; inactive; preserveAspect; endGroup; ]
The Unformatted text for this cell was not generated.
Use options in the Actions Settings dialog box to control
when Unformatted text is generated.
;[o]
-Graphics-
:[font = input; preserveAspect; startGroup; ]
Plot[{SuperArcSin[x], ArcSin[x]}, {x, -0.9, 0.9},
	PlotStyle -> GrayLevels[2]
]
:[font = postscript; PostScript; formatAsPostScript; output; inactive; preserveAspect; pictureLeft = 34; pictureWidth = 282; pictureHeight = 174; ]
%!
%%Creator: Mathematica
%%AspectRatio: 0.61803 
MathPictureStart
/Courier findfont 10  scalefont  setfont
% Scaling calculations
0.5 0.529101 0.309017 0.262824 [
[(-0.75)] 0.10317 0.30902 0 2 Msboxa
[(-0.5)] 0.23545 0.30902 0 2 Msboxa
[(-0.25)] 0.36772 0.30902 0 2 Msboxa
[(0.25)] 0.63228 0.30902 0 2 Msboxa
[(0.5)] 0.76455 0.30902 0 2 Msboxa
[(0.75)] 0.89683 0.30902 0 2 Msboxa
[(-1)] 0.4875 0.04619 1 0 Msboxa
[(-0.5)] 0.4875 0.17761 1 0 Msboxa
[(0.5)] 0.4875 0.44043 1 0 Msboxa
[(1)] 0.4875 0.57184 1 0 Msboxa
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
0.10317 0.30902 moveto
0.10317 0.31527 lineto
stroke
grestore
[(-0.75)] 0.10317 0.30902 0 2 Mshowa
gsave
0.002 setlinewidth
0.23545 0.30902 moveto
0.23545 0.31527 lineto
stroke
grestore
[(-0.5)] 0.23545 0.30902 0 2 Mshowa
gsave
0.002 setlinewidth
0.36772 0.30902 moveto
0.36772 0.31527 lineto
stroke
grestore
[(-0.25)] 0.36772 0.30902 0 2 Mshowa
gsave
0.002 setlinewidth
0.63228 0.30902 moveto
0.63228 0.31527 lineto
stroke
grestore
[(0.25)] 0.63228 0.30902 0 2 Mshowa
gsave
0.002 setlinewidth
0.76455 0.30902 moveto
0.76455 0.31527 lineto
stroke
grestore
[(0.5)] 0.76455 0.30902 0 2 Mshowa
gsave
0.002 setlinewidth
0.89683 0.30902 moveto
0.89683 0.31527 lineto
stroke
grestore
[(0.75)] 0.89683 0.30902 0 2 Mshowa
gsave
0.001 setlinewidth
0.12963 0.30902 moveto
0.12963 0.31277 lineto
stroke
grestore
gsave
0.001 setlinewidth
0.15608 0.30902 moveto
0.15608 0.31277 lineto
stroke
grestore
gsave
0.001 setlinewidth
0.18254 0.30902 moveto
0.18254 0.31277 lineto
stroke
grestore
gsave
0.001 setlinewidth
0.20899 0.30902 moveto
0.20899 0.31277 lineto
stroke
grestore
gsave
0.001 setlinewidth
0.2619 0.30902 moveto
0.2619 0.31277 lineto
stroke
grestore
gsave
0.001 setlinewidth
0.28836 0.30902 moveto
0.28836 0.31277 lineto
stroke
grestore
gsave
0.001 setlinewidth
0.31481 0.30902 moveto
0.31481 0.31277 lineto
stroke
grestore
gsave
0.001 setlinewidth
0.34127 0.30902 moveto
0.34127 0.31277 lineto
stroke
grestore
gsave
0.001 setlinewidth
0.39418 0.30902 moveto
0.39418 0.31277 lineto
stroke
grestore
gsave
0.001 setlinewidth
0.42063 0.30902 moveto
0.42063 0.31277 lineto
stroke
grestore
gsave
0.001 setlinewidth
0.44709 0.30902 moveto
0.44709 0.31277 lineto
stroke
grestore
gsave
0.001 setlinewidth
0.47354 0.30902 moveto
0.47354 0.31277 lineto
stroke
grestore
gsave
0.001 setlinewidth
0.52646 0.30902 moveto
0.52646 0.31277 lineto
stroke
grestore
gsave
0.001 setlinewidth
0.55291 0.30902 moveto
0.55291 0.31277 lineto
stroke
grestore
gsave
0.001 setlinewidth
0.57937 0.30902 moveto
0.57937 0.31277 lineto
stroke
grestore
gsave
0.001 setlinewidth
0.60582 0.30902 moveto
0.60582 0.31277 lineto
stroke
grestore
gsave
0.001 setlinewidth
0.65873 0.30902 moveto
0.65873 0.31277 lineto
stroke
grestore
gsave
0.001 setlinewidth
0.68519 0.30902 moveto
0.68519 0.31277 lineto
stroke
grestore
gsave
0.001 setlinewidth
0.71164 0.30902 moveto
0.71164 0.31277 lineto
stroke
grestore
gsave
0.001 setlinewidth
0.7381 0.30902 moveto
0.7381 0.31277 lineto
stroke
grestore
gsave
0.001 setlinewidth
0.79101 0.30902 moveto
0.79101 0.31277 lineto
stroke
grestore
gsave
0.001 setlinewidth
0.81746 0.30902 moveto
0.81746 0.31277 lineto
stroke
grestore
gsave
0.001 setlinewidth
0.84392 0.30902 moveto
0.84392 0.31277 lineto
stroke
grestore
gsave
0.001 setlinewidth
0.87037 0.30902 moveto
0.87037 0.31277 lineto
stroke
grestore
gsave
0.001 setlinewidth
0.07672 0.30902 moveto
0.07672 0.31277 lineto
stroke
grestore
gsave
0.001 setlinewidth
0.05026 0.30902 moveto
0.05026 0.31277 lineto
stroke
grestore
gsave
0.001 setlinewidth
0.02381 0.30902 moveto
0.02381 0.31277 lineto
stroke
grestore
gsave
0.001 setlinewidth
0.92328 0.30902 moveto
0.92328 0.31277 lineto
stroke
grestore
gsave
0.001 setlinewidth
0.94974 0.30902 moveto
0.94974 0.31277 lineto
stroke
grestore
gsave
0.001 setlinewidth
0.97619 0.30902 moveto
0.97619 0.31277 lineto
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
0.5 0.04619 moveto
0.50625 0.04619 lineto
stroke
grestore
[(-1)] 0.4875 0.04619 1 0 Mshowa
gsave
0.002 setlinewidth
0.5 0.17761 moveto
0.50625 0.17761 lineto
stroke
grestore
[(-0.5)] 0.4875 0.17761 1 0 Mshowa
gsave
0.002 setlinewidth
0.5 0.44043 moveto
0.50625 0.44043 lineto
stroke
grestore
[(0.5)] 0.4875 0.44043 1 0 Mshowa
gsave
0.002 setlinewidth
0.5 0.57184 moveto
0.50625 0.57184 lineto
stroke
grestore
[(1)] 0.4875 0.57184 1 0 Mshowa
gsave
0.001 setlinewidth
0.5 0.07248 moveto
0.50375 0.07248 lineto
stroke
grestore
gsave
0.001 setlinewidth
0.5 0.09876 moveto
0.50375 0.09876 lineto
stroke
grestore
gsave
0.001 setlinewidth
0.5 0.12504 moveto
0.50375 0.12504 lineto
stroke
grestore
gsave
0.001 setlinewidth
0.5 0.15132 moveto
0.50375 0.15132 lineto
stroke
grestore
gsave
0.001 setlinewidth
0.5 0.20389 moveto
0.50375 0.20389 lineto
stroke
grestore
gsave
0.001 setlinewidth
0.5 0.23017 moveto
0.50375 0.23017 lineto
stroke
grestore
gsave
0.001 setlinewidth
0.5 0.25645 moveto
0.50375 0.25645 lineto
stroke
grestore
gsave
0.001 setlinewidth
0.5 0.28273 moveto
0.50375 0.28273 lineto
stroke
grestore
gsave
0.001 setlinewidth
0.5 0.3353 moveto
0.50375 0.3353 lineto
stroke
grestore
gsave
0.001 setlinewidth
0.5 0.36158 moveto
0.50375 0.36158 lineto
stroke
grestore
gsave
0.001 setlinewidth
0.5 0.38786 moveto
0.50375 0.38786 lineto
stroke
grestore
gsave
0.001 setlinewidth
0.5 0.41415 moveto
0.50375 0.41415 lineto
stroke
grestore
gsave
0.001 setlinewidth
0.5 0.46671 moveto
0.50375 0.46671 lineto
stroke
grestore
gsave
0.001 setlinewidth
0.5 0.49299 moveto
0.50375 0.49299 lineto
stroke
grestore
gsave
0.001 setlinewidth
0.5 0.51928 moveto
0.50375 0.51928 lineto
stroke
grestore
gsave
0.001 setlinewidth
0.5 0.54556 moveto
0.50375 0.54556 lineto
stroke
grestore
gsave
0.001 setlinewidth
0.5 0.01991 moveto
0.50375 0.01991 lineto
stroke
grestore
gsave
0.001 setlinewidth
0.5 0.59812 moveto
0.50375 0.59812 lineto
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
gsave
0.004 setlinewidth
0.02381 0.0497 moveto
0.06349 0.07944 lineto
0.10317 0.10465 lineto
0.14286 0.12757 lineto
0.18254 0.14916 lineto
0.22222 0.16995 lineto
0.2619 0.19025 lineto
0.30159 0.21026 lineto
0.34127 0.23011 lineto
0.38095 0.24987 lineto
0.42063 0.26959 lineto
0.46032 0.28931 lineto
0.5 0.30902 lineto
0.53968 0.32873 lineto
0.57937 0.34844 lineto
0.61905 0.36817 lineto
0.65873 0.38793 lineto
0.69841 0.40777 lineto
0.7381 0.42778 lineto
0.77778 0.44808 lineto
0.81746 0.46887 lineto
0.85714 0.49047 lineto
0.89683 0.51339 lineto
0.93651 0.53859 lineto
0.97619 0.56834 lineto
stroke
grestore
grestore
gsave
0.5 setgray
gsave
0.004 setlinewidth
0.02381 0.01472 moveto
0.06349 0.05402 lineto
0.10317 0.08613 lineto
0.14286 0.11427 lineto
0.18254 0.13989 lineto
0.22222 0.16375 lineto
0.2619 0.18634 lineto
0.30159 0.20799 lineto
0.34127 0.22894 lineto
0.38095 0.24937 lineto
0.42063 0.26944 lineto
0.46032 0.28929 lineto
0.5 0.30902 lineto
0.53968 0.32875 lineto
0.57937 0.34859 lineto
0.61905 0.36866 lineto
0.65873 0.3891 lineto
0.69841 0.41005 lineto
0.7381 0.43169 lineto
0.77778 0.45428 lineto
0.81746 0.47814 lineto
0.85714 0.50376 lineto
0.89683 0.53191 lineto
0.93651 0.56401 lineto
0.97619 0.60332 lineto
stroke
grestore
grestore
grestore
% End of Graphics
MathPictureEnd
:[font = output; output; inactive; preserveAspect; endGroup; ]
The Unformatted text for this cell was not generated.
Use options in the Actions Settings dialog box to control
when Unformatted text is generated.
;[o]
-Graphics-
:[font = input; preserveAspect; ]
F1[x_] := Integrate[1/Sqrt[1 - t^4], {t, 0, x}]
F2[x_] = Integrate[1/Sqrt[1 - t^4], {t, 0, x}];
:[font = input; preserveAspect; startGroup; ]
Plot[F1[x], {x, -0.9, 0.9},
	DisplayFunction -> Identity
] //Timing
:[font = output; output; inactive; preserveAspect; endGroup; ]
{46.51666666666667*Second, 
  Graphics[{{Line[{{-0.9, -0.986675704681513}, 
       {-0.825, -0.873502042984858}, 
       {-0.75, -0.7775879872398534}, 
       {-0.675, -0.6903936223683836}, 
       {-0.6000000000000001, -0.608230308185614}, 
       {-0.525, -0.529120458315655}, 
       {-0.4500000000000001, -0.4518775765876363}, 
       {-0.375, -0.3757477582046911}, 
       {-0.3, -0.3002438239784022}, 
       {-0.225, -0.2250577267090971}, 
       {-0.15, -0.1500075953522747}, 
       {-0.07500000000000006, 
        -0.07500023730781614}, 
       {-(5.551115123125783*10^-17), 
        -(5.551115123125783*10^-17)}, 
       {0.07499999999999994, 0.07500023730781602}, 
       {0.1499999999999999, 0.1500075953522746}, 
       {0.2249999999999999, 0.225057726709097}, 
       {0.2999999999999999, 0.300243823978402}, 
       {0.375, 0.3757477582046909}, 
       {0.45, 0.4518775765876362}, 
       {0.525, 0.5291204583156548}, 
       {0.5999999999999999, 0.6082303081856138}, 
       {0.675, 0.6903936223683834}, 
       {0.75, 0.7775879872398532}, 
       {0.825, 0.873502042984858}, 
       {0.9, 0.986675704681513}}]}}, 
   {PlotRange -> Automatic, 
    AspectRatio -> GoldenRatio^(-1), 
    DisplayFunction :> Identity, 
    ColorOutput -> Automatic, Axes -> Automatic, 
    AxesOrigin -> Automatic, PlotLabel -> None, 
    AxesLabel -> None, Ticks -> Automatic, 
    GridLines -> None, Prolog -> {}, Epilog -> {}, 
    AxesStyle -> Automatic, Background -> Automatic, 
    DefaultColor -> Automatic, 
    DefaultFont :> $DefaultFont, 
    RotateLabel -> True, Frame -> False, 
    FrameStyle -> Automatic, 
    FrameTicks -> Automatic, FrameLabel -> None, 
    PlotRegion -> Automatic}]}
;[o]
{46.5167 Second, -Graphics-}
:[font = input; preserveAspect; startGroup; ]
Plot[Evaluate@F1[x], {x, -0.9, 0.9},
	DisplayFunction -> Identity
] //Timing
:[font = output; output; inactive; preserveAspect; endGroup; ]
{12.1*Second, Graphics[{{Line[{{-0.9, 
        -0.98667570468156}, 
       {-0.825, -0.873502042984868}, 
       {-0.75, -0.777587987239858}, 
       {-0.675, -0.6903936223683875}, 
       {-0.6000000000000001, -0.6082303081856146}, 
       {-0.525, -0.529120458315656}, 
       {-0.4500000000000001, -0.4518775765876365}, 
       {-0.375, -0.3757477582046912}, 
       {-0.3, -0.3002438239784022}, 
       {-0.225, -0.2250577267090972}, 
       {-0.15, -0.1500075953522747}, 
       {-0.07500000000000006, 
        -0.07500023730781614}, 
       {-(5.551115123125783*10^-17), 
        -(5.551115123125783*10^-17)}, 
       {0.07499999999999994, 0.07500023730781602}, 
       {0.1499999999999999, 0.1500075953522746}, 
       {0.2249999999999999, 0.2250577267090971}, 
       {0.2999999999999999, 0.3002438239784021}, 
       {0.375, 0.375747758204691}, 
       {0.45, 0.4518775765876364}, 
       {0.525, 0.5291204583156548}, 
       {0.5999999999999999, 0.6082303081856143}, 
       {0.675, 0.6903936223683874}, 
       {0.75, 0.7775879872398577}, 
       {0.825, 0.873502042984867}, 
       {0.9, 0.98667570468156}}]}}, 
   {PlotRange -> Automatic, 
    AspectRatio -> GoldenRatio^(-1), 
    DisplayFunction :> Identity, 
    ColorOutput -> Automatic, Axes -> Automatic, 
    AxesOrigin -> Automatic, PlotLabel -> None, 
    AxesLabel -> None, Ticks -> Automatic, 
    GridLines -> None, Prolog -> {}, Epilog -> {}, 
    AxesStyle -> Automatic, Background -> Automatic, 
    DefaultColor -> Automatic, 
    DefaultFont :> $DefaultFont, 
    RotateLabel -> True, Frame -> False, 
    FrameStyle -> Automatic, 
    FrameTicks -> Automatic, FrameLabel -> None, 
    PlotRegion -> Automatic}]}
;[o]
{12.1 Second, -Graphics-}
:[font = input; preserveAspect; startGroup; ]
Plot[F2[x], {x, -0.9, 0.9},
	DisplayFunction -> Identity
] //Timing
:[font = output; output; inactive; preserveAspect; endGroup; endGroup; ]
{0.5333333333333333*Second, 
  Graphics[{{Line[{{-0.9, -0.98667570468156}, 
       {-0.825, -0.873502042984868}, 
       {-0.75, -0.777587987239858}, 
       {-0.675, -0.6903936223683875}, 
       {-0.6000000000000001, -0.6082303081856146}, 
       {-0.525, -0.529120458315656}, 
       {-0.4500000000000001, -0.4518775765876365}, 
       {-0.375, -0.3757477582046912}, 
       {-0.3, -0.3002438239784022}, 
       {-0.225, -0.2250577267090972}, 
       {-0.15, -0.1500075953522747}, 
       {-0.07500000000000006, 
        -0.07500023730781614}, 
       {-(5.551115123125783*10^-17), 
        -(5.551115123125783*10^-17)}, 
       {0.07499999999999994, 0.07500023730781602}, 
       {0.1499999999999999, 0.1500075953522746}, 
       {0.2249999999999999, 0.2250577267090971}, 
       {0.2999999999999999, 0.3002438239784021}, 
       {0.375, 0.375747758204691}, 
       {0.45, 0.4518775765876364}, 
       {0.525, 0.5291204583156548}, 
       {0.5999999999999999, 0.6082303081856143}, 
       {0.675, 0.6903936223683874}, 
       {0.75, 0.7775879872398577}, 
       {0.825, 0.873502042984867}, 
       {0.9, 0.98667570468156}}]}}, 
   {PlotRange -> Automatic, 
    AspectRatio -> GoldenRatio^(-1), 
    DisplayFunction :> Identity, 
    ColorOutput -> Automatic, Axes -> Automatic, 
    AxesOrigin -> Automatic, PlotLabel -> None, 
    AxesLabel -> None, Ticks -> Automatic, 
    GridLines -> None, Prolog -> {}, Epilog -> {}, 
    AxesStyle -> Automatic, Background -> Automatic, 
    DefaultColor -> Automatic, 
    DefaultFont :> $DefaultFont, 
    RotateLabel -> True, Frame -> False, 
    FrameStyle -> Automatic, 
    Fr`meTicks -> Automatic, FrameLabel -> None, 
    PlotRegion -> Automatic}]}
;[o]
{0.533333 Second, -Graphics-}
:[font = subsubsection; inactive; Cclosed; preserveAspect; startGroup; ]
Supersin
:[font = input; preserveAspect; ]
SuperSin[x_] := Integrate[Sin[Sin[t]], {t, 0, x}]
:[font = input; preserveAspect; startGroup; ]
Plot[Evaluate@SuperSin[x], {x, -3Pi, 3Pi}] //Timing
:[font = message; inactive; preserveAspect; ]
NIntegrate::ploss: 
   Numerical integration stopping due to loss of
     precision. Achieved neither the requested
     PrecisionGoal nor AccuracyGoal; suspect one of
     the following: highly oscillatory integrand or
     the true value of the integral is 0.
:[font = message; inactive; preserveAspect; ]
                                 1
Power::infy: Infinite expression -- encountered.
                                 0.
:[font = message; inactive; preserveAspect; ]
NIntegrate::ploss: 
   Numerical integration stopping due to loss of
     precision. Achieved neither the requested
     PrecisionGoal nor AccuracyGoal; suspect one of
     the following: highly oscillatory integrand or
     the true value of the integral is 0.
:[font = postscript; PostScript; formatAsPostScript; output; inactive; preserveAspect; pictureLeft = 34; pictureWidth = 282; pictureHeight = 174; ]
%!
%%Creator: Mathematica
%%AspectRatio: 0.61803 
MathPictureStart
/Courier findfont 10  scalefont  setfont
% Scaling calculations
0.5 0.050525 0.014715 0.329475 [
[(-7.5)] 0.12106 0.01472 0 2 Msboxa
[(-5)] 0.24737 0.01472 0 2 Msboxa
[(-2.5)] 0.37369 0.01472 0 2 Msboxa
[(2.5)] 0.62631 0.01472 0 2 Msboxa
[(5)] 0.75263 0.01472 0 2 Msboxa
[(7.5)] 0.87894 0.01472 0 2 Msboxa
[(0.25)] 0.4875 0.09708 1 0 Msboxa
[(0.5)] 0.4875 0.17945 1 0 Msboxa
[(0.75)] 0.4875 0.26182 1 0 Msboxa
[(1)] 0.4875 0.34419 1 0 Msboxa
[(1.25)] 0.4875 0.42656 1 0 Msboxa
[(1.5)] 0.4875 0.50893 1 0 Msboxa
[(1.75)] 0.4875 0.5913 1 0 Msboxa
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
0.12106 0.01472 moveto
0.12106 0.02097 lineto
stroke
grestore
[(-7.5)] 0.12106 0.01472 0 2 Mshowa
gsave
0.002 setlinewidth
0.24737 0.01472 moveto
0.24737 0.02097 lineto
stroke
grestore
[(-5)] 0.24737 0.01472 0 2 Mshowa
gsave
0.002 setlinewidth
0.37369 0.01472 moveto
0.37369 0.02097 lineto
stroke
grestore
[(-2.5)] 0.37369 0.01472 0 2 Mshowa
gsave
0.002 setlinewidth
0.62631 0.01472 moveto
0.62631 0.02097 lineto
stroke
grestore
[(2.5)] 0.62631 0.01472 0 2 Mshowa
gsave
0.002 setlinewidth
0.75263 0.01472 moveto
0.75263 0.02097 lineto
stroke
grestore
[(5)] 0.75263 0.01472 0 2 Mshowa
gsave
0.002 setlinewidth
0.87894 0.01472 moveto
0.87894 0.02097 lineto
stroke
grestore
[(7.5)] 0.87894 0.01472 0 2 Mshowa
gsave
0.001 setlinewidth
0.02001 0.01472 moveto
0.02001 0.01847 lineto
stroke
grestore
gsave
0.001 setlinewidth
0.04527 0.01472 moveto
0.04527 0.01847 lineto
stroke
grestore
gsave
0.001 setlinewidth
0.07053 0.01472 moveto
0.07053 0.01847 lineto
stroke
grestore
gsave
0.001 setlinewidth
0.0958 0.01472 moveto
0.0958 0.01847 lineto
stroke
grestore
gsave
0.001 setlinewidth
0.14632 0.01472 moveto
0.14632 0.01847 lineto
stroke
grestore
gsave
0.001 setlinewidth
0.17159 0.01472 moveto
0.17159 0.01847 lineto
stroke
grestore
gsave
0.001 setlinewidth
0.19685 0.01472 moveto
0.19685 0.01847 lineto
stroke
grestore
gsave
0.001 setlinewidth
0.22211 0.01472 moveto
0.22211 0.01847 lineto
stroke
grestore
gsave
0.001 setlinewidth
0.27264 0.01472 moveto
0.27264 0.01847 lineto
stroke
grestore
gsave
0.001 setlinewidth
0.2979 0.01472 moveto
0.2979 0.01847 lineto
stroke
grestore
gsave
0.001 setlinewidth
0.32316 0.01472 moveto
0.32316 0.01847 lineto
stroke
grestore
gsave
0.001 setlinewidth
0.34842 0.01472 moveto
0.34842 0.01847 lineto
stroke
grestore
gsave
0.001 setlinewidth
0.39895 0.01472 moveto
0.39895 0.01847 lineto
stroke
grestore
gsave
0.001 setlinewidth
0.42421 0.01472 moveto
0.42421 0.01847 lineto
stroke
grestore
gsave
0.001 setlinewidth
0.44947 0.01472 moveto
0.44947 0.01847 lineto
stroke
grestore
gsave
0.001 setlinewidth
0.47474 0.01472 moveto
0.47474 0.01847 lineto
stroke
grestore
gsave
0.001 setlinewidth
0.52526 0.01472 moveto
0.52526 0.01847 lineto
stroke
grestore
gsave
0.001 setlinewidth
0.55053 0.01472 moveto
0.55053 0.01847 lineto
stroke
grestore
gsave
0.001 setlinewidth
0.57579 0.01472 moveto
0.57579 0.01847 lineto
stroke
grestore
gsave
0.001 setlinewidth
0.60105 0.01472 moveto
0.60105 0.01847 lineto
stroke
grestore
gsave
0.001 setlinewidth
0.65158 0.01472 moveto
0.65158 0.01847 lineto
stroke
grestore
gsave
0.001 setlinewidth
0.67684 0.01472 moveto
0.67684 0.01847 lineto
stroke
grestore
gsave
0.001 setlinewidth
0.7021 0.01472 moveto
0.7021 0.01847 lineto
stroke
grestore
gsave
0.001 setlinewidth
0.72736 0.01472 moveto
0.72736 0.01847 lineto
stroke
grestore
gsave
0.001 setlinewidth
0.77789 0.01472 moveto
0.77789 0.01847 lineto
stroke
grestore
gsave
0.001 setlinewidth
0.80315 0.01472 moveto
0.80315 0.01847 lineto
stroke
grestore
gsave
0.001 setlinewidth
0.82841 0.01472 moveto
0.82841 0.01847 lineto
stroke
grestore
gsave
0.001 setlinewidth
0.85368 0.01472 moveto
0.85368 0.01847 lineto
stroke
grestore
gsave
0.001 setlinewidth
0.9042 0.01472 moveto
0.9042 0.01847 lineto
stroke
grestore
gsave
0.001 setlinewidth
0.92947 0.01472 moveto
0.92947 0.01847 lineto
stroke
grestore
gsave
0.001 setlinewidth
0.95473 0.01472 moveto
0.95473 0.01847 lineto
stroke
grestore
gsave
0.001 setlinewidth
0.97999 0.01472 moveto
0.97999 0.01847 lineto
stroke
grestore
gsave
0.002 setlinewidth
0 0.01472 moveto
1 0.01472 lineto
stroke
grestore
gsave
0.002 setlinewidth
0.5 0.09708 moveto
0.50625 0.09708 lineto
stroke
grestore
[(0.25)] 0.4875 0.09708 1 0 Mshowa
gsave
0.002 setlinewidth
0.5 0.17945 moveto
0.50625 0.17945 lineto
stroke
grestore
[(0.5)] 0.4875 0.17945 1 0 Mshowa
gsave
0.002 setlinewidth
0.5 0.26182 moveto
0.50625 0.26182 lineto
stroke
grestore
[(0.75)] 0.4875 0.26182 1 0 Mshowa
gsave
0.002 setlinewidth
0.5 0.34419 moveto
0.50625 0.34419 lineto
stroke
grestore
[(1)] 0.4875 0.34419 1 0 Mshowa
gsave
0.002 setlinewidth
0.5 0.42656 moveto
0.50625 0.42656 lineto
stroke
grestore
[(1.25)] 0.4875 0.42656 1 0 Mshowa
gsave
0.002 setlinewidth
0.5 0.50893 moveto
0.50625 0.50893 lineto
stroke
grestore
[(1.5)] 0.4875 0.50893 1 0 Mshowa
gsave
0.002 setlinewidth
0.5 0.5913 moveto
0.50625 0.5913 lineto
stroke
grestore
[(1.75)] 0.4875 0.5913 1 0 Mshowa
gsave
0.001 setlinewidth
0.5 0.03119 moveto
0.50375 0.03119 lineto
stroke
grestore
gsave
0.001 setlinewidth
0.5 0.04766 moveto
0.50375 0.04766 lineto
stroke
grestore
gsave
0.001 setlinewidth
0.5 0.06414 moveto
0.50375 0.06414 lineto
stroke
grestore
gsave
0.001 setlinewidth
0.5 0.08061 moveto
0.50375 0.08061 lineto
stroke
grestore
gsave
0.001 setlinewidth
0.5 0.11356 moveto
0.50375 0.11356 lineto
stroke
grestore
gsave
0.001 setlinewidth
0.5 0.13003 moveto
0.50375 0.13003 lineto
stroke
grestore
gsave
0.001 setlinewidth
0.5 0.14651 moveto
0.50375 0.14651 lineto
stroke
grestore
gsave
0.001 setlinewidth
0.5 0.16298 moveto
0.50375 0.16298 lineto
stroke
grestore
gsave
0.001 setlinewidth
0.5 0.19593 moveto
0.50375 0.19593 lineto
stroke
grestore
gsave
0.001 setlinewidth
0.5 0.2124 moveto
0.50375 0.2124 lineto
stroke
grestore
gsave
0.001 setlinewidth
0.5 0.22887 moveto
0.50375 0.22887 lineto
stroke
grestore
gsave
0.001 setlinewidth
0.5 0.24535 moveto
0.50375 0.24535 lineto
stroke
grestore
gsave
0.001 setlinewidth
0.5 0.2783 moveto
0.50375 0.2783 lineto
stroke
grestore
gsave
0.001 setlinewidth
0.5 0.29477 moveto
0.50375 0.29477 lineto
stroke
grestore
gsave
0.001 setlinewidth
0.5 0.31124 moveto
0.50375 0.31124 lineto
stroke
grestore
gsave
0.001 setlinewidth
0.5 0.32772 moveto
0.50375 0.32772 lineto
stroke
grestore
gsave
0.001 setlinewidth
0.5 0.36066 moveto
0.50375 0.36066 lineto
stroke
grestore
gsave
0.001 setlinewidth
0.5 0.37714 moveto
0.50375 0.37714 lineto
stroke
grestore
gsave
0.001 setlinewidth
0.5 0.39361 moveto
0.50375 0.39361 lineto
stroke
grestore
gsave
0.001 setlinewidth
0.5 0.41009 moveto
0.50375 0.41009 lineto
stroke
grestore
gsave
0.001 setlinewidth
0.5 0.44303 moveto
0.50375 0.44303 lineto
stroke
grestore
gsave
0.001 setlinewidth
0.5 0.45951 moveto
0.50375 0.45951 lineto
stroke
grestore
gsave
0.001 setlinewidth
0.5 0.47598 moveto
0.50375 0.47598 lineto
stroke
grestore
gsave
0.001 setlinewidth
0.5 0.49245 moveto
0.50375 0.49245 lineto
stroke
grestore
gsave
0.001 setlinewidth
0.5 0.5254 moveto
0.50375 0.5254 lineto
stroke
grestore
gsave
0.001 setlinewidth
0.5 0.54188 moveto
0.50375 0.54188 lineto
stroke
grestore
gsave
0.001 setlinewidth
0.5 0.55835 moveto
0.50375 0.55835 lineto
stroke
grestore
gsave
0.001 setlinewidth
0.5 0.57482 moveto
0.50375 0.57482 lineto
stroke
grestore
gsave
0.001 setlinewidth
0.5 0.60777 moveto
0.50375 0.60777 lineto
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
0.02381 0.60332 moveto
0.02505 0.60322 lineto
0.02629 0.60292 lineto
0.02753 0.60243 lineto
0.02877 0.60173 lineto
0.03125 0.59976 lineto
0.03373 0.59701 lineto
0.03869 0.58923 lineto
0.04365 0.57855 lineto
0.05357 0.54925 lineto
0.06349 0.511 lineto
0.10317 0.30902 lineto
0.12302 0.20199 lineto
0.14286 0.10704 lineto
0.15278 0.06878 lineto
0.1627 0.03949 lineto
0.16766 0.0288 lineto
0.17014 0.02454 lineto
0.17262 0.02103 lineto
0.1751 0.01827 lineto
0.17634 0.01719 lineto
0.17758 0.0163 lineto
0.17882 0.01561 lineto
0.18006 0.01511 lineto
0.1813 0.01481 lineto
0.18254 0.01472 lineto
0.18378 0.01481 lineto
0.18502 0.01511 lineto
0.18626 0.01561 lineto
0.1875 0.0163 lineto
0.18998 0.01827 lineto
0.19246 0.02103 lineto
0.19742 0.0288 lineto
0.20238 0.03949 lineto
0.2123 0.06878 lineto
0.22222 0.10704 lineto
0.2619 0.30902 lineto
0.28175 0.41604 lineto
0.30159 0.511 lineto
0.31151 0.54925 lineto
0.31647 0.56514 lineto
0.32143 0.57855 lineto
0.32639 0.58923 lineto
0.33135 0.59701 lineto
0.33383 0.59976 lineto
0.33507 0.60084 lineto
0.33631 0.60173 lineto
0.33755 0.60243 lineto
0.33879 0.60292 lineto
0.34003 0.60322 lineto
Mistroke
0.34127 0.60332 lineto
0.34251 0.60322 lineto
0.34375 0.60292 lineto
0.34499 0.60243 lineto
0.34623 0.60173 lineto
0.34871 0.59976 lineto
0.35119 0.59701 lineto
0.35615 0.58923 lineto
0.36111 0.57855 lineto
0.37103 0.54925 lineto
0.38095 0.511 lineto
0.42063 0.30902 lineto
0.44048 0.20199 lineto
0.46032 0.10704 lineto
0.47024 0.06878 lineto
0.4752 0.05289 lineto
0.48016 0.03949 lineto
0.48512 0.0288 lineto
0.4876 0.02454 lineto
0.49008 0.02103 lineto
0.49256 0.01827 lineto
0.4938 0.01719 lineto
0.49504 0.0163 lineto
0.49628 0.01561 lineto
0.49752 0.01511 lineto
0.49876 0.01481 lineto
0.5 0.01472 lineto
0.50124 0.01481 lineto
0.50248 0.01511 lineto
0.50372 0.01561 lineto
0.50496 0.0163 lineto
0.50744 0.01827 lineto
0.50992 0.02103 lineto
0.51488 0.0288 lineto
0.51984 0.03949 lineto
0.52976 0.06878 lineto
0.53968 0.10704 lineto
0.57937 0.30902 lineto
0.59921 0.41604 lineto
0.61905 0.511 lineto
0.62897 0.54925 lineto
0.63393 0.56514 lineto
0.63889 0.57855 lineto
0.64385 0.58923 lineto
0.64881 0.59701 lineto
0.65129 0.59976 lineto
0.65253 0.60084 lineto
0.65377 0.60173 lineto
0.65501 0.60243 lineto
0.65625 0.60292 lineto
Mistroke
0.65749 0.60322 lineto
0.65873 0.60332 lineto
0.65997 0.60322 lineto
0.66121 0.60292 lineto
0.66245 0.60243 lineto
0.66369 0.60173 lineto
0.66617 0.59976 lineto
0.66865 0.59701 lineto
0.67361 0.58923 lineto
0.67857 0.57855 lineto
0.68849 0.54925 lineto
0.69841 0.511 lineto
0.7381 0.30902 lineto
0.75794 0.20199 lineto
0.77778 0.10704 lineto
0.7877 0.06878 lineto
0.79762 0.03949 lineto
0.80258 0.0288 lineto
0.80506 0.02454 lineto
0.80754 0.02103 lineto
0.81002 0.01827 lineto
0.81126 0.01719 lineto
0.8125 0.0163 lineto
0.81374 0.01561 lineto
0.81498 0.01511 lineto
0.81622 0.01481 lineto
0.81746 0.01472 lineto
0.8187 0.01481 lineto
0.81994 0.01511 lineto
0.82118 0.01561 lineto
0.82242 0.0163 lineto
0.8249 0.01827 lineto
0.82738 0.02103 lineto
0.83234 0.0288 lineto
0.8373 0.03949 lineto
0.84722 0.06878 lineto
0.85714 0.10704 lineto
0.89683 0.30902 lineto
0.91667 0.41604 lineto
0.93651 0.511 lineto
0.94643 0.54925 lineto
0.95139 0.56514 lineto
0.95635 0.57855 lineto
0.96131 0.58923 lineto
0.96379 0.59349 lineto
0.96627 0.59701 lineto
0.96875 0.59976 lineto
0.96999 0.60084 lineto
0.97123 0.60173 lineto
0.97247 0.60243 lineto
Mistroke
0.97371 0.60292 lineto
0.97495 0.60322 lineto
0.97619 0.60332 lineto
Mfstroke
grestore
grestore
% End of Graphics
MathPictureEnd
:[font = output; output; inactive; preserveAspect; endGroup; ]
{220.85*Second, Graphics[{{Line[{{-9.424777960769\
          38, 1.786487481950052}, 
       {-9.40023426816321, 1.786186315762798}, 
       {-9.37569057555704, 1.78528317985969}, 
       {-9.35114688295087, 1.783779160907218}, 
       {-9.3266031903447, 1.781676065659738}, 
       {-9.27751580513236, 1.775683432160446}, 
       {-9.22842841992002, 1.767333824833179}, 
       {-9.13025364949534, 1.743731547264431}, 
       {-9.03207887907066, 1.711303618588814}, 
       {-8.83572933822129, 1.622378485497551}, 
       {-8.63937979737193, 1.506276656141543}, 
       {-7.853981633974483, 0.893243740975047}, 
       {-7.461282552275759, 0.5684009250124993}, 
       {-7.068583470577035, 0.2802108258085203}, 
       {-6.872233929727673, 0.1641089964508154}, 
       {-6.675884388878311, 0.07518386336158584}, 
       {-6.577709618453631, 0.04275593468561523}, 
       {-6.52862223324129, 0.02982084656475359}, 
       {-6.479534848028949, 0.01915365711611905}, 
       {-6.430447462816608, 0.01080404978960658}, 
       {-6.405903770210437, 0.00751106749597208}, 
       {-6.381360077604267, 0.004811416290315452}, 
       {-6.356816384998097, 0.002708321042834694}, 
       {-6.332272692391927, 0.00120430209036404}, 
       {-6.307728999785757, 0.0003011661872539539}, 
       {-6.283185307179587, 0.}, 
       {-6.258641614573416, 0.000301166187254287}, 
       {-6.234097921967246, 0.001204302090363818}, 
       {-6.209554229361076, 0.002708321042834916}, 
       {-6.185010536754906, 0.004811416290315229}, 
       {-6.135923151542566, 0.01080404978960359}, 
       {-6.086835766330224, 0.01915365711611189}, 
       {-5.988660995905543, 0.04275593468561734}, 
       {-5.890486225480862, 0.07518386336158745}, 
       {-5.694136684631501, 0.1641089964508167}, 
       {-5.497787143782138, 0.28021082580851}, 
       {-4.71238898038469, 0.893243740975026}, 
       {-4.319689898685965, 1.218086556937533}, 
       {-3.926990816987241, 1.506276656141543}, 
       {-3.730641276137879, 1.622378485499251}, 
       {-3.632466505713198, 1.670624404340015}, 
       {-3.534291735288517, 1.711303618588485}, 
       {-3.436116964863836, 1.743731547264463}, 
       {-3.337942194439155, 1.767333824833962}, 
       {-3.288854809226815, 1.775683432160463}, 
       {-3.264311116620645, 1.778976414454096}, 
       {-3.239767424014474, 1.781676065659751}, 
       {-3.215223731408304, 1.78377916090723}, 
       {-3.190680038802133, 1.785283179859699}, 
       {-3.166136346195963, 1.786186315762807}, 
       {-3.141592653589793, 1.78648748195006}, 
       {-3.117048960983623, 1.786186315762805}, 
       {-3.092505268377453, 1.785283179859694}, 
       {-3.067961575771283, 1.783779160907222}, 
       {-3.043417883165112, 1.78167606565974}, 
       {-2.994330497952771, 1.775683432160448}, 
       {-2.945243112740431, 1.767333824833942}, 
       {-2.84706834231575, 1.743731547264438}, 
       {-2.748893571891069, 1.711303618588467}, 
       {-2.552544031041707, 1.622378485499237}, 
       {-2.356194490192345, 1.506276656141543}, 
       {-1.570796326794897, 0.89324374097503}, 
       {-1.178097245096172, 0.5684009250125193}, 
       {-0.7853981633974483, 0.280210825808509}, 
       {-0.5890486225480862, 0.1641089964508159}, 
       {-0.4908738521234052, 0.1158630776100524}, 
       {-0.3926990816987241, 0.07518386336158568}, 
       {-0.2945243112740431, 0.04275593468561531}, 
       {-0.2454369260617026, 0.02982084656474425}, 
       {-0.1963495408493621, 0.01915365711611158}, 
       {-0.1472621556370215, 0.01080404978960658}, 
       {-0.1227184630308513, 0.007511067495972139}, 
       {-0.098174770424681, 0.004811416290315183}, 
       {-0.07363107781851078, 
        0.002708321042834606}, 
       {-0.04908738521234052, 
        0.001204302090363865}, 
       {-0.02454369260617026, 
        0.000301166187254096}, {0., 0.}, 
       {0.02454369260617026, 0.000301166187254096}, 
       {0.04908738521234052, 0.001204302090363865}, 
       {0.07363107781851078, 0.002708321042834606}, 
       {0.098174770424681, 0.004811416290315183}, 
       {0.1472621556370215, 0.01080404978960658}, 
       {0.1963495408493621, 0.01915365711611158}, 
       {0.2945243112740431, 0.04275593468561531}, 
       {0.3926990816987241, 0.07518386336158568}, 
       {0.5890486225480862, 0.1641089964508159}, 
       {0.7853981633974483, 0.280210825808509}, 
       {1.570796326794897, 0.89324374097503}, 
       {1.963495408493621, 1.218086556937533}, 
       {2.356194490192345, 1.506276656141543}, 
       {2.552544031041707, 1.622378485499237}, 
       {2.650718801466388, 1.67062440434}, 
       {2.748893571891069, 1.711303618588467}, 
       {2.84706834231575, 1.743731547264438}, 
       {2.945243112740431, 1.767333824833942}, 
       {2.994330497952771, 1.775683432160448}, 
       {3.018874190558941, 1.778976414454083}, 
       {3.043417883165112, 1.78167606565974}, 
       {3.067961575771283, 1.783779160907222}, 
       {3.092505268377453, 1.785283179859694}, 
       {3.117048960983623, 1.786186315762805}, 
       {3.141592653589793, 1.78648748195006}, 
       {3.166136346195963, 1.786186315762807}, 
       {3.190680038802133, 1.785283179859699}, 
       {3.215223731408304, 1.78377916090723}, 
       {3.239767424014474, 1.781676065659751}, 
       {3.288854809226815, 1.775683432160463}, 
       {3.337942194439155, 1.767333824833962}, 
       {3.436116964863836, 1.743731547264463}, 
       {3.534291735288517, 1.711303618588485}, 
       {3.730641276137879, 1.622378485499251}, 
       {3.926990816987241, 1.506276656141543}, 
       {4.71238898038469, 0.893243740975026}, 
       {5.105088062083415, 0.5684009250125192}, 
       {5.497787143782138, 0.28021082580851}, 
       {5.694136684631501, 0.1641089964508167}, 
       {5.890486225480862, 0.07518386336158745}, 
       {5.988660995905543, 0.04275593468561734}, 
       {6.037748381117883, 0.02982084656474437}, 
       {6.086835766330224, 0.01915365711611189}, 
       {6.135923151542566, 0.01080404978960359}, 
       {6.160466844148736, 0.007511067495969193}, 
       {6.185010536754906, 0.004811416290315229}, 
       {6.209554229361076, 0.002708321042834916}, 
       {6.234097921967246, 0.001204302090363818}, 
       {6.258641614573416, 0.000301166187254287}, 
       {6.283185307179587, 0.}, 
       {6.307728999785757, 0.0003011661872539539}, 
       {6.332272692391927, 0.00120430209036404}, 
       {6.356816384998097, 0.002708321042834694}, 
       {6.381360077604267, 0.004811416290315452}, 
       {6.430447462816608, 0.01080404978960658}, 
       {6.479534848028949, 0.01915365711611905}, 
       {6.577709618453631, 0.04275593468561523}, 
       {6.675884388878311, 0.07518386336158584}, 
       {6.872233929727673, 0.1641089964508154}, 
       {7.068583470577035, 0.2802108258085203}, 
       {7.853981633974483, 0.893243740975047}, 
       {8.24668071567321, 1.218086556937267}, 
       {8.63937979737193, 1.506276656141543}, 
       {8.83572933822129, 1.622378485497551}, 
       {8.93390410864597, 1.670624404340322}, 
       {9.03207887907066, 1.711303618588814}, 
       {9.13025364949534, 1.743731547264431}, 
       {9.17934103470768, 1.756666635384967}, 
       {9.22842841992002, 1.767333824833179}, 
       {9.27751580513236, 1.775683432160446}, 
       {9.30205949773853, 1.77897641445408}, 
       {9.3266031903447, 1.781676065659738}, 
       {9.35114688295087, 1.783779160907218}, 
       {9.37569057555704, 1.78528317985969}, 
       {9.40023426816321, 1.786186315762798}, 
       {9.42477796076938, 1.786487481950052}}]}}, 
   {PlotRange -> Automatic, 
    AspectRatio -> GoldenRatio^(-1), 
    DisplayFunction :> $DisplayFunction, 
    ColorOutput -> Automatic, Axes -> Automatic, 
    AxesOrigin -> Automatic, PlotLabel -> None, 
    AxesLabel -> None, Ticks -> Automatic, 
    GridLines -> None, Prolog -> {}, Epilog -> {}, 
    AxesStyle -> Automatic, Background -> Automatic, 
    DefaultColor -> Automatic, 
    DefaultFont :> $DefaultFont, 
    RotateLabel -> True, Frame -> False, 
    FrameStyle -> Automatic, 
    FrameTicks -> Automatic, FrameLabel -> None, 
    PlotRegion -> Automatic}]}
;[o]
{220.85 Second, -Graphics-}
:[font = input; preserveAspect; startGroup; ]
F[x_] := SuperSin[x] - 1
xn = 2;
Do[
	xn = xn - F[xn]/F'[xn];
	Print[N[xn,16]],
{5}]
:[font = print; inactive; preserveAspect; endGroup; endGroup; endGroup; endGroup; ]
1.686903158808807
1.697880562552439
1.69788526087536
1.697885260876274
1.697885260876274
:[font = section; inactive; preserveAspect; startGroup; ]
Some Integrals
:[font = input; preserveAspect; startGroup; ]
f[x_] := x^11 (x^4 + 1)^(1/3)
Integrate[f[x], x]
:[font = output; output; inactive; preserveAspect; endGroup; ]
(1 + x^4)^(1/3)*(27/560 - (9*x^4)/560 + (3*x^8)/280 + 
    (3*x^12)/40)
;[o]
                      4      8      12
      4 1/3  27    9 x    3 x    3 x
(1 + x )    (--- - ---- + ---- + -----)
             560   560    280     40
:[font = input; preserveAspect; startGroup; ]
f[x_] := 1/Sqrt[Exp[4x] - 1]
Integrate[f[x], x]
:[font = output; output; inactive; preserveAspect; endGroup; ]
ArcTan[(-1 + E^(4*x))^(1/2)]/2
;[o]
                  4 x
ArcTan[Sqrt[-1 + E   ]]
-----------------------
           2
:[font = input; preserveAspect; startGroup; ]
Plot[{ArcTan[Sqrt[Exp[4x] - 1]], ArcSec[Exp[2x]]},
{x, 0, 1}]
:[font = postscript; PostScript; formatAsPostScript; output; inactive; preserveAspect; pictureLeft = 34; pictureWidth = 282; pictureHeight = 174; ]
%!
%%Creator: Mathematica
%%AspectRatio: 0.61803 
MathPictureStart
/Courier findfont 10  scalefont  setfont
% Scaling calculations
0.02381 0.952381 0.014715 0.410164 [
[(0.2)] 0.21429 0.01472 0 2 Msboxa
[(0.4)] 0.40476 0.01472 0 2 Msboxa
[(0.6)] 0.59524 0.01472 0 2 Msboxa
[(0.8)] 0.78571 0.01472 0 2 Msboxa
[(1)] 0.97619 0.01472 0 2 Msboxa
[(0.2)] 0.01131 0.09675 1 0 Msboxa
[(0.4)] 0.01131 0.17878 1 0 Msboxa
[(0.6)] 0.01131 0.26081 1 0 Msboxa
[(0.8)] 0.01131 0.34285 1 0 Msboxa
[(1)] 0.01131 0.42488 1 0 Msboxa
[(1.2)] 0.01131 0.50691 1 0 Msboxa
[(1.4)] 0.01131 0.58894 1 0 Msboxa
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
0.21429 0.01472 moveto
0.21429 0.02097 lineto
stroke
grestore
[(0.2)] 0.21429 0.01472 0 2 Mshowa
gsave
0.002 setlinewidth
0.40476 0.01472 moveto
0.40476 0.02097 lineto
stroke
grestore
[(0.4)] 0.40476 0.01472 0 2 Mshowa
gsave
0.002 setlinewidth
0.59524 0.01472 moveto
0.59524 0.02097 lineto
stroke
grestore
[(0.6)] 0.59524 0.01472 0 2 Mshowa
gsave
0.002 setlinewidth
0.78571 0.01472 moveto
0.78571 0.02097 lineto
stroke
grestore
[(0.8)] 0.78571 0.01472 0 2 Mshowa
gsave
0.002 setlinewidth
0.97619 0.01472 moveto
0.97619 0.02097 lineto
stroke
grestore
[(1)] 0.97619 0.01472 0 2 Mshowa
gsave
0.001 setlinewidth
0.0619 0.01472 moveto
0.0619 0.01847 lineto
stroke
grestore
gsave
0.001 setlinewidth
0.1 0.01472 moveto
0.1 0.01847 lineto
stroke
grestore
gsave
0.001 setlinewidth
0.1381 0.01472 moveto
0.1381 0.01847 lineto
stroke
grestore
gsave
0.001 setlinewidth
0.17619 0.01472 moveto
0.17619 0.01847 lineto
stroke
grestore
gsave
0.001 setlinewidth
0.25238 0.01472 moveto
0.25238 0.01847 lineto
stroke
grestore
gsave
0.001 setlinewidth
0.29048 0.01472 moveto
0.29048 0.01847 lineto
stroke
grestore
gsave
0.001 setlinewidth
0.32857 0.01472 moveto
0.32857 0.01847 lineto
stroke
grestore
gsave
0.001 setlinewidth
0.36667 0.01472 moveto
0.36667 0.01847 lineto
stroke
grestore
gsave
0.001 setlinewidth
0.44286 0.01472 moveto
0.44286 0.01847 lineto
stroke
grestore
gsave
0.001 setlinewidth
0.48095 0.01472 moveto
0.48095 0.01847 lineto
stroke
grestore
gsave
0.001 setlinewidth
0.51905 0.01472 moveto
0.51905 0.01847 lineto
stroke
grestore
gsave
0.001 setlinewidth
0.55714 0.01472 moveto
0.55714 0.01847 lineto
stroke
grestore
gsave
0.001 setlinewidth
0.63333 0.01472 moveto
0.63333 0.01847 lineto
stroke
grestore
gsave
0.001 setlinewidth
0.67143 0.01472 moveto
0.67143 0.01847 lineto
stroke
grestore
gsave
0.001 setlinewidth
0.70952 0.01472 moveto
0.70952 0.01847 lineto
stroke
grestore
gsave
0.001 setlinewidth
0.74762 0.01472 moveto
0.74762 0.01847 lineto
stroke
grestore
gsave
0.001 setlinewidth
0.82381 0.01472 moveto
0.82381 0.01847 lineto
stroke
grestore
gsave
0.001 setlinewidth
0.8619 0.01472 moveto
0.8619 0.01847 lineto
stroke
grestore
gsave
0.001 setlinewidth
0.9 0.01472 moveto
0.9 0.01847 lineto
stroke
grestore
gsave
0.001 setlinewidth
0.9381 0.01472 moveto
0.9381 0.01847 lineto
stroke
grestore
gsave
0.002 setlinewidth
0 0.01472 moveto
1 0.01472 lineto
stroke
grestore
gsave
0.002 setlinewidth
0.02381 0.09675 moveto
0.03006 0.09675 lineto
stroke
grestore
[(0.2)] 0.01131 0.09675 1 0 Mshowa
gsave
0.002 setlinewidth
0.02381 0.17878 moveto
0.03006 0.17878 lineto
stroke
grestore
[(0.4)] 0.01131 0.17878 1 0 Mshowa
gsave
0.002 setlinewidth
0.02381 0.26081 moveto
0.03006 0.26081 lineto
stroke
grestore
[(0.6)] 0.01131 0.26081 1 0 Mshowa
gsave
0.002 setlinewidth
0.02381 0.34285 moveto
0.03006 0.34285 lineto
stroke
grestore
[(0.8)] 0.01131 0.34285 1 0 Mshowa
gsave
0.002 setlinewidth
0.02381 0.42488 moveto
0.03006 0.42488 lineto
stroke
grestore
[(1)] 0.01131 0.42488 1 0 Mshowa
gsave
0.002 setlinewidth
0.02381 0.50691 moveto
0.03006 0.50691 lineto
stroke
grestore
[(1.2)] 0.01131 0.50691 1 0 Mshowa
gsave
0.002 setlinewidth
0.02381 0.58894 moveto
0.03006 0.58894 lineto
stroke
grestore
[(1.4)] 0.01131 0.58894 1 0 Mshowa
gsave
0.001 setlinewidth
0.02381 0.03112 moveto
0.02756 0.03112 lineto
stroke
grestore
gsave
0.001 setlinewidth
0.02381 0.04753 moveto
0.02756 0.04753 lineto
stroke
grestore
gsave
0.001 setlinewidth
0.02381 0.06393 moveto
0.02756 0.06393 lineto
stroke
grestore
gsave
0.001 setlinewidth
0.02381 0.08034 moveto
0.02756 0.08034 lineto
stroke
grestore
gsave
0.001 setlinewidth
0.02381 0.11315 moveto
0.02756 0.11315 lineto
stroke
grestore
gsave
0.001 setlinewidth
0.02381 0.12956 moveto
0.02756 0.12956 lineto
stroke
grestore
gsave
0.001 setlinewidth
0.02381 0.14597 moveto
0.02756 0.14597 lineto
stroke
grestore
gsave
0.001 setlinewidth
0.02381 0.16237 moveto
0.02756 0.16237 lineto
stroke
grestore
gsave
0.001 setlinewidth
0.02381 0.19519 moveto
0.02756 0.19519 lineto
stroke
grestore
gsave
0.001 setlinewidth
0.02381 0.21159 moveto
0.02756 0.21159 lineto
stroke
grestore
gsave
0.001 setlinewidth
0.02381 0.228 moveto
0.02756 0.228 lineto
stroke
grestore
gsave
0.001 setlinewidth
0.02381 0.24441 moveto
0.02756 0.24441 lineto
stroke
grestore
gsave
0.001 setlinewidth
0.02381 0.27722 moveto
0.02756 0.27722 lineto
stroke
grestore
gsave
0.001 setlinewidth
0.02381 0.29363 moveto
0.02756 0.29363 lineto
stroke
grestore
gsave
0.001 setlinewidth
0.02381 0.31003 moveto
0.02756 0.31003 lineto
stroke
grestore
gsave
0.001 setlinewidth
0.02381 0.32644 moveto
0.02756 0.32644 lineto
stroke
grestore
gsave
0.001 setlinewidth
0.02381 0.35925 moveto
0.02756 0.35925 lineto
stroke
grestore
gsave
0.001 setlinewidth
0.02381 0.37566 moveto
0.02756 0.37566 lineto
stroke
grestore
gsave
0.001 setlinewidth
0.02381 0.39207 moveto
0.02756 0.39207 lineto
stroke
grestore
gsave
0.001 setlinewidth
0.02381 0.40847 moveto
0.02756 0.40847 lineto
stroke
grestore
gsave
0.001 setlinewidth
0.02381 0.44129 moveto
0.02756 0.44129 lineto
stroke
grestore
gsave
0.001 setlinewidth
0.02381 0.45769 moveto
0.02756 0.45769 lineto
stroke
grestore
gsave
0.001 setlinewidth
0.02381 0.4741 moveto
0.02756 0.4741 lineto
stroke
grestore
gsave
0.001 setlinewidth
0.02381 0.49051 moveto
0.02756 0.49051 lineto
stroke
grestore
gsave
0.001 setlinewidth
0.02381 0.52332 moveto
0.02756 0.52332 lineto
stroke
grestore
gsave
0.001 setlinewidth
0.02381 0.53973 moveto
0.02756 0.53973 lineto
stroke
grestore
gsave
0.001 setlinewidth
0.02381 0.55613 moveto
0.02756 0.55613 lineto
stroke
grestore
gsave
0.001 setlinewidth
0.02381 0.57254 moveto
0.02756 0.57254 lineto
stroke
grestore
gsave
0.001 setlinewidth
0.02381 0.60535 moveto
0.02756 0.60535 lineto
stroke
grestore
gsave
0.002 setlinewidth
0.02381 0 moveto
0.02381 0.61803 lineto
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
gsave
0.004 setlinewidth
0.02381 0.01472 moveto
0.02505 0.0443 lineto
0.02629 0.05654 lineto
0.02877 0.07381 lineto
0.03373 0.09815 lineto
0.03869 0.11672 lineto
0.04365 0.1323 lineto
0.06349 0.17985 lineto
0.08333 0.21555 lineto
0.10317 0.245 lineto
0.14286 0.29282 lineto
0.18254 0.33135 lineto
0.22222 0.36376 lineto
0.2619 0.3917 lineto
0.30159 0.41618 lineto
0.34127 0.43785 lineto
0.38095 0.45721 lineto
0.42063 0.47459 lineto
0.46032 0.49028 lineto
0.5 0.50448 lineto
0.53968 0.51738 lineto
0.57937 0.52911 lineto
0.61905 0.53982 lineto
0.65873 0.54959 lineto
0.69841 0.55852 lineto
0.7381 0.5667 lineto
0.77778 0.57419 lineto
0.81746 0.58106 lineto
0.85714 0.58736 lineto
0.89683 0.59314 lineto
0.93651 0.59845 lineto
0.97619 0.60332 lineto
stroke
grestore
grestore
gsave
gsave
0.004 setlinewidth
0.02381 0.01472 moveto
0.02505 0.0443 lineto
0.02629 0.05654 lineto
0.02877 0.07381 lineto
0.03373 0.09815 lineto
0.03869 0.11672 lineto
0.04365 0.1323 lineto
0.06349 0.17985 lineto
0.08333 0.21555 lineto
0.10317 0.245 lineto
0.14286 0.29282 lineto
0.18254 0.33135 lineto
0.22222 0.36376 lineto
0.2619 0.3917 lineto
0.30159 0.41618 lineto
0.34127 0.43785 lineto
0.38095 0.45721 lineto
0.42063 0.47459 lineto
0.46032 0.49028 lineto
0.5 0.50448 lineto
0.53968 0.51738 lineto
0.57937 0.52911 lineto
0.61905 0.53982 lineto
0.65873 0.54959 lineto
0.69841 0.55852 lineto
0.7381 0.5667 lineto
0.77778 0.57419 lineto
0.81746 0.58106 lineto
0.85714 0.58736 lineto
0.89683 0.59314 lineto
0.93651 0.59845 lineto
0.97619 0.60332 lineto
stroke
grestore
grestore
grestore
% End of Graphics
MathPictureEnd
:[font = output; output; inactive; preserveAspect; endGroup; ]
The Unformatted text for this cell was not generated.
Use options in the Actions Settings dialog box to control
when Unformatted text is generated.
;[o]
-Graphics-
:[font = input; preserveAspect; startGroup; ]
f[x_] := x^5 Exp[a x]
Integrate[f[x], x]
:[font = output; output; inactive; preserveAspect; endGroup; ]
E^(a*x)*(-120/a^6 + (120*x)/a^5 - (60*x^2)/a^4 + 
    (20*x^3)/a^3 - (5*x^4)/a^2 + x^5/a)
;[o]
                         2       3      4    5
 a x  -120   120 x   60 x    20 x    5 x    x
E    (---- + ----- - ----- + ----- - ---- + --)
        6      5       4       3       2    a
       a      a       a       a       a
:[font = input; preserveAspect; startGroup; ]
f[x_] := x^3 Cos[a x]
Integrate[f[x], x]
:[font = output; output; inactive; preserveAspect; endGroup; ]
(3*(-2 + a^2*x^2)*Cos[a*x])/a^4 + 
  (x*(-6 + a^2*x^2)*Sin[a*x])/a^3
;[o]
         2  2                      2  2
3 (-2 + a  x ) Cos[a x]   x (-6 + a  x ) Sin[a x]
----------------------- + -----------------------
           4                         3
          a                         a
:[font = input; preserveAspect; startGroup; ]
f[x_] := x^3 Sin[a x]
Integrate[f[x], x]
:[font = output; output; inactive; preserveAspect; endGroup; ]
(x*(6 - a^2*x^2)*Cos[a*x])/a^3 + 
  (3*(-2 + a^2*x^2)*Sin[a*x])/a^4
;[o]
        2  2                      2  2
x (6 - a  x ) Cos[a x]   3 (-2 + a  x ) Sin[a x]
---------------------- + -----------------------
           3                        4
          a                        a
:[font = input; preserveAspect; startGroup; ]
f[x_] := x^3 Exp[I a x]
ans = Integrate[f[x], x]
:[font = output; output; inactive; preserveAspect; endGroup; ]
E^(I*a*x)*(-6/a^4 + (6*I*x)/a^3 + (3*x^2)/a^2 - 
    (I*x^3)/a)
;[o]
                        2      3
 I a x  -6   6 I x   3 x    I x
E      (-- + ----- + ---- - ----)
         4     3       2     a
        a     a       a
:[font = input; preserveAspect; startGroup; ]
cosint = Re[ans] //ComplexExpand
sinint = Im[ans] //ComplexExpand
:[font = output; output; inactive; preserveAspect; ]
(-6/a^4 + (3*x^2)/a^2)*Cos[a*x] + 
  ((-6*x)/a^3 + x^3/a)*Sin[a*x]
;[o]
         2                      3
 -6   3 x               -6 x   x
(-- + ----) Cos[a x] + (---- + --) Sin[a x]
  4     2                 3    a
 a     a                 a
:[font = output; output; inactive; preserveAspect; endGroup; endGroup; endGroup; ]
((6*x)/a^3 - x^3/a)*Cos[a*x] + 
  (-6/a^4 + (3*x^2)/a^2)*Sin[a*x]
;[o]
        3                      2
 6 x   x               -6   3 x
(--- - --) Cos[a x] + (-- + ----) Sin[a x]
  3    a                4     2
 a                     a     a
^*)