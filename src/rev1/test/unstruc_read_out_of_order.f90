program unstruc_read_out_of_order

   use avmesh

   implicit none

   character*80, parameter :: filename='unstruc.avm'
   integer :: i
   integer(4) :: istat

   ! file header
   integer(4)     :: avmid, meshCount

   ! unstruc specific mesh header
   integer(4) :: nNodes
   integer(4) :: nHexCells
   integer(4) :: nTetCells
   integer(4) :: nPriCells
   integer(4) :: nPyrCells
   integer(4) :: nTriFaces
   integer(4) :: nQuadFaces
   integer(4) :: nPolyFaces
   integer(4) :: polyFacesSize
   integer(4) :: nEdges
   integer(4) :: nNodesOnGeometry
   integer(4) :: nEdgesOnGeometry
   integer(4) :: nFacesOnGeometry

   ! unstruc mesh data
   real(8), allocatable :: xyz(:,:)
   real(8), allocatable :: partXyz(:,:)
   integer(4), allocatable :: triFaces(:,:)
   integer(4), allocatable :: partTriFaces(:,:)
   integer(4), allocatable :: partTriFaces2(:,:)
   integer(4), allocatable :: quadFaces(:,:)
   integer(4), allocatable :: partQuadFaces(:,:)
   integer(4), allocatable :: hexCells(:,:)
   integer(4), allocatable :: tetCells(:,:)
   integer(4), allocatable :: priCells(:,:)
   integer(4), allocatable :: pyrCells(:,:)
   integer(4), allocatable :: partHexCells(:,:)
   integer(4), allocatable :: partTetCells(:,:)
   integer(4), allocatable :: partTetCells2(:,:)
   integer(4), allocatable :: partPriCells(:,:)
   integer(4), allocatable :: partPyrCells(:,:)
   integer(4), allocatable :: edges(:,:)
   type(AVM_AMR_Node_Data), allocatable :: nodesOnGeometry(:)
   integer(4), allocatable :: edgesOnGeometry(:,:)
   integer(4), allocatable :: facesOnGeometry(:,:)
   integer(4), allocatable :: hexGeomIds(:)
   integer(4), allocatable :: tetGeomIds(:)
   integer(4), allocatable :: priGeomIds(:)
   integer(4), allocatable :: pyrGeomIds(:)
   integer(4), allocatable :: iNull(:,:)

   !try to read a non-existant file
   call avm_read_headersf(avmid, 'fake.avm', istat)
   if (istat.ne.1) stop 'ERROR: shouldnt be able to read non-existant file'

   !try to read a non-avmesh file
   call avm_read_headersf(avmid, 'vwing.grd', istat)
   if (istat.ne.1) stop 'ERROR: shouldnt be able to read non-avmesh file'

   call avm_read_headersf(avmid, filename, istat)
   if (istat.ne.0) stop 'ERROR: not an AVMesh file'

   call avm_get_intf(avmid, 'meshCount', meshCount, istat)

   print '(A,1X,I0)', 'Mesh Count is', meshCount

   if (meshCount /= 1) then
      print '(A)', 'Mesh Count incorrect, exiting'
      call exit(1)
   end if

   call avm_selectf(avmid, 'mesh', 1, istat)

   call avm_get_intf(avmid, 'nNodes', nNodes, istat)
   call avm_get_intf(avmid, 'nTriFaces', nTriFaces, istat)
   call avm_get_intf(avmid, 'nQuadFaces', nQuadFaces, istat)
   call avm_get_intf(avmid, 'nPolyFaces', nPolyFaces, istat)
   call avm_get_intf(avmid, 'polyFacesSize', polyFacesSize, istat)
   call avm_get_intf(avmid, 'nHexCells', nHexCells, istat)
   call avm_get_intf(avmid, 'nTetCells', nTetCells, istat)
   call avm_get_intf(avmid, 'nPriCells', nPriCells, istat)
   call avm_get_intf(avmid, 'nPyrCells', nPyrCells, istat)
   call avm_get_intf(avmid, 'nEdges', nEdges, istat)
   call avm_get_intf(avmid, 'nNodesOnGeometry', nNodesOnGeometry, istat)
   call avm_get_intf(avmid, 'nEdgesOnGeometry', nEdgesOnGeometry, istat)
   call avm_get_intf(avmid, 'nFacesOnGeometry', nFacesOnGeometry, istat)

   allocate(xyz(3,nNodes), &
            partXyz(3,3), &
            triFaces(5,nTriFaces), &
            partTriFaces(5,4), &
            partTriFaces2(5,1), &
            quadFaces(6,nQuadFaces), &
            partQuadFaces(6,4), &
            hexCells(8,nHexCells), &
            tetCells(4,nTetCells), &
            priCells(6,nPriCells), &
            pyrCells(5,nPyrCells), &
            partHexCells(8,1), &
            partTetCells(4,1), &
            partTetCells2(4,1), &
            partPriCells(6,1), &
            partPyrCells(5,1), &
            edges(2,nEdges), &
            nodesOnGeometry(nNodesOnGeometry), &
            edgesOnGeometry(3,nEdgesOnGeometry), &
            facesOnGeometry(3,nFacesOnGeometry), &
            hexGeomIds(nHexCells), &
            tetGeomIds(nTetCells), &
            priGeomIds(nPriCells), &
            pyrGeomIds(nPyrCells), &
            iNull(0,0) )

   call avm_unstruc_read_facesf(avmid, &
          triFaces,  5*nTriFaces, &
          quadFaces, 6*nQuadFaces, &
          istat)
   if (istat.ne.0) stop 'ERROR: failed to read faces'

   if (triFaces(3,9) /= 11) then
     print '(A)', 'triFaces incorrect, exiting'
     call exit(1)
   end if

   if (quadFaces(6,8) /= 2) then
     print '(A)', 'quadFaces incorrect, exiting'
     call exit(1)
   end if

   call avm_unstruc_read_nodes_r8f(avmid, xyz, 3*nNodes, istat)
   if (istat.ne.0) stop 'ERROR: failed to read nodes'

   print '(A)', 'finished nodes'

   if (xyz(2,12) - 15.0 > 0.00001 ) then
     print '(A)', 'xyz incorrect, exiting'
     call exit(1)
   end if

   call avm_unstruc_read_cellsf(avmid, &
          hexCells,  8*nHexCells, &
          tetCells,  4*nTetCells, &
          priCells,  6*nPriCells, &
          pyrCells,  5*nPyrCells, &
          istat)
   if (istat.ne.0) stop 'ERROR: failed to read cells'

   print '(A)', 'finished cells'

   if (pyrCells(5,1) /= 5) then
     print '(A)', 'pyrCells incorrect, exiting'
     call exit(1)
   end if

   call avm_unstruc_read_amrf(avmid, &
          nodesOnGeometry,nNodesOnGeometry, &
          edgesOnGeometry,3*nEdgesOnGeometry, &
          facesOnGeometry,3*nFacesOnGeometry, &
          istat)
   if (istat.ne.0) stop 'ERROR: failed to read amr'

   print '(A)', 'finished amr'

   if (nodesOnGeometry(4)%geomTopoId /= 7) then
     print '(A)', 'nodesOnGeometry incorrect, exiting'
     call exit(1)
   end if

   if (edgesOnGeometry(3,1) /= 8) then
     print '(A)', 'edgesOnGeometry incorrect, exiting'
     call exit(1)
   end if

   if (facesOnGeometry(3,2) /= 14) then
     print '(A)', 'facesOnGeometry incorrect, exiting'
     call exit(1)
   end if

   call avm_unstruc_read_edgesf(avmid, &
          edges,2*nEdges, &
          istat)
   if (istat.ne.0) stop 'ERROR: failed to read edges'

   print '(A)', 'finished edges'

   if (edges(2,11) /= 1) then
     print '(A)', 'edges incorrect, exiting'
     call exit(1)
   end if

   call avm_unstruc_read_amr_volumeidsf(avmid, &
          hexGeomIds,nHexCells, &
          tetGeomIds,nTetCells, &
          priGeomIds,nPriCells, &
          pyrGeomIds,nPyrCells, &
          istat)
   if (istat.ne.0) stop 'ERROR: failed to read volumeids'

   print '(A)', 'finished volumeids'

   if (pyrGeomIds(1) /= 8) then
     print '(A)', 'pyrGeomIds incorrect, exiting'
     call exit(1)
   end if

   call avm_unstruc_read_partial_cellsf(avmid, partHexCells, 1*8, 1, 1, &
                                              partTetCells, 1*4, 2, 2, &
                                              partPriCells, 1*6, 1, 1, &
                                               partPyrCells, 1*5, 1, 1, istat)
   if (istat.ne.0) stop 'ERROR: failed to read partial cells'

   print '(A)', 'finished partial cells'

   if ((partTetCells(3,1) /= 9) .or. (partHexCells(6,1) /= 8)) then
     print '(A)', 'partial cells incorrect, exiting'
     call exit(1)
   end if

   call avm_unstruc_read_trif(avmid, partTriFaces, 4*5, 5, 2, 5, OR(AVM_NODESCELLS,AVM_BOUNDARY), istat)
   if (istat.ne.0) stop 'ERROR: failed to read partial boundary tris'

   print '(A)', 'finished partial boundary tris'

   if ((partTriFaces(1,1) /= 5) .or. (partTriFaces(1,4)) /= 7 .or. (partTriFaces(4,4) /= 2)) then
     print '(A)', 'partial boundary triFaces incorrect, exiting'
     call exit(1)
   end if

   call avm_unstruc_read_trif(avmid, partTriFaces, 1*5, 5, 1, 1, OR(AVM_NODESCELLS,AVM_INTERIOR), istat)
   if (istat.ne.0) stop 'ERROR: failed to read partial interior tris'

   print '(A)', 'finished partial interior tris'

   if ((partTriFaces(1,1) /= 5) .or. (partTriFaces(4,1)) /= 3 .or. (partTriFaces(5,1) /= 2)) then
     print '(A)', 'partial interior triFaces incorrect, exiting'
     call exit(1)
   end if

   call avm_unstruc_read_quadf(avmid, partQuadFaces, 4*6, 6, 2, 5, OR(AVM_NODESCELLS,AVM_BOUNDARY), istat)
   !call avm_get_error_strf(errorMsg)
   !print *, errorMsg
   if (istat.ne.0) stop 'ERROR: failed to read partial boundary quads'

   print '(A)', 'finished partial boundary quads'

   if ((partQuadFaces(1,1) /= 3) .or. (partQuadFaces(3,4)) /= 9 .or. (partQuadFaces(6,4) /= -1)) then
     print '(A)', 'partial boundary quadFaces incorrect, exiting'
     call exit(1)
   end if

   call avm_unstruc_read_quadf(avmid, partQuadFaces, 2*6, 6, 1, 2, OR(AVM_NODESCELLS,AVM_INTERIOR), istat)
   if (istat.ne.0) stop 'ERROR: failed to read partial interior quads'

   print '(A)', 'finished partial interior quads'

   if ((partQuadFaces(1,1) /= 2) .or. (partQuadFaces(4,1)) /= 7 .or. (partQuadFaces(4,2) /= 8)) then
     print '(A)', 'partial interior quadFaces incorrect, exiting'
     call exit(1)
   end if

   call avm_unstruc_read_partial_nodes_r8f(avmid, partXyz, 3*3, 2, 4, istat)
   if (istat.ne.0) stop 'ERROR: failed to read partial nodes'

   print '(A)', 'finished partial nodes'

   if ((xyz(2,3) - 0.0 > 0.00001) .or. (xyz(3,3) - 12.0 > 0.00001)) then
     print '(A)', 'xyz incorrect, exiting'
     call exit(1)
   end if

   ! test only reading 1 cell type
   call avm_unstruc_read_partial_cellsf(avmid, iNull, 0, -1, -1, &
                                              partTetCells2, 1*4, 1, 1, &
                                              iNull, 0, -1, -1, &
                                               iNull, 0, -1, -1, istat)
   if (istat.ne.0) stop 'ERROR: failed to read only-tets partial cells'



   print '(A)', 'finished partial cells only-tets'

   if ((partTetCells2(3,1) /= 11) ) then
     print '(A)', 'partial cells only tets incorrect, exiting'
     call exit(1)
   end if

   ! test only reading one face type
   call avm_unstruc_read_partial_facesf(avmid, partTriFaces, 1*5, 4, 4, &
                                              iNull, 0, -1, -1, istat)
   if (istat.ne.0) stop 'ERROR: failed to read partial faces only-tris'

   print '(A)', 'finished partial faces only-tris'

   if (partTriFaces(2,1) /= 7) then
     print '(A)', 'partial triFaces only-tris incorrect, exiting'
     call exit(1)
   end if

   call printa("partialTriFaces",partTriFaces, 5, 4)
   call printa("triFaces", triFaces, 5, nTriFaces)
   call printa("quadFaces", quadFaces, 6, nQuadFaces)
   call printa("hexCells", hexCells, 8, nHexCells)
   call printa("tetCells", tetCells, 4, nTetCells)
   call printa("priCells", priCells, 6, nPriCells)
   call printa("pyrCells", pyrCells, 5, nPyrCells)
   call printa("edges", edges, 2, nEdges)
   print '(A)', "nodesOnGeometry"
   do i=1, nNodesOnGeometry
      print '(F10.2 F10.2 I10 I10 I10)', nodesOnGeometry(i)%u,nodesOnGeometry(i)%v, &
               nodesOnGeometry(i)%nodeIndex,nodesOnGeometry(i)%geomType,nodesOnGeometry(i)%geomTopoId
   end do
   call printa("edgesOnGeometry", edgesOnGeometry, 3, nEdgesOnGeometry)
   call printa("facesOnGeometry", facesOnGeometry, 3, nFacesOnGeometry)
   call printa("hexGeomIds", hexGeomIds, 1, nHexCells)
   call printa("tetGeomIds", tetGeomIds, 1, nTetCells)
   call printa("priGeomIds", priGeomIds, 1, nPriCells)
   call printa("pyrGeomIds", pyrGeomIds, 1, nPyrCells)

   deallocate(xyz, triFaces, partTriFaces, partTriFaces2, quadFaces, partQuadFaces)
   deallocate(hexCells, partHexCells, tetCells, partTetCells, partTetCells2, priCells, partPriCells, pyrCells, partPyrCells)
   deallocate(edges, nodesOnGeometry, edgesOnGeometry, facesOnGeometry)
   deallocate(hexGeomIds, tetGeomIds, priGeomIds, pyrGeomIds)

   call avm_closef(avmid, istat)
   write(6,'(A,A)')  'Successfully read: ', filename
end program

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
