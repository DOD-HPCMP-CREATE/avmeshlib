! This is an example of using the avmesh library with Fortran90.
! Error checking is ignored for brevity.
program example
   implicit none
   integer :: avmid, meshCount
   real*8 :: modelScale, ref_point(3)
   character*128 :: description, contactInfo
   character*128 :: meshName, meshType
   integer :: jmax,kmax,lmax
   integer :: nBndEdges,nPtsPerStrand
   integer :: nNodes,nEdges,nFaces
   integer :: nLevels,nBlocks,nFringe
   integer :: i, istat

   call avm_read_headersf(avmid, 'mixed.avm', istat)
   if (istat.ne.0) stop 'ERROR: not an AVMesh file'

   call avm_get_intf(avmid, 'meshCount', meshCount, istat)
   call avm_get_realf(avmid, 'modelScale', modelScale, istat)
   call avm_get_strf(avmid, 'contactInfo', contactInfo, istat)
   call avm_get_real_arrayf(avmid, 'referencePoint', ref_point, 3, istat)

   print '(A,1X,I0)', 'Mesh Count is', meshCount

   do i=1,meshCount
      call avm_selectf(avmid, 'mesh', i, istat)
      call avm_get_strf(avmid, 'meshName', meshName, istat)
      call avm_get_strf(avmid, 'meshType', meshType, istat)
      call avm_get_realf(avmid, 'modelScale', modelScale, istat)
      call avm_get_real_arrayf(avmid, 'referencePoint', ref_point, 3, istat)

      print '(A,I0,A,A,A,A,A)', "Mesh #", i, " named '", trim(meshName), &
                                "' is type '", trim(meshType), "'"
      print '(A,F5.2)', 'Model Scale is ', modelScale
      print '(A,3F6.2)', 'Reference Point is ', ref_point

      if (meshType=='bfstruc') then
         call avm_get_intf(avmid, 'jmax', jmax, istat)
         call avm_get_intf(avmid, 'kmax', kmax, istat)
         call avm_get_intf(avmid, 'lmax', lmax, istat)
         ! ...
      else if (meshType=='strand') then
         call avm_get_intf(avmid, 'nBndEdges', nBndEdges, istat)
         call avm_get_intf(avmid, 'nPtsPerStrand', nPtsPerStrand, istat)
         ! ...
      else if (meshType=='unstruc') then
         call avm_get_intf(avmid, 'nNodes', nNodes, istat)
         call avm_get_intf(avmid, 'nEdges', nEdges, istat)
         call avm_get_intf(avmid, 'nFaces', nFaces, istat)
         ! ...
      else if (meshType=='cart') then
         call avm_get_intf(avmid, 'nLevels', nLevels, istat)
         call avm_get_intf(avmid, 'nBlocks', nBlocks, istat)
         call avm_get_intf(avmid, 'nFringe', nFringe, istat)
         ! ...
      end if
   end do

   call avm_closef(avmid, istat)
end program example
