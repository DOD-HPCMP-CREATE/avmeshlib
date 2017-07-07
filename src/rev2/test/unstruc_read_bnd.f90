! This is an example of using the avmesh library with Fortran90.
! Error checking is ignored for brevity.
program unstruc_read_bnd
   implicit none
   integer :: avmid, meshCount
   integer :: nNodes,nEdges,nFaces
   integer :: i, istat

   integer(4) :: nBndTriFaces
   integer(4) :: nBndQuadFaces

   ! unstruc mesh data
   real(8), allocatable :: xyz(:,:)
   integer, allocatable :: bndTriFaces(:,:)
   integer, allocatable :: bndQuadFaces(:,:)

   call avm_read_headersf(avmid, 'unstruc.avm', istat)
   if (istat.ne.0) stop 'ERROR: not an AVMesh file'

   call avm_get_intf(avmid, 'meshCount', meshCount, istat)
   if (istat.ne.0) stop 'ERROR: avm_get_intf failed'

   print '(A,1X,I0)', 'Mesh Count is', meshCount

   if (meshCount /= 1) then
      print '(A)', 'Mesh Count incorrect, exiting'
      call exit(1)
   end if

   call avm_selectf(avmid, 'mesh', 1, istat)

   call avm_get_intf(avmid, 'nNodes', nNodes, istat)
   call avm_get_intf(avmid, 'nBndTriFaces', nBndTriFaces, istat)
   call avm_get_intf(avmid, 'nBndQuadFaces', nBndQuadFaces, istat)

   allocate(xyz(3,nNodes), &
            bndTriFaces(4,nBndTriFaces), &
            bndQuadFaces(5,nBndQuadFaces))

   call avm_unstruc_read_nodes_r8f(avmid, xyz, 3*nNodes, istat)
   if (istat.ne.0) stop 'ERROR: failed to read nodes'

   if (xyz(2,12) - 15.0 > 0.00001 ) then
     print '(A)', 'xyz incorrect, exiting'
     call exit(1)
   end if

   call avm_unstruc_read_bnd_facesf(avmid, &
          bndTriFaces,  4*nBndTriFaces, &
          bndQuadFaces, 5*nBndQuadFaces, &
          istat)
   if (istat.ne.0) stop 'ERROR: failed to read bnd faces'

   call printa("bndTriFaces", bndTriFaces, 4, nBndTriFaces)
   call printa("bndQuadFaces", bndQuadFaces, 5, nBndQuadFaces)

   if (bndTriFaces(2,4) /= 7) then
     print '(A)', 'bndTriFaces incorrect, exiting'
     call exit(1)
   end if

   call avm_closef(avmid, istat)
   write(6,'(A)')  'Successfully read: unstruc.avm'
end program unstruc_read_bnd

subroutine printa(n, a, nI, nJ)
   implicit none
   character(*) :: n
   integer :: i,j,nI,nJ
   integer :: a(nI,nJ)
   print *, n
   do j = 1,nJ
      print *, (a(i,j),i=1,nI)
   end do
end subroutine

subroutine printb(n, a, sizeA, nI)
   implicit none
   character(*) :: n
   integer :: i,j,k,nI,sizeA,nodesInFace
   integer :: a(sizeA)
   print *, n
   k = 1
   do i = 1,nI
      nodesInFace = a(k)
      print *, (a(j),j=k,k+nodesInFace+2)
      k = k + nodesInFace + 3
   end do
end subroutine

