module avmesh_mod
   type file_id_prefix
      sequence
      character(6) :: magicString     
      integer(4)   :: magicNumber
      integer(4)   :: formatRevision
   end type file_id_prefix

   type file_header
      sequence
      character(128) :: meshRevision
      integer(4)     :: meshCount
      character(128) :: contactName
      character(128) :: contactOrg
      character(128) :: contactEmail
      integer(4)     :: precision
      integer(4)     :: dimensions
      integer(4)     :: descriptionSize
   end type file_header

   type mesh_header
      sequence
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
      integer(4)     :: descriptionSize
   end type mesh_header

   type bfstruc_header
      sequence
      integer(4):: jmax                         
      integer(4):: kmax                   
      integer(4):: lmax                   
      integer(4):: iblank                   
      integer(4):: nPatches
      integer(4):: nPatchIntParams
      integer(4):: nPatchR8Params
      integer(4):: nPatchC80Params
   end type bfstruc_header

   type bfstruc_patch
      sequence
      integer(4):: bctype
      integer(4):: direction
      integer(4):: jbegin
      integer(4):: jend
      integer(4):: kbegin
      integer(4):: kend
      integer(4):: lbegin
      integer(4):: lend
      character(128) :: description
   end type bfstruc_patch

   type unstruc_header
      sequence
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
   end type unstruc_header

   type unstruc_patchheader
      sequence
      character(32) :: patchLabel
      character(16) :: patch_bctype
      integer(4) :: patchId
   end type unstruc_patchheader

   type strand_header
      sequence
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
   end type strand_header

   type strand_surf_patch
      sequence
      integer(4)    :: surfPatchID
      character(32) :: surfPatchBody
      character(32) :: surfPatchComp
      character(32) :: surfPatchBCType
   end type strand_surf_patch

   type strand_edge_patch
      sequence
      integer(4)    :: edgePatchID
      character(32) :: edgePatchBody
      character(32) :: edgePatchComp
      character(32) :: edgePatchBCType
      real(8)       :: nx,ny,nz
   end type strand_edge_patch

   type cart_header
      sequence
      integer(4)    :: nLevels
      integer(4)    :: nBlocks
      integer(4)    :: nFringe
      integer(4)    :: nxc
      integer(4)    :: nyc
      integer(4)    :: nzc
      real(8)       :: domXLo
      real(8)       :: domYLo
      real(8)       :: domZLo
      real(8)       :: domXHi
      real(8)       :: domYHi
      real(8)       :: domZHi
   end type cart_header

   type cart_block
      sequence
      integer(4) :: ilo(3)
      integer(4) :: ihi(3)
      integer(4) :: levNum
      integer(4) :: iblFlag
      integer(4) :: bdryFlag
      integer(4) :: iblDim(3)
      integer(4) :: blo(3)
      integer(4) :: bhi(3)
   end type cart_block

contains

   subroutine avm_write_file_header(iu, hdr, desc) 
      implicit none
      integer,intent(in) :: iu
      type(file_id_prefix) :: prefix
      type(file_header),intent(in) :: hdr
      character(128),intent(in) :: desc

      prefix%magicString   = 'AVMESH'
      prefix%magicNumber   = 1
      prefix%formatRevision = 0

      write(iu) prefix%magicString, &
                prefix%magicNumber, &
                prefix%formatRevision

      write(iu) hdr%meshRevision, &
                hdr%meshCount, &
                hdr%contactName, &
                hdr%contactOrg, &
                hdr%contactEmail, &
                hdr%precision, &
                hdr%dimensions, &
                hdr%descriptionSize

      write(iu) desc
   end subroutine avm_write_file_header

   subroutine avm_write_mesh_header(ui, mesh_hdr, mesh_desc)
      implicit none
      integer :: ui
      type(mesh_header) :: mesh_hdr
      character(128) :: mesh_desc

      write(ui) mesh_hdr%meshName, &
                  mesh_hdr%meshType, &
                  mesh_hdr%meshGenerator, &
                  mesh_hdr%changedDate, &
                  mesh_hdr%coordinateSystem, &
                  mesh_hdr%modelScale, &
                  mesh_hdr%gridUnits, &
                  mesh_hdr%reynoldsNumber, &
                  mesh_hdr%referenceLength, &
                  mesh_hdr%wallDistance, &
                  mesh_hdr%referencePoint, &
                  mesh_hdr%referencePointDescription, &
                  mesh_hdr%periodicity, &
                  mesh_hdr%periodicityDescription, &
                  mesh_hdr%refinementLevel, &
                  mesh_hdr%descriptionSize

      write(ui) mesh_desc
   end subroutine avm_write_mesh_header

   subroutine avm_write_bfstruc_header(ui, bfstruc_hdr, patchset, &
                  patchIntParams, patchR8Params, patchC80Params)
      implicit none
      integer :: i, j, ui
      type(bfstruc_header) :: bfstruc_hdr
      type(bfstruc_patch) :: patchset(bfstruc_hdr%nPatches)
      integer      :: patchIntParams (bfstruc_hdr%nPatches, bfstruc_hdr%nPatchIntParams)
      real*8       :: patchR8Params  (bfstruc_hdr%nPatches, bfstruc_hdr%nPatchR8Params)
      character*80 :: patchC80Params (bfstruc_hdr%nPatches, bfstruc_hdr%nPatchC80Params)

      write(ui) bfstruc_hdr%jmax, &
                  bfstruc_hdr%kmax, &
                  bfstruc_hdr%lmax, &
                  bfstruc_hdr%iblank, &
                  bfstruc_hdr%nPatches, &
                  bfstruc_hdr%nPatchIntParams, &
                  bfstruc_hdr%nPatchR8Params, &
                  bfstruc_hdr%nPatchC80Params

      write(ui) &
        (patchset(i)%bctype, &
         patchset(i)%direction, &
         patchset(i)%jbegin, &
         patchset(i)%jend, &
         patchset(i)%kbegin, &
         patchset(i)%kend, &
         patchset(i)%lbegin, &
         patchset(i)%lend, &
         patchset(i)%description, &
         i=1, bfstruc_hdr%nPatches)

      write(ui) ((patchIntParams(i,j), i=1,bfstruc_hdr%nPatches), j=1,bfstruc_hdr%nPatchIntParams)
      write(ui) ((patchR8Params(i,j),  i=1,bfstruc_hdr%nPatches), j=1,bfstruc_hdr%nPatchR8Params)
      write(ui) ((patchC80Params(i,j), i=1,bfstruc_hdr%nPatches), j=1,bfstruc_hdr%nPatchC80Params)
   end subroutine avm_write_bfstruc_header

   subroutine avm_write_bfstruc_mesh(iu, x,y,z, jdim,kdim,ldim)
      implicit none
      real,intent(in) :: x(:,:,:), y(:,:,:), z(:,:,:)
      integer,intent(in) :: iu, jdim,kdim,ldim
      integer :: j,k,l
      write(iu) &
        (((x(j,k,l),j=1,jdim),k=1,kdim),l=1,ldim), &
        (((y(j,k,l),j=1,jdim),k=1,kdim),l=1,ldim), &
        (((z(j,k,l),j=1,jdim),k=1,kdim),l=1,ldim)
   end subroutine avm_write_bfstruc_mesh

   subroutine avm_write_bfstruc_iblanks(iu, iblanks, jdim,kdim,ldim)
      implicit none
      integer,intent(in) :: iblanks(:,:,:)
      integer,intent(in) :: iu, jdim,kdim,ldim
      integer :: j,k,l
      write(iu) &
        (((iblanks(j,k,l),j=1,jdim),k=1,kdim),l=1,ldim)
   end subroutine avm_write_bfstruc_iblanks

   subroutine avm_write_strand_header(iu,strandHeader,surfPatch,edgePatch)
      implicit none
      integer,intent(in) :: iu
      type(strand_header),intent(in) :: strandHeader
      type(strand_surf_patch),dimension(strandHeader%nSurfPatches), &
      intent(in) :: surfPatch
      type(strand_edge_patch),dimension(strandHeader%nEdgePatches), &
      intent(in) :: EdgePatch
      integer :: n

      write(iu)strandHeader%surfaceOnly          ,&
               strandHeader%nSurfNodes           ,&
               strandHeader%nTriFaces            ,&
               strandHeader%nQuadFaces           ,&
               strandHeader%nBndEdges            ,&
               strandHeader%nPtsPerStrand        ,&
               strandHeader%nSurfPatches         ,&
               strandHeader%nEdgePatches         ,&
               strandHeader%strandLength         ,&
               strandHeader%stretchRatio         ,&
               strandHeader%smoothingThreshold   ,&
               strandHeader%strandDistribution

      do n=1,strandHeader%nSurfPatches
         write(iu)surfPatch(n)%surfPatchID        ,&
                  surfPatch(n)%surfPatchBody      ,&
                  surfPatch(n)%surfPatchComp      ,&
                  surfPatch(n)%surfPatchBCType
      end do

      do n=1,strandHeader%nEdgePatches
         write(iu)edgePatch(n)%edgePatchID     ,&
                  edgePatch(n)%edgePatchBody   ,&
                  edgePatch(n)%edgePatchComp   ,&
                  edgePatch(n)%edgePatchBCType ,&
                  edgePatch(n)%nx              ,&
                  edgePatch(n)%ny              ,&
                  edgePatch(n)%nz
      end do
   end subroutine avm_write_strand_header

   subroutine avm_write_strand_mesh(iu,meshi, &
      triConn, quadConn, bndEdges, nodeClip, faceClip, faceTag, &
      edgeTag, xSurf, pointingVec, xStrand, nTriFacesL, &
      nQuadFacesL, nBndEdgesL, nSurfNodesL, nPtsPerStrandL)

      implicit none
      integer iu, meshi
      integer :: nTriFacesL,nQuadFacesL,nBndEdgesL, &
                 nSurfNodesL,nPtsPerStrandL
      integer,dimension(3,nTriFacesL)             :: triConn
      integer,dimension(4,nQuadFacesL)            :: quadConn
      integer,dimension(2,nBndEdgesL)             :: bndEdges
      integer,dimension(  nSurfNodesL)            :: nodeClip
      integer,dimension(  nTriFacesL+nQuadFacesL) :: faceClip
      integer,dimension(  nTriFacesL+nQuadFacesL) :: faceTag
      integer,dimension(  nBndEdgesL)             :: edgeTag
      real   ,dimension(3,nSurfNodesL)            :: xSurf
      real   ,dimension(3,nSurfNodesL)            :: pointingVec
      real   ,dimension(0:nPtsPerStrandL-1)       :: xStrand

      integer n,k

    ! write the mesh

      write(iu)((triConn(k,n),k=1,3),n=1,nTriFacesL)
      write(iu)((quadConn(k,n),k=1,4),n=1,nQuadFacesL)
      write(iu)((bndEdges(k,n),k=1,2),n=1,nBndEdgesL)
      write(iu)(nodeClip(n),n=1,nSurfNodesL)
      write(iu)(faceClip(n),n=1,nTriFacesL+nQuadFacesL)
      write(iu)(faceTag(n),n=1,nTriFacesL+nQuadFacesL)
      write(iu)(edgeTag(n),n=1,nBndEdgesL)
      write(iu)((xSurf(k,n),k=1,3),n=1,nSurfNodesL)
      write(iu)((pointingVec(k,n),k=1,3),n=1,nSurfNodesL)
      write(iu)(xStrand(n),n=0,nPtsPerStrandL-1)
   end subroutine avm_write_strand_mesh

   subroutine avm_write_unstruc_header(ui, unstruc_hdr, unstruc_phdrs)
      implicit none
      integer :: ui
      type(unstruc_header) :: unstruc_hdr
      type(unstruc_patchheader) :: unstruc_phdrs(unstruc_hdr%nPatches)
      integer :: i

      write(ui) unstruc_hdr%fullViscousLayerCount, &
                  unstruc_hdr%partialViscousLayerCount, &
                  unstruc_hdr%nNodes, &
                  unstruc_hdr%nEdges, &
                  unstruc_hdr%nFaces, &
                  unstruc_hdr%nMaxNodesPerFace, &
                  unstruc_hdr%nTriFaces, &
                  unstruc_hdr%nQuadFaces, &
                  unstruc_hdr%nBndTriFaces, &
                  unstruc_hdr%nBndQuadFaces, &
                  unstruc_hdr%nCells, &
                  unstruc_hdr%nMaxNodesPerCell, &
                  unstruc_hdr%nMaxFacesPerCell, &
                  unstruc_hdr%nHexCells, &
                  unstruc_hdr%nTetCells, &
                  unstruc_hdr%nPriCells, &
                  unstruc_hdr%nPyrCells, &
                  unstruc_hdr%nPatches, &
                  unstruc_hdr%nPatchNodes, &
                  unstruc_hdr%nPatchFaces

      do i = 1, unstruc_hdr%nPatches
         write(ui) unstruc_phdrs(i)%patchLabel, &
                     unstruc_phdrs(i)%patch_bctype, &
                     unstruc_phdrs(i)%patchId
      end do
   end subroutine avm_write_unstruc_header

   subroutine avm_write_unstruc_mesh(ui, &
      xyz, nNodes, &
      triFaces, nTriFaces, &
      quadFaces, nQuadFaces)

      implicit none
      integer :: ui, i, j
      integer :: nNodes, nQuadFaces, nTriFaces
      real :: xyz(3,nNodes)
      integer :: triFaces(5,nTriFaces)
      integer :: quadFaces(6,nQuadFaces)

      ! nodes
      write(ui) ((xyz(i,j),i=1,3),j=1,nNodes)
      ! Tri patch faces
      do j = 1,nTriFaces
          if(triFaces(5,j) < 0) then
              write(ui) (triFaces(i,j),i=1,5)
          end if
      end do
      ! Quad patch faces
      do j = 1,nQuadFaces
          if(quadFaces(6,j) < 0) then
              write(ui) (quadFaces(i,j),i=1,6)
          end if
      end do
      ! Tri interior faces
      do j = 1,nTriFaces
          if(triFaces(5,j) > 0) then
              write(ui) (triFaces(i,j),i=1,5)
          end if
      end do
      ! Quad interior faces
      do j = 1,nQuadFaces
          if(quadFaces(6,j) > 0) then
              write(ui) (quadFaces(i,j),i=1,6)
          end if
      end do
   end subroutine avm_write_unstruc_mesh

   subroutine avm_write_cart_header(iu, cart_hdr, ratios, cart_blks) 
      implicit none
      integer,intent(in) :: iu
      type(cart_header),intent(in) :: cart_hdr
      integer,intent(in)           :: ratios(cart_hdr%nLevels)
      type(cart_block),intent(in)  :: cart_blks(cart_hdr%nBlocks)
      integer :: i,b

      write(iu) cart_hdr%nLevels, &
                cart_hdr%nBlocks, &
                cart_hdr%nFringe, &
                cart_hdr%nxc, &
                cart_hdr%nyc, &
                cart_hdr%nzc, &
                cart_hdr%domXLo, &
                cart_hdr%domYLo, &
                cart_hdr%domZLo, &
                cart_hdr%domXHi, &
                cart_hdr%domYHi, &
                cart_hdr%domZHi 
      
      write(iu) (ratios(i),i=1,cart_hdr%nLevels)

      do b=1,cart_hdr%nBlocks
         write(iu) (cart_blks(b)%ilo(i),i=1,3), &
                   (cart_blks(b)%ihi(i),i=1,3), &
                   cart_blks(b)%levNum, &
                   cart_blks(b)%iblFlag, &
                   cart_blks(b)%bdryFlag, &
                   (cart_blks(b)%iblDim(i),i=1,3), &
                   (cart_blks(b)%blo(i),i=1,3), &
                   (cart_blks(b)%bhi(i),i=1,3)
      end do
   end subroutine avm_write_cart_header

   subroutine avm_write_cart_iblanks(iu, iblanks, jdim,kdim,ldim)
      implicit none
      integer,intent(in) :: iblanks(:,:,:)
      integer,intent(in) :: iu, jdim,kdim,ldim
      integer :: j,k,l
      write(iu) &
         (((iblanks(j,k,l),j=1,jdim),k=1,kdim),l=1,ldim)
   end subroutine avm_write_cart_iblanks
end module

module data_mod
   use avmesh_mod

   integer, parameter :: nMesh=4
   character*80, parameter :: filename='mixed.avm'

 !
 ! avmesh format
   type(file_header) :: file_hdr
   character(128)    :: file_description
   type(mesh_header) :: mesh_hdrs(nMesh)
   character(128)    :: mesh_descriptions(nMesh)

 !
 ! bfstruc mesh
   integer, parameter :: n_patches=4, n_patch_params=5
   integer, parameter :: jdim=2, kdim=3, ldim=4
   type(bfstruc_header) :: bfstruc_hdr
   type(bfstruc_patch) :: patchset(n_patches)
   integer      :: patchIntParams(n_patches, n_patch_params)
   real*8       :: patchR8Params (n_patches, n_patch_params)
   character*80 :: patchC80Params(n_patches, n_patch_params)
   real x(jdim, kdim, ldim), &
        y(jdim, kdim, ldim), &
        z(jdim, kdim, ldim)
   integer iblanks(jdim, kdim, ldim)

 !
 ! strand mesh
   integer, parameter :: nSurfNodesL    = 6
   integer, parameter :: nTriFacesL     = 2
   integer, parameter :: nQuadFacesL    = 1
   integer, parameter :: nBndEdgesL     = 6
   integer, parameter :: nPtsPerStrandL = 4
   integer, parameter :: nSurfPatchesL  = 2
   integer, parameter :: nEdgePatchesL  = 4
   type(strand_header)     :: strandHeader
   type(strand_surf_patch) :: strandSurfPatch(nSurfPatchesL)
   type(strand_edge_patch) :: strandEdgePatch(nEdgePatchesL)
   integer, dimension(3,nTriFacesL)             :: triConn
   integer, dimension(4,nQuadFacesL)            :: quadConn
   integer, dimension(2,nBndEdgesL)             :: bndEdges
   integer, dimension(  nSurfNodesL)            :: nodeClip
   integer, dimension(  nTriFacesL+nQuadFacesL) :: faceClip
   integer, dimension(  nTriFacesL+nQuadFacesL) :: faceTag
   integer, dimension(  nBndEdgesL)             :: edgeTag
   real   , dimension(3,nSurfNodesL)            :: xSurf
   real   , dimension(3,nSurfNodesL)            :: pointingVec
   real   , dimension(0:nPtsPerStrandL-1)       :: xStrand

 !
 ! unstruc mesh
   integer, parameter :: nPatches = 2
   type(unstruc_header) :: unstruc_hdr
   type(unstruc_patchheader) :: unstruc_patchhdrs(nPatches)
   real, allocatable :: xyz(:,:)
   integer, allocatable :: triFaces(:,:)
   integer, allocatable :: quadFaces(:,:)

 !
 ! cart mesh
   integer, parameter :: nLevels=3, nBlocks=6, nFringe=3
   type(cart_header) :: cart_hdr
   integer           :: ratios(nLevels)
   type(cart_block)  :: cart_blks(nBlocks)
   integer,parameter :: block2_iblDim(3) = (/4,6,9/)
   integer,parameter :: block6_iblDim(3) = (/8,9,10/)
   integer :: block2_iblanks(block2_iblDim(1), block2_iblDim(2), block2_iblDim(3))
   integer :: block6_iblanks(block6_iblDim(1), block6_iblDim(2), block6_iblDim(3))
end module data_mod

program mixed_write

   use avmesh_mod
   use data_mod
   implicit none

 !
 ! initialization
   file_hdr%meshRevision               = '1.0'
   file_hdr%meshCount                  = nMesh
   file_hdr%contactName                = 'Josh Calahan'
   file_hdr%contactOrg                 = 'Aerospace Testing Alliance'
   file_hdr%contactEmail               = 'joshua.calahan@arnold.af.mil'
   file_hdr%precision                   = 1
   file_hdr%dimensions                  = 3 
   file_description = 'An example AVMesh file with mixed grids.'
   file_hdr%descriptionSize = len(file_description)

   call init_bfstruc_mesh()
   call init_strand_mesh()
   call init_unstruc_mesh()
   call init_cart_mesh()

   open(unit=10, file=filename, access='stream', form='unformatted')

   call avm_write_file_header(10, file_hdr, file_description) 

   call avm_write_mesh_header(10, mesh_hdrs(1), mesh_descriptions(1))
   call avm_write_bfstruc_header(10, bfstruc_hdr, patchset, &
           patchIntParams, patchR8Params, patchC80Params)

   call avm_write_mesh_header(10, mesh_hdrs(2), mesh_descriptions(2))
   call avm_write_strand_header(10, strandHeader, strandSurfPatch, strandEdgePatch)

   call avm_write_mesh_header(10, mesh_hdrs(3), mesh_descriptions(3))
   call avm_write_unstruc_header(10, unstruc_hdr, unstruc_patchhdrs)

   call avm_write_mesh_header(10, mesh_hdrs(4), mesh_descriptions(4))
   call avm_write_cart_header(10, cart_hdr, ratios, cart_blks) 

   call write_bfstruc_mesh()
   call write_strand_mesh()
   call write_unstruc_mesh()
   call write_cart_mesh()

   close(10)

   write(6,'(A,A)')  'Successfully wrote: ', filename
end program mixed_write

subroutine init_bfstruc_mesh
   use data_mod
   implicit none
   integer :: i,j,k,l

   mesh_hdrs(1)%meshName                   = 'one'
   mesh_hdrs(1)%meshType                   = 'bfstruc'
   mesh_hdrs(1)%meshGenerator              = 'mixed_write'
   mesh_hdrs(1)%changedDate                = 'December 20, 2010'
   mesh_hdrs(1)%coordinateSystem           = 'xByUzL'
   mesh_hdrs(1)%modelScale                 = 1.0
   mesh_hdrs(1)%gridUnits                  = 'ft'
   mesh_hdrs(1)%reynoldsNumber             = 5.0
   mesh_hdrs(1)%referenceLength            = 1.0
   mesh_hdrs(1)%wallDistance               = 1.0
   mesh_hdrs(1)%referencePoint(:)          = (/1.0,2.0,3.0/)
   mesh_hdrs(1)%referencePointDescription = 'origin'
   mesh_hdrs(1)%periodicity                 = 1.0
   mesh_hdrs(1)%periodicityDescription     = 'full-span'
   mesh_hdrs(1)%refinementLevel            = 1

   mesh_descriptions(1) = 'A nonsensical bfstruc mesh that just demonstrates the format.'
   mesh_hdrs(1)%descriptionSize = len(mesh_descriptions(1))

   bfstruc_hdr%jmax            = jdim
   bfstruc_hdr%kmax            = kdim
   bfstruc_hdr%lmax            = ldim
   bfstruc_hdr%iblank          = 1
   bfstruc_hdr%nPatches        = n_patches
   bfstruc_hdr%nPatchIntParams = n_patch_params
   bfstruc_hdr%nPatchR8Params  = n_patch_params
   bfstruc_hdr%nPatchC80Params = n_patch_params

   do i=1,n_patches
      patchset(i)%bctype    = i
      patchset(i)%direction = i
      patchset(i)%jbegin    = i
      patchset(i)%jend      = i
      patchset(i)%kbegin    = i
      patchset(i)%kend      = i
      patchset(i)%lbegin    = i
      patchset(i)%lend      = i
      write(patchset(i)%description, '(A,I1,A)') 'patch ', i, ' description'

      do j=1,n_patch_params
         patchIntParams(i,j) = j
         patchR8Params (i,j) = real(j)
         write(patchC80Params(i,j), '(A,I1,A,I1,A,I1,A)') &
            'bc character parameter (bc=', i, ', param=', j, ')'
      end do
   end do

   do j = 1,jdim
      do k = 1,kdim
         do l = 1,ldim
            x(j,k,l) = j
            y(j,k,l) = k
            z(j,k,l) = l
            iblanks(j,k,l) = j*100+k*10+l*1
         enddo
      enddo
   enddo
end subroutine init_bfstruc_mesh

subroutine init_strand_mesh
   use data_mod
   implicit none
   integer :: i

   mesh_hdrs(2)%meshName                   = 'two'
   mesh_hdrs(2)%meshType                   = 'strand'
   mesh_hdrs(2)%meshGenerator              = 'mixed_write'
   mesh_hdrs(2)%changedDate                = 'May 6, 2010'
   mesh_hdrs(2)%coordinateSystem           = 'xByUzL'
   mesh_hdrs(2)%modelScale                 = 1.0
   mesh_hdrs(2)%gridUnits                  = 'm'
   mesh_hdrs(2)%reynoldsNumber             = 5.0
   mesh_hdrs(2)%referenceLength            = 1.0
   mesh_hdrs(2)%wallDistance               = 1.0
   mesh_hdrs(2)%referencePoint(:)          = (/4.0,5.0,6.0/)
   mesh_hdrs(2)%referencePointDescription = 'origin'
   mesh_hdrs(2)%periodicity                 = 1.0
   mesh_hdrs(2)%periodicityDescription     = 'full-span'
   mesh_hdrs(2)%refinementLevel            = 1

   mesh_descriptions(2) = 'A nonsensical strand mesh that just demonstrates the format.'
   mesh_hdrs(2)%descriptionSize = len(mesh_descriptions(2))

   strandHeader%surfaceOnly        = 0 ! FALSE
   strandHeader%nSurfNodes         = nSurfNodesL
   strandHeader%nTriFaces          = nTriFacesL
   strandHeader%nQuadFaces         = nQuadFacesL
   strandHeader%nBndEdges          = nBndEdgesL
   strandHeader%nPtsPerStrand      = nPtsPerStrandL
   strandHeader%nSurfPatches       = nSurfPatchesL
   strandHeader%nEdgePatches       = nEdgePatchesL
   strandHeader%strandLength       = 2.
   strandHeader%stretchRatio       = 1.
   strandHeader%smoothingThreshold = 1.
   strandHeader%strandDistribution = 'geometric'

   do i=1,nSurfPatchesL
      strandSurfPatch(i)%surfPatchID     = i
      strandSurfPatch(i)%surfPatchBody   = 'flat plate'
      strandSurfPatch(i)%surfPatchComp   = 'surface'
      strandSurfPatch(i)%surfPatchBCType = 'no slip'
   end do

   do i=1,nEdgePatchesL
      strandEdgePatch(i)%edgePatchID     = i
      strandEdgePatch(i)%edgePatchBody   = 'plate boundary'
      strandEdgePatch(i)%edgePatchComp   = 'edge'
      strandEdgePatch(i)%edgePatchBCType = 'symmetry'
      strandEdgePatch(i)%nx              = 1.0
      strandEdgePatch(i)%ny              = 0.0
      strandEdgePatch(i)%nz              = 0.0
   end do
end subroutine init_strand_mesh

subroutine init_unstruc_mesh
   use data_mod
   implicit none
   integer :: j

   mesh_hdrs(3)%meshName = 'three'
   mesh_hdrs(3)%meshType = 'unstruc'
   mesh_hdrs(3)%meshGenerator = 'mixed_write'
   mesh_hdrs(3)%changedDate = 'June 15, 2010'
   mesh_hdrs(3)%coordinateSystem = 'xByUzL'
   mesh_hdrs(3)%modelScale = 1.0
   mesh_hdrs(3)%gridUnits = 'in'
   mesh_hdrs(3)%reynoldsNumber = 1.0
   mesh_hdrs(3)%referenceLength = 1.0
   mesh_hdrs(3)%wallDistance = 1.0
   mesh_hdrs(3)%referencePoint(:) = (/7.0,8.0,9.0/)
   mesh_hdrs(3)%referencePointDescription = 'origin'
   mesh_hdrs(3)%periodicity = 1.0
   mesh_hdrs(3)%periodicityDescription = 'full-span'
   mesh_hdrs(3)%refinementLevel = 1
   mesh_descriptions(3) = 'A nonsensical unstruc mesh that just demonstrates the format.'
   mesh_hdrs(3)%descriptionSize = len(mesh_descriptions(3))

   unstruc_hdr%fullViscousLayerCount = 0
   unstruc_hdr%partialViscousLayerCount = 0
   unstruc_hdr%nNodes = 12
   unstruc_hdr%nEdges = 24
   unstruc_hdr%nFaces = 17
   unstruc_hdr%nMaxNodesPerFace = 4
   unstruc_hdr%nTriFaces = 9
   unstruc_hdr%nQuadFaces = 8
   unstruc_hdr%nBndTriFaces = 8
   unstruc_hdr%nBndQuadFaces = 6
   unstruc_hdr%nCells = 4
   unstruc_hdr%nMaxNodesPerCell = 8
   unstruc_hdr%nMaxFacesPerCell = 6
   unstruc_hdr%nHexCells = 1
   unstruc_hdr%nTetCells = 1
   unstruc_hdr%nPriCells = 1
   unstruc_hdr%nPyrCells = 1
   unstruc_hdr%nPatches = 2
   unstruc_hdr%nPatchNodes = 12
   unstruc_hdr%nPatchFaces = 14
   do j = 1,nPatches
       write(unstruc_patchhdrs(j)%patchLabel,'(A,I0,A)')  'patch ',j,' label'
       write(unstruc_patchhdrs(j)%patch_bctype,'(A,I0,A)')  'patch ',j,' bctype'
       unstruc_patchhdrs(j)%patchId = -j
   end do

   allocate(xyz(3,unstruc_hdr%nNodes), &
            triFaces(5,unstruc_hdr%nTriFaces), &
            quadFaces(6,unstruc_hdr%nQuadFaces))

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
end subroutine init_unstruc_mesh

subroutine init_cart_mesh
   use data_mod
   implicit none

   mesh_hdrs(4)%meshName                   = 'four'
   mesh_hdrs(4)%meshType                   = 'cart'
   mesh_hdrs(4)%meshGenerator              = 'write_test001_cart'
   mesh_hdrs(4)%changedDate                = 'May 7, 2010'
   mesh_hdrs(4)%coordinateSystem           = 'xByUzL'
   mesh_hdrs(4)%modelScale                 = 1.0
   mesh_hdrs(4)%gridUnits                  = 'm'
   mesh_hdrs(4)%reynoldsNumber             = 1.0
   mesh_hdrs(4)%referenceLength            = 1.0
   mesh_hdrs(4)%wallDistance               = 1.0
   mesh_hdrs(4)%referencePoint(:)          = (/10.0,11.0,12.0/)
   mesh_hdrs(4)%referencePointDescription = 'origin'
   mesh_hdrs(4)%periodicity                 = 1.0
   mesh_hdrs(4)%periodicityDescription     = 'full-span'
   mesh_hdrs(4)%refinementLevel            = 1

   mesh_descriptions(4) = 'A nonsensical cart mesh that just demonstrates the format.'
   mesh_hdrs(4)%descriptionSize = len(mesh_descriptions(4))

   cart_hdr%nLevels = nLevels
   cart_hdr%nBlocks = nBlocks
   cart_hdr%nFringe = nFringe
   cart_hdr%nxc     = 10
   cart_hdr%nyc     = 10
   cart_hdr%nzc     = 10
   cart_hdr%domXLo  = 0.
   cart_hdr%domYLo  = 0.
   cart_hdr%domZLo  = 0.
   cart_hdr%domXHi  = 10.
   cart_hdr%domYHi  = 10.
   cart_hdr%domZHi  = 10.

   ! level 1
   ratios(1) = 1
   ! level 2
   ratios(2) = 2
   ! level 3
   ratios(3) = 4

   ! block 1 - level 1
   cart_blks(1)%levNum    = 1
   cart_blks(1)%ilo(:)    = (/1,1,1/)
   cart_blks(1)%ihi(:)    = (/10,10,10/)
   cart_blks(1)%iblflag   = 1 ! all points unblanked
   cart_blks(1)%bdryflag  = 1 ! resides on a farfield boundary
   cart_blks(1)%iblDim(:) = 0 
   cart_blks(1)%blo(:)    = (/1,1,1/)
   cart_blks(1)%bhi(:)    = (/1,1,1/) 

   ! block 2 - level 2
   cart_blks(2)%levNum    = 2
   cart_blks(2)%ilo(:)    = (/7,7,7/)
   cart_blks(2)%ihi(:)    = (/14,14,14/)
   cart_blks(2)%iblflag   = 2 ! mixed iblanks
   cart_blks(2)%bdryflag  = 0 ! not on a boundary 
   cart_blks(2)%iblDim(:) = block2_iblDim
   cart_blks(2)%blo(:)    = -1
   cart_blks(2)%bhi(:)    = -1
   ! set all points unblanked, then a few blanked (for testing)
   block2_iblanks = 1
   block2_iblanks(1,1,1) = 0
   block2_iblanks(3,5,7) = 0
   block2_iblanks(4,6,9) = 0

   ! block 3 - level 3
   cart_blks(3)%levNum    = 3
   cart_blks(3)%ilo(:)    = (/17,17,17/)
   cart_blks(3)%ihi(:)    = (/20,20,24/)
   cart_blks(3)%iblflag   = 1 ! all points unblanked
   cart_blks(3)%bdryflag  = 0 ! not on a boundary 
   cart_blks(3)%iblDim(:) = 0 
   cart_blks(3)%blo(:)    = -1
   cart_blks(3)%bhi(:)    = -1

   ! block 4 - level 3
   cart_blks(4)%levNum    = 3
   cart_blks(4)%ilo(:)    = (/21,17,17/)
   cart_blks(4)%ihi(:)    = (/24,20,24/)
   cart_blks(4)%iblflag   = 1 ! all points unblanked
   cart_blks(4)%bdryflag  = 0 ! not on a boundary 
   cart_blks(4)%iblDim(:) = 0 
   cart_blks(4)%blo(:)    = -1
   cart_blks(4)%bhi(:)    = -1

   ! block 5 - level 3
   cart_blks(5)%levNum    = 3
   cart_blks(5)%ilo(:)    = (/17,21,17/)
   cart_blks(5)%ihi(:)    = (/20,24,24/)
   cart_blks(5)%iblflag   = 1 ! all points unblanked
   cart_blks(5)%bdryflag  = 0 ! not on a boundary 
   cart_blks(5)%iblDim(:) = 0 
   cart_blks(5)%blo(:)    = -1
   cart_blks(5)%bhi(:)    = -1

   ! block 6 - level 3
   cart_blks(6)%levNum    = 3
   cart_blks(6)%ilo(:)    = (/21,21,17/)
   cart_blks(6)%ihi(:)    = (/24,24,24/)
   cart_blks(6)%iblflag   = 2 ! mixed iblanks
   cart_blks(6)%bdryflag  = 0 ! not on a boundary 
   cart_blks(6)%iblDim(:) = block6_iblDim 
   cart_blks(6)%blo(:)    = -1
   cart_blks(6)%bhi(:)    = -1
   block6_iblanks = 1
   block6_iblanks(1,1,1) = 0
   block6_iblanks(2,2,8) = 0
   block6_iblanks(8,9,10) = 0
end subroutine init_cart_mesh

subroutine write_bfstruc_mesh
   use data_mod
   implicit none
   call avm_write_bfstruc_mesh(10, x,y,z, jdim,kdim,ldim)
   call avm_write_bfstruc_iblanks(10, iblanks, jdim,kdim,ldim)
end subroutine write_bfstruc_mesh

subroutine write_strand_mesh
   use data_mod
   implicit none
   integer :: n,k

   ! generate the mesh (nonsensical)
   ! set the values based on mesh number to check
   ! that we can get out what we write to disk

   do n=1,nTriFacesL
      do k=1,3
         triConn(k,n) = n+k
      end do
   end do

   quadConn    = 1
   bndEdges    = 1
   nodeClip    = 1
   faceClip    = 1
   faceTag     = 1
   edgeTag     = 1
   xSurf       = real(1)
   pointingVec = real(1)

   do n=0,nPtsPerStrandL-1
      xStrand(n) = real(n)
   end do

   call avm_write_strand_mesh(10, n, &
      triConn, quadConn, bndEdges, nodeClip, faceClip, faceTag, &
      edgeTag, xSurf, pointingVec, xStrand, nTriFacesL, &
      nQuadFacesL, nBndEdgesL, nSurfNodesL, nPtsPerStrandL)
end subroutine write_strand_mesh

subroutine write_unstruc_mesh
   use data_mod
   implicit none
   call avm_write_unstruc_mesh(10, &
           xyz, unstruc_hdr%nNodes, &
           triFaces, unstruc_hdr%nTriFaces, &
           quadFaces, unstruc_hdr%nQuadFaces)
end subroutine write_unstruc_mesh

subroutine write_cart_mesh
   use data_mod
   implicit none
   integer :: b,jd,kd,ld

   do b=1,cart_hdr%nBlocks
      if (cart_blks(b)%iblFlag==2) then
         jd = cart_blks(b)%iblDim(1)
         kd = cart_blks(b)%iblDim(2)
         ld = cart_blks(b)%iblDim(3)
         if (b==2) then
            call avm_write_cart_iblanks(10, block2_iblanks, jd,kd,ld) 
         else if (b==6) then
            call avm_write_cart_iblanks(10, block6_iblanks, jd,kd,ld) 
         endif
      end if
   end do
end subroutine write_cart_mesh
