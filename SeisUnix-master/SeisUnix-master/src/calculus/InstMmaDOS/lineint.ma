(*^

::[paletteColors = 128; automaticGrouping; currentKernel; 
	fontset = title, inactive, noPageBreakBelow, nohscroll, preserveAspect, groupLikeTitle, center, M7, bold, L1, e8,  24, "Times"; ;
	fontset = subtitle, inactive, noPageBreakBelow, nohscroll, preserveAspect, groupLikeTitle, center, M7, bold, L1, e6,  18, "Times"; ;
	fontset = subsubtitle, inactive, noPageBreakBelow, nohscroll, preserveAspect, groupLikeTitle, center, M7, italic, L1, e6,  14, "Times"; ;
	fontset = section, inactive, noPageBreakBelow, nohscroll, preserveAspect, groupLikeSection, grayBox, M22, bold, L1, a20,  18, "Times"; ;
	fontset = subsection, inactive, noPageBreakBelow, nohscroll, preserveAspect, groupLikeSection, blackBox, M19, bold, L1, a15,  14, "Times"; ;
	fontset = subsubsection, inactive, noPageBreakBelow, nohscroll, preserveAspect, groupLikeSection, whiteBox, M18, bold, L1, a12,  12, "Times"; ;
	fontset = text, inactive, nohscroll, noKeepOnOnePage, preserveAspect, M7, L1,  16, "Times"; ;
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
	fontset = help, inactive, nohscroll, noKeepOnOnePage, preserveAspect, M7, L1,  18, "Times"; ;
	fontset = clipboard, inactive, nohscroll, noKeepOnOnePage, preserveAspect, M7, L1,  12;
	fontset = completions, inactive, nohscroll, noKeepOnOnePage, preserveAspect, M7, L1,  12, "Courier"; ;
	fontset = special1, inactive, nohscroll, noKeepOnOnePage, preserveAspect, M7, L1,  12;
	fontset = special2, inactive, nohscroll, noKeepOnOnePage, preserveAspect, M7, L1,  12;
	fontset = special3, inactive, nohscroll, noKeepOnOnePage, preserveAspect, M7, L1,  12;
	fontset = special4, inactive, nohscroll, noKeepOnOnePage, preserveAspect, M7, L1,  12;
	fontset = special5, inactive, nohscroll, noKeepOnOnePage, preserveAspect, M7, L1,  12;]
:[font = title; inactive; preserveAspect; startGroup; ]
Line Integrals and Work
:[font = section; inactive; Cclosed; preserveAspect; startGroup; ]
Exercise 1.
:[font = text; inactive; preserveAspect; ]
We will drop the letter `a' for the calculations:
:[font = input; preserveAspect; ]
x[t_] := 2t;
y[t_] := t;
FT[t_] := x'[t] + y'[t] Exp[c x[t]- y[t]];
c = 0;
:[font = text; inactive; preserveAspect; ]
St. line path with c = 0:
:[font = input; preserveAspect; ]
work = Integrate[FT[t], {t, 0, 1}] //N
:[font = text; inactive; preserveAspect; ]
Integrating "up, the over", noting x = 0 on "up" part, 
and y' = 0 on "over" part:
:[font = input; preserveAspect; ]
Clear[c];
work = Integrate[Exp[0 - y[t]], {t, 0, 1}] + 2
:[font = input; preserveAspect; endGroup; ]
N[work]
:[font = section; inactive; Cclosed; preserveAspect; startGroup; ]
Exercise 2.
:[font = text; inactive; preserveAspect; ]
The Parabolic paths, first coming in horizontally:
:[font = input; preserveAspect; ]
c = 0.1;
alf = 1/4;
beta = (1 + 4alf)/2;

x[t_] := 2t;
y[t_] := 2t(beta - 2 alf t)
FT[t_] := x'[t] + y'[t] Exp[c x[t]- y[t]];
WorkStLine = Integrate[FT[t], {t, 0,1}] //N
ParametricPlot[{x[t],y[t]}, {t, 0, 1}]
 
:[font = text; inactive; preserveAspect; ]
A  parabolic path with 'large' alf, swinging north.
Note Work decreases:
:[font = input; preserveAspect; ]
c = 0.1;
alf = 3;
beta = (1 + 4alf)/2;

x[t_] := 2t;
y[t_] := 2t(beta - 2 alf t)
FT[t_] := x'[t] + y'[t] Exp[c x[t]- y[t]];
WorkStLine = Integrate[FT[t], {t, 0,1}] //N
ParametricPlot[{x[t],y[t]}, {t, 0, 1}]
 
:[font = text; inactive; preserveAspect; ]
Now for alf < 0; not a good idea:
:[font = input; preserveAspect; endGroup; ]
c = 0.1;
alf = -2;
beta = (1 + 4alf)/2;

x[t_] := 2t;
y[t_] := 2t(beta - 2 alf t)
FT[t_] := x'[t] + y'[t] Exp[c x[t]- y[t]];
WorkStLine = Integrate[FT[t], {t, 0,1}] //N
ParametricPlot[{x[t],y[t]}, {t, 0, 1}]
 
:[font = section; inactive; Cclosed; preserveAspect; startGroup; ]
Exercise 3.
:[font = text; inactive; preserveAspect; ]
Here's a circuitous path from (0,0) to (2,1);
:[font = input; preserveAspect; ]
x[t_] := 2t +  Sin[2Pi t]
y[t_] := t^2 + 1 -  Cos[2Pi t]
ParametricPlot[{x[t],y[t]}, {t, 0, 1}]

:[font = input; preserveAspect; ]
arclength = NIntegrate[Sqrt[x'[t]^2 + y'[t]^2], {t, 0, 1}]
:[font = text; inactive; preserveAspect; ]
Note:  best to use NIntegrate
:[font = input; preserveAspect; ]
c = 0.1;
work = NIntegrate[x'[t] + y'[t]Exp[c x[t] -y[t]], 
		{t, 0, 1}]
:[font = text; inactive; preserveAspect; ]
Bigger, but not by much.
:[font = input; preserveAspect; ]
c = 0;
work = NIntegrate[x'[t] + y'[t]Exp[c x[t] -y[t]], 
		{t, 0, 1}]
:[font = text; inactive; preserveAspect; endGroup; endGroup; ]
The old 'independence of path' result.
^*)