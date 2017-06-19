program cart_write
   implicit none
   character*80, parameter :: filename='cart.avm'
   integer, parameter :: nMesh=1, nLevels=3, nBlocks=6, nFringe=3
   integer :: b,i,j,k,l,jd,kd,ld
   integer :: avmid, istat, avmeshRev=1
   character*256 :: str

   real*8 :: refPt(3)

 !
 ! cart header types/data 
 !
   type cart_block
      sequence
      integer(4) :: ilo(3)
      integer(4) :: ihi(3)
      integer(4) :: levNum
      integer(4) :: iblFlag
      integer(4) :: bdryFlag
      integer(4) :: iblDim(3)
      integer(4) :: blo(3)
      integer(4) :: bhi(3)
   end type cart_block

   integer :: ratios(nLevels)
   type(cart_block) :: cart_blks(nBlocks)

 !
 ! cart mesh data (just iblanks)
 !
   integer,parameter :: block2_iblDim(3) = (/4,6,9/)
   integer,parameter :: block6_iblDim(3) = (/8,9,10/)
   integer :: block2_iblanks(block2_iblDim(1), block2_iblDim(2), block2_iblDim(3))
   integer :: block6_iblanks(block6_iblDim(1), block6_iblDim(2), block6_iblDim(3))

   character*80 :: write_endian
   write_endian = "native"
   if (command_argument_count() > 0) then
      call get_command_argument(1, write_endian)
   endif

 !
 ! initialization
 !
   call avm_new_filef(avmid, filename, avmeshRev, istat)
   if (istat.ne.0) stop 'ERROR: avm_new_file'

   call avm_set_strf(avmid, 'AVM_WRITE_ENDIAN', write_endian, istat)
   if (istat.ne.0) stop 'ERROR: avm_set_str("AVM_WRITE_ENDIAN")'

   refPt(:) = (/0.0,0.0,0.0/)

   ! file header fields
   call avm_selectf(avmid, 'header', 0, istat)
   call avm_set_strf(avmid, 'formatVersion', '1.0', istat)
   call avm_set_intf(avmid, 'meshCount', nMesh, istat)
   call avm_set_strf(avmid, 'contactInfo', 'Andy Wissink: awissink@mail.arc.nasa.gov', istat)
   call avm_set_intf(avmid, 'precision', 2, istat)
   call avm_set_intf(avmid, 'dimensions', 3, istat)
   call avm_set_strf(avmid, 'description', &
                            'An example AVMesh file with a cartesian grid.', istat)

   ! generic mesh fields
   call avm_selectf(avmid, 'mesh', 1, istat)
   write(str, '(A,I0)') 'cart mesh # ', 1 
   call avm_set_strf(avmid, 'meshName', str, istat)
   call avm_set_strf(avmid, 'meshType', 'cart', istat)
   call avm_set_strf(avmid, 'meshGenerator', 'cart_write', istat)
   call avm_set_strf(avmid, 'coordinateSystem', 'xByUzL', istat)
   call avm_set_realf(avmid, 'modelScale', 1.0d0, istat)
   call avm_set_strf(avmid, 'gridUnits', 'm', istat)
   call avm_set_realf(avmid, 'referenceLength', 1.0d0, istat)
   call avm_set_realf(avmid, 'referenceArea', 1.0d0, istat)
   call avm_set_real_arrayf(avmid, 'referencePoint', refPt, 3, istat)
   call avm_set_strf(avmid, 'referencePointDescription', 'origin', istat)
   call avm_set_strf(avmid, 'meshDescription', &
             'A nonsensical cart mesh that just demonstrates the format.', istat)

   ! cart specific header fields

   call avm_set_intf(avmid, 'nLevels', nLevels, istat)
   call avm_set_intf(avmid, 'nBlocks', nBlocks, istat)
   call avm_set_intf(avmid, 'nFringe', nFringe, istat)
   call avm_set_intf(avmid, 'nxc', 10, istat)
   call avm_set_intf(avmid, 'nyc', 10, istat)
   call avm_set_intf(avmid, 'nzc', 10, istat)
   call avm_set_realf(avmid, 'domXLo', 0.0d0, istat)
   call avm_set_realf(avmid, 'domYLo', 0.0d0, istat)
   call avm_set_realf(avmid, 'domZLo', 0.0d0, istat)
   call avm_set_realf(avmid, 'domXHi', 10.0d0, istat)
   call avm_set_realf(avmid, 'domYHi', 10.0d0, istat)
   call avm_set_realf(avmid, 'domZHi', 10.0d0, istat)

   ! cart level/ratio data

   call avm_selectf(avmid, 'level', 1, istat)
   call avm_set_intf(avmid, 'iRatio', 1, istat)
   call avm_selectf(avmid, 'level', 2, istat)
   call avm_set_intf(avmid, 'iRatio', 2, istat)
   call avm_selectf(avmid, 'level', 3, istat)
   call avm_set_intf(avmid, 'iRatio', 4, istat)

   ! cart block data

   ! block 1 - level 1
   cart_blks(1)%levNum    = 1
   cart_blks(1)%ilo(:)    = (/1,1,1/)
   cart_blks(1)%ihi(:)    = (/10,10,10/)
   cart_blks(1)%iblflag   = 1 ! all points unblanked
   cart_blks(1)%bdryflag  = 1 ! resides on a farfield boundary
   cart_blks(1)%iblDim(:) = 0 
   cart_blks(1)%blo(:)    = (/1,1,1/)
   cart_blks(1)%bhi(:)    = (/1,1,1/) 

   ! block 2 - level 2
   cart_blks(2)%levNum    = 2
   cart_blks(2)%ilo(:)    = (/7,7,7/)
   cart_blks(2)%ihi(:)    = (/14,14,14/)
   cart_blks(2)%iblflag   = 2 ! mixed iblanks
   cart_blks(2)%bdryflag  = 0 ! not on a boundary 
   cart_blks(2)%iblDim(:) = block2_iblDim
   cart_blks(2)%blo(:)    = -1
   cart_blks(2)%bhi(:)    = -1
   ! set all points unblanked, then a few blanked (for testing)
   block2_iblanks = 1
   block2_iblanks(1,1,1) = 0
   block2_iblanks(3,5,7) = 0
   block2_iblanks(4,6,9) = 0

   ! block 3 - level 3
   cart_blks(3)%levNum    = 3
   cart_blks(3)%ilo(:)    = (/17,17,17/)
   cart_blks(3)%ihi(:)    = (/20,20,24/)
   cart_blks(3)%iblflag   = 1 ! all points unblanked
   cart_blks(3)%bdryflag  = 0 ! not on a boundary 
   cart_blks(3)%iblDim(:) = 0 
   cart_blks(3)%blo(:)    = -1
   cart_blks(3)%bhi(:)    = -1

   ! block 4 - level 3
   cart_blks(4)%levNum    = 3
   cart_blks(4)%ilo(:)    = (/21,17,17/)
   cart_blks(4)%ihi(:)    = (/24,20,24/)
   cart_blks(4)%iblflag   = 1 ! all points unblanked
   cart_blks(4)%bdryflag  = 0 ! not on a boundary 
   cart_blks(4)%iblDim(:) = 0 
   cart_blks(4)%blo(:)    = -1
   cart_blks(4)%bhi(:)    = -1

   ! block 5 - level 3
   cart_blks(5)%levNum    = 3
   cart_blks(5)%ilo(:)    = (/17,21,17/)
   cart_blks(5)%ihi(:)    = (/20,24,24/)
   cart_blks(5)%iblflag   = 1 ! all points unblanked
   cart_blks(5)%bdryflag  = 0 ! not on a boundary 
   cart_blks(5)%iblDim(:) = 0 
   cart_blks(5)%blo(:)    = -1
   cart_blks(5)%bhi(:)    = -1

   ! block 6 - level 3
   cart_blks(6)%levNum    = 3
   cart_blks(6)%ilo(:)    = (/21,21,17/)
   cart_blks(6)%ihi(:)    = (/24,24,24/)
   cart_blks(6)%iblflag   = 2 ! mixed iblanks
   cart_blks(6)%bdryflag  = 0 ! not on a boundary 
   cart_blks(6)%iblDim(:) = block6_iblDim 
   cart_blks(6)%blo(:)    = -1
   cart_blks(6)%bhi(:)    = -1
   ! set all points unblanked, then a few blanked (for testing)
   block6_iblanks = 1
   block6_iblanks(1,1,1) = 0
   block6_iblanks(2,2,8) = 0
   block6_iblanks(8,9,10) = 0

   do b=1,nBlocks
      call avm_selectf(avmid, 'block', b, istat)
      call avm_set_int_arrayf(avmid, 'ilo', cart_blks(b)%ilo, 3, istat)
      call avm_set_int_arrayf(avmid, 'ihi', cart_blks(b)%ihi, 3, istat)
      call avm_set_intf(avmid, 'levNum', cart_blks(b)%levNum, istat)
      call avm_set_intf(avmid, 'iblFlag', cart_blks(b)%iblFlag, istat)
      call avm_set_intf(avmid, 'bdryFlag', cart_blks(b)%bdryFlag, istat)
      call avm_set_int_arrayf(avmid, 'iblDim', cart_blks(b)%iblDim, 3, istat)
      call avm_set_int_arrayf(avmid, 'blo', cart_blks(b)%blo, 3, istat)
      call avm_set_int_arrayf(avmid, 'bhi', cart_blks(b)%bhi, 3, istat)
   end do

 !
 ! write
 ! 
   call avm_write_headersf(avmid, istat) 
   if (istat.ne.0) stop 'ERROR: avm_write_headers'

   call avm_selectf(avmid, 'mesh', 1, istat)

   do b=1,nBlocks
      call avm_selectf(avmid, 'block', b, istat)
      if (cart_blks(b)%iblFlag==2) then
         jd = cart_blks(b)%iblDim(1)
         kd = cart_blks(b)%iblDim(2)
         ld = cart_blks(b)%iblDim(3)
         if (b==2) then
            call avm_cart_write_iblankf(avmid, block2_iblanks, jd*kd*ld, istat)
         else if (b==6) then
            call avm_cart_write_iblankf(avmid, block6_iblanks, jd*kd*ld, istat)
         endif
         if (istat.ne.0) stop 'ERROR: avm_cart_write_iblank'
      end if
   end do

   call avm_closef(avmid, istat)
   if (istat.ne.0) stop 'ERROR: avm_close'

   write(6,'(A,A)')  'Successfully wrote: ', filename
end program cart_write
