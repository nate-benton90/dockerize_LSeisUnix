#! /bin/sh
#  Author:  April 2020: John Stockwell
# copyright 2020  John Stockwell

# SIR model

# Important quantities:						
# r0 = number of new infections per single infected host (basic reproduction rate)  	
#  1 < r0 < 1.5 for influenza, (2.2 to 2.7 for Covid-19), 12 to	
# 18 for measles.						
#  b, k, s0, and r0 are related via				
#  k = b*s0/r0 = b/r0 when s0/N and s0=N 			
#  								
#  It is often easier to determine the recovery rate k (in units
#  of h and to determine reasonable estimate of s0 and of r0 	
#  and to calculate the infection rate b = kr0/s0 or b=kr0	
#  when s0=N and is normalized by N.				
#								
# S = total number susceptible to the infection			
# I = total number of those capable of passing on the infection	
# R = total number removed = dead + recovered			
#								
# When xi is nonzero, then there is a potential that fraction of 
# the removed population can be reinfected.			
#
# When mu>0 and nu>0 birth and death occur.
# 
#  The SIR model describes an epidemic in terms of
#    s = susceptibles in a population
#    i = infectives in a population
#    r = removed = recovered + dead
# 
#    s0 = initial value of the susceptibles
#    i0 = initial value of the infectives
#    r0 = initial removed value = 0
#    
#    s(t) + i(t) + r(t) = r0 + r0   = N for the unnormalized case.
#    If normalized by total population N, then s(t) + i(t) + r(t) = 1 
#    and s(t) starts at its maxium value of s0/N, if normalization.   
#    
#    r0 = b*s0/k  = basic reproduction rate
#    b = rate of infection
#    k = rate removal = recovery rate + death rate
#    xi = re-infection rate 
#    mu = birth rate  
#    nu = death rate
#     
#    The encounters between susceptibles and the infectives is represented
#    by the product  s*i 
# 
#   SIR model:   (Here ' = d/dt )
# 	s'(t) =  - b*s(t)*i(t) 
# 	i'(t) = b*s(t)*i(t)- k*i(t)
# 	r'(t) = k*i(t) 
# 
#   SIR model with Baker 2020 social distancing:  
# 	s'(t) =  - b*s*i/(1 + gamma*i) 
# 	i'(t) = b*s*i/(1 + gamma*i) - k*i 
# 	r'(t) = k*i 
#     
#   The reactive social distancing is to replace b with b/(1+gamma*i) which 
#   changes as the number of infectives increases)  
#     
#  SIR model with vital dynamics (mu birth rate, nu death rate):  
# 	s'(t) = mu - nu*s - b*s*i 
# 	i'(t) = b*s*i - k*i - nu*i 
# 	r'(t) = k*i -  nu*r
# 
#   SIRS model with vital dynamics (mu birth rate, nu death rate),
#    and xi reinfection:  
# 	s'(t) = mu - nu*s + xi*r - b*s*i 
# 	i'(t) = b*s*i - k*i - nu*i 
# 	r'(t) = k*i - xi*r - nu*r
# 
#  s(t)= susceptible members 
#  i(t)= infectives
#  r(t)= removed members = recovered + dead + sequestered
# 
#  There is an impiled flow from s(t) -> i(t) -> r(t), though infected
#  who are quarantined immediately become part of R(t). 
# 
#  The product xi*r are the reinfected members of the recovered group,
#  and are thus removed from the recovered group and fed back to the 
#  susceptible group.
#  
#  The product b*s*i denotes the interaction of the infective population with
#  the susceptible population..
# 
								
### SIR model 								
 						
# Hong Kong Flu 1968-1969:			
# https://services.math.duke.edu/education/ccp/materials/diffcalc/sir/sir1.html
# Population is N=s0=7.9 million, r0=1.5, the average period of	
# infectiveness is  3 days so k=1/3, b=r0k=(3/2)(1/3)=0.5, and initial
# infected is I0=10.						
#								
#  Normalized by N						
 sir_epidemic h=1 i0=10 stepmax=200 k=.3333 b=.5 N=7.9e6 mode=SIR |
      xgraph n=200 nplot=3 d1=1 style=normal \
 width=1000 height=1000  \
 title="New York winter 1968-1969 Hong Kong Flu"  &

#  Normalized by N, output scaled by N
 sir_epidemic h=1 scale=1 i0=10 stepmax=200 k=.3333 b=.5 N=7.9e6 mode=SIR |
      xgraph n=200 nplot=3 d1=1 style=normal  \
 width=1000 height=1000  \
 title="New York winter 1968-1969 Hong Kong Flu"  &

############ Vital dynamics ###################
# an example based on the Hong Kong flu data
#  Normalized by N						
 sir_epidemic h=1 i0=10 stepmax=200 mu=.001 nu=10 \
    k=.3333 b=.5 N=7.9e6 mode=SIR |
      xgraph n=200 nplot=3 d1=1 style=normal \
 width=1000 height=1000  \
 title="New York winter 1968-1969 Hong Kong Flu, birth and death"  &

#  Normalized by N, output scaled by N
 sir_epidemic h=1 scale=1 i0=10 stepmax=200 mu=.001 nu=10 k=.3333 b=.5 N=7.9e6 mode=SIR |
      xgraph n=200 nplot=3 d1=1 style=normal  \
 width=1000 height=1000  \
 title="New York winter 1968-1969 Hong Kong Flu, birth and death"  &

#### Social distancing #######
#  Normalized by N
# social distancing gamma=20 following Baker 2020
 sir_epidemic h=1 i0=10 stepmax=200 gamma=20 k=.3333 b=.5 N=7.9e6 mode=SIR |
      xgraph n=200 nplot=3 d1=1 style=normal \
 width=1000 height=1000  \
 title="New York winter 1968-1969 Hong Kong Flu with social distancing "  &

#  Normalized by N, output scaled by N
 sir_epidemic h=1 scale=1 i0=10 stepmax=200 gamma=20 k=.3333 b=.5 \
  N=7.9e6 mode=SIR | xgraph n=200 nplot=3 d1=1 style=normal  \
 width=1000 height=1000  \
 title="New York winter 1968-1969 Hong Kong Flu with social distancing "  &

########## Re-infection ##############
# with re-infection it starts to look more like a predator-prey model
# Re-infection 0.01% re-infection rate xi=0.0001
#  Normalized by N						
 sir_epidemic h=1 i0=10 stepmax=20000 k=.3333 b=.5 xi=.0001 N=7.9e6 mode=SIR |
      xgraph n=20000 nplot=3 d1=1 style=normal \
 width=1000 height=1000  \
 title="New York winter 1968-1969 Hong Kong Flu, with re-infection"  &

#  Normalized by N, output scaled by N
 sir_epidemic h=1 scale=1 i0=10 stepmax=20000 k=.3333 b=.5 xi=.0001 N=7.9e6 \
  mode=SIR | xgraph n=20000 nplot=3 d1=1 style=normal  \
 width=1000 height=1000  \
 title="New York winter 1968-1969 Hong Kong Flu, with re-infection"  &


exit 0
