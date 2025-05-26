ccccc dip scan routine
        subroutine dipscn(data,amp,work,nt,nx,hy,
     1                    pxo,pyo,amo,ipow,
     2                    ixo,dxo,nxo,
     3                    to,px,py,nto,npx,npy,
     4                    ht,hx,tt,tx,ty,
     5                    perc,ipxo,ipyo,wk1,wk2)

        real data(nt,nx,-hy:hy),amp(npx,npy,nto),work(nt)
        integer nt,nx,hy
        real pxo(nto,nxo),pyo(nto,nxo),amo(nto,nxo)
        integer ixo,dxo,nxo,ipow
        real to(nto),px(npx),py(npy)
        integer nto,npx,npy
        integer ht,hx 
        real tt(-ht:ht),tx(-hx:hx),ty(-hy:hy)
        real perc,wk1(nto),wk2(nto)
        integer ipxo(nto),ipyo(nto)
ccc	    real xg(2000),xam(200000)

cccc dip scan 
        do ix=1,nxo
           ixx = ixo + (ix-1)*dxo
           ix0 = ixx - hx
           ixn = ixx + hx
           if(ix0.lt.1) ix0 = 1
           if(ixn.gt.nx) ixn = nx
           n2 = ixn - ix0
           scale = 1.0
           if(n2.gt.1.) scale = 1./n2
           do ipy=1,npy
              do ipx=1,npx
                 do it=1,nt
                    work(it) = 0.
                 end do
                 do iy=-hy,hy
                    tpy = py(ipy)*iy
                    do i2=ix0,ixn
                       i22 = i2 - ixx
                       tpx = px(ipx)*i22
                       tp = tpx + tpy
                       i10 = tp
                       res = tp - i10
cccc		       tmp = tx(i22)*ty(iy)*scale
                       s1 = (1.-res)
                       s2 = res
cccc                      s1 = (1.-res)*tmp
cccc                      s2 = res*tmp
                       do it=1,nt
                          i1 = i10 + it
                          if (i1.gt.1.and.i1.lt.nt) then
                             work(it) = work(it)+s1*data(i1,i2,iy)
     1                            + s2*data(i1+1,i2,iy)
                          end if
                       end do
                    end do
                 end do
ccc sum over t-window
                 if (ipow.eq.1) then
                    do it=1,nt
                       work(it) = abs(work(it))
                    end do
                 else if(ipow.eq.2) then
                    do it=1,nt
                       work(it) = work(it)*work(it)
                    end do
                 else
                    tmp = ipow
                    do it=1,nt
                       work(it) = work(it)**tmp
                    end do
                 end if

ccccc sum over time window
                 do it=1,nto
                     itt = to(it)
                     it1 = itt - ht
                     itn = itt + ht
                     if(it1.lt.1) it1 = 1
                     if(itn.gt.nt) itn = nt
                     n1 = itn - it1 + 1
                     tmp = 0.
                     do i1=it1,itn
                        tmp = tmp + work(i1)*tt(i1-itt)
                     end do 
                     if(n1.gt.1) tmp = tmp/n1
                     amp(ipx,ipy,it) = tmp
                 end do

              end do
           end do
ccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccc
c           call dump2xplot(data(1,ix0,0),nt,n2+1,1,"data")
c           call dump2xplot(amp,npx,nto,0,"amp")
ccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccc

ccc pick dip (maximum semblance) 
           do it=1,nto
              ixx = npx/2+1
              iyy = npy/2+1
              tmp = abs(amp(ixx,iyy,it))
              do ipy=1,npy
                 do ipx=1,npx
                    a = abs(amp(ipx,ipy,it))
                    if (a.gt.tmp) then
                       tmp = a
                       ixx = ipx
                       iyy = ipy
                    end if
                 end do
              end do
              ipxo(it) = ixx
              ipyo(it) = iyy
              wk1(it) = tmp
           end do
ccccccccccccccccccccccccccccccccccccccccccccccccccccc
c           do it=1,nto
c              xg(it*2-1) = it
c              xg(it*2) = ipxo(it)
c           end do
c           call dump2xgraph(xg,nto,1,"b edit","it","ipx","seismic")
c		   do ipx=1,npx
c           do it=1,nto
c              xam(it+(ipx-1)*nto) = amp(ipx,1,it) 
c           end do
c           end do
c           do it=1,nto
c              xam(it+(ipxo(it)-1)*nto) = 0.
c           end do
c           call dump2xplot(xam,nto,npx,0,"amo")
ccccccccccccccccccccccccccccccccccccccccccccccccccccc


ccc edit picks 
cccc remove week-energe noise picks
           tmp = nto * perc
           itn = tmp
           if (itn.lt.0) itn=0
           if (itn.gt.nto-1) itn = nto-1
           do it=1,nto
              wk2(it) = wk1(it)
           end do
           call qkfind(itn,nto,wk1)
           tmp = wk1(itn+1)
           i1 = 0
           do it=1,nto
              if (wk2(it).ge.tmp ) then
                 i1 = i1 + 1
                 wk1(i1) = it
              end if
           end do
cccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccc
c           do it=1,i1
c              xg(it*2-1) = wk1(it)
c			  tmp = wk1(it) + 0.5
c			  itmp = tmp
c              xg(it*2) = ipxo(itmp)
c           end do
c           call dump2xgraph(xg,i1,1,"a edit","it","ipx","seismic")
cccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccc

		   ipp = 1
           do it=1,nto
              tmp = it
cc              call findx(wk1,i1,tmp,ipp)
                call bisear(i1,1,wk1,tmp,ipp)
              if (ipp.ge.i1) then
                 ip  = wk1(i1) + 0.5
                 ixx = ipxo(ip) 
                 iyy = ipyo(ip)
              else if(tmp.le.wk1(1)) then
                 ip  = wk1(1) + 0.5
                 ixx = ipxo(ip)
                 iyy = ipyo(ip)
              else 
                 ip  = wk1(ipp) + 0.5
                 ip2  = wk1(ipp+1) + 0.5
                 scale = (tmp-wk1(ipp))/(wk1(ipp+1)-wk1(ipp))
                 tmp1  = ipxo(ip) + scale*(ipxo(ip2)-ipxo(ip))
                 ixx = tmp1
                 tmp1  = ipyo(ip) + scale*(ipyo(ip2)-ipyo(ip))
                 iyy = tmp1
              end if
              pxo(it,ix) = px(ixx)
              pyo(it,ix) = py(iyy)
              amo(it,ix) = amp(ixx,iyy,it)
           end do
        end do
c        do it=1,nto
c           xg(it*2-1) = it
c           xg(it*2) = pxo(it,1)
c        end do
c        call dump2xgraph(xg,nto,1,"output px","it","ipx","seismic")
        return
        end

        subroutine findx(xn,n,x,ipp)
        real xn(n),x
        integer n, ipp
		if(x.lt.xn(ipp)) return
        do i=ipp,n-1
			 if (xn(i).le.x .and. x.le.xn(i+1)) then
                indx = i
                goto 100
             end if
        end do
		indx = n
100     continue
		ipp = indx
        return
        end
