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
Integration in Mathematica
:[font = section; inactive; Cclosed; preserveAspect; startGroup; ]
Exercise 1
:[font = input; preserveAspect; endGroup; ]
Table[Integrate[x^n, {x, -3, 3}], {n, 1, 5, 2}]
:[font = section; inactive; Cclosed; preserveAspect; startGroup; ]
Normal Probability
:[font = input; preserveAspect; ]
n[z_] := 1/Sqrt[2Pi] Exp[-z^2/2]
:[font = input; preserveAspect; ]
Plot[n[z], {z, -3.2, 3.2},
	Ticks -> {
	   {{-3, "-3"}, {-2, "-2"}, {-1, "-1"},
	   	{1, "1"}, {2, "2"}, {3, "3"}},
	   Automatic
	}
]
:[font = input; preserveAspect; ]
P[k_] := Sqrt[2/Pi] Integrate[Exp[-z^2/2], {z, 0, k}]
:[font = input; preserveAspect; ]
Plot[P[k], {k, -3.2, 3.2},
	Ticks -> {
	   {{-3, "-3"}, {-2, "-2"}, {-1, "-1"},
	   	{1, "1"}, {2, "2"}, {3, "3"}},
	   Automatic
	}
]
:[font = text; inactive; preserveAspect; ]
The following illustrates a bug in our version of Mathematica.  The integral should be 1.
;[s]
3:0,0;50,1;61,2;89,-1;
3:1,13,10,Times,0,14,0,0,0;1,13,10,Times,2,14,0,0,0;1,13,10,Times,0,14,0,0,0;
:[font = input; preserveAspect; ]
n[x_] := 1/Sqrt[2Pi sigma^2] Exp[-(x - mu)^2/(2 sigma^2)]
Integrate[n[x], {x, -Infinity, Infinity}]
:[font = input; preserveAspect; ]
n0[x_] := n[x]/. {mu -> 0, sigma -> 1}
Integrate[n0[x], {x, -Infinity, Infinity}]
:[font = input; preserveAspect; ]
p[z_] := 1/Sqrt[2Pi] Exp[-z^2/2]
Integrate[p[z], {z, -Infinity, Infinity}]
:[font = input; preserveAspect; ]
Integrate[p[z], {z, -1, 1}]
:[font = input; preserveAspect; ]
Integrate[p[z], z]
:[font = input; preserveAspect; endGroup; ]
onesigma = Integrate[p[z], {z, -1, 1}] //N
:[font = section; inactive; Cclosed; preserveAspect; startGroup; ]
Method of Substitution with Mathematica
:[font = text; inactive; preserveAspect; ]
We know that the differential is a formal guide to change of variables in an integral: f(x) dx becomes f(g(u)) g'(u) du under the one-to-one change of variables x=g(u).
:[font = text; inactive; preserveAspect; ]
Illustrative Example:  Simplify the integral of 1/( 1+e^(2x) ) by making the change of variables u = e^x.  To apply the above theory, we need the substitution in the form x = g(u) (inverse function!), that is, x = ln u:
:[font = input; preserveAspect; ]
f[x_] := 1/(1 + Exp[2x])
g[u_] := Log[u]      (* substitution: x = g[u] *)
integrand[u_] = f[g[u]] g'[u]  (* as in theory *)
:[font = input; preserveAspect; ]
answeru = Integrate[integrand[u], u]
ginverse[x_] := Exp[x]  (* inverse -- to get back *)
answerx = answeru /. u -> ginverse[x] //PowerExpand
:[font = text; inactive; preserveAspect; ]
In this case Mma could do the original integral:
:[font = input; preserveAspect; ]
Integrate[f[x], x]
:[font = text; inactive; preserveAspect; ]
Repeat with limits 0, 1 ...  Now do not have to return to x, but we still need the `inverse' function ... for the limits!  Starting from integrand[u] again:
:[font = input; preserveAspect; endGroup; ]
ginverse[x_] := Exp[x]  (* inverse -- for limits *)
a = 0; b = 1;
A = ginverse[a]
B = ginverse[b]
Integrate[integrand[u], {u, A, B}]
:[font = section; inactive; Cclosed; preserveAspect; startGroup; ]
Exercise 7
:[font = input; preserveAspect; endGroup; ]
Plot[{1/2 Sin[x]^2, -1/2 Cos[x]^2}, {x, 0, 2Pi}]
:[font = section; inactive; Cclosed; preserveAspect; startGroup; ]
Exercise 8
:[font = input; preserveAspect; ]
P[k_] := Sqrt[2/Pi] Integrate[Exp[-z^2/2], {z, 0, k}]
:[font = input; preserveAspect; endGroup; ]
Table[{k, N@P[k]}, {k, 3}] //TableForm
:[font = section; inactive; Cclosed; preserveAspect; startGroup; ]
Exercise 9
:[font = input; preserveAspect; startGroup; ]
v = .9;
kn = 1.5;
Do[
	kn = N[kn - (P[kn] - v)/P'[kn]];
	Print[kn],
{6}]
:[font = print; inactive; preserveAspect; endGroup; ]
1.62977
1.64467
1.64485
1.64485
1.64485
1.64485
:[font = input; preserveAspect; ]
P[1.64485]//N
:[font = input; preserveAspect; startGroup; ]
v = .95;
kn = 2.0;
Do[
	kn = N[kn - (P[kn] - v)/P'[kn]];
	Print[kn],
{6}]
:[font = print; inactive; preserveAspect; endGroup; ]
1.95833
1.95996
1.95996
1.95996
1.95996
1.95996
:[font = input; preserveAspect; ]
P[1.95996]//N
;[s]
3:0,0;2,1;9,2;13,-1;
3:1,10,8,Courier,1,12,0,0,0;1,10,8,Courier,0,12,0,0,0;1,10,8,Courier,1,12,0,0,0;
:[font = input; preserveAspect; startGroup; ]
v = .99;
kn = 3.0;
Do[
	kn = N[kn - (P[kn] - v)/P'[kn]];
	Print[kn],
{6}]
:[font = print; inactive; preserveAspect; endGroup; ]
2.17639
2.43775
2.55465
2.57527
2.57583
2.57583
:[font = input; preserveAspect; endGroup; ]
P[2.57583]//N
;[s]
3:0,0;2,1;9,2;13,-1;
3:1,10,8,Courier,1,12,0,0,0;1,10,8,Courier,0,12,0,0,0;1,10,8,Courier,1,12,0,0,0;
:[font = section; inactive; Cclosed; preserveAspect; startGroup; ]
Exercise 10
:[font = text; inactive; preserveAspect; ]
Mathematica can not do the original integral:
;[s]
2:0,0;11,1;45,-1;
2:1,13,10,Times,2,14,0,0,0;1,13,10,Times,0,14,0,0,0;
:[font = input; preserveAspect; ]
f[x_] := Cos[z Cos[x]]  (* integrand *)
g[u_] := ArcCos[u]      (* substitution: x = g[u] *)
ginverse[x_] := Cos[x]  (* inverse -- for limits *)
integrand[u_] = f[g[u]] g'[u]  (* as in theory *)
:[font = input; preserveAspect; ]
a = 0; b = Pi/2;
A = ginverse[a]
B = ginverse[b]
:[font = input; preserveAspect; ]
h[z_] = Integrate[integrand[u], {u, A, B}]
:[font = input; preserveAspect; endGroup; ]
Plot[h[z], {z, -12, 12}]
:[font = section; inactive; Cclosed; preserveAspect; startGroup; ]
Substitution with Dt--Instructors Only
:[font = text; inactive; preserveAspect; ]
 We know that the differential is a formal guide to change of variables in an integral: f(x) dx becomes f(g(u)) g'(u) du under the one-to-one change of variables x=g(u).  The operator Dt[]: can be used to represent the differential in Mathematica:
;[s]
3:0,0;235,1;246,2;248,-1;
3:1,13,10,Times,0,14,0,0,0;1,13,10,Times,2,14,0,0,0;1,11,8,Times,0,12,0,0,0;
:[font = input; preserveAspect; ]
Clear[f, x]
Dt[f[x]]
:[font = subsection; inactive; Cclosed; preserveAspect; startGroup; ]
Illustrative Example
:[font = text; inactive; preserveAspect; ]
Illustrative Example:  Simplify the integral of 1/( 1+e^(2x) ) by making the change of variables u = e^x.  To apply the Dt[]: operator, we need the substitution in the form x = g(u) (inverse function!), that is, x = ln u:
:[font = input; preserveAspect; endGroup; ]
Dt[x]/Sqrt[1 + E^(2x)] /. x -> Log[u]
:[font = subsection; inactive; Cclosed; preserveAspect; startGroup; ]
The `Hard Integral' of Techniques Project
:[font = input; preserveAspect; ]
integrand1 = 
	(Sqrt[1 - Cos[t]])/Cos[t] Dt[t] /. t -> ArcCos[u] //
	Simplify
:[font = text; inactive; preserveAspect; ]
Forcing Mma to dump that annoying factor:
:[font = input; preserveAspect; endGroup; ]
integrand1 = Sqrt[(integrand1/Dt[u])^2] Dt[u] //PowerExpand
:[font = subsection; inactive; Cclosed; preserveAspect; startGroup; ]
Subst with Dt for normal
:[font = input; preserveAspect; ]
n[x_] := 1/Sqrt[2Pi sigma^2] Exp[-(x - mu)^2/(2 sigma^2)]
dn = n[x] Dt[x]
:[font = input; preserveAspect; ]
dn /. x -> mu + sigma u
:[font = input; preserveAspect; ]
dn = n[x] Dt[x, Constants -> {mu, sigma}]
:[font = input; preserveAspect; ]
dn /. x -> mu + sigma u
:[font = input; preserveAspect; endGroup; ]
dn /. x -> mu + sigma u //PowerExpand
:[font = subsection; inactive; preserveAspect; startGroup; ]
Reference
:[font = text; inactive; preserveAspect; ]
We first saw this methodology in the Mathematica Journal, Volume 3, Issue 3, Summer 1993  in the ``Tricks of the Trade'' column edited by Paul Abbott.  Here our notes on that entry:
:[font = subsubsection; inactive; Cclosed; preserveAspect; startGroup; ]
Change of Variables
:[font = text; inactive; preserveAspect; ]
The integral
:[font = input; preserveAspect; ]
Integrate[Cos[k Cos[phi]], {phi, 0, Pi/2}]
:[font = text; inactive; preserveAspect; ]
is not directly computable in Mathematica version 2.1.  However, the change of variable, Cos[phi] -> z leads to the integrand
:[font = input; preserveAspect; ]
Cos[k Cos[phi]] Dt[phi] /. phi -> ArcCos[z]
:[font = text; inactive; preserveAspect; ]
    Since the range of phi is from 0 to Pi/2 the integration limits are 1 and 0 respectively.  Hence:
:[font = input; preserveAspect; endGroup; ]
Integrate[% / Dt[z], {z, 1, 0}] //PowerExpand
:[font = subsubsection; inactive; Cclosed; preserveAspect; startGroup; ]
Remarks
:[font = input; preserveAspect; ]
$Version
:[font = text; inactive; preserveAspect; ]
1. Apparently, in version 2.1, that final answer came out with a ``k'' instead of the ``Abs[k]'' we got above. 
2. Recall what  Dt means when it doesn't have a second argument.  It means TOTAL DIFFERENTIAL:
:[font = input; preserveAspect; ]
Dt[x^2 + y^2 + z^2]
:[font = text; inactive; preserveAspect; ]
3. We found the technique hard to follow, since it starts by indicating the substitution as z = Cos[phi], but really gives the principal role to  the inverse transformation phi = ArcCos[z].  Let us try to say it with more familiar letters:
:[font = input; preserveAspect; ]
f[x_] := Cos[k Cos[x]]  (* integrand *)
g[u_] := ArcCos[u]      (* substitution: x = g[u] *)
ginverse[x_] := Cos[x]  (* inverse -- for limits *)
integrandx[x_] = (f[x] dx) / dx  (* not really used *)
integrandu[u_] = (f[x] Dt[x] /. x -> g[u]) / Dt[u]

:[font = input; preserveAspect; ]
a = 0; b = Pi/2;
A = ginverse[a]
B = ginverse[b]
:[font = input; preserveAspect; ]
Integrate[integrandu[u], {u, A, B}]
:[font = text; inactive; preserveAspect; ]
Even better, avoid introducing the meaningless du and the less familiar Dt all together, to obtain the teachable code:
:[font = input; preserveAspect; ]
f[x_] := Cos[k Cos[x]]  (* integrand *)
g[u_] := ArcCos[u]      (* substitution: x = g[u] *)
ginverse[x_] := Cos[x]  (* inverse -- for limits *)
integrand[u_] = f[g[u]] g'[u]  (* as in theory *)
:[font = input; preserveAspect; ]
a = 0; b = Pi/2;
A = ginverse[a]
B = ginverse[b]
:[font = input; preserveAspect; endGroup; ]
Integrate[integrand[u], {u, A, B}]

:[font = subsubsection; inactive; Cclosed; preserveAspect; startGroup; ]
Some additional note on the `Hard' Integral
:[font = text; inactive; preserveAspect; ]
Mathematica can not do the original integral:
;[s]
2:0,0;11,1;45,-1;
2:1,13,10,Times,2,14,0,0,0;1,13,10,Times,0,14,0,0,0;
:[font = input; preserveAspect; ]
f[x_] := (Sqrt[1 - Cos[x]]) /Cos[x];  (* integrand *)
Integrate[f[x], x]
:[font = text; inactive; preserveAspect; ]
Carry out the substitution u = cos x, to change from f(x) dx to f(g(u)) g'(u) du as the theory dictates, we need it in the inverse form x = g(u) = arccos(u) :
:[font = input; preserveAspect; ]
g[u_] := ArcCos[u]      (* substitution: x = g[u] *)
integrandu[u_] = f[g[u]] g'[u]  (* as in theory *)
:[font = text; inactive; preserveAspect; ]
Now Mma can do the integral:
:[font = input; preserveAspect; ]
answeru = Integrate[integrandu[u], u]
:[font = text; inactive; preserveAspect; ]
Do some post simplification by hand.
:[font = input; preserveAspect; ]
answeruhand = - 2 ArcTanh[-Sqrt[1 + u]]
:[font = input; preserveAspect; ]
answeru /. u -> .3 //N
:[font = input; preserveAspect; ]
answeru /. u -> .7 //N
:[font = text; inactive; preserveAspect; ]
Oh my goodness, the answer is presented as complex!  I guess this isn't the greatest example for Mathematica.
;[s]
3:0,0;97,1;108,2;109,-1;
3:1,13,10,Times,0,14,0,0,0;1,13,10,Times,2,14,0,0,0;1,13,10,Times,0,14,0,0,0;
:[font = input; preserveAspect; ]
answeruhand //ComplexExpand//Simplify
:[font = input; preserveAspect; ]
ginverse[x_] := Cos[x]  (* inverse -- to get back *)
answerx = answeruhand /. u -> ginverse[x]
:[font = input; preserveAspect; ]
answerx /. x -> 0
:[font = input; preserveAspect; ]
answerx /. x -> 0 //N
:[font = input; preserveAspect; ]
Plot[answerx, {x, Pi/2 +.01, 3Pi/2 - .01}]
:[font = input; preserveAspect; endGroup; endGroup; endGroup; endGroup; ]
f[x_] := (Sqrt[1 - Cos[x]]) /Cos[x];  (* integrand *)
Integrate[f[x], {x, Pi/2, Pi}]
^*)