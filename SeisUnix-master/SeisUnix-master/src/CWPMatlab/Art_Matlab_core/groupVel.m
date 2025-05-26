%==============================================================================
% Compute the group velocity 
%==============================================================================

function [dqp1, dqp2] = groupVel(p, Cij, isegment, NMOindex);

%==============================================================================
% The second derivatives of the Christofel determinant are computed
% if NMOindex is nonzero
%==============================================================================

global unity33 indexTM
global TypeAnis
global RayCode
global f ff

%==============================================================================
layer = RayCode(2,isegment);

[G,A,B,C]=chris_deriv(p,Cij);
dG(:,:,1)=A;
dG(:,:,2)=B;
dG(:,:,3)=C;
% Find the derivatives of the determinant  d det(G)/d p(m)
  for m=1:3
    f(m) = det([dG(:,1,m),    G(:,2),    G(:,3)]) + ...
           det([   G(:,1), dG(:,2,m),    G(:,3)]) + ...
           det([   G(:,1),    G(:,2), dG(:,3,m)]);
  end;
end;
 
%==============================================================================
% Group-velocity vector
gr = f/sum(p.*f);
vgr = sqrt(sum(gr.^2));
ugroup = gr'/vgr;
dqp1 = - gr(1)/gr(3);
dqp2 = -gr(2)/gr(3);

%==============================================================================
% Compute the second-order derivatives of the Christoffel determinant  
if NMOindex ~= 0
  if TypeAnis(layer,:) == 'ISO'
    ff = 2*unity33;  

  else
%   The second derivatives of the Christoffel matrix
    for i=1:3;   for l=1:3;   for m=1:3;   for n=1:3;
      ddG(i,l,m,n) = Cij(indexTM(i,m),indexTM(n,l)) + ...
                     Cij(indexTM(i,n),indexTM(m,l));
    end;   end;   end;   end; 

%   The second derivatives of the Christoffel determinant
%   d^2 det(G)/[d p(m) d p(n)]
    for m=1:3;   for n=1:m;
      ff(m,n) = det([ddG(:,1,m,n),       G(:,2),       G(:,3)]) + ...
                det([   dG(:,1,m),    dG(:,2,n),       G(:,3)]) + ...
                det([   dG(:,1,m),       G(:,2),    dG(:,3,n)]) + ...
                det([   dG(:,1,n),    dG(:,2,m),       G(:,3)]) + ...
                det([      G(:,1), ddG(:,2,m,n),       G(:,3)]) + ...
                det([      G(:,1),    dG(:,2,m),    dG(:,3,n)]) + ...
                det([   dG(:,1,n),       G(:,2),    dG(:,3,m)]) + ...
                det([      G(:,1),    dG(:,2,n),    dG(:,3,m)]) + ...
                det([      G(:,1),       G(:,2), ddG(:,3,m,n)]);
    end;   end;
%   Fill the symmetric part
    ff(1,2) = ff(2,1);   ff(1,3) = ff(3,1);   ff(2,3) = ff(3,2);
  end;
end;

%==============================================================================












