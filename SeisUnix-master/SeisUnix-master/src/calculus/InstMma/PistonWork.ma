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
The Piston Problem
:[font = section; inactive; Cclosed; preserveAspect; startGroup; ]
Exercise 1
:[font = text; inactive; preserveAspect; ]
Since angular velocity is  2 rad/sec and radius is  1 (let's say) foot;  P moves at 2 ft/sec.
Starting with  L^2 = x^2 + (q - y)^2  one gets, since  P = ( cos 2t, sin 2t):
;[s]
3:0,0;109,1;130,2;171,-1;
3:1,11,8,Times,0,12,0,0,0;1,10,8,Courier,1,12,0,0,0;1,11,8,Times,0,12,0,0,0;
:[font = input; preserveAspect; ]
q[t_] := Sin[2t] + Sqrt[L^2 - Cos[2t]^2]
:[font = input; preserveAspect; endGroup; ]
q'[t]
:[font = section; inactive; Cclosed; preserveAspect; startGroup; ]
Exercise 2
:[font = text; inactive; preserveAspect; endGroup; ]
Typical responses:   I would expect  q  to go up and down, much like a mass on a spring.
q will reach its max. at (0, 1) and its min. at (0, -1), so q' = 0 at these points.  I would expect q' to reach its max. = 2  at (1, 0) 
:[font = section; inactive; Cclosed; preserveAspect; startGroup; ]
Exercise 3
:[font = text; inactive; preserveAspect; ]
q' is in Ex. 1 above.  Now for "large" L:
:[font = input; preserveAspect; ]
L = 2;
Plot[{q[t], q'[t]}, {t, 0, 2Pi},
	  PlotStyle -> {GrayLevel[0], GrayLevel[.5]}]
:[font = text; inactive; preserveAspect; ]
Note that q' exceed 2.0; a bit of a surprise.
Now for "small" L:
:[font = input; preserveAspect; ]
L = 1.01;
Plot[{q[t], q'[t]}, {t, 0, 2Pi},
	  PlotStyle -> {GrayLevel[0], GrayLevel[.5]}]
:[font = text; inactive; preserveAspect; ]
For L almost 1.0, there are some surprises.   For 0 <= t <= pi/2  Q moves up and down as expected.  But  Q  stays near (0, 0)  for a substantial time period when  P  is near the  x-axis.
Also  q'  spikes e.g. just as  P  moves above  (1, 0); and exceeds 2.0 considerably.
We now take  L  very close to 1.0 and observe.
:[font = input; preserveAspect; ]
L = 1.0001;
Plot[{q[t], q'[t]}, {t, 0, 2Pi},
	  PlotStyle -> {GrayLevel[0], GrayLevel[.5]}]
:[font = text; inactive; preserveAspect; endGroup; endGroup; ]
And it seems q' is approaching a max. of 4.0.  To prove this rigorously, would not be easy.
For example, one wants  limit as  L->  1 of |q'_max|; but we observe that as  L -> 1.0  the
t-value is approaching  0.  So some sort of double (or coupled) limit is called for.
^*)