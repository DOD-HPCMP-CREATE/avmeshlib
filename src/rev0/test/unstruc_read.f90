program unstruc_read
   implicit none
   character*80, parameter :: filename='unstruc.avm'
   character :: c
   integer :: m,p,i,j

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
   integer(4)     :: descriptionSize

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
   character(16) :: patch_bctype
   integer(4) :: patchId

   ! unstruc mesh data
   real, allocatable :: xyz(:,:)
   integer, allocatable :: triFaces(:,:)
   integer, allocatable :: quadFaces(:,:)

   ! array for data dimensions
   integer, allocatable :: dims(:,:)

   open(unit=10, file=filename, access='stream', form='unformatted')

   ! read file header
   read(10) magicString, &
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

   ! read variable length file description and discard it
   read(10) (c,i=1,descriptionSize)

   ! allocate dims array (for data reading)
   allocate(dims(meshCount,7))

   ! read mesh headers
   do m=1,meshCount

      ! read generic mesh header
      read(10) meshName, &
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

      ! read variable length mesh description and discard it
      read(10) (c,i=1,descriptionSize)

      ! read unstruct specific mesh header
      read(10) fullViscousLayerCount, &
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
                quadFaces(6,nQuadFaces), &
                triFaces(5,nTriFaces))

       ! nodes
       read(10) ((xyz(i,j),i=1,3),j=1,nNodes)
       ! Tri faces
       read(10) ((triFaces(i,j),i=1,5),j=1,nBndTriFaces)
       ! Quad faces
       read(10) ((quadFaces(i,j),i=1,6),j=1,nBndQuadFaces)
       ! Tri interior faces
       read(10) ((triFaces(i,j),i=1,5),j=nBndTriFaces+1, &
                                  nBndTriFaces+(nTriFaces-nBndTriFaces))
       ! Quad interior faces
       read(10) ((quadFaces(i,j),i=1,6),j=nBndQuadFaces+1, &
                                  nBndQuadFaces+(nQuadFaces-nBndQuadFaces))

       call printa("triFaces", triFaces, 5, nTriFaces)
       call printa("quadFaces", quadFaces, 6, nQuadFaces)

       deallocate(xyz, triFaces, quadFaces) 
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

