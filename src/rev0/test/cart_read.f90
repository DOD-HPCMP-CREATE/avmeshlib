program cart_read
   implicit none
   character*80, parameter :: filename='cart.avm'
   character :: c
   integer :: b,i, j,k,l, jd,kd,ld

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

   ! cart specific mesh header
   integer(4) :: nLevels
   integer(4) :: nBlocks
   integer(4) :: nFringe
   integer(4) :: nxc
   integer(4) :: nyc
   integer(4) :: nzc
   real(8)    :: domXLo
   real(8)    :: domYLo
   real(8)    :: domZLo
   real(8)    :: domXHi
   real(8)    :: domYHi
   real(8)    :: domZHi
   integer(4) :: ratio

   ! cart block data 
   integer(4) :: ilo(3)
   integer(4) :: ihi(3)
   integer(4) :: levNum
   integer(4) :: iblFlag
   integer(4) :: bdryFlag
   integer(4) :: iblDim(3)
   integer(4) :: blo(3)
   integer(4) :: bhi(3)

   ! cart iblank data
   integer,allocatable :: iblanks(:,:,:)

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

   ! read cart specific mesh header
   read(10) nLevels, &
            nBlocks, &
            nFringe, &
            nxc, &
            nyc, &
            nzc, &
            domXLo, &
            domYLo, &
            domZLo, &
            domXHi, &
            domYHi, &
            domZHi 

   do i=1,nLevels
      read(10) ratio
   end do

   ! allocate dims array (for data reading)
   allocate(dims(nBlocks,4))

   do b=1,nBlocks
      read(10) (ilo(i),i=1,3), &
               (ihi(i),i=1,3), &
               levNum, &
               iblFlag, &
               bdryFlag, &
               (iblDim(i),i=1,3), &
               (blo(i),i=1,3), &
               (bhi(i),i=1,3)
      dims(b,1) = iblFlag
      dims(b,2) = iblDim(1)
      dims(b,3) = iblDim(2)
      dims(b,4) = iblDim(3)
   end do

   do b=1,nBlocks
      iblFlag = dims(b,1)
      jd = dims(b,2)
      kd = dims(b,3)
      ld = dims(b,4)

      if (iblFlag==2) then
         allocate(iblanks(jd,kd,ld))
         read(10)(((iblanks(j,k,l),j=1,jd),k=1,kd),l=1,ld)
         deallocate(iblanks)
      end if
   end do

   write(6,'(A,A)')  'Successfully read: ', filename
end program cart_read
