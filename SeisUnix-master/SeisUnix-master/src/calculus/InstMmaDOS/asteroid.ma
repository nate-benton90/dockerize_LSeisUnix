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
Asteroid Problem 
:[font = section; inactive; preserveAspect; startGroup; ]
Exercise 3
:[font = input; preserveAspect; ]
Table[{x, Cos[x] - x}, {x, 0, Pi/2, 0.2}] //TableForm
:[font = input; preserveAspect; ]
Table[{x, Cos[x] - x}, {x, .6, .8, .05}] //TableForm
:[font = input; preserveAspect; endGroup; ]
FindRoot[Cos[x] == x , {x, .6, .8}]
:[font = section; inactive; preserveAspect; startGroup; ]
Asteroid Problem
:[font = subsection; inactive; preserveAspect; startGroup; ]
Bad Start
:[font = input; preserveAspect; endGroup; ]
s = 400.0; h = 1.0;
Plot[Cos[s/r] - r/(r + h), {r, 0, 10}]
:[font = subsection; inactive; preserveAspect; startGroup; ]
Typical Linear Search for Root
:[font = input; preserveAspect; ]
Plot[Cos[s/r] - r/(r + h), {r, 500, 1000}]
:[font = input; preserveAspect; ]
Plot[Cos[s/r] - r/(r + h), {r, 1000, 5000}]
:[font = input; preserveAspect; endGroup; ]
Plot[Cos[s/r] - r/(r + h), {r, 5000, 25000}]
:[font = subsection; inactive; preserveAspect; startGroup; ]
Figure 7 -- Root is bracketed at last
:[font = input; preserveAspect; endGroup; ]
Plot[Cos[s/r] - r/(r + h), {r, 25000, 125000}]
:[font = subsection; inactive; preserveAspect; startGroup; ]
Zooming in on asteroid root near 80,000
:[font = input; preserveAspect; ]
Plot[Cos[s/r] - r/(r + h), {r, 60000, 100000}]
:[font = input; preserveAspect; ]
Plot[Cos[s/r] - r/(r + h), {r, 79500, 80500}]
:[font = input; preserveAspect; ]
s = 400.0; h = 1.0;
Table[{r, Cos[s/r] - r/(r + h)},
{r, 79900, 80050, 10}]// TableForm
:[font = input; preserveAspect; ]
s = 400.0; h = 1.0;
Table[{r, Cos[s/r] - r/(r + h)},
{r, 80000, 80010, 1}]// TableForm
:[font = input; preserveAspect; endGroup; ]
s = 400.0; h = 1.0;
Table[{r, Cos[s/r] - r/(r + h)},
{r, 80000, 80001, 0.1}]// TableForm
:[font = subsection; inactive; preserveAspect; startGroup; ]
Table search based on physical reasoning
:[font = input; preserveAspect; endGroup; ]
s = 400.0; h = 1.0;
Table[{r, Cos[s/r] - r/(r + h)},
{r, 25000, 400000, 50000}]// TableForm
:[font = subsection; inactive; preserveAspect; startGroup; ]
Using FindRoot 
:[font = input; preserveAspect; ]
s = 400.0; h = 1.0;
FindRoot[Cos[s/r] == r/(r + h), {r, 80000}]
:[font = input; preserveAspect; ]
s = 400.0; h = 1.0;
FindRoot[Cos[s/r] == r/(r + h), {r, 100000}]
:[font = input; preserveAspect; ]
s = 400.0; h = 1.0;
FindRoot[Cos[s/r] == r/(r + h), {r, 79654.7}]
:[font = input; preserveAspect; ]
s = 400.0; h = 1.0;
FindRoot[Cos[s/r] == r/(r + h), {r, 79997.9}]
:[font = input; preserveAspect; endGroup; endGroup; endGroup; ]
s = 400.0; h = 1.0;
FindRoot[Cos[s/r] == r/(r + h), {r, 80000.8}]
^*)