program unstruc_read
   use avmesh
   implicit none
   character*80, parameter :: filename='unstruc.avm'
   character :: c
   integer :: m,p,i,j,dummy

   ! file header
   character(6)   :: magicString     
   integer(4)     :: magicNumber
   integer(4)     :: formatRevision
   integer(4)     :: meshCount
   character(128) :: contactInfo
   integer(4)     :: precision
   integer(4)     :: dimensions
   integer(4)     :: descriptionSize

   ! generic mesh header
   character(128) :: meshName
   character(128) :: meshType
   character(128) :: meshGenerator
   character(128) :: coordinateSystem
   real(8)        :: modelScale
   character(128) :: gridUnits
   real(8)        :: referenceLength
   real(8)        :: referenceArea
   real(8)        :: referencePoint(3)
   character(128) :: referencePointDescription
   character(128) :: meshDescription
   character(len=:), allocatable :: fileDescription

   ! unstruc specific mesh header
   integer(4) :: nNodes
   integer(4) :: nFaces
   integer(4) :: nCells
   integer(4) :: nMaxNodesPerFace
   integer(4) :: nMaxNodesPerCell
   integer(4) :: nMaxFacesPerCell
   integer(4) :: nPatches
   integer(4) :: nHexCells
   integer(4) :: nTetCells
   integer(4) :: nPriCells
   integer(4) :: nPyrCells
   integer(4) :: nPolyCells
   integer(4) :: nBndTriFaces
   integer(4) :: nTriFaces
   integer(4) :: nBndQuadFaces
   integer(4) :: nQuadFaces
   integer(4) :: nBndPolyFaces
   integer(4) :: nPolyFaces
   integer(4) :: bndPolyFacesSize
   integer(4) :: polyFacesSize
   integer(4) :: nEdges
   integer(4) :: nNodesOnGeometry
   integer(4) :: nEdgesOnGeometry
   integer(4) :: nFacesOnGeometry
   integer(4) :: geomRegionId

   ! unstruc patch vars 
   character(32) :: patchLabel
   character(16) :: patch_bctype
   integer(4) :: patchId

   ! unstruc mesh data
   real(8), allocatable :: xyz(:,:)
   integer(4), allocatable :: triFaces(:,:)
   integer(4), allocatable :: quadFaces(:,:)
   integer(4), allocatable :: polyFaces(:)
   integer(4), allocatable :: hexCells(:,:)
   integer(4), allocatable :: tetCells(:,:)
   integer(4), allocatable :: priCells(:,:)
   integer(4), allocatable :: pyrCells(:,:)
   integer(4), allocatable :: edges(:,:)
   type(AVM_AMR_Node_Data), allocatable :: nodesOnGeometry(:)
   integer(4), allocatable :: edgesOnGeometry(:,:)
   integer(4), allocatable :: facesOnGeometry(:,:)
   integer(4), allocatable :: hexGeomIds(:)
   integer(4), allocatable :: tetGeomIds(:)
   integer(4), allocatable :: priGeomIds(:)
   integer(4), allocatable :: pyrGeomIds(:)

   ! array for data dimensions
   integer(4), allocatable :: dims(:,:)

   open(unit=10, file=filename, access='stream', form='unformatted')

   ! read file header
   read(10) magicString, &
            magicNumber, &
            formatRevision, &
            meshCount, &
            contactInfo, &
            precision, &
            dimensions, &
            descriptionSize

   print '(A,1X,I0)', 'Mesh Count is', meshCount

   if (meshCount /= 1) then
      print '(A)', 'Mesh Count incorrect, exiting'
      call exit(1)
   end if

   ! read variable length file description
   allocate(character(descriptionSize) :: fileDescription)
   read(10) fileDescription

   ! allocate dims array (for data reading)
   allocate(dims(meshCount,7))

   ! read mesh headers
   do m=1,meshCount

      ! read generic mesh header
      read(10) meshName, &
               meshType, &
               meshGenerator, &
               coordinateSystem, &
               modelScale, &
               gridUnits, &
               referenceLength, &
               referenceArea, &
               referencePoint, &
               referencePointDescription, &
               meshDescription

      ! read unstruct specific mesh header
      read(10) nNodes, &
               nFaces, &
               nCells, &
               nMaxNodesPerFace, &
               nMaxNodesPerCell, &
               nMaxFacesPerCell, &
               nPatches, &
               nHexCells, &
               nTetCells, &
               nPriCells, &
               nPyrCells, &
               nPolyCells, &
               nBndTriFaces, &
               nTriFaces, &
               nBndQuadFaces, &
               nQuadFaces, &
               nBndPolyFaces, &
               nPolyFaces, &
               bndPolyFacesSize, &
               polyFacesSize, &
               nEdges, &
               nNodesOnGeometry, &
               nEdgesOnGeometry, &
               nFacesOnGeometry, &
               geomRegionId

      print *, fileDescription
      print '(A,1X,I0)', 'nNodes is', nNodes
      print '(A,1X,I0)', 'bndPolyFacesSize is', bndPolyFacesSize
      print '(A,1X,I0)', 'polyFacesSize is', polyFacesSize

      if (fileDescription /= 'An example AVMesh file with some unstruc grids.') then
         print '(A)', 'fileDescription incorrect, exiting'
         call exit(1)
      end if

      if (formatRevision /= 1) then
         print '(A)', 'formatRevision incorrect, exiting'
         call exit(1)
      end if

      if (nNodes /= 12) then
         print '(A)', 'nNodes incorrect, exiting'
         call exit(1)
      end if

      if (nPatches /= 2) then
         print '(A)', 'nPatches incorrect, exiting'
         call exit(1)
      end if

      if (nNodesOnGeometry /= 4) then
         print '(A)', 'nNodesOnGeometry incorrect, exiting'
         call exit(1)
      end if

      ! populate dims array
      dims(m,1) = nNodes
      dims(m,2) = nTriFaces
      dims(m,3) = nQuadFaces

      ! read patches
      do p=1,nPatches
         read(10) patchLabel, &
                  patch_bctype, &
                  patchId
      end do

   end do

   ! read mesh data
   do m=1,meshCount
       nNodes     = dims(m,1)
       nTriFaces  = dims(m,2)
       nQuadFaces = dims(m,3)

       allocate(xyz(3,nNodes), &
                triFaces(5,nTriFaces), &
                quadFaces(6,nQuadFaces), &
                polyFaces(polyFacesSize), &
                hexCells(8,nHexCells), &
                tetCells(4,nTetCells), &
                priCells(6,nPriCells), &
                pyrCells(5,nPyrCells), &
                edges(2,nEdges), &
                nodesOnGeometry(nNodesOnGeometry), &
                edgesOnGeometry(3,nEdgesOnGeometry), &
                facesOnGeometry(3,nFacesOnGeometry), &
                hexGeomIds(nHexCells), &
                tetGeomIds(nTetCells), &
                priGeomIds(nPriCells), &
                pyrGeomIds(nPyrCells))


       ! nodes
       read(10) ((xyz(i,j),i=1,3),j=1,nNodes)
       ! Tri patch faces
       read(10) ((triFaces(i,j),i=1,5),j=1,nBndTriFaces)
       ! Quad patch faces
       read(10) ((quadFaces(i,j),i=1,6),j=1,nBndQuadFaces)
       ! Poly patch faces
       if (bndPolyFacesSize>0) then
         read(10) (polyFaces(i),i=1,bndPolyFacesSize)
       end if

       ! Tri interior faces
       read(10) ((triFaces(i,j),i=1,5),j=nBndTriFaces+1,nTriFaces)
       ! Quad interior faces
       read(10) ((quadFaces(i,j),i=1,6),j=nBndQuadFaces+1,nQuadFaces)
       ! Poly interior faces
       if (polyFacesSize>0) then
         read(10) (polyFaces(i),i=bndPolyFacesSize+1,polyFacesSize)
       end if

       ! Hex cells
       read(10) ((hexCells(i,j),i=1,8),j=1,nHexCells)
       ! Tet cells
       read(10) ((tetCells(i,j),i=1,4),j=1,ntetCells)
       ! Prism cells
       read(10) ((priCells(i,j),i=1,6),j=1,nPriCells)
       ! Pyramid cells
       read(10) ((pyrCells(i,j),i=1,5),j=1,nPyrCells)

       ! Edges
       read(10) ((edges(i,j),i=1,2),j=1,nEdges)

       ! AMR

       !Note: Normally the commented-out read below would be preferable, to avoid explicitly accounting
       !      for the defined-type padding, but these test cases use -convert big_endian to simulate
       !      reading/writing a non-native endianness, and -convert big_endian does not know how to convert
       !      defined types such as AVM_AMR_Node_Data, so we have to read it in one int/double at a time.
       !read(10) (nodesOnGeometry(i),i=1,nNodesOnGeometry)
       read(10) (nodesOnGeometry(i)%u, &
                 nodesOnGeometry(i)%v, &
                 nodesOnGeometry(i)%nodeIndex, &
                 nodesOnGeometry(i)%geomType, &
                 nodesOnGeometry(i)%geomTopoId, &
                 nodesOnGeometry(i)%dummy, &
                 i=1,nNodesOnGeometry)
       read(10) ((edgesOnGeometry(i,j),i=1,3),j=1,nEdgesOnGeometry)
       read(10) ((facesOnGeometry(i,j),i=1,3),j=1,nFacesOnGeometry)

       ! AMR volume geometry Ids
       read(10) (hexGeomIds(i),i=1,nHexCells)
       read(10) (tetGeomIds(i),i=1,nTetCells)
       read(10) (priGeomIds(i),i=1,nPriCells)
       read(10) (pyrGeomIds(i),i=1,nPyrCells)

       call printa("triFaces", triFaces, 5, nTriFaces)
       call printa("quadFaces", quadFaces, 6, nQuadFaces)
       call printb("polyFaces", polyFaces, polyFacesSize, nPolyFaces)
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

       if (triFaces(3,9) /= 11) then
         print '(A)', 'triFaces incorrect, exiting'
         call exit(1)
       end if

       if (quadFaces(6,8) /= 2) then
         print '(A)', 'quadFaces incorrect, exiting'
         call exit(1)
       end if

       if (polyFaces(54) /= 8) then
         print '(A)', 'polyFaces incorrect, exiting'
         call exit(1)
       end if

       if (pyrCells(5,1) /= 5) then
         print '(A)', 'pyrCells incorrect, exiting'
         call exit(1)
       end if

       if (edges(2,11) /= 1) then
         print '(A)', 'edges incorrect, exiting'
         call exit(1)
       end if

       if (nodesOnGeometry(4)%geomTopoId /= 7) then
         print '(A)', 'nodesOnGeometry incorrect, exiting'
         call exit(1)
       end if

       if (edgesOnGeometry(3,1) /= 8) then
         print '(A)', 'edgesOnGeometry incorrect, exiting'
         call exit(1)
       end if

       if (facesOnGeometry(1,1) /= 18) then
         print '(A)', 'facesOnGeometry incorrect, exiting'
         call exit(1)
       end if

       if (facesOnGeometry(1,2) /= 25) then
         print '(A)', 'facesOnGeometry incorrect, exiting'
         call exit(1)
       end if

       if (facesOnGeometry(3,2) /= 14) then
         print '(A)', 'facesOnGeometry incorrect, exiting'
         call exit(1)
       end if

       if (pyrGeomIds(1) /= 8) then
         print '(A)', 'pyrGeomIds incorrect, exiting'
         call exit(1)
       end if

       deallocate(xyz, triFaces, quadFaces, polyFaces)
       deallocate(hexCells, tetCells, priCells, pyrCells)
       deallocate(edges, nodesOnGeometry, edgesOnGeometry, facesOnGeometry)
       deallocate(hexGeomIds, tetGeomIds, priGeomIds, pyrGeomIds)

   end do

   close(10)

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

