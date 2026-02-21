#! /bin/sh
#  Author:  April 2020: John Stockwell
# copyright 2020  John Stockwell

# SIR model

# Important quantities:						
# R0 = the basic reproductive rate, which is the number of new 
# infections per single infected host  	
#  1 < R0 < 1.5 for influenza, (2.2 to 2.7 for Covid-19), 12 to	
# 18 for measles.						
#  b, k, s0, and R0 are related via				
#  k = b*s0/R0 = b/R0 when s0/N and s0=N 			
#  								
#  It is often easier to determine the recovery rate k (in units
#  of h and to determine reasonable estimate of s0 and of R0 	
#  and to calculate the infection rate b = k*R0/s0 or b=k*R0	
#  when s0=N and is normalized by N.				
#								
# s = total number susceptible to the infection			
# i = total number of those capable of passing on the infection	
# r = total number removed = dead + recovered			
#								
# When xi is nonzero, then there is a potential that fraction of 
# the removed population can be reinfected.			

 
#  The SIR model describes an epidemic in terms of
#    s = susceptibles in a population
#    i = infectives in a population
#    r = removed = recovered + dead
# 
#    s0 = initial value of the susceptibles
#    i0 = initial value of the infectives
#    r0 = initial removed value = 0
#    
#    s(t) + i(t) + r(t) = s0 + i0   = N for the unnormalized case.
#    If normalized by total population N, then s(t) + i(t) + r(t) = 1 
#    and s(t) starts at its maxium value of s0/N.   
#    
#    R0 = b*s0/k  = basic reproduction rate = b/k when s0=N and s0=s0/N
#    b = rate of infection
#    k = rate removal = recovery rate + death rate
#    xi = re-infection rate 
#    mu = birth rate  
#    nu = death rate
#     
#    The encounters between susceptibles and the infectives are represented
#    by the product s*i
# 
#   SIR model:  
# 	s'(t) =  - b*s*i 
# 	i'(t) = b*s*i- k*i 
# 	r'(t) = k*i 
#     
#   SIR model with vital statistics (mu birth rate, nu death rate):  
# 	s'(t) = mu - nu*s - b*s*i 
# 	i'(t) = b*s*i - k*i - nu*i 
# 	r'(t) = ki -  nur
# 
#   SIRS model with vital statistics (mu birth rate, nu death rate) and reinfection:  
# 	s'(t) = mu - nu*s + xi*r - b*s*i 
# 	i'(t) = b*s*i - k*i - nu*i 
# 	r'(t) = k*i - xi*r - nu*r
# 
#  s(t)= susceptible members 
#  i(t)= infectives
#  r(t)= removed members = recovered + dead + sequestered
# 
#  There is an impiled flow from s(t) -> i(t) -> r(t), though infected
#  who are quarantined immediately become part of r(t). 
# 
#  The product xi*r are the reinfected members of the recovered group,
#  and are thus removed from the recovered group and fed back to 
#  the susceptible group.
#  
#  The product b*s*i denotes the interaction of the infective population with
#  the susceptible population..
# 
								
### SIR model 								
# influenza in an English boarding school, 1978:		
# N=762 i0=1,  2 students infected per day, 1/2 of the infected	
# population removed per day. Take b=2 k=0.5 			
#								
# Normalized by N:						
 sir_epidemic h=0.1 stepmax=200 i0=1 b=2 k=.5 N=762 mode=SIR |
 xgraph n=200 nplot=3 d1=.1 style=normal label1="days" \
 title="Influenza: English boarding school" \
 width=1000 height=1000 &


# real data

cat data.txt | awk '{ print $2 }' | a2b n1=1 > real_data.bin

				
# Normalized by N, output scaled by N:
# 
 sir_epidemic h=.1 stepmax=200 i0=1 b=2 k=.5 N=762 mode=SIR scale=1 > model_data.bin

cat real_data.bin model_data.bin |
 xgraph n=15,200,200,200 nplot=4 d1=1,.1,.1,.1 \
 linewidth=0,3,3,3 linecolor=4,5,4,6  \
 marksize=20,0,0,0 mark=3,0,0,0 \
 style=normal label1="days" \
 title="Model and Data: b=2 k=.5" \
 width=1000 height=1000  \
 label2="number of students"  &

				
# Better fit taking b=1.75
# Normalized by N, output scaled by N:
# 
 sir_epidemic h=.1 stepmax=200 i0=1 b=1.75 k=.5 N=762 mode=SIR scale=1 > model_data_2.bin

cat real_data.bin model_data_2.bin |
 xgraph n=15,200,200,200 nplot=4 d1=1,.1,.1,.1 \
 linewidth=0,3,3,3 linecolor=4,5,4,6  \
 marksize=20,0,0,0 mark=3,0,0,0 \
 style=normal label1="days" \
 title="Model and Data: better fit b=1.75 k=.5  " \
 width=1000 height=1000  \
 label2="number of students" &				

# allow a 1% reinfection rate xi=.01

 sir_epidemic h=.1 stepmax=2000 i0=1 b=1.75 k=.5 N=762 xi=.01  mode=SIR scale=1 > model_data_3.bin

cat model_data_3.bin |
 xgraph n=2000,2000,2000 nplot=3 d1=.1,.1,.1 \
 linewidth=3,3,3 linecolor=5,4,6  \
 marksize=0,0,0 mark=0,0,0 \
 style=normal label1="days" \
 title="Model and Data: b=1.75 k=.5 with 1% reinfection " \
 width=1000 height=1000  \
 label2="number of students" &				


exit 0
