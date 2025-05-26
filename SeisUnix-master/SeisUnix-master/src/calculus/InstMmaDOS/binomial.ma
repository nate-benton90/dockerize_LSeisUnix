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
The Binomial Theorem and Calculus
:[font = section; inactive; Cclosed; preserveAspect; startGroup; ]
Exercise 7
:[font = input; preserveAspect; endGroup; ]
f[x_] := (1 + x)^5
g[x_] := 1 + 5x
a  = 0.0; h = 100.0;
Do[
	h = h/10;
	Plot[{f[x], g[x]}, {x, a-h, a+h},
		PlotStyle -> {GrayLevel[0.0], GrayLevel[0.5]},
		PlotLabel -> FontForm[h, {"Times-Bold", 16}]
	],
{5}]
:[font = section; inactive; Cclosed; preserveAspect; startGroup; ]
Exercise 8
:[font = input; preserveAspect; endGroup; ]
f[x_] := (1 + x)^(1/3)
g[x_] := 1 + x/3
a  = 0.0; h = 100.0;
Do[
	h = h/10;
	Plot[{f[x], g[x]}, {x, a-h, a+h},
		PlotStyle -> {GrayLevel[0.0], GrayLevel[0.5]},
		PlotLabel -> FontForm[h, {"Times-Bold", 16}]
	],
{5}]
:[font = section; inactive; Cclosed; preserveAspect; startGroup; ]
Exercise 9
:[font = input; preserveAspect; endGroup; ]
S = 1;
Print["k", "        ", "S"]
Do[
	S = S + 1.0/2^k;
	Print[k, "        ", N[S, 3]],
{k, 1, 20}]
:[font = section; inactive; Cclosed; preserveAspect; startGroup; ]
Exercise 10
:[font = input; preserveAspect; endGroup; ]
S = 0;
Print["k", "        ", "S[k]"]
Do[
	S = S + 1.0/k^2;
	Print[k, "        ", N[S, 3]],
{k, 1, 90}]
:[font = section; inactive; Cclosed; preserveAspect; startGroup; ]
Exercise 11
:[font = input; preserveAspect; ]
S = 0;
Print["k", "        ", "S[k]"]
Do[
	S = S + 1.0/k;
	Print[k, "        ", N[S, 4]],
{k, 1, 9}]
:[font = input; preserveAspect; endGroup; endGroup; ]
S = 0;
Print["k", "        ", "S[k]"]
Do[
	S = S + 1.0/k;
	If[Mod[k, 10] == 0, (* Then *)
		Print[k, "        ", N[S, 4]]
	],
{k, 1, 90}]
^*)