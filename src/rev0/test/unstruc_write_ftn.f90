program unstruc_write
   implicit none
   character*80, parameter :: filename='unstruc.avm'
   integer :: i, j, m, p
   integer, parameter :: descriptionSize = 256
   character(descriptionSize) :: description

   ! file header
   character(6)   :: magicString     
   integer(4)     :: magicNumber
   integer(4)     :: formatRevision
   character(128) :: meshRevision
   integer(4)     :: meshCount
   character(128) :: contactName
   character(128) :: contactOrg
   character(128) :: contactEmail
   integer(4)     :: precision
   integer(4)     :: dimensions

   ! generic mesh header
   character(128) :: meshName
   character(128) :: meshType
   character(128) :: meshGenerator
   character(128) :: changedDate
   character(128) :: coordinateSystem
   real(8)        :: modelScale
   character(128) :: gridUnits
   real(8)        :: reynoldsNumber
   real(8)        :: referenceLength
   real(8)        :: wallDistance
   real(8)        :: referencePoint(3)
   character(128) :: referencePointDescription
   real(8)        :: periodicity
   character(128) :: periodicityDescription
   integer(4)     :: refinementLevel

   ! unstruc specific mesh header
   integer(4) :: fullViscousLayerCount
   integer(4) :: partialViscousLayerCount
   integer(4) :: nNodes
   integer(4) :: nEdges
   integer(4) :: nFaces
   integer(4) :: nMaxNodesPerFace
   integer(4) :: nTriFaces
   integer(4) :: nQuadFaces
   integer(4) :: nBndTriFaces
   integer(4) :: nBndQuadFaces
   integer(4) :: nCells
   integer(4) :: nMaxNodesPerCell
   integer(4) :: nMaxFacesPerCell
   integer(4) :: nHexCells
   integer(4) :: nTetCells
   integer(4) :: nPriCells
   integer(4) :: nPyrCells
   integer(4) :: nPatches
   integer(4) :: nPatchNodes
   integer(4) :: nPatchFaces

   ! unstruc patch vars 
   character(32) :: patchLabel
   character(16) :: patchType
   integer(4) :: patchId

   ! unstruc mesh data
   real, allocatable :: xyz(:,:)
   integer, allocatable :: triFaces(:,:)
   integer, allocatable :: quadFaces(:,:)

 !
 ! initialization
 !
    open(unit=10, file=filename, access='stream', form='unformatted')

    magicString = 'AVMESH'
    magicNumber = 1
    formatRevision = 0
    meshRevision = '1.0'
    meshCount = 1
    contactName = 'David McDaniel'
    contactOrg = 'UAB'
    contactEmail = 'david.mcdaniel.ctr@eglin.af.mil'
    precision = 1
    dimensions = 3
    description = 'An example AVMesh file with some unstruc grids.'

    ! write file header
    write(10) magicString, &
              magicNumber, &
              formatRevision, &
              meshRevision, &
              meshCount, &
              contactName, &
              contactOrg, &
              contactEmail, &
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
       changedDate = 'June 15, 2010'
       coordinateSystem = 'xByUzL'
       modelScale = 1.0d0
       gridUnits = 'in'
       reynoldsNumber = 1.0d0
       referenceLength = 1.0d0
       wallDistance = 1.0d0
       referencePoint = (/0.0,0.0,0.0/)
       referencePointDescription = 'origin'
       periodicity = 1.0d0
       periodicityDescription = 'full-span'
       refinementLevel = 1
       description = 'A nonsensical unstruc mesh that just demonstrates the format.'

       ! unstruc mesh fields
       fullViscousLayerCount = 0
       partialViscousLayerCount = 0
       nNodes = 12
       nEdges = 24
       nFaces = 17
       nMaxNodesPerFace = 4
       nTriFaces = 9
       nQuadFaces = 8
       nBndTriFaces = 8
       nBndQuadFaces = 6
       nCells = 4
       nMaxNodesPerCell = 8
       nMaxFacesPerCell = 6
       nHexCells = 1
       nTetCells = 1
       nPriCells = 1
       nPyrCells = 1
       nPatches = 2
       nPatchNodes = 12
       nPatchFaces = 14

       ! write generic mesh header
       write(10) meshName, &
                 meshType, &
                 meshGenerator, &
                 changedDate, &
                 coordinateSystem, &
                 modelScale, &
                 gridUnits, &
                 reynoldsNumber, &
                 referenceLength, &
                 wallDistance, &
                 referencePoint, &
                 referencePointDescription, &
                 periodicity, &
                 periodicityDescription, &
                 refinementLevel, &
                 descriptionSize

       ! write variable length mesh description
       write(10) description 

       ! write unstruct specific mesh header
       write(10) fullViscousLayerCount, &
                 partialViscousLayerCount, &
                 nNodes, &
                 nEdges, &
                 nFaces, &
                 nMaxNodesPerFace, &
                 nTriFaces, &
                 nQuadFaces, &
                 nBndTriFaces, &
                 nBndQuadFaces, &
                 nCells, &
                 nMaxNodesPerCell, &
                 nMaxFacesPerCell, &
                 nHexCells, &
                 nTetCells, &
                 nPriCells, &
                 nPyrCells, &
                 nPatches, &
                 nPatchNodes, &
                 nPatchFaces

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
             quadFaces(6,nQuadFaces), &
             triFaces(5,nTriFaces))

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

    triFaces(:,1) = (/5, 12, 6, 3, -1/)
    triFaces(:,2) = (/5, 12, 11, 3, -1/)
    triFaces(:,3) = (/6, 11, 12, 3, -1/)
    triFaces(:,4) = (/5, 6, 11, 3, 2/)
    triFaces(:,5) = (/6, 7, 11, 2, -1/)
    triFaces(:,6) = (/7, 8, 11, 2, -1/)
    triFaces(:,7) = (/5, 8, 11, 2, -1/)
    triFaces(:,8) = (/7, 8, 9, 4, -1/)
    triFaces(:,9) = (/2, 3, 10, 4, -2/)

    quadFaces(:,1) = (/1, 4, 5, 6, 1, -1/)
    quadFaces(:,2) = (/3, 8, 5, 4, 1, -1/)
    quadFaces(:,3) = (/2, 3, 8, 7, 1, 4/)
    quadFaces(:,4) = (/1, 6, 7, 2, 1, -1/)
    quadFaces(:,5) = (/5, 6, 7, 8, 1, 2/)
    quadFaces(:,6) = (/1, 2, 3, 4, 1, -2/)
    quadFaces(:,7) = (/3, 10, 9, 8, 4, -1/)
    quadFaces(:,8) = (/2, 10, 9, 7, 4, -1/)

    call printa("triFaces", triFaces, 5, nTriFaces)
    call printa("quadFaces", quadFaces, 6, nQuadFaces)

    !
    ! write the file data
    !

    ! mesh data
    do m = 1,meshCount

        ! nodes
        write(10) ((xyz(i,j),i=1,3),j=1,nNodes)
        ! Tri patch faces
        do j = 1,nTriFaces
            if(triFaces(5,j) < 0) then
                write(10) (triFaces(i,j),i=1,5)
            end if
        end do
        ! Quad patch faces
        do j = 1,nQuadFaces
            if(quadFaces(6,j) < 0) then
                write(10) (quadFaces(i,j),i=1,6)
            end if
        end do
        ! Tri interior faces
        do j = 1,nTriFaces
            if(triFaces(5,j) > 0) then
                write(10) (triFaces(i,j),i=1,5)
            end if
        end do
        ! Quad interior faces
        do j = 1,nQuadFaces
            if(quadFaces(6,j) > 0) then
                write(10) (quadFaces(i,j),i=1,6)
            end if
        end do

    end do

    deallocate(xyz, triFaces, quadFaces) 
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

