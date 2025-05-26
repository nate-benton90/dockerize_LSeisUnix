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
Tank Problem
:[font = section; inactive; Cclosed; preserveAspect; startGroup; ]
Definite Integrals with Mathematica
:[font = input; preserveAspect; ]
f[x_] := x^3; a = 5; b = 10;
Integrate[f[x], {x, a, b}]
:[font = input; preserveAspect; endGroup; ]
Integrate[x^3, {x, 5, 10}]
:[font = section; inactive; Cclosed; preserveAspect; startGroup; ]
Exercise 5
:[font = input; preserveAspect; startGroup; ]
4 r Integrate[Sqrt[1 - (x/r)^2], {x, 0, r}]
:[font = output; output; inactive; preserveAspect; endGroup; ]
Pi*r^2
;[o]
    2
Pi r
:[font = input; preserveAspect; startGroup; ]
4 b Integrate[Sqrt[1 - (x/a)^2], {x, 0, a}]
:[font = output; output; inactive; preserveAspect; endGroup; ]
a*b*Pi
;[o]
a b Pi
:[font = input; preserveAspect; startGroup; ]
4 a Integrate[Sqrt[1 - (y/b)^2], {y, 0, b}]
:[font = output; output; inactive; preserveAspect; endGroup; endGroup; ]
a*b*Pi
;[o]
a b Pi
:[font = section; inactive; Cclosed; preserveAspect; startGroup; ]
Exercise 6
:[font = input; preserveAspect; startGroup; ]
2 L a Integrate[Sqrt[1 - (y/b)^2], {y, -b, h}]
:[font = output; output; inactive; preserveAspect; endGroup; ]
2*a*L*((h*(1 - h^2/b^2)^(1/2))/2 + (b*Pi)/4 + 
    (b*ArcSin[h/b])/2)
;[o]
                   2
                  h
       h Sqrt[1 - --]                   h
                   2           b ArcSin[-]
                  b     b Pi            b
2 a L (-------------- + ---- + -----------)
             2           4          2
:[font = input; preserveAspect; startGroup; ]
% //TeXForm
:[font = output; output; inactive; preserveAspect; fontLeading = 0; endGroup; endGroup; ]
2\,a\,L\,\left( {{h\,
        {\sqrt{1 - {{{h^2}}\over {{b^2}}}}}}\over 2} + 
    {{b\,\pi }\over 4} + 
    {{b\,\arcsin ({h\over b})}\over 2} \right)
;[o]
2\,a\,L\,\left( {{h\,{\sqrt{1 - 
            {{{h^2}}\over {{b^2}}}}}}\over 2} + 
    {{b\,\pi }\over 4} + 
    {{b\,\arcsin ({h\over b})}\over 2} \right)
:[font = section; inactive; Cclosed; preserveAspect; startGroup; ]
Exercise 7
:[font = input; preserveAspect; startGroup; ]
L = 20; a = 10; b = 5;
V[h_] = 2 L a Integrate[Sqrt[1 - (y/b)^2], {y, -b, h}]//
        Simplify
:[font = output; output; inactive; preserveAspect; endGroup; ]
20*(2*h*(25 - h^2)^(1/2) + 25*Pi + 50*ArcSin[h/5])
;[o]
                   2                      h
20 (2 h Sqrt[25 - h ] + 25 Pi + 50 ArcSin[-])
                                          5
:[font = input; preserveAspect; startGroup; ]
Table[{h, V[h]}, {h, -5, 5}] //N//TableForm
:[font = output; output; inactive; preserveAspect; endGroup; endGroup; ]
TableForm[{{-5., 0}, {-4., 163.5011087932844}, 
   {-3., 447.2952180016123}, {-2., 792.6734251309414}, 
   {-1., 1173.479226581911}, {0, 1570.796326794897}, 
   {1., 1968.113427007882}, {2., 2348.919228458852}, 
   {3., 2694.297435588182}, {4., 2978.091544796509}, 
   {5., 3141.592653589793}}]
;[o]
-5.   0

-4.   163.501

-3.   447.295

-2.   792.673

-1.   1173.48

0     1570.8

1.    1968.11

2.    2348.92

3.    2694.3

4.    2978.09

5.    3141.59
:[font = section; inactive; Cclosed; preserveAspect; startGroup; ]
Exercise 8
:[font = input; preserveAspect; ]

:[font = input; preserveAspect; startGroup; ]
L = 20; a = 10; b = 5;
V[h_] = 2 L a Integrate[Sqrt[1 - (y/b)^2], {y, -b, h}]//N
Vprime[h_] = 2 L a Sqrt[1 - (h/b)^2]//N
:[font = output; output; inactive; preserveAspect; ]
400.*(3.926990816987241 + 
    0.1*(h*(25. - 1.*h^2)^(1/2) + 25.*ArcSin[0.2*h]))
;[o]
                                      2
400. (3.92699 + 0.1 (h Sqrt[25. - 1. h ] + 
 
       25. ArcSin[0.2 h]))
:[font = output; output; inactive; preserveAspect; endGroup; ]
400.*(1. - 0.04*h^2)^(1/2)
;[o]
                     2
400. Sqrt[1. - 0.04 h ]
:[font = input; preserveAspect; startGroup; ]
c = 500.0;
hn = -2.8;
Do [
        hn = hn - (V[hn] - c)/Vprime[hn];
        Print[hn],
{5}]
:[font = print; inactive; preserveAspect; endGroup; ]
-2.83758
-2.8377
-2.8377
-2.8377
-2.8377
:[font = input; preserveAspect; startGroup; ]
c = 1000.0;
hn = -1.5;
Do [
        hn = hn - (V[hn] - c)/Vprime[hn];
        Print[hn],
{5}]
:[font = print; inactive; preserveAspect; endGroup; ]
-1.44738
-1.44747
-1.44747
-1.44747
-1.44747
:[font = input; preserveAspect; startGroup; ]
c = 1500.0;
hn = -0.2;
Do [
        hn = hn - (V[hn] - c)/Vprime[hn];
        Print[hn],
{5}]
:[font = print; inactive; preserveAspect; endGroup; endGroup; ]
-0.177026
-0.177028
-0.177028
-0.177028
-0.177028
:[font = section; inactive; Cclosed; preserveAspect; startGroup; ]
Exercise 9
:[font = input; preserveAspect; ]
L = 20; a = 10; b = 5;
Pi a b L //N
:[font = input; preserveAspect; endGroup; endGroup; ]
% / 2
^*)