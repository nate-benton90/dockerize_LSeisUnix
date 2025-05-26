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
Limits with Mathematica
;[s]
2:0,0;12,1;23,-1;
2:1,21,16,Times,1,24,0,0,0;1,22,17,Times,3,24,0,0,0;
:[font = section; inactive; Cclosed; preserveAspect; plain; startGroup; ]
Exercise 2
:[font = input; preserveAspect; ]
f[x_] := (Sin[Tan[x]] - Tan[Sin[x]]) /x^7
a = 0; ntimes = 5;
Table[{x, f[x]} /. x -> a + 1/2^k, {k, 1, ntimes}] //
N //TableForm
Clear[a, ntimes];
:[font = input; preserveAspect; ]
Limit[f[x], x -> 0]
:[font = input; preserveAspect; ]
f[x_] := 1/Sin[x]^2 - 1/x^2
a = 0; ntimes = 5;
Table[{x, f[x]} /. x -> a + 1/2^k, {k, 1, ntimes}] //
N //TableForm
Clear[a, ntimes];
:[font = input; preserveAspect; endGroup; ]
Limit[f[x], x -> 0]
:[font = section; inactive; Cclosed; preserveAspect; startGroup; ]
Exercise 3
:[font = input; preserveAspect; ]
Limit[(x^3 - x^2 - 4x + 4)/(x - 1), x -> 0]
:[font = input; preserveAspect; ]
Limit[Sin[x]/x, x -> 0]
:[font = input; preserveAspect; ]
Limit[(1 - Cos[x])/x, x -> 0]
:[font = input; preserveAspect; ]
Limit[Sin[5x]/x, x -> 0]
:[font = input; preserveAspect; endGroup; ]
Limit[(1 + x)^(1/x), x -> 0]
:[font = section; inactive; Cclosed; preserveAspect; startGroup; ]
Exercise 4
:[font = input; preserveAspect; ]
Limit[(1 + 2x)^(1/x), x -> 0]
:[font = input; preserveAspect; ]
Limit[(1 + 3x)^(1/x), x -> 0]
:[font = input; preserveAspect; ]
Limit[(1 - 2x)^(1/x), x -> 0]
:[font = input; preserveAspect; endGroup; ]
Limit[(1 + a x)^(1/x), x -> 0]
:[font = section; inactive; Cclosed; preserveAspect; startGroup; ]
Exercise 5
:[font = input; preserveAspect; ]
Limit[(1 + x^2)^(1/x), x -> 0]
:[font = input; preserveAspect; ]
Limit[(1 + 2x^2)^(1/x), x -> 0]
:[font = input; preserveAspect; ]
Limit[(1 + x^3)^(1/x), x -> 0]
:[font = input; preserveAspect; ]
Limit[(1 + x^(3/2))^(1/x), x -> 0]
:[font = input; preserveAspect; endGroup; ]
Limit[(1 + Sqrt[x])^(1/x), x -> 0]
:[font = section; inactive; Cclosed; preserveAspect; startGroup; ]
Exercise 6
:[font = input; preserveAspect; endGroup; endGroup; ]
Limit[Sin[a x]/x, x -> 0]
^*)