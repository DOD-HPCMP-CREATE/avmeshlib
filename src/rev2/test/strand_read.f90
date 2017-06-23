program strand_read
   implicit none
   character*80, parameter :: filename='strand.avm'
   integer :: m,p,k,n,i
   character :: c

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
   real(8)        :: referenceLength(3)
   real(8)        :: referenceArea
   real(8)        :: referencePoint(3)
   character(128) :: referencePointDescription
   character(128) :: meshDescription

   ! strand specific mesh header
   integer(4)    :: surfaceOnly
   integer(4)    :: nNodes
   integer(4)    :: nStrands
   integer(4)    :: nTriFaces
   integer(4)    :: nQuadFaces
   integer(4)    :: nPolyFaces
   integer(4)    :: polyFacesSize
   integer(4)    :: nBndEdges
   integer(4)    :: nPtsPerStrand
   integer(4)    :: nSurfPatches
   integer(4)    :: nEdgePatches
   real(8)       :: strandLength
   real(8)       :: stretchRatio
   real(8)       :: smoothingThreshold
   character(32) :: strandDistribution
   integer(4)    :: nNodesOnGeometry
   integer(4)    :: nEdgesOnGeometry
   integer(4)    :: nFacesOnGeometry

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
   integer,allocatable,dimension(:,:) :: triFaces
   integer,allocatable,dimension(:,:) :: quadFaces
   integer,allocatable,dimension(:) :: polyFaces
   integer,allocatable,dimension(:,:) :: bndEdges
   integer,allocatable,dimension(:)   :: nodeID
   integer,allocatable,dimension(:)   :: nodeClip
   integer,allocatable,dimension(:)   :: faceTag
   integer,allocatable,dimension(:)   :: edgeTag
   real*8 ,allocatable,dimension(:,:) :: xyz
   real*8 ,allocatable,dimension(:,:) :: pointingVec
   real*8 ,allocatable,dimension(:)   :: xStrand

   ! array for data dimensions
   integer, allocatable :: dims(:,:)

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

   if (meshCount /= 3) then
      print '(A)', 'Mesh Count incorrect, exiting'
      call exit(1)
   end if

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
               coordinateSystem, &
               modelScale, &
               gridUnits, &
               referenceLength, &
               referenceArea, &
               referencePoint, &
               referencePointDescription, &
               meshDescription

      ! read strand specific mesh header
      read(10) surfaceOnly, &
               nNodes, &
               nStrands, &
               nTriFaces, &
               nQuadFaces, &
               nPolyFaces, &
               polyFacesSize, &
               nBndEdges, &
               nPtsPerStrand, &
               nSurfPatches, &
               nEdgePatches, &
               strandLength, &
               stretchRatio, &
               smoothingThreshold, &
               strandDistribution, &
               nNodesOnGeometry, &
               nEdgesOnGeometry, &
               nFacesOnGeometry

      if (polyFacesSize /= 10) then
         print '(A)', 'polyFacesSize incorrect, exiting'
         call exit(1)
      end if

      ! populate dims array
      dims(m,1) = nBndEdges
      dims(m,2) = nPtsPerStrand
      dims(m,3) = nQuadFaces
      dims(m,4) = nNodes
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
      nNodes    = dims(m,4)
      nTriFaces     = dims(m,5)

      allocate(triFaces(3,nTriFaces), &
               quadFaces(4,nQuadFaces), &
               polyFaces(polyFacesSize), &
               bndEdges(2,nBndEdges), &
               nodeID(nStrands), &
               nodeClip(nStrands), &
               faceTag(nTriFaces+nQuadFaces+nPolyFaces), &
               edgeTag(nBndEdges), &
               xyz(3,nNodes), &
               pointingVec(3,nStrands), &
               xStrand(nPtsPerStrand))

      read(10) ((xyz(k,n),k=1,3),n=1,nNodes), &
               (nodeID(n),n=1,nStrands), &
               (nodeClip(n),n=1,nStrands), &
               ((pointingVec(k,n),k=1,3),n=1,nStrands), &
               (xStrand(n),n=1,nPtsPerStrand), &
               ((triFaces(k,n),k=1,3),n=1,nTriFaces), &
               ((quadFaces(k,n),k=1,4),n=1,nQuadFaces), &
               (polyFaces(n),n=1,polyFacesSize), &
               (faceTag(n),n=1,nTriFaces+nQuadFaces+nPolyFaces), &
               ((bndEdges(k,n),k=1,2),n=1,nBndEdges), &
               (edgeTag(n),n=1,nBndEdges)

      if (faceTag(1) /= 1) then
         print '(A)', 'faceTag(1) incorrect, exiting'
         call exit(1)
      end if

      call printaf("xyz", xyz, 3, nNodes)

      if (xyz(1,1) /= 0.0) then
         print '(A)', 'xyz(1,1) incorrect, exiting'
         call exit(1)
      end if

      if (xyz(2,6) /= 12.0) then
         print '(A)', 'xyz(2,6) incorrect, exiting'
         call exit(1)
      end if

      if (nodeID(8) /= 6) then
         print '(A)', 'nodeID(8) incorrect, exiting'
         call exit(1)
      end if

      if (nodeClip(8) /= 2) then
         print '(A)', 'nodeClip(8) incorrect, exiting'
         call exit(1)
      end if

      call printaf("pointingVec", pointingVec, 3, nStrands)

      if (pointingVec(3,8) /= 0.85) then
         print '(A)', 'pointingVec(8) incorrect, exiting'
         call exit(1)
      end if

      if (xStrand(4) /= 3 + m) then
         print '(A)', 'xStrand(4) incorrect, exiting'
         call exit(1)
      end if

      if (triFaces(3,2) /= 5 + m) then
         print '(A)', 'triFaces(3,2) incorrect, exiting'
         call exit(1)
      end if

      if (quadFaces(3,1) /= 4) then
         print '(A)', 'quadFaces(3,1) incorrect, exiting'
         call exit(1)
      end if

      if (polyFaces(10) /= 10) then
         print '(A)', 'polyFaces(10) incorrect, exiting'
         call exit(1)
      end if

      if (faceTag(4) /= 4) then
         print '(A)', 'faceTag(4) incorrect, exiting'
         call exit(1)
      end if

      call printa("bndEdges", bndEdges, 2, nBndEdges)

      if (bndEdges(2,6) /= 4) then
         print '(A)', 'bndEdges(2,6) incorrect, exiting'
         call exit(1)
      end if

      if (edgeTag(6) /= 6) then
         print '(A)', 'edgeTag(6) incorrect, exiting'
         call exit(1)
      end if

      deallocate(triFaces, quadFaces, polyFaces, bndEdges, nodeID, nodeClip, &
                 faceTag, edgeTag, xyz, pointingVec, xStrand)
   end do

   close(10)

   write(6,'(A,A)')  'Successfully read: ', filename
end program strand_read

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

subroutine printaf(n, a, nI, nJ)
   implicit none
   character(*) :: n
   integer :: i,j,nI,nJ
   real*8 :: a(nI,nJ)
   print *, n
   do j = 1,nJ
      print *, (a(i,j),i=1,nI)
   end do
end subroutine
