% finite element elastic wave equation in 1D %
% second order homogeneous formulation with a free surface %
% conditionally stable 9/04 %

clear all;
runtim = cputime;

% load the input variables %
alpha = load('alpha.txt');
alpha = alpha';
beta = load('beta.txt');
beta = beta';
rho = load('rho.txt');
rho = rho';
hvec = load('grid.txt');
hvec = hvec';
inp = load('inputs.txt');
    
% set up the internal variables %
xsteps = length(hvec)+1; % total number of grid points %
usrc = zeros(xsteps,1); % vector with nonzero entries at the sources in the u equation of motion %
wsrc = zeros(xsteps,1); % vector with nonzero entries at the sources in the w equation of motion %
p = inp(1); % ray parameter %
for ii=1:inp(18)
    if(inp(17+3*ii) == 1) % a forcing term in the equation of motion for horizontal displacement %
        usrc(inp(16+3*ii)) = inp(18+3*ii);
    elseif(inp(17+3*ii) == 2) % a forcing term in the equation of motion for vertical displacement %
        wsrc(inp(16+3*ii)) = inp(18+3*ii);
    elseif(inp(17+3*ii) == 3) % a purely P-wave upgoing field %
        wsrc(inp(16+3*ii)) = 1;
        usrc(inp(16+3*ii)) = -(p*beta(16+3*ii))/sqrt(1-((p*beta(16+3*ii))^2));
    elseif(inp(17+3*ii) == 4) % a purely P-wave downgoing field %
        wsrc(inp(16+3*ii)) = 1;
        usrc(inp(16+3*ii)) = (p*beta(16+3*ii))/sqrt(1-((p*beta(16+3*ii))^2));
    elseif(inp(17+3*ii) == 5) % a purely S-wave upgoing field %
        usrc(inp(16+3*ii)) = 1;
        wsrc(inp(16+3*ii)) = (p*alpha(16+3*ii))/sqrt(1-((p*alpha(16+3*ii))^2));
    elseif(inp(17+3*ii) == 6) % a purely S-wave downgoing field %
        usrc(inp(16+3*ii)) = 1;
        wsrc(inp(16+3*ii)) = -(p*alpha(16+3*ii))/sqrt(1-((p*alpha(16+3*ii))^2));
    end
end
k = inp(2); % time step interval %
totsteps = inp(3); % total number of time steps %
fa = 1/(2*pi*pi*inp(16)*inp(16)); % a constant related to the peak frequency of the sources %
tdel = inp(17); % time delay until initiation of sources %
sampl = inp(4); % output fields every this number of time steps %
tsteps = floor(totsteps/sampl); % total number of times to output fields %
Ns = inp(6); % number of random fractures %
mindist = inp(7); % minimum distance between fractures %
upperf = inp(8); % upper side of the fracture zone %
lowerf = inp(9); % lower side of the fracture zone %
etat = inp(10); %m/Pa%
etan = inp(11); %m/Pa%
srcw = zeros(1,totsteps);
if(inp(15) == 1) % read in the user supplied source waveform %
    srcw = load('srcwav.txt');
else
end

% check stability %
CFL = max(alpha./sqrt(1-(alpha.*alpha*p*p)))*(k/min(hvec));
disp(sprintf('stability criterion =  %g',CFL))
if (CFL >= 0.5)
    disp(sprintf('instability is possible - terminate this program by <ctrl> C'))
    disp(sprintf('redo with smaller timesteps or larger grid spacing'))
else
    disp(sprintf('simulation is stable - everything okay'))
end

% initialize displacements, velocities, and accelerations %
% velocity potential in the water %
u = zeros(xsteps,2);
ut = u;
utt = u;
w = zeros(xsteps,2);
wt = w;
wtt = w;
upd = zeros(xsteps,2);
wpd = upd;
usd = upd;
wsd = upd;
upu = zeros(xsteps,2);
wpu = upu;
usu = upu;
wsu = upu;
usv = zeros(xsteps,tsteps);
utsv = usv;
uttsv = usv;
wsv = zeros(xsteps,tsteps);
wtsv = wsv;
wttsv = wsv;
dvg = zeros(xsteps,tsteps);
crl = dvg;
uz = zeros(xsteps,1);
wz = zeros(xsteps,1);
sxzsv = zeros(xsteps,tsteps);
szzsv = zeros(xsteps,tsteps);
sxxsv = zeros(xsteps,tsteps);

% load initial conditions if necessary %
if(inp(14) == 1)
    u(:,1) = load('uinit.txt');
    ut(:,1) = load('utinit.txt');
    w(:,1) = load('winit.txt');
    wt(:,1) = load('wtinit.txt');
else
end

% calculate some auxilliary variables, the isotropic moduli %
lambda = ((alpha.^2)-2*(beta.^2)).*rho; %Pa%
mu = (beta.^2).*rho; %Pa%

% these variables are used for the absorbing boundary %
pangleu = asin(p*alpha(1));
appalphau = alpha(1)/cos(pangleu);
appbetau = beta(1)/cos(asin((beta(1)*sin(pangleu))/alpha(1)));
pangled = asin(p*alpha(xsteps));
appalphad = alpha(xsteps)/cos(pangled);
appbetad = beta(xsteps)/cos(asin((beta(xsteps)*sin(pangled))/alpha(xsteps)));

% make this differencer, for the spatial derivatives, better, especially at the edges %
Kdf = diag(1./([hvec(1:(xsteps-2)) 1]+[1 hvec(2:(xsteps-1))]),1);
Kdf = Kdf - diag(1./([hvec(1:(xsteps-2)) 1]+[1 hvec(2:(xsteps-1))]),-1);
Kdf = sparse(Kdf);
Kdf(1,1) = -1/hvec(1);
Kdf(1,2) = 1/hvec(1);
Kdf(xsteps,xsteps) = 1/hvec(xsteps-1);
Kdf(xsteps,xsteps-1) = -1/hvec(xsteps-1);

% make dmu and dlambda using the differencer %
dmu = (Kdf*mu')';
dlam = (Kdf*lambda')';
dpm = dlam + 2*dmu;

% make the matrices %

% 0 means the edges are free by default %
M2 = diag(([0 hvec].*(-(p/12)*([1 dmu(1:(xsteps-1))]./[1 rho(1:(xsteps-1))]) - ...
                        (p/4)*(dmu./rho))) + ...
           ([hvec 0].*(-(p/12)*([dmu(2:xsteps) 1]./[rho(2:xsteps) 1]) - ...
                        (p/4)*(dmu./rho))),0);
M2 = M2 + diag(-(p/12)*hvec.*((dmu(1:(xsteps-1))./rho(1:(xsteps-1))) + ...
                              (dmu(2:xsteps)./rho(2:xsteps))),1);
M2 = M2 + diag(-(p/12)*hvec.*((dmu(1:(xsteps-1))./rho(1:(xsteps-1))) + ...
                              (dmu(2:xsteps)./rho(2:xsteps))),-1);
M2 = sparse(M2);

% 0 means the edges are free by default %
wM2 = diag(([0 hvec].*(-(p/12)*([1 dlam(1:(xsteps-1))]./[1 rho(1:(xsteps-1))]) - ...
                        (p/4)*(dlam./rho))) + ...
           ([hvec 0].*(-(p/12)*([dlam(2:xsteps) 1]./[rho(2:xsteps) 1]) - ...
                        (p/4)*(dlam./rho))),0);
wM2 = wM2 + diag(-(p/12)*hvec.*((dlam(1:(xsteps-1))./rho(1:(xsteps-1))) + ...
                              (dlam(2:xsteps)./rho(2:xsteps))),1);
wM2 = wM2 + diag(-(p/12)*hvec.*((dlam(1:(xsteps-1))./rho(1:(xsteps-1))) + ...
                              (dlam(2:xsteps)./rho(2:xsteps))),-1);
wM2 = sparse(wM2);

% 0 means the edges are free by default %
M = diag(([0 hvec]*(1/3)+[hvec 0]*(1/3)),0);
M = M + diag(hvec/6,1);
M = M + diag(hvec/6,-1);
M = sparse(M);

% edges are reassigned after using 1 for division %
S = diag(-(beta.^2).*((1./[1 hvec])+(1./[hvec 1])),0);
S = S + diag((beta(1:(xsteps-1)).^2)./hvec,1);
S = S + diag((beta(2:xsteps).^2)./hvec,-1);
S = sparse(S);
S(1,1) = -(beta(1)^2)/hvec(1);
S(xsteps,xsteps) = -(beta(xsteps)^2)/hvec(xsteps-1);

% edges are reassigned after using 1 for division %
wS = diag(-(alpha.^2).*((1./[1 hvec])+(1./[hvec 1])),0);
wS = wS + diag((alpha(1:(xsteps-1)).^2)./hvec,1);
wS = wS + diag((alpha(2:xsteps).^2)./hvec,-1);
wS = sparse(wS);
wS(1,1) = -(alpha(1)^2)/hvec(1);
wS(xsteps,xsteps) = -(alpha(xsteps)^2)/hvec(xsteps-1);

% reassign split nodes at the end %
Kd = diag((p/6)*(([alpha(2:xsteps) 1].^2)-([beta(2:xsteps) 1].^2)-([1 alpha(1:(xsteps-1))].^2)+...
                 ([1 beta(1:(xsteps-1))].^2)),0);
Kd = Kd + diag((-p/3)*((alpha(1:(xsteps-1)).^2)-(beta(1:(xsteps-1)).^2)+((alpha(2:xsteps).^2)/2)-...
                       ((beta(2:xsteps).^2)/2)),1);
Kd = Kd + diag((p/3)*((alpha(2:xsteps).^2)-(beta(2:xsteps).^2)+((alpha(1:(xsteps-1)).^2)/2)-...
                      ((beta(1:(xsteps-1)).^2)/2)),-1);
Kd = sparse(Kd);
Kd(1,1) = p*((((alpha(2)^2)/6)-((2*(alpha(1)^2))/3))-(((beta(2)^2)/6)-((2*(beta(1)^2))/3)));
Kd(xsteps,xsteps) = -p*((((alpha(xsteps-1)^2)/6)-((2*(alpha(xsteps)^2))/3))...
                    -(((beta(xsteps-1)^2)/6)-((2*(beta(xsteps)^2))/3)));

% reassign split nodes at the end %
Kd2 = diag(-(1/6)*(([dmu(2:xsteps) 1]./[rho(2:xsteps) 1])-([1 dmu(1:(xsteps-1))]./[1 rho(1:(xsteps-1))])),0);
Kd2 = Kd2 + diag((1/3)*((dmu(1:(xsteps-1))./rho(1:(xsteps-1)))+(dmu(2:xsteps)./(2*rho(2:xsteps)))),1);
Kd2 = Kd2 + diag(-(1/3)*((dmu(2:xsteps)./rho(2:xsteps))+(dmu(1:(xsteps-1))./(2*rho(1:(xsteps-1))))),-1);
Kd2 = sparse(Kd2);
Kd2(1,1) = -(((dmu(2)/rho(2))*(1/6))-((dmu(1)/rho(1))*(2/3)));
Kd2(xsteps,xsteps) = (((dmu(xsteps-1)/rho(xsteps-1))*(1/6))-((dmu(xsteps)/rho(xsteps))*(2/3)));

% reassign split nodes at the end %
wKd2 = diag(-(1/6)*(([dpm(2:xsteps) 1]./[rho(2:xsteps) 1])-([1 dpm(1:(xsteps-1))]./[1 rho(1:(xsteps-1))])),0);
wKd2 = wKd2 + diag((1/3)*((dpm(1:(xsteps-1))./rho(1:(xsteps-1)))+(dpm(2:xsteps)./(2*rho(2:xsteps)))),1);
wKd2 = wKd2 + diag(-(1/3)*((dpm(2:xsteps)./rho(2:xsteps))+(dpm(1:(xsteps-1))./(2*rho(1:(xsteps-1))))),-1);
wKd2 = sparse(wKd2);
wKd2(1,1) = -(((dpm(2)/rho(2))*(1/6))-((dpm(1)/rho(1))*(2/3)));
wKd2(xsteps,xsteps) = (((dpm(xsteps-1)/rho(xsteps-1))*(1/6))-((dpm(xsteps)/rho(xsteps))*(2/3)));

% prepare boundary matrices %
Sm1 = sparse(zeros(xsteps,xsteps));
Sm2 = sparse(zeros(xsteps,xsteps));
Sm3 = sparse(zeros(xsteps,xsteps));
wSm1 = sparse(zeros(xsteps,xsteps));
wSm2 = sparse(zeros(xsteps,xsteps));
wSm3 = sparse(zeros(xsteps,xsteps));

% put in the free surface  - there is no SM1 since no slip occurs here %
if (inp(12) == 1)
    Sm2(1,1) = lambda(1)/rho(1);
    Sm3(1,1) = -dmu(1)/rho(1);
    wSm2(1,1) = mu(1)/rho(1);
    wSm3(1,1) = -dpm(1)/rho(1);
else
end

if (inp(13) == 1)
    Sm2(xsteps,xsteps) = -lambda(xsteps)/rho(xsteps);
    Sm3(xsteps,xsteps) = dmu(xsteps)/rho(xsteps);
    wSm2(xsteps,xsteps) = -mu(xsteps)/rho(xsteps);
    wSm3(xsteps,xsteps) = dpm(xsteps)/rho(xsteps);
else
end

% getting the locations of the random fractures set in the subroutine fractur %
rlocsf = load('fracas.txt');

for ii=1:Ns
    
    % M2 %
    M2(rlocsf(ii),rlocsf(ii)) = hvec(rlocsf(ii)-1)*(-(p/12)*...
                               (dmu(rlocsf(ii)-1)/rho(rlocsf(ii)-1)) - ...
                               (p/4)*(dmu(rlocsf(ii))/rho(rlocsf(ii))));
    M2(rlocsf(ii),rlocsf(ii)+1) = 0;
    M2(rlocsf(ii)+1,rlocsf(ii)+1) = hvec(rlocsf(ii)+1)*(-(p/12)*...
                                   (dmu(rlocsf(ii)+2)/rho(rlocsf(ii)+2)) - ...
                                   (p/4)*(dmu(rlocsf(ii)+1)/rho(rlocsf(ii)+1)));
    M2(rlocsf(ii)+1,rlocsf(ii)) = 0;
    
    % wM2 %
    wM2(rlocsf(ii),rlocsf(ii)) = hvec(rlocsf(ii)-1)*(-(p/12)*...
                               (dlam(rlocsf(ii)-1)/rho(rlocsf(ii)-1)) - ...
                               (p/4)*(dlam(rlocsf(ii))/rho(rlocsf(ii))));
    wM2(rlocsf(ii),rlocsf(ii)+1) = 0;
    wM2(rlocsf(ii)+1,rlocsf(ii)+1) = hvec(rlocsf(ii)+1)*(-(p/12)*...
                                   (dlam(rlocsf(ii)+2)/rho(rlocsf(ii)+2)) - ...
                                   (p/4)*(dlam(rlocsf(ii)+1)/rho(rlocsf(ii)+1)));
    wM2(rlocsf(ii)+1,rlocsf(ii)) = 0;
    
    % M %
    M(rlocsf(ii),rlocsf(ii)) = hvec(rlocsf(ii)-1)/3;
    M(rlocsf(ii),rlocsf(ii)+1) = 0;
    M(rlocsf(ii)+1,rlocsf(ii)+1) = hvec(rlocsf(ii)+1)/3;
    M(rlocsf(ii)+1,rlocsf(ii)) = 0;
    
    % S %
    S(rlocsf(ii),rlocsf(ii)) = -(beta(rlocsf(ii))^2)/hvec(rlocsf(ii)-1);
    S(rlocsf(ii),rlocsf(ii)+1) = 0;
    S(rlocsf(ii)+1,rlocsf(ii)+1) = -(beta(rlocsf(ii)+1)^2)/hvec(rlocsf(ii)+1);
    S(rlocsf(ii)+1,rlocsf(ii)) = 0;
    
    % wS %
    wS(rlocsf(ii),rlocsf(ii)) = -(alpha(rlocsf(ii))^2)/hvec(rlocsf(ii)-1);
    wS(rlocsf(ii),rlocsf(ii)+1) = 0;
    wS(rlocsf(ii)+1,rlocsf(ii)+1) = -(alpha(rlocsf(ii)+1)^2)/hvec(rlocsf(ii)+1);
    wS(rlocsf(ii)+1,rlocsf(ii)) = 0;

    % Kd %
    Kd(rlocsf(ii),rlocsf(ii)) = -p*((((alpha(rlocsf(ii)-1)^2)/6)-((2*(alpha(rlocsf(ii))^2))/3))...
                                -(((beta(rlocsf(ii)-1)^2)/6)-((2*(beta(rlocsf(ii))^2))/3)));
    Kd(rlocsf(ii),rlocsf(ii)+1) = 0;
    Kd(rlocsf(ii)+1,rlocsf(ii)+1) = p*((((alpha(rlocsf(ii)+2)^2)/6)-((2*(alpha(rlocsf(ii)+1)^2))/3))...
                                -(((beta(rlocsf(ii)+2)^2)/6)-((2*(beta(rlocsf(ii)+1)^2))/3)));
    Kd(rlocsf(ii)+1,rlocsf(ii)) = 0;
    
    % Kd2 %
    Kd2(rlocsf(ii),rlocsf(ii)) = (((dmu(rlocsf(ii)-1)/rho(rlocsf(ii)-1))*(1/6))-...
                                 ((dmu(rlocsf(ii))/rho(rlocsf(ii)))*(2/3)));
    Kd2(rlocsf(ii),rlocsf(ii)+1) = 0;
    Kd2(rlocsf(ii)+1,rlocsf(ii)+1) = -(((dmu(rlocsf(ii)+2)/rho(rlocsf(ii)+2))*(1/6))-...
                                     ((dmu(rlocsf(ii)+1)/rho(rlocsf(ii)+1))*(2/3)));
    Kd2(rlocsf(ii)+1,rlocsf(ii)) = 0;
    
    % wKd2 %
    wKd2(rlocsf(ii),rlocsf(ii)) = (((dpm(rlocsf(ii)-1)/rho(rlocsf(ii)-1))*(1/6))-...
                                 ((dpm(rlocsf(ii))/rho(rlocsf(ii)))*(2/3)));
    wKd2(rlocsf(ii),rlocsf(ii)+1) = 0;
    wKd2(rlocsf(ii)+1,rlocsf(ii)+1) = -(((dpm(rlocsf(ii)+2)/rho(rlocsf(ii)+2))*(1/6))-...
                                      ((dpm(rlocsf(ii)+1)/rho(rlocsf(ii)+1))*(2/3)));
    wKd2(rlocsf(ii)+1,rlocsf(ii)) = 0;
    
    % Kdf, one sided derivative at a fracture %
    Kdf(rlocsf(ii),rlocsf(ii)+1) = 0;
    Kdf(rlocsf(ii),rlocsf(ii)) = 1/hvec(rlocsf(ii)-1);
    Kdf(rlocsf(ii),rlocsf(ii)-1) = -1/hvec(rlocsf(ii)-1);
    
    Kdf(rlocsf(ii)+1,rlocsf(ii)+2) = 1/hvec(rlocsf(ii)+1);
    Kdf(rlocsf(ii)+1,rlocsf(ii)+1) = -1/hvec(rlocsf(ii)+1);
    Kdf(rlocsf(ii)+1,rlocsf(ii)) = 0;
    
    % Sm1, the matrix for linear slip %
    Sm1(rlocsf(ii),rlocsf(ii)) = -1*(1/rho(rlocsf(ii)));
    Sm1(rlocsf(ii),rlocsf(ii)+1) = 1*(1/rho(rlocsf(ii)));
    Sm1(rlocsf(ii)+1,rlocsf(ii)+1) = -1*(1/rho(rlocsf(ii)+1));
    Sm1(rlocsf(ii)+1,rlocsf(ii)) = 1*(1/rho(rlocsf(ii)+1));
    
    % Sm2 %
    Sm2(rlocsf(ii),rlocsf(ii)) = -lambda(rlocsf(ii))/rho(rlocsf(ii));
    Sm2(rlocsf(ii)+1,rlocsf(ii)+1) = lambda(rlocsf(ii)+1)/rho(rlocsf(ii)+1);
    
    % wSm2 %
    wSm2(rlocsf(ii),rlocsf(ii)) = -mu(rlocsf(ii))/rho(rlocsf(ii));
    wSm2(rlocsf(ii)+1,rlocsf(ii)+1) = mu(rlocsf(ii)+1)/rho(rlocsf(ii)+1);
    
    % Sm3 %
    Sm3(rlocsf(ii),rlocsf(ii)) = dmu(rlocsf(ii))/rho(rlocsf(ii));
    Sm3(rlocsf(ii)+1,rlocsf(ii)+1) = -dmu(rlocsf(ii)+1)/rho(rlocsf(ii)+1);
    
    % wSm3 %
    wSm3(rlocsf(ii),rlocsf(ii)) = dpm(rlocsf(ii))/rho(rlocsf(ii));
    wSm3(rlocsf(ii)+1,rlocsf(ii)+1) = -dpm(rlocsf(ii)+1)/rho(rlocsf(ii)+1);
    
end

% absorbing bcs on the ends - include M2 and Kd2 %
if(inp(13) == 0)
	S(xsteps,xsteps) = 0;
	S(xsteps,xsteps-1) = 0;
	Kd(xsteps,xsteps) = 0;
	Kd(xsteps,xsteps-1) = 0;
	M2(xsteps,xsteps) = 0;
	M2(xsteps,xsteps-1) = 0;
	Kd2(xsteps,xsteps) = 0;
	Kd2(xsteps,xsteps-1) = 0;
    wS(xsteps,xsteps) = 0;
    wS(xsteps,xsteps-1) = 0;
    wM2(xsteps,xsteps) = 0;
    wM2(xsteps,xsteps-1) = 0;
    wKd2(xsteps,xsteps) = 0;
    wKd2(xsteps,xsteps-1) = 0;
else
end

if(inp(12) == 0)
    S(1,1) = 0;
    S(1,2) = 0;
    Kd(1,1) = 0;
    Kd(1,2) = 0;
    M2(1,1) = 0;
    M2(1,2) = 0;
    Kd2(1,1) = 0;
    Kd2(1,2) = 0;
    wS(1,1) = 0;
    wS(1,2) = 0;
    wM2(1,1) = 0;
    wM2(1,2) = 0;
    wKd2(1,1) = 0;
    wKd2(1,2) = 0;
else
end

% time march %
count = 1;
% output for percent finished %
pct = .25;
% output that the time loop has begun %
disp(sprintf('time loop begun'))
for tt=1:totsteps

% prediction only really needs displacements and velocities from the previous timestep %
u(:,2) = u(:,1) + (k/2)*ut(:,1);
%+ 0.5*k*k*utt(:,1);
ut(:,2) = ut(:,1) + 0*k*utt(:,1);

w(:,2) = w(:,1) + (k/2)*wt(:,1);
%+ 0.5*k*k*wtt(:,1);
wt(:,2) = wt(:,1) + 0*k*wtt(:,1);

% solver for the u equation of motion %
vec1 = S*u(:,2) + Kd*wt(:,2) + M2*wt(:,2) + Kd2*u(:,2) + ...
       M*usrc*srcw(tt) + M*usrc*(1-inp(15))*(-2 + 4*(((tt*k-tdel)^2)/(fa)))*exp((-(tt*k-tdel)^2)/(fa)) + ...
       ([0 hvec].*((1/2)-((p^2)/6)*([0 alpha(1:(xsteps-1))].^2)-((p^2)/3)*(alpha.^2)) + ...
                  [hvec 0].*((1/2)-((p^2)/6)*([alpha(2:xsteps) 0].^2)-((p^2)/3)*(alpha.^2)))'.* ...
    [ (1-inp(12))*(usu(1,1)+(appalphau*k/(2*hvec(1)))*(-usu(3,1)+4*usu(2,1)-3*usu(1,1))...
    +upu(1,1)+(appbetau*k/(2*hvec(1)))*(-upu(3,1)+4*upu(2,1)-3*upu(1,1))) zeros(1,xsteps-2) ...
    (1-inp(13))*(usd(xsteps,1)-(appalphad*k/(2*hvec(xsteps-1)))*(3*usd(xsteps,1)-4*usd(xsteps-1,1)+usd(xsteps-2,1))...
    +upd(xsteps,1)-(appbetad*k/(2*hvec(xsteps-1)))*(3*upd(xsteps,1)-4*upd(xsteps-1,1)+upd(xsteps-2,1)))]'...
        + (1/etat)*Sm1*u(:,2) + p*Sm2*wt(:,2) + Sm3*u(:,2);

% find acceleration at the new time step %
utt(:,2) = vec1./([0 hvec].*((1/2)-((p^2)/6)*([0 alpha(1:(xsteps-1))].^2)-((p^2)/3)*(alpha.^2)) + ...
                  [hvec 0].*((1/2)-((p^2)/6)*([alpha(2:xsteps) 0].^2)-((p^2)/3)*(alpha.^2)))';

% corrector %
ut(:,2) = ut(:,1) + k*utt(:,2);
u(:,2) = u(:,2) + ((2*k)/4)*ut(:,1) + 0.5*k*k*utt(:,2);

% solver for the w equation of motion %
vec1 = wS*w(:,2) + Kd*ut(:,2) + wM2*ut(:,2) + wKd2*w(:,2) + ...
       M*wsrc*srcw(tt) + M*wsrc*(1-inp(15))*(-2 + 4*(((tt*k-tdel)^2)/(fa)))*exp((-(tt*k-tdel)^2)/(fa)) + ...
       ([0 hvec].*((1/2)-((p^2)/6)*([0 beta(1:(xsteps-1))].^2)-((p^2)/3)*(beta.^2)) + ...
                  [hvec 0].*((1/2)-((p^2)/6)*([beta(2:xsteps) 0].^2)-((p^2)/3)*(beta.^2)))'.* ...
    [ (1-inp(12))*(wsu(1,1)+(appalphau*k/(2*hvec(1)))*(-wsu(3,1)+4*wsu(2,1)-3*wsu(1,1))...
    +wpu(1,1)+(appbetau*k/(2*hvec(1)))*(-wpu(3,1)+4*wpu(2,1)-3*wpu(1,1))) zeros(1,xsteps-2) ...
    (1-inp(13))*(wsd(xsteps,1)-(appalphad*k/(2*hvec(xsteps-1)))*(3*wsd(xsteps,1)-4*wsd(xsteps-1,1)+wsd(xsteps-2,1))...
    +wpd(xsteps,1)-(appbetad*k/(2*hvec(xsteps-1)))*(3*wpd(xsteps,1)-4*wpd(xsteps-1,1)+wpd(xsteps-2,1)))]'...
        + (1/etan)*Sm1*w(:,2) + p*wSm2*ut(:,2) + wSm3*w(:,2);

% find acceleration at the new time step %
wtt(:,2) = vec1./([0 hvec].*((1/2)-((p^2)/6)*([0 beta(1:(xsteps-1))].^2)-((p^2)/3)*(beta.^2)) + ...
                  [hvec 0].*((1/2)-((p^2)/6)*([beta(2:xsteps) 0].^2)-((p^2)/3)*(beta.^2)))';

% corrector 2 %
wt(:,2) = wt(:,1) + k*wtt(:,2);
w(:,2) = w(:,2) + ((2*k)/4)*wt(:,1) + 0.5*k*k*wtt(:,2);

% wavefield splitting only valid at boundary for absorbing property %

% upd means the u wavefield going down (positive z direction) without the p wave %
upd(:,2) = -(tan(pangled)*wtt(:,2) - utt(:,2))/(tan(pangled)*...
            tan(asin((beta(xsteps)/alpha(xsteps))*sin(pangled)))+1);
        
% wpd means the w wavefield going down (positive z direction) without the p wave %
wpd(:,2) = tan(asin((beta(xsteps)/alpha(xsteps))*sin(pangled)))*... 
            ((tan(pangled)*wtt(:,2) - utt(:,2))/(tan(pangled)*...
            tan(asin((beta(xsteps)/alpha(xsteps))*sin(pangled)))+1));
        
% wsd means the w wavefield going down (positive z direction) without the s wave %
wsd(:,2) = (tan(asin((beta(xsteps)/alpha(xsteps))*sin(pangled)))*utt(:,2) + wtt(:,2))/...
            (tan(pangled)*tan(asin((beta(xsteps)/alpha(xsteps))*sin(pangled)))+1);
        
% usd means the u wavefield going down (positive z direction) without the s wave %
usd(:,2) = tan(pangled)*(tan(asin((beta(xsteps)/alpha(xsteps))*sin(pangled)))*utt(:,2) + wtt(:,2))/...
            (tan(pangled)*tan(asin((beta(xsteps)/alpha(xsteps))*sin(pangled)))+1);

% upu means the u wavefield going up (negative z direction) without the p wave %
upu(:,2) = (tan(pangleu)*wtt(:,2) + utt(:,2))/(tan(pangleu)*...
            tan(asin((beta(1)/alpha(1))*sin(pangleu)))+1);
        
% wpu means the w wavefield going up (negative z direction) without the p wave %
wpu(:,2) = tan(asin((beta(1)/alpha(1))*sin(pangleu)))*... 
            ((tan(pangleu)*wtt(:,2) + utt(:,2))/(tan(pangleu)*...
            tan(asin((beta(1)/alpha(1))*sin(pangleu)))+1));
        
% wsu means the w wavefield going up (negative z direction) without the s wave %
wsu(:,2) = -(tan(asin((beta(1)/alpha(1))*sin(pangleu)))*utt(:,2) - wtt(:,2))/...
            (tan(pangleu)*tan(asin((beta(1)/alpha(1))*sin(pangleu)))+1);
        
% usu means the u wavefield going up (negative z direction) without the s wave %
usu(:,2) = tan(pangleu)*(tan(asin((beta(1)/alpha(1))*sin(pangleu)))*utt(:,2) - wtt(:,2))/...
            (tan(pangleu)*tan(asin((beta(1)/alpha(1))*sin(pangleu)))+1);

% output %
if(floor(tt/sampl) == ceil(tt/sampl))
% output horizontal and vertical displacements, velocities, and accelerations %
    usv(:,count) = u(:,2);
    utsv(:,count) = ut(:,2);
    uttsv(:,count) = utt(:,2);
    wsv(:,count) = w(:,2);
    wtsv(:,count) = wt(:,2);
    wttsv(:,count) = wtt(:,2);
% store vertical derivatives of the displacements in dummy variables %
    uz = Kdf*u(:,2);
    wz = Kdf*w(:,2);
% output the divergence and curl %
    dvg(:,count) = wz-p*ut(:,2);  % dimensionless %
    crl(:,count) = -uz-p*wt(:,2); % dimensionless %
% output the three relavent stresses: xz, zz, and xx, where x is the horizontal and z is the vertical %
    sxzsv(:,count) = mu'.*(uz-p*wt(:,2));                   % Pa %
    szzsv(:,count) = (lambda'+(2*mu')).*wz-p*lambda'.*ut(:,2); % Pa %
    sxxsv(:,count) = lambda'.*wz-p*(lambda'+(2*mu')).*ut(:,2); % Pa %
    count = count + 1;
else
end

if floor((tt/totsteps)/pct)
	disp(sprintf('time loop %d%% done',pct*100))
	pct = pct + .25;
else
end

% update fields with memory %
for ii=1:1
    u(:,ii) = u(:,ii+1);
    ut(:,ii) = ut(:,ii+1);
    utt(:,ii) = utt(:,ii+1);
    w(:,ii) = w(:,ii+1);
    wt(:,ii) = wt(:,ii+1);
    wtt(:,ii) = wtt(:,ii+1);
    
    upd(:,ii) = upd(:,ii+1);
    wpd(:,ii) = wpd(:,ii+1);
    usd(:,ii) = usd(:,ii+1);
    wsd(:,ii) = wsd(:,ii+1);
    
    upu(:,ii) = upu(:,ii+1);
    wpu(:,ii) = wpu(:,ii+1);
    usu(:,ii) = usu(:,ii+1);
    wsu(:,ii) = wsu(:,ii+1);
end

end

runtim = (cputime-runtim)/60;
disp(sprintf('total runtime %g minutes',runtim))

