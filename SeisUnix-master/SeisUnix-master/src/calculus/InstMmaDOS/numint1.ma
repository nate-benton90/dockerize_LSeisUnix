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
Numerical Integration I
:[font = section; inactive; Cclosed; preserveAspect; startGroup; ]
Exercise 2
:[font = input; preserveAspect; ]
f[x_] := x^4 + 1
:[font = input; preserveAspect; ]
Integrate[f[x], {x, 0, 5}]
:[font = input; preserveAspect; endGroup; ]
a = 0.0; b = 5.0;
f[x_] := x^4 + 1
:[font = section; inactive; Cclosed; preserveAspect; startGroup; ]
Exercise 4
:[font = input; preserveAspect; ]
n = 5;  delx = (b - a)/n;
Rsum =  delx Sum[f[a + i delx] , {i, 0, n-1}] //N
:[font = input; preserveAspect; ]
n = 10;  delx = (b - a)/n;
Rsum =  delx Sum[f[a + i delx] , {i, 0, n-1}] //N
:[font = input; preserveAspect; endGroup; ]
n = 100;  delx = (b - a)/n;
Rsum =  delx Sum[f[a + i delx] , {i, 0, n-1}] //N
:[font = section; inactive; Cclosed; preserveAspect; startGroup; ]
Exercise 5
:[font = input; preserveAspect; ]
exact = 630;
n = 10;
delx = (b - a)/n;
p = 1;
Rsum = delx Sum[f[a + (i+p)delx], {i, 0, n-1}] //N
:[font = input; preserveAspect; endGroup; ]
exact = 630;
n = 100;
delx = (b - a)/n;
p = 1;
Rsum = delx Sum[f[a + (i+p)delx], {i, 0, n-1}] //N
:[font = section; inactive; Cclosed; preserveAspect; startGroup; ]
Exercise 6
:[font = text; inactive; preserveAspect; ]
The straightforward thing is to do nine runs of this type:
:[font = input; preserveAspect; ]
a = 0.0; b = 5.0;
f[x_] := x^4 + 1
exact = 630;
n = 10;
delx = (b - a)/n;
p = 0;
Rsum = delx Sum[f[a + (i+p)delx], {i, 0, n-1}] //N;
exact - Rsum
:[font = text; inactive; preserveAspect; ]
Here's a wizard solution:
:[font = input; preserveAspect; ]
Clear[Rsum];
Rsum[f_, a_, b_, n_, p_] := 
Module[
	{delx = (b-a)/n},
	delx Sum[f[a + (i+p)delx], {i, 0, n-1}] //N
]
:[font = input; preserveAspect; ]
f[x_] := x^4 + 1
a = 0.0; b = 5.0;
exact = 630;
TableForm[
	Table[
		{exact - Rsum[f,a,b,10^k,0],
		 exact - Rsum[f,a,b,10^k,0.5],
		 exact - Rsum[f,a,b,10^k,1]},
	{k, 1, 3}]
]
:[font = text; inactive; preserveAspect; ]
Or if we want to build in the headings as well:
:[font = input; preserveAspect; endGroup; ]
f[x_] := x^4 + 1
a = 0.0; b = 5.0;
exact = 630;
TableForm[
	Table[
		{exact - Rsum[f,a,b,10^k,0],
		 exact - Rsum[f,a,b,10^k,0.5],
		 exact - Rsum[f,a,b,10^k,1]},
	{k, 1, 3}],
	TableHeadings ->
		{{"10", "100", "1000"}, {"0.", "0.5", "1.0"}}
]
:[font = section; inactive; Cclosed; preserveAspect; startGroup; ]
Exercise 7
:[font = input; preserveAspect; endGroup; endGroup; ]
f[x_] := Sqrt[100 - x^2]
a = 0.0; b = 10.0;
exact = 100Pi/4 //N;
TableForm[
	Table[
		{exact - Rsum[f,a,b,10^k,0],
		 exact - Rsum[f,a,b,10^k,0.5],
		 exact - Rsum[f,a,b,10^k,1]},
	{k, 1, 3}],
	TableHeadings ->
		{{"10", "100", "1000"}, {"0.", "0.5", "1.0"}}
]
^*)