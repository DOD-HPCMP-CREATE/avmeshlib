program strand_read
   implicit none
   character*80, parameter :: filename='strand.avm'
   integer :: m,p,k,n,i
   character :: c

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

   ! strand specific mesh header
   integer(4)    :: surfaceOnly
   integer(4)    :: nSurfNodes
   integer(4)    :: nTriFaces
   integer(4)    :: nQuadFaces
   integer(4)    :: nBndEdges
   integer(4)    :: nPtsPerStrand
   integer(4)    :: nSurfPatches
   integer(4)    :: nEdgePatches
   real(8)       :: strandLength
   real(8)       :: stretchRatio
   real(8)       :: smoothingThreshold
   character(32) :: strandDistribution

   ! strand surface patch
   integer(4)    :: surfPatchID
   character(32) :: surfPatchBody
   character(32) :: surfPatchComp
   character(32) :: surfPatchBCType

   ! strand edge patch
   integer(4)    :: edgePatchID
   character(32) :: edgePatchBody
   character(32) :: edgePatchComp
   character(32) :: edgePatchBCType
   real(8)       :: nx,ny,nz

   ! strand mesh data
   integer,allocatable,dimension(:,:) :: triConn
   integer,allocatable,dimension(:,:) :: quadConn
   integer,allocatable,dimension(:,:) :: bndEdges
   integer,allocatable,dimension(:)   :: nodeClip
   integer,allocatable,dimension(:)   :: faceClip
   integer,allocatable,dimension(:)   :: faceTag
   integer,allocatable,dimension(:)   :: edgeTag
   real*8 ,allocatable,dimension(:,:) :: xSurf
   real*8 ,allocatable,dimension(:,:) :: pointingVec
   real*8 ,allocatable,dimension(:)   :: xStrand

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
   allocate(dims(meshCount,5))

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

      ! read strand specific mesh header
      read(10) surfaceOnly, &
               nSurfNodes, &
               nTriFaces, &
               nQuadFaces, &
               nBndEdges, &
               nPtsPerStrand, &
               nSurfPatches, &
               nEdgePatches, &
               strandLength, &
               stretchRatio, &
               smoothingThreshold, &
               strandDistribution

      ! populate dims array
      dims(m,1) = nBndEdges
      dims(m,2) = nPtsPerStrand
      dims(m,3) = nQuadFaces
      dims(m,4) = nSurfNodes
      dims(m,5) = nTriFaces

      ! read surface patches
      do p=1,nSurfPatches
         read(10) surfPatchID, &
                  surfPatchBody, &
                  surfPatchComp, &
                  surfPatchBCType
      end do

      ! read edge patches
      do p=1,nEdgePatches
         read(10) edgePatchID, &
                  edgePatchBody, &
                  edgePatchComp, &
                  edgePatchBCType, &
                  nx, &
                  ny, &
                  nz
      end do
   end do

   ! read mesh data
   do m=1,meshCount
      nBndEdges     = dims(m,1)
      nPtsPerStrand = dims(m,2)
      nQuadFaces    = dims(m,3)
      nSurfNodes    = dims(m,4)
      nTriFaces     = dims(m,5)

      allocate(triConn(3,nTriFaces), &
               quadConn(4,nQuadFaces), &
               bndEdges(2,nBndEdges), &
               nodeClip(nSurfNodes), &
               faceClip(nTriFaces+nQuadFaces), &
               faceTag(nTriFaces+nQuadFaces), &
               edgeTag(nBndEdges), &
               xSurf(3,nSurfNodes), &
               pointingVec(3,nSurfNodes), &
               xStrand(nPtsPerStrand))

      read(10) ((triConn(k,n),k=1,3),n=1,nTriFaces), &
               ((quadConn(k,n),k=1,4),n=1,nQuadFaces), &
               ((bndEdges(k,n),k=1,2),n=1,nBndEdges), &
               (nodeClip(n),n=1,nSurfNodes), &
               (faceClip(n),n=1,nTriFaces+nQuadFaces), &
               (faceTag(n),n=1,nTriFaces+nQuadFaces), &
               (edgeTag(n),n=1,nBndEdges), &
               ((xSurf(k,n),k=1,3),n=1,nSurfNodes), &
               ((pointingVec(k,n),k=1,3),n=1,nSurfNodes), &
               (xStrand(n),n=1,nPtsPerStrand)

      deallocate(triConn, quadConn, bndEdges, nodeClip, faceClip, &
                 faceTag, edgeTag, xSurf, pointingVec, xStrand)
   end do

   close(10)

   write(6,'(A,A)')  'Successfully read: ', filename
end program strand_read
