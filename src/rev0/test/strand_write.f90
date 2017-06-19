program strand_write
   implicit none
   character*80, parameter :: filename='strand.avm'
   integer, parameter :: nMesh=3, nSurfPatches=2, nEdgePatches=4, &
                         nSurfNodes=6, nTriFaces=2, nQuadFaces=1, &
                         nBndEdges=6, nPtsPerStrand=4
   real*8 :: value, refPt(3)
   integer :: avmid, istat, k, n, m, avmeshRev=0
   character*256 :: str

   integer,dimension(3,nTriFaces)            :: triConn
   integer,dimension(4,nQuadFaces)           :: quadConn
   integer,dimension(2,nBndEdges)            :: bndEdges
   integer,dimension(  nSurfNodes)           :: nodeClip
   integer,dimension(  nTriFaces+nQuadFaces) :: faceClip
   integer,dimension(  nTriFaces+nQuadFaces) :: faceTag
   integer,dimension(  nBndEdges)            :: edgeTag
   real*8 ,dimension(3,nSurfNodes)           :: xSurf
   real*8 ,dimension(3,nSurfNodes)           :: pointingVec
   real*8 ,dimension(0:nPtsPerStrand-1)      :: xStrand

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

   call avm_selectf(avmid, 'header', 0, istat)
   call avm_set_strf(avmid, 'formatVersion', '1.0', istat)
   call avm_set_strf(avmid, 'meshRevision', '1.0', istat)
   call avm_set_intf(avmid, 'meshCount', nMesh, istat)
   call avm_set_strf(avmid, 'contactName', 'Aaron Katz', istat)
   call avm_set_strf(avmid, 'contactOrg', 'Ames Education Associates', istat)
   call avm_set_strf(avmid, 'contactEmail', 'akatz@merlin.arc.nasa.gov', istat)
   call avm_set_intf(avmid, 'precision', 2, istat)
   call avm_set_intf(avmid, 'dimensions', 3, istat)
   call avm_set_strf(avmid, 'description', &
                      'An example AVMesh file with some strand grids.', istat)

   do n=1,nMesh
      call avm_selectf(avmid, 'mesh', n, istat)

      write(str, '(A,I0)') 'strand mesh # ', n

      call avm_set_strf(avmid, 'meshName', str, istat)
      call avm_set_strf(avmid, 'meshType', 'strand', istat)
      call avm_set_strf(avmid, 'meshGenerator', 'strand_write', istat)
      call avm_set_strf(avmid, 'changedDate', 'May 6, 2010', istat)
      call avm_set_strf(avmid, 'coordinateSystem', 'xByUzL', istat)
      call avm_set_realf(avmid, 'modelScale', 1.0d0, istat)
      call avm_set_strf(avmid, 'gridUnits', 'm', istat)
      call avm_set_realf(avmid, 'reynoldsNumber', 5.0d0, istat)
      call avm_set_realf(avmid, 'referenceLength', 1.0d0, istat)
      call avm_set_realf(avmid, 'wallDistance', 1.0d0, istat)
      call avm_set_real_arrayf(avmid, 'referencePoint', refPt, 3, istat)
      call avm_set_strf(avmid, 'referencePointDescription', 'origin', istat)
      call avm_set_realf(avmid, 'periodicity', 1.0d0, istat)
      call avm_set_strf(avmid, 'periodicityDescription', 'full-span', istat)
      call avm_set_intf(avmid, 'refinementLevel', 1, istat)
      call avm_set_strf(avmid, 'description', &
            'A nonsensical strand mesh that just demonstrates the format.', istat)
      call avm_set_intf(avmid, 'nSurfNodes', nSurfNodes, istat)
      call avm_set_intf(avmid, 'nTriFaces', nTriFaces, istat)
      call avm_set_intf(avmid, 'nQuadFaces', nQuadFaces, istat)
      call avm_set_intf(avmid, 'nBndEdges', nBndEdges, istat)
      call avm_set_intf(avmid, 'nPtsPerStrand', nPtsPerStrand, istat)
      call avm_set_intf(avmid, 'nSurfPatches', nSurfPatches, istat)
      call avm_set_intf(avmid, 'nEdgePatches', nEdgePatches, istat)
      call avm_set_realf(avmid, 'strandLength', 2.0d0, istat)
      call avm_set_realf(avmid, 'stretchRatio', 1.0d0, istat)
      call avm_set_realf(avmid, 'smoothingThreshold', 1.0d0, istat)
      call avm_set_strf(avmid, 'strandDistribution', 'geometric', istat)

      do m=1,nSurfPatches
         call avm_selectf(avmid, 'surfPatch', m, istat)
         call avm_set_intf(avmid, 'surfPatchId', m, istat)
         call avm_set_strf(avmid, 'surfPatchBody', 'flat plate', istat)
         call avm_set_strf(avmid, 'surfPatchComp', 'surface', istat)
         call avm_set_strf(avmid, 'surfPatchBCType', 'no slip', istat)
      end do

      do m=1,nEdgePatches
         call avm_selectf(avmid, 'edgePatch', m, istat)
         call avm_set_intf(avmid, 'edgePatchId', m, istat)
         call avm_set_strf(avmid, 'edgePatchBody', 'plate boundary', istat)
         call avm_set_strf(avmid, 'edgePatchComp', 'edge', istat)
         call avm_set_strf(avmid, 'edgePatchBCType', 'symmetry', istat)
         call avm_set_realf(avmid, 'nx', 1.0d0, istat)
         call avm_set_realf(avmid, 'ny', 0.0d0, istat)
         call avm_set_realf(avmid, 'nz', 0.0d0, istat)
      end do
   end do

 !
 ! write
 ! 
   call avm_write_headersf(avmid, istat) 
   if (istat.ne.0) stop 'ERROR: avm_write_headers'

   do m=1,nMesh
    ! generate the mesh (nonsensical)
    ! set the values based on mesh number to check
    ! that we can get out what we write to disk

      do n=1,nTriFaces
         do k=1,3
            triConn(k,n) = m+n+k
         end do
      end do

      quadConn    = m
      bndEdges    = m
      nodeClip    = m
      faceClip    = m
      faceTag     = m
      edgeTag     = m
      xSurf       = real(m)
      pointingVec = real(m)

      do n=0,nPtsPerStrand-1
         xStrand(n) = real(m)+n
      end do

      call avm_strand_write_r8f(avmid, &
            triConn,     3*nTriFaces, &           
            quadConn,    4*nQuadFaces, &          
            bndEdges,    2*nBndEdges, &           
            nodeClip,    nSurfNodes, &          
            faceClip,    nTriFaces+nQuadFaces, &
            faceTag,     nTriFaces+nQuadFaces, &
            edgeTag,     nBndEdges, &           
            xSurf,       3*nSurfNodes, &          
            pointingVec, 3*nSurfNodes, &          
            xStrand,     nPtsPerStrand, &     
            istat)
      if (istat.ne.0) stop 'ERROR: avm_strand_write_r8'
   end do

   call avm_closef(avmid, istat)
   if (istat.ne.0) stop 'ERROR: avm_close'

   write(6,'(A,A)')  'Successfully wrote: ', filename
end program strand_write
