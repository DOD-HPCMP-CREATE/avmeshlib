program strand_write
   implicit none
   character*80, parameter :: filename='strand.avm'
   integer, parameter :: nMesh=3, nSurfPatches=2, nEdgePatches=4, &
                         nNodes=6, nStrands=8, nTriFaces=2, nQuadFaces=1, &
                         nPolyFaces=1, polyFacesSize=10, &
                         nBndEdges=6, nPtsPerStrand=4
   real*8 :: value, refPt(3)
   integer :: avmid, istat, k, n, m, avmeshRev=1
   character*256 :: str

   integer,dimension(3,nTriFaces)            :: triFaces
   integer,dimension(4,nQuadFaces)           :: quadFaces
   integer,dimension(polyFacesSize)          :: polyFaces
   integer,dimension(2,nBndEdges)            :: bndEdges
   integer,dimension(  nStrands)           :: nodeID
   integer,dimension(  nStrands)           :: nodeClip
   integer,dimension(  nTriFaces+nQuadFaces+nPolyFaces) :: faceTag
   integer,dimension(  nBndEdges)            :: edgeTag
   real*8 ,dimension(3,nNodes)           :: xyz
   real*8 ,dimension(3,nStrands)           :: pointingVec
   real*8 ,dimension(0:nPtsPerStrand-1)      :: xStrand

   character*80 :: write_endian

   character*256 :: errorMsg

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
   call avm_set_intf(avmid, 'meshCount', nMesh, istat)
   call avm_set_strf(avmid, 'contactInfo', 'Aaron Katz', istat)
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
      call avm_set_strf(avmid, 'coordinateSystem', 'xByUzL', istat)
      call avm_set_realf(avmid, 'modelScale', 1.0d0, istat)
      call avm_set_strf(avmid, 'gridUnits', 'm', istat)
      call avm_set_realf(avmid, 'referenceLength', 1.0d0, istat)
      call avm_set_realf(avmid, 'referenceArea', 1.0d0, istat)
      call avm_set_real_arrayf(avmid, 'referencePoint', refPt, 3, istat)
      call avm_set_strf(avmid, 'referencePointDescription', 'origin', istat)
      call avm_set_strf(avmid, 'meshDescription', &
            'A nonsensical strand mesh that just demonstrates the format.', istat)
      call avm_set_intf(avmid, 'nNodes', nNodes, istat)
      call avm_set_intf(avmid, 'nStrands', nStrands, istat)
      call avm_set_intf(avmid, 'nTriFaces', nTriFaces, istat)
      call avm_set_intf(avmid, 'nQuadFaces', nQuadFaces, istat)
      call avm_set_intf(avmid, 'nPolyFaces', nPolyFaces, istat)
      call avm_set_intf(avmid, 'polyFacesSize', polyFacesSize, istat)
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
             triFaces(k,n) = m+n+k
          end do
       end do

      do k=1,10
         polyFaces(k) = k
      end do

      do k=1,nTriFaces+nQuadFaces+nPolyFaces
         faceTag(k) = k
      end do

      quadFaces(:,1) = (/1,3,4,5/)
      bndEdges(:,1) = (/1,3/)
      bndEdges(:,2) = (/2,5/)
      bndEdges(:,3) = (/1,5/)
      bndEdges(:,4) = (/1,6/)
      bndEdges(:,5) = (/2,3/)
      bndEdges(:,6) = (/3,4/)
      nodeID(:) = (/1,2,3,3,4,4,5,6/)
      nodeClip(:) = (/4,4,3,3,4,4,3,2/)
      pointingVec(:,1) = (/0.75,0.75,0.75/)
      pointingVec(:,2) = (/0.9,0.8,0.8/)
      pointingVec(:,3) = (/0.9,0.8,0.9/)
      pointingVec(:,4) = (/0.8,0.8,0.9/)
      pointingVec(:,5) = (/0.8,0.9,0.9/)
      pointingVec(:,6) = (/0.8,0.9,0.8/)
      pointingVec(:,7) = (/0.8,0.9,0.8/)
      pointingVec(:,8) = (/0.85,0.95,0.85/)
      edgeTag(:) = (/1,3,4,2,5,6/)
      xyz(:,1) = (/0.0,0.0,0.0/)
      xyz(:,2) = (/12.0,0.0,0.0/)
      xyz(:,3) = (/12.0,0.0,12.0/)
      xyz(:,4) = (/0.0,0.0,12.0/)
      xyz(:,5) = (/0.0,12.0,12.0/)
      xyz(:,6) = (/0.0,12.0,0.0/)

      do n=0,nPtsPerStrand-1
         xStrand(n) = real(m)+n
      end do

      call avm_strand_write_nodes_r8f(avmid, &
            xyz,       3*nNodes, &          
            nodeID,    nStrands, &          
            nodeClip,    nStrands, &          
            pointingVec, 3*nStrands, &          
            xStrand,     nPtsPerStrand, &     
            istat)
      call avm_get_error_strf(errorMsg)
      if (istat.ne.0) stop 'ERROR: avm_strand_write_nodes_r8'
      call avm_strand_write_facesf(avmid, &
            triFaces,     3*nTriFaces, &           
            quadFaces,    4*nQuadFaces, &          
            polyFaces,    polyFacesSize, &          
            faceTag,     nTriFaces+nQuadFaces+nPolyFaces, &
            istat)
      if (istat.ne.0) stop 'ERROR: avm_strand_write_faces'
      call avm_strand_write_edgesf(avmid, &
            bndEdges,    2*nBndEdges, &           
            edgeTag,     nBndEdges, &           
            istat)
      if (istat.ne.0) stop 'ERROR: avm_strand_write_r8'
   end do

   call avm_closef(avmid, istat)
   if (istat.ne.0) stop 'ERROR: avm_close'

   write(6,'(A,A)')  'Successfully wrote: ', filename
end program strand_write
