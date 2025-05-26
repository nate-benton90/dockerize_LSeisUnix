%==============================================================================
% Solve the Christoffel equation for vertical slowness
%==============================================================================

function q = christoffel(p1, p2, Cij, isegment);

global unity33 indexTM
global TypeAnis
global RayCode Nsegment NMOCode

%==============================================================================
layer = RayCode(2,isegment);

% Construct the Christoffel polynomial 
if TypeAnis(layer,:) == 'ISO'
% Isotropy
  c33 = Cij(3,3);   c44 = Cij(4,4);  
  f(1) = c33*c44^2;
  f(2) = 0;
  f(3) = c44*(-c44 + c33*(-2 + 3*c44*(p1^2 + p2^2))); 
  f(4) = 0;
  f(5) = (-1 + c44*(p1^2 + p2^2))*(-2*c44 + c33*(-1 + 3*c44*(p1^2 + p2^2)));
  f(6) = 0;
  f(7) = (-1 + c33*(p1^2 + p2^2))*(-1 + c44*(p1^2 + p2^2))^2;

else
% Anisotropy  
[f]=chris_poly(p1,p2,Cij);
end

%==============================================================================
% Solve the Christoffel equation
p3found = roots(f);

%==============================================================================
% Select appropriate roots

% Distinguish between up- and down-going rays
if RayCode(3,isegment) > RayCode(2,isegment);
  direction = +1;
else
  direction = -1;
end;

nroot = 0;
for iroot=1:6
  if abs(imag(p3found(iroot))) < 1.e-4 
       % Find the sign of the vertical component of group velocity
      % dg3 is the vertical group velocity and g1 is the horizontal group velocity
         if sign(real(p3found(iroot))) == direction
    nroot = nroot + 1;
    p3select(nroot) = real(p3found(iroot));
end
  end;
end;

 
if nroot > 3
    nroot=0; 
    clear p3select
    for iroot=1:6
  if abs(imag(p3found(iroot))) < 1.e-4 
       % Find the sign of the vertical component of group velocity
      % dg3 is the vertical group velocity and g1 is the horizontal group velocity
      [g3, g1] = groupVel3([p1, p2, p3found(iroot)], Cij, isegment,0);
      if sign(real(g3)) == direction
    nroot = nroot + 1;
    p3select(nroot) = real(p3found(iroot));
end;
  end;
end;
end;

if nroot ~= 0;   p3sort = sort(abs(p3select));   end;

%==============================================================================
% Separate rays according to the type

if nroot == 3
% Normal case: all three roots are real
  q = direction*p3sort(RayCode(1,isegment));

elseif nroot == 2
% One complex root
  if RayCode(1,isegment) == 1;   
    q = NaN;  
  else
    q = direction*p3sort(RayCode(1,isegment)-1);
  end;

elseif nroot == 1
% Two complex roots
  if RayCode(1,isegment) == 1  |  RayCode(1,isegment) == 2;
    q = NaN;
  else
    q = direction*p3sort(1);
  end;

else
% All three roots are complex and  nroot = 0
  q = NaN;   
end;

%==============================================================================

