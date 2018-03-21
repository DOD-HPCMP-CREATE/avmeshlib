program unstruc_write
   implicit none
   character*80, parameter :: filename='unstruc_ftn.avm'
   integer :: i, j, k, m, p
   integer :: nodesInFace, rightCellIndex
   integer, parameter :: descriptionSize = 256
   character(descriptionSize) :: description

   ! file header
   character(6)   :: magicString     
   integer(4)     :: magicNumber
   integer(4)     :: formatRevision
   integer(4)     :: meshCount
   character(128) :: contactInfo
   integer(4)     :: precision
   integer(4)     :: dimensions

   ! generic mesh header
   character(128) :: meshName
   character(128) :: meshType
   character(128) :: meshGenerator
   character(128) :: coordinateSystem
   real(8)        :: modelScale
   character(128) :: gridUnits
   real(8)        :: referenceLength(3)
   real(8)        :: referenceArea
   real(8)        :: referencePoint(3)
   character(128) :: referencePointDescription
   integer(4)     :: refined
   character(128) :: meshDescription

   ! unstruc specific mesh header
   integer(4) :: nNodes
   integer(4) :: nFaces
   integer(4) :: nCells
   integer(4) :: nMaxNodesPerFace
   integer(4) :: nMaxNodesPerCell
   integer(4) :: nMaxFacesPerCell
   integer(4) :: facePolyOrder
   integer(4) :: cellPolyOrder
   integer(4) :: nPatches
   integer(4) :: nHexCells
   integer(4) :: nTetCells
   integer(4) :: nPriCells
   integer(4) :: nPyrCells
   integer(4) :: nBndTriFaces
   integer(4) :: nTriFaces
   integer(4) :: nBndQuadFaces
   integer(4) :: nQuadFaces
   integer(4) :: nEdges
   integer(4) :: nNodesOnGeometry
   integer(4) :: nEdgesOnGeometry
   integer(4) :: nFacesOnGeometry
   integer(4) :: geomRegionId


   ! unstruc patch vars 
   character(32) :: patchLabel
   character(16) :: patchType
   integer(4) :: patchId

   ! unstruc mesh data
   real(8), allocatable :: xyz(:,:)
   integer(4), allocatable :: triFaces(:,:)
   integer(4), allocatable :: quadFaces(:,:)
   integer(4), allocatable :: hexCells(:,:)
   integer(4), allocatable :: tetCells(:,:)
   integer(4), allocatable :: priCells(:,:)
   integer(4), allocatable :: pyrCells(:,:)
   integer(4), allocatable :: edges(:,:)
   type AMR_Node_Data
      sequence
      real*8  :: u, v
      integer(4) :: nodeIndex, geomType, geomTopoId, dummy
   end type AMR_Node_Data
   type(AMR_Node_Data), allocatable :: nodesOnGeometry(:)
   integer(4), allocatable :: edgesOnGeometry(:,:)
   integer(4), allocatable :: facesOnGeometry(:,:)
   integer(4), allocatable :: hexGeomIds(:)
   integer(4), allocatable :: tetGeomIds(:)
   integer(4), allocatable :: priGeomIds(:)
   integer(4), allocatable :: pyrGeomIds(:)

 !
 ! initialization
 !
    open(unit=10, file=filename, access='stream', form='unformatted')

    magicString = 'AVMESH'
    magicNumber = 1
    formatRevision = 2
    meshCount = 1
    contactInfo = 'David McDaniel'
    precision = 2
    dimensions = 3
    description = 'An example AVMesh file with some unstruc grids.'

    ! write file header
    write(10) magicString, &
              magicNumber, &
              formatRevision, &
              meshCount, &
              contactInfo, &
              precision, &
              dimensions, &
              descriptionSize

    write(10) description

    ! read mesh headers
    do m=1,meshCount

       ! generic mesh fields
       write(meshName, '(A,I0)') 'unstruc mesh # ', m
       meshType = 'unstruc'
       meshGenerator = 'unstruc_write'
       coordinateSystem = 'xByUzL'
       modelScale = 1.0d0
       gridUnits = 'in'
       referenceLength = (/1.0d0,1.0d0,1.0d0/)
       referenceArea = 1.0d0
       referencePoint = (/0.0d0,0.0d0,0.0d0/)
       referencePointDescription = 'origin'
       refined = 0
       meshDescription = 'A nonsensical unstruc mesh that just demonstrates the format.'

       ! unstruc mesh fields
       nNodes = 12
       nFaces = 17
       nCells = 4
       nMaxNodesPerFace = 4
       nMaxNodesPerCell = 8
       nMaxFacesPerCell = 6
       facePolyOrder = 1
       cellPolyOrder = 1
       nPatches = 2
       nHexCells = 1
       nTetCells = 2
       nPriCells = 1
       nPyrCells = 1
       nBndTriFaces = 8
       nTriFaces = 9
       nBndQuadFaces = 6
       nQuadFaces = 8
       nEdges = 12
       nNodesOnGeometry = 4
       nEdgesOnGeometry = 1
       nFacesOnGeometry = 2
       geomRegionId = -1

       ! write generic mesh header
       write(10) meshName, &
                 meshType, &
                 meshGenerator, &
                 coordinateSystem, &
                 modelScale, &
                 gridUnits, &
                 referenceLength, &
                 referenceArea, &
                 referencePoint, &
                 referencePointDescription, &
                 refined, &
                 meshDescription

       ! write unstruct specific mesh header
       write(10) nNodes, &
                 nFaces, &
                 nCells, &
                 nMaxNodesPerFace, &
                 nMaxNodesPerCell, &
                 nMaxFacesPerCell, &
                 facePolyOrder, &
                 cellPolyOrder, &
                 nPatches, &
                 nHexCells, &
                 nTetCells, &
                 nPriCells, &
                 nPyrCells, &
                 nBndTriFaces, &
                 nTriFaces, &
                 nBndQuadFaces, &
                 nQuadFaces, &
                 nEdges, &
                 nNodesOnGeometry, &
                 nEdgesOnGeometry, &
                 nFacesOnGeometry, &
                 geomRegionId

       ! write patches
       do p=1,nPatches
          write(patchLabel,'(A,I0,A)') 'patch ',p,' label'
          write(patchType,'(A,I0,A)') 'patch ',p,' bctype'
          patchId = -p

          write(10) patchLabel, &
                    patchType, &
                    patchId
       end do
    end do

    !
    ! unstruc mesh data initialization
    !
    allocate(xyz(3,nNodes), &
             triFaces(4,nTriFaces), &
             quadFaces(5,nQuadFaces), &
             hexCells(8,nHexCells), &
             tetCells(4,ntetCells), &
             priCells(6,nPriCells), &
             pyrCells(5,nPyrCells), &
             edges(2,nEdges), &
             nodesOnGeometry(nNodesOnGeometry), &
             edgesOnGeometry(3,nNodesOnGeometry), &
             facesOnGeometry(3,nNodesOnGeometry), &
             hexGeomIds(nHexCells), &
             tetGeomIds(nTetCells), &
             priGeomIds(nPriCells), &
             pyrGeomIds(nPyrCells))


    xyz(:,1) = (/0.0,0.0,0.0/)
    xyz(:,2) = (/12.0,0.0,0.0/)
    xyz(:,3) = (/12.0,0.0,12.0/)
    xyz(:,4) = (/0.0,0.0,12.0/)
    xyz(:,5) = (/0.0,12.0,12.0/)
    xyz(:,6) = (/0.0,12.0,0.0/)
    xyz(:,7) = (/12.0,12.0,0.0/)
    xyz(:,8) = (/12.0,12.0,12.0/)
    xyz(:,9) = (/18.0,12.0,6.0/)
    xyz(:,10) = (/18.0,0.0,6.0/)
    xyz(:,11) = (/6.0,18.0,6.0/)
    xyz(:,12) = (/-6.0,15.0,6.0/)

    triFaces(:,1) = (/5, 12, 6, -1/)
    triFaces(:,2) = (/5, 12, 11, -1/)
    triFaces(:,3) = (/6, 11, 12, -1/)
    triFaces(:,4) = (/5, 6, 11, 1/)
    triFaces(:,5) = (/6, 7, 11, -1/)
    triFaces(:,6) = (/7, 8, 11, -1/)
    triFaces(:,7) = (/5, 8, 11, -1/)
    triFaces(:,8) = (/7, 8, 9, -1/)
    triFaces(:,9) = (/2, 3, 10, -2/)

    quadFaces(:,1) = (/1, 4, 5, 6, -1/)
    quadFaces(:,2) = (/3, 8, 5, 4, -1/)
    quadFaces(:,3) = (/2, 3, 8, 7, 1/)
    quadFaces(:,4) = (/1, 6, 7, 2, -1/)
    quadFaces(:,5) = (/5, 6, 7, 8, 1/)
    quadFaces(:,6) = (/1, 2, 3, 4, -2/)
    quadFaces(:,7) = (/3, 10, 9, 8, -1/)
    quadFaces(:,8) = (/2, 10, 9, 7, -1/)

    hexCells(:,1) = (/1, 4, 5, 6, 3, 8, 5, 4/)
    tetCells(:,1) = (/5, 12, 11, 3/)
    tetCells(:,2) = (/6, 10, 9, 7/)
    priCells(:,1) = (/6, 11, 12, 3, 5, 6/)
    pyrCells(:,1) = (/2, 3, 10, 4, 5/)

    edges(:,1) = (/2, 3/)
    edges(:,2) = (/4, 5/)
    edges(:,3) = (/6, 8/)
    edges(:,4) = (/3, 5/)
    edges(:,5) = (/7, 2/)
    edges(:,6) = (/9, 3/)
    edges(:,7) = (/11, 4/)
    edges(:,8) = (/1, 5/)
    edges(:,9) = (/6, 10/)
    edges(:,10) = (/5, 7/)
    edges(:,11) = (/12, 1/)
    edges(:,12) = (/6, 3/)

    nodesOnGeometry(1)%nodeIndex = 1
    nodesOnGeometry(1)%geomType = 0
    nodesOnGeometry(1)%geomTopoId = 1
    nodesOnGeometry(1)%dummy = 1
    nodesOnGeometry(1)%u = 0.0
    nodesOnGeometry(1)%v = 0.0
    nodesOnGeometry(2)%nodeIndex = 5
    nodesOnGeometry(2)%geomType = 1
    nodesOnGeometry(2)%geomTopoId = 3
    nodesOnGeometry(2)%dummy = 1
    nodesOnGeometry(2)%u = 0.5
    nodesOnGeometry(2)%v = 0.0
    nodesOnGeometry(3)%nodeIndex = 11
    nodesOnGeometry(3)%geomType = 2
    nodesOnGeometry(3)%geomTopoId = 5
    nodesOnGeometry(3)%dummy = 1
    nodesOnGeometry(3)%u = 0.5
    nodesOnGeometry(3)%v = 0.7
    nodesOnGeometry(4)%nodeIndex = 12
    nodesOnGeometry(4)%geomType = 1
    nodesOnGeometry(4)%geomTopoId = 7
    nodesOnGeometry(4)%dummy = 1
    nodesOnGeometry(4)%u = 0.6
    nodesOnGeometry(4)%v = 0.0

    edgesOnGeometry(:,1) = (/4, 1, 8/)

    !Note: The face ordering changes when the faces are reordered to write the bnd faces first.
    !      A "real" native writer would need to keep track of this and update the face ID's in
    !      the facesOnGeometry section. For this test case, the values have been hardcoded to
    !      their final values: face 11 stays the same, and face 14 becomes face 17.
    !      If you use the avmesh API to read/write, all of this is handled under the hood.
    facesOnGeometry(:,1) = (/11, 2, 12/)
    facesOnGeometry(:,2) = (/17, 2, 14/)

    hexGeomIds(1) = 1
    tetGeomIds(1) = 2
    tetGeomIds(2) = 3
    priGeomIds(1) = 7
    pyrGeomIds(1) = 8

    call printa("triFaces", triFaces, 4, nTriFaces)
    call printa("quadFaces", quadFaces, 5, nQuadFaces)
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

    !
    ! write the file data
    !

    ! mesh data
    do m = 1,meshCount

        ! nodes
        write(10) ((xyz(i,j),i=1,3),j=1,nNodes)
        ! Tri patch faces
        do j = 1,nTriFaces
            if(triFaces(4,j) < 0) then
                write(10) (triFaces(i,j),i=1,4)
            end if
        end do
        ! Quad patch faces
        do j = 1,nQuadFaces
            if(quadFaces(5,j) < 0) then
                write(10) (quadFaces(i,j),i=1,5)
            end if
        end do

        ! Tri interior faces
        do j = 1,nTriFaces
            if(triFaces(4,j) >= 0) then
                write(10) (triFaces(i,j),i=1,4)
            end if
        end do
        ! Quad interior faces
        do j = 1,nQuadFaces
            if(quadFaces(5,j) >= 0) then
                write(10) (quadFaces(i,j),i=1,5)
            end if
        end do

        ! Hex cells
        write(10) ((hexCells(i,j),i=1,8),j=1,nHexCells)
        ! Tet cells
        write(10) ((tetCells(i,j),i=1,4),j=1,ntetCells)
        ! Prism cells
        write(10) ((priCells(i,j),i=1,6),j=1,nPriCells)
        ! Pyramid cells
        write(10) ((pyrCells(i,j),i=1,5),j=1,nPyrCells)

        ! Edges
        write(10) ((edges(i,j),i=1,2),j=1,nEdges)

        ! AMR
        !Note: Normally the commented-out write below would be preferable, to avoid explicitly accounting
        !      for the defined-type padding, but these test cases use -convert big_endian to simulate
        !      reading/writing a non-native endianness, and -convert big_endian does not know how to convert
        !      defined types such as AMR_Node_Data, so we have to write it out one int/double at a time.
        !write(10) (nodesOnGeometry(i),i=1,nNodesOnGeometry)
        write(10) (nodesOnGeometry(i)%u, &
                   nodesOnGeometry(i)%v, &
                   nodesOnGeometry(i)%nodeIndex, &
                   nodesOnGeometry(i)%geomType, &
                   nodesOnGeometry(i)%geomTopoId, &
                   nodesOnGeometry(i)%dummy, &
                   i=1,nNodesOnGeometry)
        write(10) ((edgesOnGeometry(i,j),i=1,3),j=1,nEdgesOnGeometry)
        write(10) ((facesOnGeometry(i,j),i=1,3),j=1,nFacesOnGeometry)

        ! AMR volume geometry Ids
        do i=1,nHexCells
           write(10) (hexGeomIds(i))
        end do
        do i=1,nTetCells
           write(10) (tetGeomIds(i))
        end do
        do i=1,nPriCells
           write(10) (priGeomIds(i))
        end do
        do i=1,nPyrCells
           write(10) (pyrGeomIds(i))
        end do
    end do

    deallocate(xyz, triFaces, quadFaces)
    deallocate(hexCells, tetCells, priCells, pyrCells)
    deallocate(edges, nodesOnGeometry, edgesOnGeometry, facesOnGeometry)
    deallocate(hexGeomIds, tetGeomIds, priGeomIds, pyrGeomIds)

    close(10)

    print '(1X,A)',  'Successfully wrote: ' // trim(filename)
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
