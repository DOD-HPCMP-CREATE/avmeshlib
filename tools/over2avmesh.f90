program over2avmesh
   implicit none

 !
 ! parameters read in
 !
   integer :: n_mesh
   integer, dimension(:), allocatable :: jdim, kdim, ldim, nPatches
   integer, dimension(:,:), allocatable :: ibdir, ibtyp, &
                                           jbcs, jbce, &
                                           kbcs, kbce, &
                                           lbcs, lbce
   real, dimension(:,:), allocatable :: bcpar1, bcpar2
   character*128, dimension(:), allocatable :: meshNames

 !
 ! other parameters
 !
   integer :: i_mesh, i_patch, j, k, l
   integer, parameter :: max_patches = 500
   real*8 :: r8value, refPt(3), transVec(3), rotMat(9)
   integer :: avmid, istat

 !
 ! avmesh bfstruc mesh data
 !
   real*4, dimension(:,:,:), allocatable :: x,y,z
   real*8, dimension(:,:,:), allocatable :: x8,y8,z8
   integer, dimension(:,:,:), allocatable :: iblanks

 !
 ! initialization
 !
   logical :: exists
   integer :: iargc, precision, has_iblanks, ierr
   character*256 :: over_namelist, p3d_filename, avmesh_filename

   over_namelist = 'over.namelist'
   p3d_filename = 'grid.in'
   avmesh_filename = 'grid.avm'
   iargc = command_argument_count()

   if (iargc.gt.0 .and. iargc.ne.3) call usage()

   if (iargc.eq.3) then
      call get_command_argument(1, over_namelist)
      call get_command_argument(2, p3d_filename)
      call get_command_argument(3, avmesh_filename)
   end if

   inquire(file=over_namelist, exist=exists)
   if (.not. exists) then
      write(6,'(A,A,A)') "ERROR: '", trim(over_namelist), "' does not exist."
      call usage()
   end if

   inquire(file=p3d_filename, exist=exists)
   if (.not. exists) then
      write(6,'(A,A,A)') "ERROR: '", trim(p3d_filename), "' does not exist."
      call usage()
   end if

 !
 ! initialization (overflow)
 !
   open(unit=11, file=p3d_filename, form='unformatted', status='OLD')

   read(11,iostat=ierr) j,k,l

   if (ierr.eq.0) then
      write(6,'(A)') 'single-grid'
      n_mesh = 1
      allocate (jdim(n_mesh), kdim(n_mesh), ldim(n_mesh))
      jdim(n_mesh) = j
      kdim(n_mesh) = k
      ldim(n_mesh) = l
   else
      backspace(11)
      read(11) n_mesh
      write(6,'(A)') 'multi-grid'
      allocate (jdim(n_mesh), kdim(n_mesh), ldim(n_mesh))
      read(11) (jdim(i_mesh),kdim(i_mesh),ldim(i_mesh),i_mesh=1,n_mesh)
   end if

   ! read the first mesh record to determine
   ! precision and if we have iblanks or not

   allocate (iblanks(jdim(1),kdim(1),ldim(1)))
   allocate (x8(jdim(1),kdim(1),ldim(1)))
   allocate (y8(jdim(1),kdim(1),ldim(1)))
   allocate (z8(jdim(1),kdim(1),ldim(1)))
   allocate (x(jdim(1),kdim(1),ldim(1)))
   allocate (y(jdim(1),kdim(1),ldim(1)))
   allocate (z(jdim(1),kdim(1),ldim(1)))

   has_iblanks = 1
   precision = 2
   read(11,iostat=ierr) &
      (((x8(j,k,l),j=1,jdim(1)),k=1,kdim(1)),l=1,ldim(1)), &
      (((y8(j,k,l),j=1,jdim(1)),k=1,kdim(1)),l=1,ldim(1)), &
      (((z8(j,k,l),j=1,jdim(1)),k=1,kdim(1)),l=1,ldim(1)), &
      (((iblanks(j,k,l),j=1,jdim(1)),k=1,kdim(1)),l=1,ldim(1))

   if (ierr.ne.0) then
      backspace(11)
      has_iblanks = 0
      precision = 2
      read(11,iostat=ierr) &
         (((x8(j,k,l),j=1,jdim(1)),k=1,kdim(1)),l=1,ldim(1)), &
         (((y8(j,k,l),j=1,jdim(1)),k=1,kdim(1)),l=1,ldim(1)), &
         (((z8(j,k,l),j=1,jdim(1)),k=1,kdim(1)),l=1,ldim(1))
   end if

   if (ierr.ne.0) then
      backspace(11)
      has_iblanks = 1
      precision = 1
      read(11,iostat=ierr) &
         (((x(j,k,l),j=1,jdim(1)),k=1,kdim(1)),l=1,ldim(1)), &
         (((y(j,k,l),j=1,jdim(1)),k=1,kdim(1)),l=1,ldim(1)), &
         (((z(j,k,l),j=1,jdim(1)),k=1,kdim(1)),l=1,ldim(1)), &
         (((iblanks(j,k,l),j=1,jdim(1)),k=1,kdim(1)),l=1,ldim(1))
   end if

   if (ierr.ne.0) then
      backspace(11)
      has_iblanks = 0
      precision = 1
      read(11,iostat=ierr) &
         (((x(j,k,l),j=1,jdim(1)),k=1,kdim(1)),l=1,ldim(1)), &
         (((y(j,k,l),j=1,jdim(1)),k=1,kdim(1)),l=1,ldim(1)), &
         (((z(j,k,l),j=1,jdim(1)),k=1,kdim(1)),l=1,ldim(1))
   end if

   backspace(11)
   if (ierr.ne.0) then
      stop 'ERROR: reading first mesh record'
   end if

   deallocate (iblanks)
   deallocate (x8,y8,z8)
   deallocate (x,y,z)

   write(6,'(A,I1)') 'iblanks: ', has_iblanks
   write(6,'(A,I1)') 'precision: ', precision

 !
 ! initialization (avmesh)
 !
   call avm_new_filef(avmid, avmesh_filename, istat)
   if (istat.ne.0) stop 'ERROR: avm_new_file'

   refPt(:) = (/0.0,0.0,0.0/)
   transVec(:) = (/0.0,0.0,0.0/)
   rotMat(1:3) = (/1.0,0.0,0.0/)
   rotMat(4:6) = (/0.0,1.0,0.0/)
   rotMat(7:9) = (/0.0,0.0,1.0/)

   call avm_selectf(avmid, "header", 0, istat)
   call avm_set_strf(avmid, "formatVersion", "1.0", istat)
   call avm_set_strf(avmid, "meshRevision", "", istat)
   call avm_set_intf(avmid, "meshCount", n_mesh, istat)
   call avm_set_strf(avmid, "contactName", "", istat)
   call avm_set_strf(avmid, "contactOrg", "", istat)
   call avm_set_strf(avmid, "contactEmail", "", istat)
   call avm_set_intf(avmid, "precision", precision, istat)
   call avm_set_intf(avmid, "dimensions", 3, istat)
   call avm_set_real_arrayf(avmid, "referencePoint", refPt, 3, istat)
   call avm_set_strf(avmid, "referencePointDescription", "origin", istat)
   call avm_set_strf(avmid, "coordinateSystem", "xByUzL", istat)
   call avm_set_realf(avmid, "modelScale", 1.0d0, istat)
   call avm_set_strf(avmid, "gridUnits", "ft", istat)
   call avm_set_strf(avmid, "description", &
        "An AVMesh file converted from Overflow mesh and namelist file.", istat)

   allocate (jbcs(max_patches,n_mesh),jbce(max_patches,n_mesh))
   allocate (kbcs(max_patches,n_mesh),kbce(max_patches,n_mesh))
   allocate (lbcs(max_patches,n_mesh),lbce(max_patches,n_mesh))
   allocate (ibdir(max_patches,n_mesh),ibtyp(max_patches,n_mesh))
   allocate (bcpar1(max_patches,n_mesh),bcpar2(max_patches,n_mesh))
   allocate (meshNames(n_mesh))
   allocate (nPatches(n_mesh))

   call bcread(n_mesh,jbcs,jbce,kbcs,kbce,lbcs,lbce, &
         ibdir,ibtyp,bcpar1,bcpar2,jdim,kdim,ldim,nPatches, &
         meshNames,over_namelist)

   do i_mesh=1,n_mesh
      call avm_selectf(avmid, "mesh", i_mesh, istat)

      ! generic mesh fields
      call avm_set_strf(avmid, "meshName", meshNames(i_mesh), istat)
      call avm_set_strf(avmid, "meshType", "bfstruc", istat)
      call avm_set_strf(avmid, "meshGenerator", "over2avmesh", istat)
      call avm_set_strf(avmid, "changedDate", "", istat)
      call avm_set_strf(avmid, "coordinateSystem", "", istat)
      call avm_set_realf(avmid, "modelScale", 1.0d0, istat)
      call avm_set_strf(avmid, "gridUnits", "", istat)
      call avm_set_realf(avmid, "reynoldsNumber", 1.0d0, istat)
      call avm_set_realf(avmid, "referenceLength", 1.0d0, istat)
      call avm_set_realf(avmid, "wallDistance", 1.0d0, istat)
      call avm_set_real_arrayf(avmid, "referencePoint", refPt, 3, istat)
      call avm_set_strf(avmid, "referencePointDescription", "", istat)
      call avm_set_real_arrayf(avmid, "translationVector", transVec, 3, istat)
      call avm_set_real_arrayf(avmid, "rotationMatrix", rotMat, 9, istat)
      call avm_set_realf(avmid, "periodicity", 1.0d0, istat)
      call avm_set_strf(avmid, "periodicityDescription", "", istat)
      call avm_set_intf(avmid, "refinementLevel", 1, istat)
      call avm_set_intf(avmid, "iblank", has_iblanks, istat)
      call avm_set_strf(avmid, "description", "", istat)

      ! bfstruc mesh fields
      call avm_set_intf(avmid, "jmax", jdim(i_mesh), istat)
      call avm_set_intf(avmid, "kmax", kdim(i_mesh), istat)
      call avm_set_intf(avmid, "lmax", ldim(i_mesh), istat)
      call avm_set_intf(avmid, "nPatches", nPatches(i_mesh), istat)
      call avm_set_intf(avmid, "nPatchIntParams", 1, istat)
      call avm_set_intf(avmid, "nPatchR8Params", 2, istat)
      call avm_set_intf(avmid, "nPatchC80Params", 1, istat)

      do i_patch=1,nPatches(i_mesh)
         call avm_selectf(avmid, "patch", i_patch, istat)
         call avm_set_intf(avmid, "bctype", ibtyp(i_patch,i_mesh), istat)
         call avm_set_intf(avmid, "direction", ibdir(i_patch,i_mesh), istat)
         call avm_set_intf(avmid, "jbegin", jbcs(i_patch,i_mesh), istat)
         call avm_set_intf(avmid, "jend", jbce(i_patch,i_mesh), istat)
         call avm_set_intf(avmid, "kbegin", kbcs(i_patch,i_mesh), istat)
         call avm_set_intf(avmid, "kend", kbce(i_patch,i_mesh), istat)
         call avm_set_intf(avmid, "lbegin", lbcs(i_patch,i_mesh), istat)
         call avm_set_intf(avmid, "lend", lbce(i_patch,i_mesh), istat)
         call avm_set_strf(avmid, "patch_description", "", istat)

         call avm_selectf(avmid, "patchParam", 1, istat)
         call avm_set_intf(avmid, "patch_int_param", 1, istat)
         call avm_set_intf(avmid, "patch_c80_param", 'bc character parameter', istat)
         r8value = bcpar1(i_patch,i_mesh)
         call avm_set_realf(avmid, "patch_r8_param", r8value, istat)

         call avm_selectf(avmid, "patchParam", 2, istat)
         r8value = bcpar2(i_patch,i_mesh)
         call avm_set_realf(avmid, "patch_r8_param", r8value, istat)
      end do
   end do

 !
 ! write
 ! 
   call avm_write_headersf(avmid, istat) 
   if (istat.ne.0) stop 'ERROR: avm_write_headers'

   do i_mesh=1,n_mesh
      call avm_selectf(avmid, "mesh", i_mesh, istat)

      if (precision.eq.1) then
         allocate (x(jdim(i_mesh),kdim(i_mesh),ldim(i_mesh)), &
                   y(jdim(i_mesh),kdim(i_mesh),ldim(i_mesh)), &
                   z(jdim(i_mesh),kdim(i_mesh),ldim(i_mesh)))

         if(has_iblanks.eq.1) then
            allocate (iblanks(jdim(i_mesh),kdim(i_mesh),ldim(i_mesh)))
            read(11) &
               (((x(j,k,l),j=1,jdim(i_mesh)),k=1,kdim(i_mesh)),l=1,ldim(i_mesh)), &
               (((y(j,k,l),j=1,jdim(i_mesh)),k=1,kdim(i_mesh)),l=1,ldim(i_mesh)), &
               (((z(j,k,l),j=1,jdim(i_mesh)),k=1,kdim(i_mesh)),l=1,ldim(i_mesh)), &
               (((iblanks(j,k,l),j=1,jdim(i_mesh)),k=1,kdim(i_mesh)),l=1,ldim(i_mesh))
         else
            read(11) &
               (((x(j,k,l),j=1,jdim(i_mesh)),k=1,kdim(i_mesh)),l=1,ldim(i_mesh)), &
               (((y(j,k,l),j=1,jdim(i_mesh)),k=1,kdim(i_mesh)),l=1,ldim(i_mesh)), &
               (((z(j,k,l),j=1,jdim(i_mesh)),k=1,kdim(i_mesh)),l=1,ldim(i_mesh))
         end if

         call avm_bfstruc_write_xyz_r4f(avmid, x,y,z, &
                 jdim(i_mesh)*kdim(i_mesh)*ldim(i_mesh), istat)
         if (istat.ne.0) stop 'ERROR: avm_bfstruc_write_xyz_r4'

         if(has_iblanks.eq.1) then
            call avm_bfstruc_write_iblankf(avmid, iblanks, &
                    jdim(i_mesh)*kdim(i_mesh)*ldim(i_mesh), istat)
            if (istat.ne.0) stop 'ERROR: avm_bfstruc_write_iblank'
            deallocate (iblanks)
         end if

         deallocate (x,y,z)
      else if (precision.eq.2) then
         allocate (x8(jdim(i_mesh),kdim(i_mesh),ldim(i_mesh)), &
                   y8(jdim(i_mesh),kdim(i_mesh),ldim(i_mesh)), &
                   z8(jdim(i_mesh),kdim(i_mesh),ldim(i_mesh)))

         if(has_iblanks.eq.1) then
            allocate (iblanks(jdim(i_mesh),kdim(i_mesh),ldim(i_mesh)))
            read(11) &
               (((x8(j,k,l),j=1,jdim(i_mesh)),k=1,kdim(i_mesh)),l=1,ldim(i_mesh)), &
               (((y8(j,k,l),j=1,jdim(i_mesh)),k=1,kdim(i_mesh)),l=1,ldim(i_mesh)), &
               (((z8(j,k,l),j=1,jdim(i_mesh)),k=1,kdim(i_mesh)),l=1,ldim(i_mesh)), &
               (((iblanks(j,k,l),j=1,jdim(i_mesh)),k=1,kdim(i_mesh)),l=1,ldim(i_mesh))
         else
            read(11) &
               (((x8(j,k,l),j=1,jdim(i_mesh)),k=1,kdim(i_mesh)),l=1,ldim(i_mesh)), &
               (((y8(j,k,l),j=1,jdim(i_mesh)),k=1,kdim(i_mesh)),l=1,ldim(i_mesh)), &
               (((z8(j,k,l),j=1,jdim(i_mesh)),k=1,kdim(i_mesh)),l=1,ldim(i_mesh))
         end if

         call avm_bfstruc_write_xyz_r8f(avmid, x8,y8,z8, &
                 jdim(i_mesh)*kdim(i_mesh)*ldim(i_mesh), istat)
         if (istat.ne.0) stop 'ERROR: avm_bfstruc_write_xyz_r8'

         if(has_iblanks.eq.1) then
            call avm_bfstruc_write_iblankf(avmid, iblanks, &
                    jdim(i_mesh)*kdim(i_mesh)*ldim(i_mesh), istat)
            if (istat.ne.0) stop 'ERROR: avm_bfstruc_write_iblank'
            deallocate (iblanks)
         end if

         deallocate (x8,y8,z8)
      end if
   end do

   call avm_closef(avmid, istat)
   if (istat.ne.0) stop 'ERROR: avm_close'

   close(11)   
end program over2avmesh

subroutine usage()
   print *, 'Usage: over2avmesh <over_namelist> <p3d_filename> <avmesh_filename>'
   print *, '             over_namelist defaults to over.namelist'
   print *, '             p3d_filename defaults to grid.in'
   print *, '             avmesh_filename defaults to grid.avmesh'
   stop
end subroutine usage

subroutine bcread(nm,jbcso,jbceo,kbcso,kbceo,lbcso,lbceo, &
            ibdiro,ibtypo,bcpar1o,bcpar2o,jd,kd,ld,nofbc, &
            meshnames,filename)

! Read overflow namelist input to get boundary condition information

      Implicit NONE

      INTEGER,  PARAMETER :: NOFBCS = 500
      integer nm
      integer jd(nm),kd(nm),ld(nm)
      integer jbcso(nofbcs,nm),jbceo(nofbcs,nm)
      integer kbcso(nofbcs,nm),kbceo(nofbcs,nm)
      integer lbcso(nofbcs,nm),lbceo(nofbcs,nm)
      integer ibdiro(nofbcs,nm),ibtypo(nofbcs,nm)
      real bcpar1o(nofbcs,nm),bcpar2o(nofbcs,nm)

      integer mesh,mm,n,nofbct,nbc
      integer jbcs(nofbcs),jbce(nofbcs)
      integer kbcs(nofbcs),kbce(nofbcs)
      integer lbcs(nofbcs),lbce(nofbcs)
      integer ibdir(nofbcs),ibtyp(nofbcs)
      integer nofbc(nm)
      real bcpar1(nofbcs),bcpar2(nofbcs)
      character*40 bcfile(nofbcs)
      character*128 name
      character*128 meshnames(nm)
      character*(*) filename

      NAMELIST /GRDNAM/ name

      NAMELIST /BCINP/ ibtyp,ibdir,jbcs,jbce,kbcs,kbce,lbcs,lbce, &
                       bcpar1,bcpar2,nbc,bcfile

      open(8,file=filename,form='formatted',status='OLD')
      do 10 mm=1,nm
        read(8,GRDNAM,ERR=10,END=10)
        meshnames(mm) = name

        do n=1,nofbcs
          jbcs(n)   = 9999
          jbce(n)   = 9999
          kbcs(n)   = 9999
          kbce(n)   = 9999
          lbcs(n)   = 9999
          lbce(n)   = 9999
          ibdir(n)  = 9999
          ibtyp(n)  = 9999
          bcpar1(n) = -1.0
          bcpar2(n) = 1.0
        enddo    
!
!       read namelist
!      
        read(8,BCINP,ERR=10,END=10)
!
!       get the number of bc's in namelist for each grid
!
        loop1:do n=1,nofbcs
          if( ibtyp(n).eq.9999 ) then
              nofbct  = n-1
              exit loop1   
          endif
          nofbct     = n
        enddo loop1   
        nofbc(mm) = nofbct
        print *,'Mesh ',mm,' has ',nofbc(mm),' bcs'

        do n=1,nofbc(mm)
          ibtypo(n,mm)  = ibtyp(n)
          ibdiro(n,mm)  = ibdir(n)
          bcpar1o(n,mm) = bcpar1(n)
          bcpar2o(n,mm) = bcpar2(n)
          jbcso(n,mm)   = jbcs(n)
          jbceo(n,mm)   = jbce(n)
          kbcso(n,mm)   = kbcs(n)
          kbceo(n,mm)   = kbce(n)
          lbcso(n,mm)   = lbcs(n)
          lbceo(n,mm)   = lbce(n)
        enddo    

 10   continue

      close(8)
end subroutine bcread
