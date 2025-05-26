% model builder %
% make the grid, physical properties, initial conditions, and source waveform %

clear all;
% make vector of P-wave velocities, S-wave velocities, density, and the grid spacings %
alpha = 2500*ones(1,2000)-0*600*[ zeros(1,1396) ones(1,604)]+0*600*[ zeros(1,1405) ones(1,595)];
beta = 1250*ones(1,2000)-0*600*[ zeros(1,1396) ones(1,604)];
rho = 2000*ones(1,2000)-0*100*[ zeros(1,1396) ones(1,604)]+0*100*[ zeros(1,1405) ones(1,595)];
hvec = ones(1,1999); %m%

% column vectors get written out as columns %
alpha = alpha';
beta = beta';
rho = rho';
hvec = hvec';

% save these in input files %
save 'alpha.txt' alpha -ascii -double
save 'beta.txt' beta -ascii -double
save 'rho.txt' rho -ascii -double
save 'grid.txt' hvec -ascii -double

% make some initial conditions for a P- or an S-wave for a uniform grid %
% upgoing P-wave %
p = 0.00028868; % ray parameter %
vp = alpha(500); % p-velocity at gridpoint 1500 %
ka = 1/(2*pi*pi*.007*.007); % constant related to peak spatial wavenumber (=.007) %
h = 1; % the grid spacing, here uniform and equal to one %
zo = 500; % initial location of P-wave in spatial distance (need to know the grid-spacing) %
uinit = (-2/(ka^3))*([1:2000]*h-zo).*exp((-([1:2000]*h-zo).^2)/(ka))*((vp*p)/sqrt(1-((vp*p)^2)));
winit = (-2/(ka^3))*([1:2000]*h-zo).*exp((-([1:2000]*h-zo).^2)/(ka));
utinit = (-1/(p*sqrt((1/((vp*p)^2))-1)))*(1/(ka^3))*(-2 + 4*((([1:2000]*h-zo).^2)/(ka))).*...
         exp((-([1:2000]*h-zo).^2)/(ka))*((vp*p)/sqrt(1-((vp*p)^2)));
wtinit = (-1/(p*sqrt((1/((vp*p)^2))-1)))*(1/(ka^3))*(-2 + 4*((([1:2000]*h-zo).^2)/(ka))).*exp((-([1:2000]*h-zo).^2)/(ka));

% column vectors get written out as columns %
uinit = uinit';
utinit = utinit';
winit = winit';
wtinit = wtinit';

% save these in input files %
save 'uinit.txt' uinit -ascii -double
save 'winit.txt' winit -ascii -double
save 'utinit.txt' utinit -ascii -double
save 'wtinit.txt' wtinit -ascii -double

% make a user supplied source waveform %
%k = 0.0002; % the time step to be used in the simulation %
%fa = 1/(2*pi*pi*10*10); % constant related to peak spatial wavenumber (=.007) %
%tdel = .1; % initial location of P-wave in gridpoints %
%srcw = (-2 + 4*((([1:20000]*k-tdel).^2)/(fa))).*exp((-([1:20000]*k-tdel).^2)/(fa));

% make into a column %
%srcw = srcw';

% save it %
%save 'srcwav.txt' srcw -ascii -double
