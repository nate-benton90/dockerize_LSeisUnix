      	integer i, j, ncurves, npoints, stdout, cf
      	parameter (ncurves = 2, npoints = 6, stdout = 6, cf = 8)
	real x(1:npoints), y(1:npoints)

*     	...Open output curve file.
 10  	open(unit = cf, file = 'LINES', access = 'sequential',
     :	     form = 'unformatted', status = 'new', err = 20)
      	go to 30
 20     write(stdout, '(a)') ' ***Can not open curve file.'
        stop

 30	do 50 j = 1, ncurves
         	write(cf) npoints
      		do 40 i = 1, npoints
			x(i) = float(10*(j-1) + i-1)
			y(i) = float(i-1)
 40   		continue
         	write(cf) (x(i), y(i), i = 1, npoints) 
 50	continue

      	stop
      	end
