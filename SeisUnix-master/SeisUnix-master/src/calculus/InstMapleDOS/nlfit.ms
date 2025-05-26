# Linearization of Nonlinear Data
# Illustrative Example
> ddata := [8.6, 10.7, 11., 11.4, 12.0, 13.3, 14.5, 16.0,17.3,18.0,20.6]:\
vdata := [10.3, 18.8, 18.2, 21.4, 19.1, 27.4, 36.3, 38.3,55.4,51.5,77.]:
> with(plots):\
data := zip( (x,y) -> [x,y], ddata, vdata);\
plot(data, style = POINT);
  data := [[8.6, 10.3], [10.7, 18.8], [11., 18.2], [11.4, 21.4], [12.0, 19.1],

      [13.3, 27.4], [14.5, 36.3], [16.0, 38.3], [17.3, 55.4], [18.0, 51.5],

      [20.6, 77.]]

> with(stats):\
cof := linregress( vdata, ddata);\
linearFit := cof[1] + cof[2]*x;
                       cof := [-41.13128168, 5.385554747]

                   linearFit :=  - 41.13128168 + 5.385554747 x

> lnDdata := map( x -> ln(x), ddata):\
lnVdata := map( x -> ln(x), vdata):\
lndata := zip( (x,y) -> [x,y], lnDdata, lnVdata);\
plot(lndata, style = POINT);
       lndata := [[2.151762203, 2.332143895], [2.370243741, 2.933856870],

           [2.397895273, 2.901421594], [2.433613355, 3.063390922],

           [2.484906650, 2.949688335], [2.587764035, 3.310543013],

           [2.674148649, 3.591817741], [2.772588722, 3.645449896],

           [2.850706502, 4.014579594], [2.890371758, 3.941581808],

           [3.025291076, 4.343805422]]

# The Log data look more linear to me--justifying a power law.
> with(stats):\
cof := linregress( lnVdata, lnDdata);\
linearFit := cof[1] + cof[2]*d;
                       cof := [-2.468542415, 2.241055600]

                   linearFit :=  - 2.468542415 + 2.241055600 d

> grlinear := plot(linearFit, d = 2.152..3.025):\
grdata := plot(lndata, style = POINT):\
display([grdata, grlinear]);
# As in many practical cases, the log transformation provides an excellent result.  We could improve the latter by searching for the best exponent, but  again, only marginally.
# Exercise 4
> adata := [58, 108, 149, 228, 778, 1426]:\
Tdata := [87.97, 224.70, 365.26, 686.98, 4332.59, 10759.20]:\
data := zip( (x,y) -> [x,y], adata, Tdata):\
lnAdata := map( x -> ln(x), adata):\
lnTdata := map( x -> ln(x), Tdata):\
lndata := evalf( zip( (x,y) -> [x,y], lnAdata, lnTdata) );
       lndata := [[4.060443011, 4.476995847], [4.682131227, 5.414766179],

           [5.003946306, 5.900609429], [5.429345629, 6.532305180],

           [6.656726524, 8.373920795], [7.262628601, 9.283516482]]

> cof := linregress( lnTdata, lnAdata);\
linearFit := cof[1] + cof[2]*a;
                       cof := [-1.609897050, 1.499959640]

                   linearFit :=  - 1.609897050 + 1.499959640 a

# The log linear fit would give  y = k x^m where: 
> m := cof[2];
                                m := 1.499959640

# So m is close to the theoretical value.  Let's go after k:
> k := exp(cof[1]);
                                k := .1999081936

# Very close to 0.2.  Let's use m and k and in original data:
> grlinear := plot(k * a^m, a = 58.. 1426):\
grdata := plot(data, style = POINT):\
display([grdata, grlinear]);
# Exercise 5
> xdata := [56, 67.5, 75, 82.5, 90, 100, 110]:\
wdata := [292.5, 340, 375, 377.5, 412.5, 425, 455]:\
data := zip( (x,y) -> [x,y], xdata, wdata):\
lnXdata := map( x -> ln(x), xdata):\
lnWdata := map( x -> ln(x), wdata):\
lndata := evalf( zip( (x,y) -> [x,y], lnXdata, lnWdata) );
       lndata := [[4.025351691, 5.678464667], [4.212127598, 5.828945618],

           [4.317488114, 5.926926026], [4.412798293, 5.933570569],

           [4.499809670, 6.022236206], [4.605170186, 6.052089169],

           [4.700480366, 6.120297419]]

> cof := linregress( lnWdata, lnXdata);\
linearFit := cof[1] + cof[2]*x;
                        cof := [3.161294941, .6315056193]

                    linearFit := 3.161294941 + .6315056193 x

> m := cof[2];\
k := exp( cof[1]);
                                m := .6315056193

                                k := 23.60113823

> grlinear := plot(k * x^m, x = 56.. 110):\
grdata := plot(data, style = POINT):\
display([grdata, grlinear]);
# Exercise 6
> xdata := [0, 20, 40, 60, 80, 100, 120, 140, 160, 170, 180, 190]:\
ydata := [5.3, 9.638, 17.07, 31.44, 50.19, 76.2, 106.0, 132.2, \
           179.3, 203.3, 226.5, 248.7]:\
data := zip( (x,y) -> [x,y], xdata, ydata);
    data := [[0, 5.3], [20, 9.638], [40, 17.07], [60, 31.44], [80, 50.19],

        [100, 76.2], [120, 106.0], [140, 132.2], [160, 179.3], [170, 203.3],

        [180, 226.5], [190, 248.7]]

# Exponential Fit -- y = c E^(ax)
> lnYdata := map( x -> ln(x), ydata):\
lndata := evalf( zip( (x,y) -> [x,y], xdata, lnYdata) );
      lndata := [[0, 1.667706821], [20., 2.265713618], [40., 2.837322537],

          [60., 3.448080968], [80., 3.915815804], [100., 4.333361463],

          [120., 4.663439094], [140., 4.884315927], [160., 5.189060381],

          [170., 5.314682721], [180., 5.422744945], [190., 5.516247351]]

> cof := linregress( lnYdata, xdata);\
linearFit := cof[1] + cof[2]*x;
                       cof := [2.051904286, .01971082554]

                    linearFit := 2.051904286 + .01971082554 x

> a := cof[2];\
c := exp( cof[1]);
                                a := .01971082554

                                c := 7.782707505

> grlinear := plot(c * exp(a*x), x = 0.. 250):\
grdata := plot(data, style = POINT):\
display([grdata, grlinear]);
# Logistic Fit
> M := 300.0:\
Ydata := map( y -> ln(M/y - 1.0), ydata);
   Ydata := [4.018251069, 3.405414802, 2.807876980, 2.144993385, 1.604884825,

       1.077391333, .6044190648, .2384568665, -.3957522530, -.7430693186,

       -1.125459540, -1.578556597]

> cof := linregress( Ydata, xdata);\
linearFit := cof[1] + cof[2]*x;
                       cof := [3.943921183, -.02799063776]

                    linearFit := 3.943921183 - .02799063776 x

> a := -cof[2];\
c := cof[1];
                                a := .02799063776

                                c := 3.943921183

> grlinear := plot( M/(1.0+exp(c - a*x)), x = 0.. 250):\
grdata := plot(data, style = POINT):\
display([grdata, grlinear]);
# Note it is failing at a bad place; at 1990, so try:
> M := 340.0:\
Ydata := map( y -> ln(M/y - 1.0), ydata):\
cof := linregress( Ydata, xdata):\
linearFit := cof[1] + cof[2]*x:\
a := -cof[2]:  c := cof[1]:\
grlinear := plot( M/(1.0+exp(c - a*x)), x = 0.. 250):\
grdata := plot(data, style = POINT):\
display([grdata, grlinear]);
# Looks pretty good.  To find the resulting population at year 2050, use  x = 250:
> subs(x = 250, M/(1.0+exp(c - a*x)));
                                      340.0
                             -----------------------
                             1.0 + exp(-2.545009178)

> evalf(");
                                   315.2607535

> 
