% put in some fractures into the model %

clear all;

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

xsteps = length(hvec)+1; % total number of grid points %
Ns = inp(6); % number of random fractures %
mindist = inp(7); % minimum distance between fractures %
upperf = inp(8); % upper side of the fracture zone %
lowerf = inp(9); % lower side of the fracture zone %

% getting the locations of the random fractures, mindist has been set above %
rnum = 0;
count = 1;
rlocs = randperm(xsteps);
rlocsf = zeros(1,Ns);
while (rnum < Ns)
    if ( rlocs(count) >= upperf & rlocs(count) <= lowerf )
		if (rnum > 0)
			nsc = 0;
			for ii=1:rnum
				if ( abs(rlocs(count)-rlocsf(ii)) > mindist )
				else
					nsc = 1;
				end
			end
			if (nsc == 0)
				rnum = rnum + 1;
				rlocsf(rnum) = rlocs(count);
			else
			end
		else
			rnum = rnum + 1;
			rlocsf(1) = rlocs(count);
		end	
    else
	end
count = count + 1;
end

rlocsfs = sort(rlocsf);
rlocsfs(Ns+1) = length(alpha);

alphan = zeros(1,length(alpha)+Ns);
betan = zeros(1,length(beta)+Ns);
rhon = zeros(1,length(rho)+Ns);
hvecn = zeros(1,length(hvec)+Ns);
ndn = -1;
nd = 0;
for ii=1:(Ns+1)
    begnn = ndn + 2;
    ndn = rlocsfs(ii)+(ii-1);
    begn = nd + 1;
    nd = rlocsfs(ii);
    
    alphan(begnn:ndn) = alpha(begn:nd);
    betan(begnn:ndn) = beta(begn:nd);
    rhon(begnn:ndn) = rho(begn:nd);
    hvecn(begnn:(ndn-1)) = hvec(begn:(nd-1));
    
    if (ii < (Ns+1))
        alphan(ndn+1) = alpha(nd);
        betan(ndn+1) = beta(nd);
        rhon(ndn+1) = rho(nd);
        hvecn(ndn) = 1;
        hvecn(ndn+1) = hvec(ndn);
    else
    end
end

fraclocs = zeros(1,Ns);
for ii=1:Ns
    fraclocs(ii) = rlocsfs(ii) + (ii-1);
end

% column vectors get written out as columns %
alphan = alphan';
betan = betan';
rhon = rhon';
hvecn = hvecn';
fraclocs = fraclocs';

% save these in input files %
save 'alpha.txt' alphan -ascii -double
save 'beta.txt' betan -ascii -double
save 'rho.txt' rhon -ascii -double
save 'grid.txt' hvecn -ascii -double
save 'fracas.txt' fraclocs -ascii -double

