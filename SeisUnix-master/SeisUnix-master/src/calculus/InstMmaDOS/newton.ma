(*^

::[paletteColors = 128; automaticGrouping; currentKernel; 
	fontset = title, inactive, noPageBreakBelow, nohscroll, preserveAspect, groupLikeTitle, center, M7, bold, L1, e8,  24, "Times"; ;
	fontset = subtitle, inactive, noPageBreakBelow, nohscroll, preserveAspect, groupLikeTitle, center, M7, bold, L1, e6,  18, "Times"; ;
	fontset = subsubtitle, inactive, noPageBreakBelow, nohscroll, preserveAspect, groupLikeTitle, center, M7, italic, L1, e6,  14, "Times"; ;
	fontset = section, inactive, noPageBreakBelow, nohscroll, preserveAspect, groupLikeSection, grayBox, M22, bold, L1, a20,  18, "Times"; ;
	fontset = subsection, inactive, noPageBreakBelow, nohscroll, preserveAspect, groupLikeSection, blackBox, M19, bold, L1, a15,  14, "Times"; ;
	fontset = subsubsection, inactive, noPageBreakBelow, nohscroll, preserveAspect, groupLikeSection, whiteBox, M18, bold, L1, a12,  12, "Times"; ;
	fontset = text, inactive, nohscroll, noKeepOnOnePage, preserveAspect, M7, L1,  12;
	fontset = smalltext, inactive, nohscroll, noKeepOnOnePage, preserveAspect, M7, L1,  10, "Times"; ;
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
Newton's Method
:[font = section; inactive; Cclosed; preserveAspect; startGroup; ]
Introduction
:[font = subsection; inactive; preserveAspect; startGroup; ]
Statements for Figure 1
:[font = input; preserveAspect; ]
f[x_] := x^2 - 5
:[font = input; preserveAspect; endGroup; ]
Plot[f[x], {x, -8, 8}]
:[font = subsection; inactive; preserveAspect; startGroup; ]
Statements for Figure 2
:[font = input; preserveAspect; ]
a = 6.0;
y = f[a] + f'[a] (x - a)
:[font = input; preserveAspect; endGroup; ]
Plot[{f[x], y}, {x, 0, 10}]
:[font = subsection; inactive; preserveAspect; startGroup; ]
The prime notation for derivatives
:[font = input; preserveAspect; endGroup; ]
f[x_] := x^3
f'[x]
:[font = subsection; inactive; preserveAspect; startGroup; ]
Accurate Sqrt[5]
:[font = input; preserveAspect; endGroup; endGroup; ]
N[Sqrt[5], 20]
:[font = section; inactive; Cclosed; preserveAspect; startGroup; ]
Exercise 2
:[font = input; preserveAspect; endGroup; ]
f[x_] := x^2 - 5
xn = 6.0;
Do[
    xn = xn - f[xn]/f'[xn];
    Print[ xn ],
{5}]
:[font = section; inactive; Cclosed; preserveAspect; startGroup; ]
Exercise 3
:[font = input; preserveAspect; endGroup; ]
f[x_] := x^2 - 5
xn = 15.0;
Do[
    xn = xn - f[xn]/f'[xn];
    Print[ xn ],
{6}]
:[font = section; inactive; Cclosed; preserveAspect; startGroup; ]
Exercise 4
:[font = input; preserveAspect; ]
f[x_] := x^2 - 5
xn = 15.0;
Do[
    xn = xn - f[xn]/f'[xn];
    Print[ N[xn, 16] ],
{8}]
:[font = input; preserveAspect; endGroup; ]
f[x_] := x^2 - 5
xn = 200.0;
Do[
    xn = xn - f[xn]/f'[xn];
    Print[ N[xn, 10] ],
{16}]
:[font = section; inactive; Cclosed; preserveAspect; startGroup; ]
Exercise 5
:[font = input; preserveAspect; endGroup; ]
Do[
	Print["c = ", c];
	f[x_] := x^2 - c;
	xn = 1.0;
	Do[
	    xn = xn - f[xn]/f'[xn];
	    Print["        ", N[xn, 6] ],
	{5}],
{c, .5, 4.0, .5}]
:[font = section; inactive; Cclosed; preserveAspect; startGroup; ]
Exercise 6
:[font = input; preserveAspect; ]
f[x_] := (x^3 - 2.1 x^2 + x - 2) / (x^6 + 1)
:[font = input; preserveAspect; ]
f[-10]
f[10]
:[font = input; preserveAspect; ]
Plot[f[x], {x, -10, 10}]
:[font = input; preserveAspect; ]
Plot[f[x], {x, -10, 10}, PlotRange -> {-.1, .1}]
:[font = input; preserveAspect; ]
Plot[f[x], {x, -3, 3}]
:[font = input; preserveAspect; ]
Plot[f[x], {x, 1.5, 3.5}]
:[font = input; preserveAspect; ]
xn = 2.0;
Do[
    xn = xn - f[xn]/f'[xn];
    Print[ N[xn, 6] ],
{8}]
:[font = input; preserveAspect; ]
xn = 3.0;
Do[
    xn = xn - f[xn]/f'[xn];
    Print[ N[xn, 6] ],
{16}]
:[font = input; preserveAspect; ]
xn = 0.5;
Do[
    xn = xn - f[xn]/f'[xn];
    Print[ N[xn, 6] ],
{16}]
:[font = input; preserveAspect; ]
Plot[f[x], {x, 0, 1.0}]
:[font = input; preserveAspect; ]
f'[0.5]
:[font = input; preserveAspect; ]
xn = -1.0;
Do[
    xn = xn - f[xn]/f'[xn];
    Print[ N[xn, 6] ],
{16}]
:[font = input; preserveAspect; endGroup; endGroup; ]
Plot[f[x], {x, -3, 0}]
^*)