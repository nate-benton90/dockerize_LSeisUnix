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
Tutorial Exercises
:[font = text; inactive; preserveAspect; ]
Executing the commands will produce the material used in the Instructor Answer file. 
:[font = section; inactive; Cclosed; preserveAspect; startGroup; ]
Exercise 1
:[font = text; inactive; preserveAspect; ]
Either of the following does it:
:[font = input; preserveAspect; ]
Sin[Pi/3] //N
:[font = input; preserveAspect; ]
N[ Sin[Pi/3] ]
:[font = text; inactive; preserveAspect; ]
``Somehow'' could be a  hand calculator (either on the trig function or on the square root).   Here's another possibility:
:[font = input; preserveAspect; endGroup; ]
Sqrt[3]/2 //N
:[font = section; inactive; Cclosed; preserveAspect; startGroup; ]
Exercise 2
:[font = input; preserveAspect; endGroup; ]
N[Sqrt[2], 16]
:[font = section; inactive; Cclosed; preserveAspect; startGroup; ]
Exercise 3
:[font = input; preserveAspect; ]
f[x_] := x^2 + 1
g[x_] := x^3 - x^2 - 9 x + 9
:[font = input; preserveAspect; endGroup; ]
Plot[{f[x], g[x]}, {x, -5, 5}]
:[font = section; inactive; Cclosed; preserveAspect; startGroup; ]
Exercise 4
:[font = input; preserveAspect; ]
r = g[x]/f[x]
:[font = input; preserveAspect; endGroup; ]
Plot[r, {x, -20, 20}]
:[font = section; inactive; Cclosed; preserveAspect; startGroup; ]
Exercise 5
:[font = input; preserveAspect; ]
G[x_] := x^3 + 2 x^2 - 15 x - 30
:[font = input; preserveAspect; ]
Plot[G[x], {x, -20, 20}]
:[font = input; preserveAspect; ]
Plot[G[x], {x, -5, 5}]
:[font = input; preserveAspect; ]
Plot[G[x], {x, 3.8, 4}]
:[font = input; preserveAspect; ]
Factor[G[x]]
:[font = text; inactive; preserveAspect; ]
Alternately, we can use the ``afterthought'' notation:
:[font = input; preserveAspect; ]
G[x] //Factor
:[font = input; preserveAspect; ]
R = G[x]/(x^4 + 3)
:[font = input; preserveAspect; ]
Plot[R, {x, -20, 20}]
:[font = input; preserveAspect; ]
Plot[R, {x, 10, 20}]
:[font = input; preserveAspect; endGroup; endGroup; ]
Plot[R, {x, 100, 200}]
^*)