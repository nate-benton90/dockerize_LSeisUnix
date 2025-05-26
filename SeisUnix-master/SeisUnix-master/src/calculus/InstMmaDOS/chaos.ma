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
Chaos
:[font = section; inactive; Cclosed; preserveAspect; startGroup; ]
Slow Convergence  Example
:[font = input; preserveAspect; ]
f[x_] := 27(x-2/3)^3 (x^2 + 1)(x - 6) //Expand
:[font = input; preserveAspect; ]
f[x]
:[font = input; preserveAspect; ]
Plot[f[x], {x, -2, 2}]
:[font = input; preserveAspect; endGroup; ]
xn = 1.0;
Do[
    xn = xn - f[xn]/f'[xn];
    Print[ N[xn, 5] ],
{24}]
:[font = section; inactive; Cclosed; preserveAspect; startGroup; ]
Chaos
:[font = subsection; inactive; Cclosed; preserveAspect; startGroup; ]
Basic Definitions and Plots
:[font = input; preserveAspect; ]
f[x_] := x^3 - x
s3 = 1/Sqrt[3]//N
s5 = 1/Sqrt[5]//N
:[font = input; preserveAspect; ]
Plot[f[x], {x, -1.2, 1.2},
	Ticks -> {
	   {{-s3, ""}, {-s5, ""}, {s5, "s5"}, {s3, "s3"}},
	   Automatic
	}
]
:[font = input; preserveAspect; ]
Plot[f[x], {x, -4, 4}, PlotRange -> All]
:[font = input; preserveAspect; ]
xn = s3;
Do[
    xn = xn - f[xn]/f'[xn];
    Print[ N[xn, 5] ],
{3}]
:[font = input; preserveAspect; ]
xn = s5;
Do[
    xn = xn - f[xn]/f'[xn];
    Print[ N[xn, 16] ],
{24}]
:[font = input; preserveAspect; ]
xn = 0.8;
Do[
    xn = xn - f[xn]/f'[xn];
    Print[ N[xn, 16] ],
{8}]
:[font = input; preserveAspect; ]
xn = 2.0;
Do[
    xn = xn - f[xn]/f'[xn];
    Print[ N[xn, 16] ],
{8}]
:[font = input; preserveAspect; endGroup; ]
xn = 0.4;
Do[
    xn = xn - f[xn]/f'[xn];
    Print[ N[xn, 16] ],
{8}]
:[font = subsection; inactive; Cclosed; preserveAspect; startGroup; ]
Computation of Magic Numbers
:[font = input; preserveAspect; endGroup; ]
sigfig = 16;  kmax = 20;
s3 = 1/Sqrt[3]; s5 = 1/Sqrt[5];
xn = s3;
magic =
	Table[xn = x /.
		FindRoot[2x^3 == xn (1 - 3x^2), {x, s5, s3}] //
		(N[#, sigfig])&,
	{kmax}]
:[font = subsection; inactive; Cclosed; preserveAspect; startGroup; ]
Average of Magic Numbers
:[font = input; preserveAspect; ]
Table[N[0.5 (magic[[k]] + magic[[k+1]]), 6], {k, kmax-1}]
:[font = input; preserveAspect; ]
aver = N[{0.448955494080794, 
  0.4475028601129699, 0.4472617771831841, 
  0.4472216249731682, 0.4472148976776751, 0.447213}, 6]
:[font = input; preserveAspect; ]
N[s5, 9]
:[font = input; preserveAspect; endGroup; endGroup; endGroup; ]
sigfig = 8; nextra = 8;
Do[
	Print[" "];
	Print["k = ", k];
	xn = aver[[k]];
	Do[
		xn = xn - f[xn]/f'[xn];
		Print[ N[xn, sigfig] ],
	{k + nextra}],
{k, Length[aver]}]
^*)