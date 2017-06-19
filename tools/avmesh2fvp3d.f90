program avmesh2fvp3d
   implicit none

 !
 ! parameters read in
 !
   integer :: n_mesh
   integer, parameter :: max_bcs = 500
   integer, dimension(:), allocatable :: jdim, kdim, ldim, n_bcs
   integer, dimension(:,:), allocatable :: ibdir, ibtyp, &
                                           jbcs, jbce, &
                                           kbcs, kbce, &
                                           lbcs, lbce
 !
 ! other parameters
 !
   integer :: m, p, j,k,l
   integer :: avmeshid, istat

 !
 ! generic mesh header vars
 !
   character*16  :: meshType 
   integer       :: precision, iblank

 !
 ! bfstruc mesh data
 !
   real*4, dimension(:,:,:), allocatable :: x,y,z
   real*8, dimension(:,:,:), allocatable :: x8,y8,z8
   integer, dimension(:,:,:), allocatable :: iblanks

 !
 ! initialize
 !
   character*256 :: avmesh_filename, p3d_filename
   if (command_argument_count().ne.2) &
      stop 'Usage: avmesh2fvp3d <avmesh_filename> <p3d_filename>'
   call get_command_argument(1, avmesh_filename)
   call get_command_argument(2, p3d_filename)

   call avm_read_mesh_headersf(avmeshid, avmesh_filename, istat)
   if (istat.ne.0) stop 'ERROR: avm_read_mesh_headersf'

   call avm_selectf(avmeshid, "header", 0, istat)
   call avm_get_intf(avmeshid, "precision", precision, istat)
   call avm_get_intf(avmeshid, "meshCount", n_mesh, istat)

   allocate (jdim(n_mesh),kdim(n_mesh),ldim(n_mesh))
   allocate (ibdir(max_bcs,n_mesh),ibtyp(max_bcs,n_mesh))
   allocate (jbcs(max_bcs,n_mesh),jbce(max_bcs,n_mesh))
   allocate (kbcs(max_bcs,n_mesh),kbce(max_bcs,n_mesh))
   allocate (lbcs(max_bcs,n_mesh),lbce(max_bcs,n_mesh))
   allocate (n_bcs(n_mesh))

 !
 ! bc i/o
 !
   do m = 1, n_mesh
      call avm_selectf(avmeshid, "mesh", m, istat)
      call avm_get_strf(avmeshid, "meshType", meshType, istat)
      call avm_get_intf(avmeshid, "iblank", iblank, istat)

      call avm_get_intf(avmeshid, "jdim", jdim(m), istat)
      call avm_get_intf(avmeshid, "kdim", kdim(m), istat)
      call avm_get_intf(avmeshid, "ldim", ldim(m), istat)

      call avm_get_intf(avmeshid, "nPatches", n_bcs(m), istat)

      do p = 1, n_bcs(m)
         call avm_selectf(avmeshid, "patch", p, istat)
         call avm_get_intf(avmeshid, "bctype", ibtyp(p,m), istat)
         call avm_get_intf(avmeshid, "direction", ibdir(p,m), istat)
         call avm_get_intf(avmeshid, "jbegin", jbcs(p,m), istat)
         call avm_get_intf(avmeshid, "jend", jbce(p,m), istat)
         call avm_get_intf(avmeshid, "kbegin", kbcs(p,m), istat)
         call avm_get_intf(avmeshid, "kend", kbce(p,m), istat)
         call avm_get_intf(avmeshid, "lbegin", lbcs(p,m), istat)
         call avm_get_intf(avmeshid, "lend", lbce(p,m), istat)
      end do
   end do

   call bcmap(n_mesh,jbcs,jbce,kbcs,kbce,lbcs,lbce, &
              ibdir,ibtyp,jdim,kdim,ldim,n_bcs)

   call bcout(n_mesh,jbcs,jbce,kbcs,kbce,lbcs,lbce, &
              ibdir,ibtyp,jdim,kdim,ldim,n_bcs, &
              p3d_filename)
 !
 ! mesh i/o
 !
   open(10,file=p3d_filename,form='unformatted')
   if (n_mesh.gt.1) write(10) n_mesh
   write(10) (jdim(m),kdim(m),ldim(m),m=1,n_mesh)

   do m = 1, n_mesh
      call avm_selectf(avmeshid, "mesh", m)
      if (precision.eq.1) then
         allocate (x(jdim(m),kdim(m),ldim(m)), &
                   y(jdim(m),kdim(m),ldim(m)), &
                   z(jdim(m),kdim(m),ldim(m)))
         call avm_bfstruc_read_xyz_r4f(avmeshid, x,y,z, jdim(m)*kdim(m)*ldim(m), istat)
         if (istat.ne.0) stop 'ERROR: avm_bfstruc_read_xyz_r4f'
      else if (precision.eq.2) then
         allocate (x8(jdim(m),kdim(m),ldim(m)), &
                   y8(jdim(m),kdim(m),ldim(m)), &
                   z8(jdim(m),kdim(m),ldim(m)))
         call avm_bfstruc_read_xyz_r8f(avmeshid, x8,y8,z8, jdim(m)*kdim(m)*ldim(m), istat)
         if (istat.ne.0) stop 'ERROR: avm_bfstruc_read_xyz_r8f'
      end if

      if(iblank.eq.1) then
         allocate (iblanks(jdim(m),kdim(m),ldim(m)))
         call avm_bfstruc_read_iblankf(avmeshid, iblanks, jdim(m)*kdim(m)*ldim(m), istat)
         if (istat.ne.0) stop 'ERROR: avm_bfstruc_read_iblankf'
      end if

      if (precision.eq.1) then
         if(iblank.eq.1) then
            write(10) &
              (((x(j,k,l),j=1,jdim(m)),k=1,kdim(m)),l=1,ldim(m)), &
              (((y(j,k,l),j=1,jdim(m)),k=1,kdim(m)),l=1,ldim(m)), &
              (((z(j,k,l),j=1,jdim(m)),k=1,kdim(m)),l=1,ldim(m)), &
              (((iblanks(j,k,l),j=1,jdim(m)),k=1,kdim(m)),l=1,ldim(m))
         else
            write(10) &
              (((x(j,k,l),j=1,jdim(m)),k=1,kdim(m)),l=1,ldim(m)), &
              (((y(j,k,l),j=1,jdim(m)),k=1,kdim(m)),l=1,ldim(m)), &
              (((z(j,k,l),j=1,jdim(m)),k=1,kdim(m)),l=1,ldim(m))
         end if
         deallocate (x,y,z)
      else if (precision.eq.2) then      
         if(iblank.eq.1) then
            write(10) &
              (((x8(j,k,l),j=1,jdim(m)),k=1,kdim(m)),l=1,ldim(m)), &
              (((y8(j,k,l),j=1,jdim(m)),k=1,kdim(m)),l=1,ldim(m)), &
              (((z8(j,k,l),j=1,jdim(m)),k=1,kdim(m)),l=1,ldim(m)), &
              (((iblanks(j,k,l),j=1,jdim(m)),k=1,kdim(m)),l=1,ldim(m))
         else
            write(10) &
              (((x8(j,k,l),j=1,jdim(m)),k=1,kdim(m)),l=1,ldim(m)), &
              (((y8(j,k,l),j=1,jdim(m)),k=1,kdim(m)),l=1,ldim(m)), &
              (((z8(j,k,l),j=1,jdim(m)),k=1,kdim(m)),l=1,ldim(m))
         end if
         deallocate (x8,y8,z8)
      end if

      if (allocated(iblanks)) deallocate (iblanks)
   end do

 !
 ! terminate
 !
   close(10)

   deallocate (jdim,kdim,ldim)
   deallocate (ibdir,ibtyp)
   deallocate (jbcs,jbce)
   deallocate (kbcs,kbce)
   deallocate (lbcs,lbce)
   deallocate (n_bcs)

   call avm_closef(avmeshid, istat)

end program avmesh2fvp3d

subroutine bcmap(nm,jbcs,jbce,kbcs,kbce,lbcs,lbce, &
            ibdir,ibtyp,jd,kd,ld,nofbc)

      Implicit NONE

      INTEGER,  PARAMETER :: NOFBCS = 500
      integer nm
      integer jd(nm),kd(nm),ld(nm)
      integer jbcs(nofbcs,nm),jbce(nofbcs,nm)
      integer kbcs(nofbcs,nm),kbce(nofbcs,nm)
      integer lbcs(nofbcs,nm),lbce(nofbcs,nm)
      integer ibdir(nofbcs,nm),ibtyp(nofbcs,nm)

      integer mm,n
      integer nofbc(nm)

      do mm=1,nm
        do n=1,nofbc(mm)
          if( jbcs(n,mm).lt.0 ) then
              jbcs(n,mm)  = jd(mm)+jbcs(n,mm)+1
          endif
          if( jbce(n,mm).lt.0 ) then
              jbce(n,mm)  = jd(mm)+jbce(n,mm)+1
          endif
          if( kbcs(n,mm).lt.0 ) then
              kbcs(n,mm)  = kd(mm)+kbcs(n,mm)+1
          endif
          if( kbce(n,mm).lt.0 ) then
              kbce(n,mm)  = kd(mm)+kbce(n,mm)+1
          endif
          if( lbcs(n,mm).lt.0 ) then
              lbcs(n,mm)  = ld(mm)+lbcs(n,mm)+1
          endif
          if( lbce(n,mm).lt.0 ) then
              lbce(n,mm)  = ld(mm)+lbce(n,mm)+1
          endif
!
!         quick range check
!
          if( jbcs(n,mm).gt.jd(mm) ) then
              print *,' '
              print *,' BC INPUT ERROR '
              print *,'GRID ',mm,' JBCSO EXCEEDS JMAX ', &
                       jbcs(n,mm),jd(mm)
          endif
          if( jbce(n,mm).gt.jd(mm) ) then
              print *,' '
              print *,' BC INPUT ERROR '
              print *,'GRID ',mm,' JBCEO EXCEEDS JMAX ', &
                       jbce(n,mm),jd(mm)
          endif
          if( kbcs(n,mm).gt.kd(mm) ) then
              print *,' '
              print *,' BC INPUT ERROR '
              print *,'GRID ',mm,' KBCSO EXCEEDS KMAX ', &
                       kbcs(n,mm),kd(mm)
          endif
          if( kbce(n,mm).gt.kd(mm) ) then
              print *,' '
              print *,' BC INPUT ERROR '
              print *,'GRID ',mm,' KBCEO EXCEEDS KMAX ', &
                       kbce(n,mm),kd(mm)
          endif
          if( lbcs(n,mm).gt.ld(mm) ) then
              print *,' '
              print *,' BC INPUT ERROR '
              print *,'GRID ',mm,' LBCSO EXCEEDS LMAX ', &
                       lbcs(n,mm),ld(mm)
          endif
          if( lbce(n,mm).gt.ld(mm) ) then
              print *,' '
              print *,' BC INPUT ERROR '
              print *,'GRID ',mm,' LBCEO EXCEEDS LMAX ', &
                       lbce(n,mm),ld(mm)
          endif
        enddo    
      enddo 
end subroutine bcmap

subroutine bcout(nm,jbcso,jbceo,kbcso,kbceo,lbcso,lbceo, &
           ibdiro,ibtypo,jd,kd,ld,nofbc,filein)
  
!  Write fvbnd file for fieldview

      Implicit NONE
 
      INTEGER,  PARAMETER :: NOFBCS = 500
      integer nm
      integer jd(nm),kd(nm),ld(nm)
      integer jbcso(nofbcs,nm),jbceo(nofbcs,nm)
      integer kbcso(nofbcs,nm),kbceo(nofbcs,nm)
      integer lbcso(nofbcs,nm),lbceo(nofbcs,nm)
      integer ibdiro(nofbcs,nm),ibtypo(nofbcs,nm)
      integer nofbc(nm)
      character*256 filein, fileout

      integer n,i,mesh,j1,j2,j3,k1,k2,k3,l1,l2,l3
      integer i1,ibdir,ii,mgo

!***********************************************************************

      fileout   = filein
      loop1:do ii=2,256
        IF( fileout(II:II).EQ.' ' ) then
            fileout(II:II) = '.'
            fileout(II+1:II+1) = 'f'
            fileout(II+2:II+2) = 'v'
            fileout(II+3:II+3) = 'b'
            fileout(II+4:II+4) = 'n'
            fileout(II+5:II+5) = 'd'
            fileout(II+6:II+6) = ' '
            exit loop1
        endif
      enddo loop1

      open(99,file=fileout,form='formatted')

      write(99,*)'FVBND 1 4'

!     list bc types for fv
!   1
      write(99,*)'inviscid adiabatic wall EXTRAP (1)'
!   2
      write(99,*)'viscous adiabatic wall EXTRAP (5)'
!   3
      write(99,*)'viscous constant temp wall EXTRAP (7)'
!   4
      write(99,*)'symmetry plane (11,12,13,17)'
!   5
      write(99,*)'polar axis (14,15,16)'
!   6
      write(99,*)'overlap(10)'
!   7
      write(99,*)'implicit slit (51,52,53)'
!   8
      write(99,*)'explicit slit (151,152,153)'
!   9
      write(99,*)'average (101)'
!   10
      write(99,*)'rieman outflow (31,32)'
!   11
      write(99,*)'constant pressure outflow (33)'
!   12
      write(99,*)'extrapolation outflow (30)'
!   13
      write(99,*)'porous wall outflow (89)'
!   14
      write(99,*)'axisymmetric boundary (90)'
!   15
      write(99,*)'generic bc 1 (201)'
!   16
      write(99,*)'generic bc 2 (202)'
!   17
      write(99,*)'generic bc 3 (203)'
!   18
      write(99,*)'generic bc 4 (204)'
!   19
      write(99,*)'generic bc 5 (205)'
!   20
      write(99,*)'frozen outflow (49)'
!   21
      write(99,*)'freestream outflow (40)'
!   22
      write(99,*)'nozzle inflow (41)'
!   23
      write(99,*)'inviscid adiabatic wall Normal mom (2)'
!   24
      write(99,*)'viscous adiabatic wall Normal mom (6)'
!   25
      write(99,*)'viscous constant temp wall Normal mom (8)'
!   26
      write(99,*)'generic porous wall'
!   27
      write(99,*)'rieman outflow - freestream (47,48)'
!   28
      write(99,*)'explicit overlap(110)'

!     write boundaries

      write(99,*)'BOUNDARIES'

      mgo     = 0
   
      do n=1,nm

          mgo   = mgo+1

!  clear subset for each grid for plot3d
 
          DO I = 1,NOFBC(n)

!  GET INFORMATION FOR BC'S

            mesh  = mgo

!  RANGES CONTAINS START AND END LOCATIONS OF SURFACE

            IBDIR     = ibdiro(i,n)/abs(ibdiro(i,n))
            J1        = jbcso(i,n)         
            J2        = jbceo(i,n)         
            if( j1.gt.j2 ) then
                j3     = j2
                j2     = j1
                j1     = j3
            endif
            K1        = kbcso(i,n)       
            K2        = kbceo(i,n)       
            if( k1.gt.k2 ) then
                k3     = k2
                k2     = k1
                k1     = k3
            endif
            L1        = lbcso(i,n)       
            L2        = lbceo(i,n)       
            if( l1.gt.l2 ) then
                l3     = l2
                l2     = l1
                l1     = l3
            endif

!  TEST ON BOUNDARY CONDITION NAME

!     ***** BCWALL *****

            IF( ibtypo(i,n).EQ.1.or.ibtypo(i,n).EQ.2 ) THEN
!1             write(99,*)'inviscid adiabatic wall'
               write(99,991)1,mesh,j1,j2,k1,k2,l1,l2,ibdir
            ELSEIF( ibtypo(i,n).EQ.5.or.ibtypo(i,n).EQ.6 ) THEN
!3             write(6,*)'viscous adiabatic wall'
               write(99,991)2,mesh,j1,j2,k1,k2,l1,l2,ibdir
            ELSEIF( ibtypo(i,n).EQ.7.or.ibtypo(i,n).EQ.8 ) THEN
!4             write(6,*)'viscous constant temp wall'
               write(99,991)3,mesh,j1,j2,k1,k2,l1,l2,ibdir

!     ***** BCSYMP *****

            ELSE IF( ibtypo(i,n).EQ.11 .or. &
                     ibtypo(i,n).EQ.12 .or. &
                     ibtypo(i,n).EQ.13 .or. &
                     ibtypo(i,n).EQ.17 ) THEN
               if( j1.eq.j2 ) then
                   if( j1.eq.1) then
                        j1    = 1
                        j2    = 1
                   else
                        j1    = j1
                        j2    = j1
                   endif
               endif
               if( k1.eq.k2 ) then
                   if( k1.eq.1) then
                        k1    = 1
                        k2    = 1
                   else
                        k1    = k1
                        k2    = k1
                   endif
               endif
               if( l1.eq.l2 ) then
                   if( l1.eq.1) then
                        l1    = 1
                        l2    = 1
                   else
                        l1    = l1
                        l2    = l1
                   endif
               endif

!5             write(6,*)'symmetry plane'
               write(99,991)4,mesh,j1,j2,k1,k2,l1,l2,ibdir

!     ***** BCPAXIS *****

            ELSE IF( ibtypo(i,n).EQ.14 .or. &
                     ibtypo(i,n).EQ.15 .or. &
                     ibtypo(i,n).EQ.16 ) THEN

!6             write(6,*)'polar axis'
               write(99,991)5,mesh,j1,j2,k1,k2,l1,l2,ibdir

!     ***** BCOLAP *****

            ELSE IF( ibtypo(i,n).EQ.10 ) THEN

!7             write(6,*)'overlap'
               write(99,991)6,mesh,j1,j2,k1,k2,l1,l2,ibdir
               if( j1.eq.j2 ) then
                   j1   = jd(mesh)
                   j2   = jd(mesh)
               endif
               if( k1.eq.k2 ) then
                   k1   = kd(mesh)
                   k2   = kd(mesh)
               endif
               if( l1.eq.l2 ) then
                   l1   = ld(mesh)
                   l2   = ld(mesh)
               endif
               write(99,991)6,mesh,j1,j2,k1,k2,l1,l2,ibdir

            ELSE IF( ibtypo(i,n).EQ.110 ) THEN

!7             write(6,*)'explicit overlap'
               write(99,991)28,mesh,j1,j2,k1,k2,l1,l2,ibdir
               if( j1.eq.j2 ) then
                   j1   = jd(mesh)
                   j2   = jd(mesh)
               endif
               if( k1.eq.k2 ) then
                   k1   = kd(mesh)
                   k2   = kd(mesh)
               endif
               if( l1.eq.l2 ) then
                   l1   = ld(mesh)
                   l2   = ld(mesh)
               endif
               write(99,991)28,mesh,j1,j2,k1,k2,l1,l2,ibdir

!     ***** IMPLICIT BCSLIT *****

            ELSE IF( ibtypo(i,n).EQ.51 .or. &
                     ibtypo(i,n).EQ.52 .or. &
                     ibtypo(i,n).EQ.53 ) THEN

!8             write(6,*)'slit'
               write(99,991)7,mesh,j1,j2,k1,k2,l1,l2,ibdir

!     ***** EXPLICIT BCSLIT *****

            ELSE IF( ibtypo(i,n).EQ.151 .or. &
                     ibtypo(i,n).EQ.152 .or. &
                     ibtypo(i,n).EQ.153 ) THEN

!8             write(6,*)'slit'
               write(99,991)8,mesh,j1,j2,k1,k2,l1,l2,ibdir

!     ***** BCAVRG *****

            ELSE IF( ibtypo(i,n).EQ.101 ) THEN

!9             write(6,*)'average'
               write(99,991)9,mesh,j1,j2,k1,k2,l1,l2,ibdir

!     ***** BCOFREM *****

            ELSE IF( ibtypo(i,n).EQ.31 .or. ibtypo(i,n).EQ.32 ) THEN

!10            write(6,*)'rieman outflow'
               write(99,991)10,mesh,j1,j2,k1,k2,l1,l2,ibdir

            ELSE IF( ibtypo(i,n).EQ.47 .or. ibtypo(i,n).EQ.48 ) THEN

!10            write(6,*)'rieman outflow'
               write(99,991)27,mesh,j1,j2,k1,k2,l1,l2,ibdir

!     ***** BCOFP *****

            ELSE IF( ibtypo(i,n).EQ.33 ) THEN

!11            write(6,*)'constant pressure outflow'
               write(99,991)11,mesh,j1,j2,k1,k2,l1,l2,ibdir

!     ***** BCOFEXT *****

            ELSE IF( ibtypo(i,n).EQ.30 ) THEN

!12            write(6,*)'extrapolation outflow'
               write(99,991)12,mesh,j1,j2,k1,k2,l1,l2,ibdir

!     ***** BCPORW *****

            ELSE IF( ibtypo(i,n).EQ.89 ) THEN

!13            write(6,*)'porous wall outflow'
               write(99,991)13,mesh,j1,j2,k1,k2,l1,l2,ibdir

!     ***** BCAXI *****

            ELSE IF( ibtypo(i,n).EQ.90 ) THEN

!14            write(6,*)'axisymmetric boundary'
               write(99,991)14,mesh,j1,j2,k1,k2,l1,l2,ibdir

!     ***** BCGEN1 *****

            ELSE IF( ibtypo(i,n).EQ.201 ) THEN

!15            write(6,*)'generic bc 1'
               write(99,991)15,mesh,j1,j2,k1,k2,l1,l2,ibdir

!     ***** BCGEN2 *****

            ELSE IF( ibtypo(i,n).EQ.202 ) THEN

!16            write(6,*)'generic bc 2'
               write(99,991)16,mesh,j1,j2,k1,k2,l1,l2,ibdir

!     ***** BCGEN3 *****

            ELSE IF( ibtypo(i,n).EQ.203 ) THEN

!17            write(6,*)'generic bc 3'
               write(99,991)17,mesh,j1,j2,k1,k2,l1,l2,ibdir

!     ***** BCGEN4 *****

            ELSE IF( ibtypo(i,n).EQ.204 ) THEN

!18            write(6,*)'generic bc 4'
               write(99,991)18,mesh,j1,j2,k1,k2,l1,l2,ibdir

!     ***** BCGEN5 *****

            ELSE IF( ibtypo(i,n).EQ.205 ) THEN

!19            write(6,*)'generic bc 5'
               write(99,991)19,mesh,j1,j2,k1,k2,l1,l2,ibdir

!     ***** BC FROZEN ******

            ELSE IF( ibtypo(i,n).EQ.49 ) THEN

               write(99,991)20,mesh,j1,j2,k1,k2,l1,l2,ibdir

!     ***** BC FREESTREAM ******

            ELSE IF( ibtypo(i,n).EQ.40 ) THEN

               write(99,991)21,mesh,j1,j2,k1,k2,l1,l2,ibdir

!     ***** NOZZLE INFLOW ******

            ELSE IF( ibtypo(i,n).EQ.41 ) THEN

               write(99,991)22,mesh,j1,j2,k1,k2,l1,l2,ibdir

!     ***** Inviscid wall -Normal momentum *********

            ELSE IF( ibtypo(i,n).EQ.2 ) THEN

               write(99,991)23,mesh,j1,j2,k1,k2,l1,l2,ibdir

!     ***** Viscous wall -Normal momentum *********

            ELSE IF( ibtypo(i,n).EQ.6 ) THEN

               write(99,991)24,mesh,j1,j2,k1,k2,l1,l2,ibdir

!     ***** Viscous wall - Constant Temp *********

            ELSE IF( ibtypo(i,n).EQ.8 ) THEN

               write(99,991)25,mesh,j1,j2,k1,k2,l1,l2,ibdir

!     ***** Generic Porous wall *********

            ELSE IF( ibtypo(i,n).EQ.91 ) THEN

               I1      = 1
               write(99,991)26,mesh,j1,j2,k1,k2,l1,l2,ibdir
               if( abs(ibdiro(i,n)).eq.1 )then
                   write(99,991)26,mesh,I1,I1,k1,k2,l1,l2,ibdir
               elseif( abs(ibdiro(i,n)).eq.2 )then
                   write(99,991)26,mesh,J1,J2,I1,I1,l1,l2,ibdir
               elseif( abs(ibdiro(i,n)).eq.3 )then
                   write(99,991)26,mesh,J1,J2,K1,K2,I1,I1,ibdir
               endif

            ENDIF

        ENDDO    
      ENDDO    

      close(99)

991   format(8i8,1x,'F',1x,i8)
end subroutine bcout

