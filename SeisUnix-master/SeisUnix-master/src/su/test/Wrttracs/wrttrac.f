************************************************************************
* Make some traces to test suahed
************************************************************************
                  
      	integer i, j, ntr, ns, stdout, btf
      	parameter (ntr = 9, ns = 64, stdout = 6, btf = 8)
	real x(1:ns)

*     	...Open output bare trace file.
 10  	open(unit = btf, file = 'BTRACES', access = 'sequential',
     :	     form = 'unformatted', status = 'new', err = 20)
      	go to 30
 20     write(stdout, '(a)') ' ***Can not open bare trace file.'
        stop

 30	do 50 j = 1, ntr
      		do 40 i = 1, ns
			x(i) = 0.0
 40   		continue
		x(8) = 1.0
         	write(btf) (x(i), i = 1, ns) 
 50	continue

      	stop
      	end
