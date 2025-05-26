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
Techniques of Integration
:[font = section; inactive; Cclosed; preserveAspect; startGroup; ]
Integrals with Powers of Trig Functions
:[font = subsection; inactive; Cclosed; preserveAspect; startGroup; ]
Sines and Cosines
:[font = input; preserveAspect; ]
Integrate[Cos[x]^3 Sin[x]^2, x]
:[font = input; preserveAspect; ]
Expand[1/3 Sin[x]^3 - 1/5 Sin[x]^5, Trig -> True]
:[font = text; inactive; preserveAspect; ]
Need to load in Trig package for TrigReduce..
:[font = input; preserveAspect; ]
<<Algebra`Master`
:[font = input; preserveAspect; ]
Integrate[Cos[x]^3 Sin[x]^2, x] //TrigReduce //Expand
:[font = input; preserveAspect; ]
Integrate[Sin[x]^3 Cos[x]^2, x]
:[font = input; preserveAspect; ]
Integrate[Sin[x]^3 Cos[x]^2, x] //TrigReduce //Expand
:[font = input; preserveAspect; ]
f[x_] := Cos[x]^9
Integrate[f[x], x]
% //TrigReduce //Expand
:[font = input; preserveAspect; ]
Integrate[Sin[x]^2 Cos[x]^2, x]
% //TrigReduce //Expand
:[font = input; preserveAspect; ]
f[x_] := Sin[x]^6
Integrate[f[x], x]
:[font = input; preserveAspect; endGroup; ]
handanswer =
	1/8 (5x/2 - 2 Sin[2x] + 3 Sin[4x]/8 + (Sin[2x]^3)/6);
Expand[handanswer, Trig -> True]
:[font = subsection; inactive; Cclosed; preserveAspect; startGroup; ]
Tans and Secs
:[font = input; preserveAspect; ]
Integrate[Sec[2x]^6, x]
:[font = subsubsection; inactive; preserveAspect; startGroup; ]
We see that Mathematica doesn't quite do what we'd like with Tangents and
Secants in this problem.  We can force it to do so by defining a rule (you are
not expected to understand the details) to convert even powers of Sec to Tan:
;[s]
3:0,0;12,1;23,2;231,-1;
3:1,10,8,Times,1,12,0,0,0;1,10,8,Times,2,12,0,0,0;1,10,8,Times,1,12,0,0,0;
:[font = input; preserveAspect; ]
sectotan =
	Sec[m_. x_]^n_Integer?Positive -> 
		(1 - Tan[m x]^2)^Quotient[n,2] Sec[m x]^Mod[n,2];
:[font = input; preserveAspect; ]
Integrate[Sec[2x]^6, x] /. sectotan // Expand
:[font = input; preserveAspect; ]
Integrate[Tan[x]^3 Sec[x]^3, x]
:[font = input; preserveAspect; ]
Integrate[Tan[x]^3 Sec[x]^3, x] /. sectotan // Expand
:[font = input; preserveAspect; ]
Integrate[Tan[x]^6, x]
:[font = input; preserveAspect; endGroup; endGroup; endGroup; ]
Integrate[Tan[x]^6, x] /. sectotan // Expand
:[font = section; inactive; Cclosed; preserveAspect; startGroup; ]
Sqrt[a^2 - x^2] Integrals
:[font = input; preserveAspect; ]
Integrate[1/Sqrt[a^2-x^2], x]
:[font = input; preserveAspect; ]
Integrate[1/Sqrt[a^2-x^2], x] //PowerExpand
:[font = input; preserveAspect; ]
Integrate[x^2/Sqrt[a^2-x^2], x] //PowerExpand
:[font = input; preserveAspect; ]
Integrate[x^4/Sqrt[a^2-x^2], x] //PowerExpand
:[font = input; preserveAspect; endGroup; ]
Integrate[x^6/Sqrt[a^2-x^2], x] //PowerExpand
:[font = section; inactive; Cclosed; preserveAspect; startGroup; ]
Hard Integral
:[font = text; inactive; preserveAspect; ]
Mathematica can not do the original integral:
;[s]
2:0,0;11,1;45,-1;
2:1,13,10,Times,2,14,0,0,0;1,13,10,Times,0,14,0,0,0;
:[font = input; preserveAspect; ]
integrand1 = (Sqrt[1 - Cos[t]]) /Cos[t];
Integrate[integrand1, t]
:[font = text; inactive; preserveAspect; ]
Carry out the substitution u = cos t:
:[font = input; preserveAspect; ]
Integrate[-1/(u Sqrt[1 + u]), u]
:[font = text; inactive; preserveAspect; ]
v = sqrt(1 + u) :
:[font = input; preserveAspect; ]
Integrate[2/(1 - v^2), v]
:[font = input; preserveAspect; endGroup; ]
% /. v -> Sqrt[1 + u] /. u -> Cos[t]
:[font = section; inactive; Cclosed; preserveAspect; startGroup; ]
Method of Substitution in Mathematica
:[font = text; inactive; preserveAspect; ]
This is just for the Instructors.  We know that the differential is a formal guide to change of variables in an integral: f(x) dx becomes f(g(u)) g'(u) du under the one-to-one change of variables x=g(u).  The operator \verb:Dt[]: can be used to represent the differential in \Mma:
:[font = input; preserveAspect; ]
Dt[f[x]]
:[font = text; inactive; preserveAspect; ]
Illustrative Example:  Simplify the integral of 1/( 1+e^(2x) ) by making the change of variables u = e^x.  To apply the \verb:Dt[]: operator, we need the substitution in the form x = g(u) (inverse function!), that is, x = ln u:
:[font = input; preserveAspect; ]
Dt[x]/Sqrt[1 + E^(2x)] /. x -> Log[u]
:[font = input; preserveAspect; ]
integrand1 = 
	(Sqrt[1 - Cos[t]])/Cos[t] Dt[t] /. t -> ArcCos[u] //
	Simplify
:[font = text; inactive; preserveAspect; ]
Forcing Mma to dump that annoying factor:
:[font = input; preserveAspect; ]
integrand1 = Sqrt[(integrand1/Dt[u])^2] Dt[u] //PowerExpand
:[font = input; preserveAspect; endGroup; endGroup; ]
integrand2 = integrand1 /. u -> v^2 - 1 //PowerExpand
^*)