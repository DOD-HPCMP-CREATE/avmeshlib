program bfstruc_read
   implicit none
   character*80, parameter :: filename='bfstruc.avm'
   character*80 :: c80
   character :: c
   integer :: m,p,pp,i
   integer :: j,k,l
   real*8 :: r8

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

   ! bfstruc specific mesh header
   integer(4) :: jmax                         
   integer(4) :: kmax                   
   integer(4) :: lmax                   
   integer(4) :: iblank                   
   integer(4) :: nPatches
   integer(4) :: nPatchIntParams
   integer(4) :: nPatchR8Params
   integer(4) :: nPatchC80Params

   ! bfstruc patch vars 
   integer(4):: bctype
   integer(4):: direction
   integer(4):: jbegin
   integer(4):: jend
   integer(4):: kbegin
   integer(4):: kend
   integer(4):: lbegin
   integer(4):: lend
   character(128) :: description

   ! bfstruc mesh data
   real, allocatable :: x(:,:,:), y(:,:,:), z(:,:,:)
   integer, allocatable :: iblanks(:,:,:)

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
   allocate(dims(meshCount,3))

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

      ! read bfstruc specific mesh header
      read(10) jmax, &
               kmax, &
               lmax, &
               iblank, &
               nPatches, &
               nPatchIntParams, &
               nPatchR8Params, &
               nPatchC80Params

      ! populate dims array
      dims(m,1) = jmax
      dims(m,2) = kmax
      dims(m,3) = lmax

      ! read patches
      do p=1,nPatches
         read(10) bctype, &
                  direction, &
                  jbegin, &
                  jend, &
                  kbegin, &
                  kend, &
                  lbegin, &
                  lend, &
                  description
      end do

      ! read int patch parameters
      do p=1,nPatches
         do pp=1,nPatchIntParams
            read(10) i
         end do
      end do

      ! read real patch parameters
      do p=1,nPatches
         do pp=1,nPatchR8Params
            read(10) r8
         end do
      end do

      ! read c80 patch parameters
      do p=1,nPatches
         do pp=1,nPatchC80Params
            read(10) c80
         end do
      end do
   end do

   ! read mesh data
   do m=1,meshCount
      jmax = dims(m,1)
      kmax = dims(m,2)
      lmax = dims(m,3)

      allocate(x(jmax,kmax,lmax), &
               y(jmax,kmax,lmax), &
               z(jmax,kmax,lmax), &
               iblanks(jmax,kmax,lmax))

      read(10) &
        (((x(j,k,l),j=1,jmax),k=1,kmax),l=1,lmax), &
        (((y(j,k,l),j=1,jmax),k=1,kmax),l=1,lmax), &
        (((z(j,k,l),j=1,jmax),k=1,kmax),l=1,lmax)

      read(10) &
        (((iblanks(j,k,l),j=1,jmax),k=1,kmax),l=1,lmax)

      deallocate(x,y,z,iblanks)
   end do

   close(10)

   write(6,'(A,A)')  'Successfully read: ', filename
end program bfstruc_read
