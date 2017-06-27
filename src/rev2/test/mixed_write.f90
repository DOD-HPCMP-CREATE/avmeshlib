module avmesh_mod
   use avmesh
   type file_id_prefix
      sequence
      character(6) :: magicString     
      integer(4)   :: magicNumber
      integer(4)   :: formatRevision
   end type file_id_prefix

   type file_header
      sequence
      integer(4)     :: meshCount
      character(128) :: contactInfo
      integer(4)     :: precision
      integer(4)     :: dimensions
      integer(4)     :: descriptionSize
   end type file_header

   type mesh_header
      sequence
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
   end type mesh_header

   type unstruc_header
      sequence
      integer(4) :: nNodes
      integer(4) :: nFaces
      integer(4) :: nCells
      integer(4) :: nMaxNodesPerFace
      integer(4) :: nMaxNodesPerCell
      integer(4) :: nMaxFacesPerCell
      integer(4) :: bndFacePolyOrder
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

contains

   subroutine avm_write_file_header(iu, hdr, desc) 
      implicit none
      integer,intent(in) :: iu
      type(file_id_prefix) :: prefix
      type(file_header),intent(in) :: hdr
      character(128),intent(in) :: desc

      prefix%magicString   = 'AVMESH'
      prefix%magicNumber   = 1
      prefix%formatRevision = 2

      write(iu) prefix%magicString, &
                prefix%magicNumber, &
                prefix%formatRevision

      write(iu) hdr%meshCount, &
                hdr%contactInfo, &
                hdr%precision, &
                hdr%dimensions, &
                hdr%descriptionSize

      write(iu) desc
   end subroutine avm_write_file_header

   subroutine avm_write_mesh_header(ui, mesh_hdr)
      implicit none
      integer :: ui
      type(mesh_header) :: mesh_hdr

      write(ui) mesh_hdr%meshName, &
                  mesh_hdr%meshType, &
                  mesh_hdr%meshGenerator, &
                  mesh_hdr%coordinateSystem, &
                  mesh_hdr%modelScale, &
                  mesh_hdr%gridUnits, &
                  mesh_hdr%referenceLength, &
                  mesh_hdr%referenceArea, &
                  mesh_hdr%referencePoint, &
                  mesh_hdr%referencePointDescription, &
                  mesh_hdr%meshDescription

   end subroutine avm_write_mesh_header

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
               strandHeader%nNodes           ,&
               strandHeader%nStrands           ,&
               strandHeader%nTriFaces            ,&
               strandHeader%nQuadFaces           ,&
               strandHeader%nPolyFaces           ,&
               strandHeader%polyFacesSize        ,&
               strandHeader%nBndEdges            ,&
               strandHeader%nPtsPerStrand        ,&
               strandHeader%nSurfPatches         ,&
               strandHeader%nEdgePatches         ,&
               strandHeader%strandLength         ,&
               strandHeader%stretchRatio         ,&
               strandHeader%smoothingThreshold   ,&
               strandHeader%strandDistribution   ,&
               strandHeader%nNodesOnGeometry     ,&
               strandHeader%nEdgesOnGeometry     ,&
               strandHeader%nFacesOnGeometry

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
      strand_triFaces, strand_quadFaces, strand_polyFaces, bndEdges, nodeID, nodeClip, faceTag, &
      edgeTag, strand_xyz, pointingVec, xStrand, nTriFacesL, &
      nQuadFacesL, nPolyFacesL, polyFacesSizeL, nBndEdgesL, nNodesL, nStrandsL, nPtsPerStrandL)

      implicit none
      integer iu, meshi
      integer :: nTriFacesL,nQuadFacesL,nPolyFacesL,polyFacesSizeL,nBndEdgesL, &
                 nNodesL,nStrandsL,nPtsPerStrandL
      integer,dimension(3,nTriFacesL)             :: strand_triFaces
      integer,dimension(4,nQuadFacesL)            :: strand_quadFaces
      integer,dimension(polyFacesSizeL)            :: strand_polyFaces
      integer,dimension(2,nBndEdgesL)             :: bndEdges
      integer,dimension(  nStrandsL)            :: nodeID
      integer,dimension(  nStrandsL)            :: nodeClip
      integer,dimension(  nTriFacesL+nQuadFacesL+nPolyFacesL) :: faceTag
      integer,dimension(  nBndEdgesL)             :: edgeTag
      real   ,dimension(3,nNodesL)            :: strand_xyz
      real   ,dimension(3,nStrandsL)            :: pointingVec
      real   ,dimension(0:nPtsPerStrandL-1)       :: xStrand

      integer n,k

    ! write the mesh

      write(iu)((strand_xyz(k,n),k=1,3),n=1,nNodesL)
      write(iu)(nodeID(n),n=1,nStrandsL)
      write(iu)(nodeClip(n),n=1,nStrandsL)
      write(iu)((pointingVec(k,n),k=1,3),n=1,nStrandsL)
      write(iu)(xStrand(n),n=0,nPtsPerStrandL-1)

      write(iu)((strand_triFaces(k,n),k=1,3),n=1,nTriFacesL)
      write(iu)((strand_quadFaces(k,n),k=1,4),n=1,nQuadFacesL)
      write(iu)(strand_polyFaces(k),k=1,polyFacesSizeL)
      write(iu)(faceTag(n),n=1,nTriFacesL+nQuadFacesL+nPolyFacesL)

      write(iu)((bndEdges(k,n),k=1,2),n=1,nBndEdgesL)
      write(iu)(edgeTag(n),n=1,nBndEdgesL)

   end subroutine avm_write_strand_mesh

   subroutine avm_write_unstruc_header(ui, unstruc_hdr, unstruc_phdrs)
      implicit none
      integer :: ui
      type(unstruc_header) :: unstruc_hdr
      type(unstruc_patchheader) :: unstruc_phdrs(unstruc_hdr%nPatches)
      integer :: i

      write(ui)   unstruc_hdr%nNodes, &
                  unstruc_hdr%nFaces, &
                  unstruc_hdr%nCells, &
                  unstruc_hdr%nMaxNodesPerFace, &
                  unstruc_hdr%nMaxNodesPerCell, &
                  unstruc_hdr%nMaxFacesPerCell, &
                  unstruc_hdr%bndFacePolyOrder, &
                  unstruc_hdr%cellPolyOrder, &
                  unstruc_hdr%nPatches, &
                  unstruc_hdr%nHexCells, &
                  unstruc_hdr%nTetCells, &
                  unstruc_hdr%nPriCells, &
                  unstruc_hdr%nPyrCells, &
                  unstruc_hdr%nBndTriFaces, &
                  unstruc_hdr%nTriFaces, &
                  unstruc_hdr%nBndQuadFaces, &
                  unstruc_hdr%nQuadFaces, &
                  unstruc_hdr%nEdges, &
                  unstruc_hdr%nNodesOnGeometry, &
                  unstruc_hdr%nEdgesOnGeometry, &
                  unstruc_hdr%nFacesOnGeometry, &
                  unstruc_hdr%geomRegionId

      do i = 1, unstruc_hdr%nPatches
         write(ui) unstruc_phdrs(i)%patchLabel, &
                     unstruc_phdrs(i)%patch_bctype, &
                     unstruc_phdrs(i)%patchId
      end do
   end subroutine avm_write_unstruc_header

   subroutine avm_write_unstruc_mesh(ui, &
      xyz, nNodes, &
      triFaces, nTriFaces, &
      quadFaces, nQuadFaces, &
      hexCells, nHexCells, &
      tetCells, nTetCells, &
      priCells, nPriCells, &
      pyrCells, nPyrCells, &
      edges, nEdges, &
      nodesOnGeometry, nNodesOnGeometry, &
      edgesOnGeometry, nEdgesOnGeometry, &
      facesOnGeometry, nFacesOnGeometry, &
      hexGeomIds, &
      tetGeomIds, &
      priGeomIds, &
      pyrGeomIds)

      implicit none
      integer :: ui, i, j
      integer :: k, nodesInFace, rightCellIndex
      integer :: nNodes, nTriFaces, nQuadFaces
      integer :: nHexCells, nTetCells, nPriCells, nPyrCells
      integer :: nEdges
      integer nNodesOnGeometry, nEdgesOnGeometry, nFacesOnGeometry
      real :: xyz(3,nNodes)
      integer :: triFaces(5,nTriFaces)
      integer :: quadFaces(6,nQuadFaces)
      integer :: hexCells(8, nHexCells)
      integer :: tetCells(4, nTetCells)
      integer :: priCells(6, nPriCells)
      integer :: pyrCells(5, nPyrCells)
      integer :: edges(2,nEdges)
      type(AVM_AMR_Node_Data) :: nodesOnGeometry(nNodesOnGeometry)
      integer :: edgesOnGeometry(3,nEdgesOnGeometry)
      integer :: facesOnGeometry(3,nFacesOnGeometry)
      integer :: hexGeomIds(nHexCells)
      integer :: tetGeomIds(nTetCells)
      integer :: priGeomIds(nPriCells)
      integer :: pyrGeomIds(nPyrCells)

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

      ! Hex cells
      write(ui) ((hexCells(i,j),i=1,8),j=1,nHexCells)
      ! Tet cells
      write(ui) ((tetCells(i,j),i=1,4),j=1,ntetCells)
      ! Prism cells
      write(ui) ((priCells(i,j),i=1,6),j=1,nPriCells)
      ! Pyramid cells
      write(ui) ((pyrCells(i,j),i=1,5),j=1,nPyrCells)

      ! Edges
      write(ui) ((edges(i,j),i=1,2),j=1,nEdges)

      ! AMR
      !write(ui) (nodesOnGeometry(i),i=1,nNodesOnGeometry)
      write(ui) (nodesOnGeometry(i)%u, &
                 nodesOnGeometry(i)%v, &
                 nodesOnGeometry(i)%nodeIndex, &
                 nodesOnGeometry(i)%geomType, &
                 nodesOnGeometry(i)%geomTopoId, &
                 nodesOnGeometry(i)%dummy, &
                 i=1,nNodesOnGeometry)

      write(ui) ((edgesOnGeometry(i,j),i=1,3),j=1,nEdgesOnGeometry)
      write(ui) ((facesOnGeometry(i,j),i=1,3),j=1,nFacesOnGeometry)

      ! AMR volume geometry Ids
      write(ui) (hexGeomIds(i),i=1,nHexCells)
      write(ui) (tetGeomIds(i),i=1,nTetCells)
      write(ui) (priGeomIds(i),i=1,nPriCells)
      write(ui) (pyrGeomIds(i),i=1,nPyrCells)
   end subroutine avm_write_unstruc_mesh
end module

module data_mod
   use avmesh_mod

   integer, parameter :: nMesh=2
   character*80, parameter :: filename='mixed.avm'

 !
 ! avmesh format
   type(file_header) :: file_hdr
   character(128)    :: file_description
   type(mesh_header) :: mesh_hdrs(nMesh)

 !
 ! strand mesh
   integer, parameter :: nNodesL    = 6
   integer, parameter :: nStrandsL    = 8
   integer, parameter :: nTriFacesL     = 2
   integer, parameter :: nQuadFacesL    = 1
   integer, parameter :: nPolyFacesL    = 1
   integer, parameter :: polyFacesSizeL = 10
   integer, parameter :: nBndEdgesL     = 6
   integer, parameter :: nPtsPerStrandL = 4
   integer, parameter :: nSurfPatchesL  = 2
   integer, parameter :: nEdgePatchesL  = 4
   type(strand_header)     :: strandHeader
   type(strand_surf_patch) :: strandSurfPatch(nSurfPatchesL)
   type(strand_edge_patch) :: strandEdgePatch(nEdgePatchesL)
   integer, dimension(3,nTriFacesL)             :: strand_triFaces
   integer, dimension(4,nQuadFacesL)            :: strand_quadFaces
   integer, dimension(polyFacesSizeL)           :: strand_polyFaces
   integer, dimension(2,nBndEdgesL)             :: bndEdges
   integer, dimension(  nStrandsL)            :: nodeID
   integer, dimension(  nStrandsL)            :: nodeClip
   integer, dimension(  nTriFacesL+nQuadFacesL+nPolyFacesL) :: faceTag
   integer, dimension(  nBndEdgesL)             :: edgeTag
   real   , dimension(3,nNodesL)            :: strand_xyz
   real   , dimension(3,nStrandsL)            :: pointingVec
   real   , dimension(0:nPtsPerStrandL-1)       :: xStrand

 !
 ! unstruc mesh
   integer, parameter :: nPatches = 2
   type(unstruc_header) :: unstruc_hdr
   type(unstruc_patchheader) :: unstruc_patchhdrs(nPatches)
   real, allocatable :: xyz(:,:)
   integer, allocatable :: triFaces(:,:)
   integer, allocatable :: quadFaces(:,:)
   integer, allocatable :: hexCells(:,:)
   integer, allocatable :: tetCells(:,:)
   integer, allocatable :: priCells(:,:)
   integer, allocatable :: pyrCells(:,:)
   integer, allocatable :: edges(:,:)
   type(AVM_AMR_Node_Data), allocatable :: nodesOnGeometry(:)
   integer, allocatable :: edgesOnGeometry(:,:)
   integer, allocatable :: facesOnGeometry(:,:)
   integer, allocatable :: hexGeomIds(:)
   integer, allocatable :: tetGeomIds(:)
   integer, allocatable :: priGeomIds(:)
   integer, allocatable :: pyrGeomIds(:)

end module data_mod

program mixed_write

   use avmesh_mod
   use data_mod
   implicit none

 !
 ! initialization
   file_hdr%meshCount                  = nMesh
   file_hdr%contactInfo               = 'Josh Calahan: joshua.calahan@arnold.af.mil'
   file_hdr%precision                   = 1
   file_hdr%dimensions                  = 3 
   file_description = 'An example AVMesh file with mixed grids.'
   file_hdr%descriptionSize = len(file_description)

   call init_strand_mesh()
   call init_unstruc_mesh()

   open(unit=10, file=filename, access='stream', form='unformatted')

   call avm_write_file_header(10, file_hdr, file_description) 
   write(6,'(A)')  'wrote file header '

   call avm_write_mesh_header(10, mesh_hdrs(1))
   write(6,'(A)')  'wrote mesh header '
   call avm_write_strand_header(10, strandHeader, strandSurfPatch, strandEdgePatch)
   write(6,'(A)')  'wrote strand header '

   call avm_write_mesh_header(10, mesh_hdrs(2))
   write(6,'(A)')  'wrote mesh header '
   call avm_write_unstruc_header(10, unstruc_hdr, unstruc_patchhdrs)
   write(6,'(A)')  'wrote unstruc header '

   call write_strand_mesh()
   write(6,'(A)')  'wrote strand mesh'
   call write_unstruc_mesh()
   write(6,'(A)')  'wrote unstruc mesh'

   close(10)

   write(6,'(A,A)')  'Successfully wrote: ', filename
end program mixed_write

subroutine init_strand_mesh
   use data_mod
   implicit none
   integer :: i

   mesh_hdrs(1)%meshName                   = 'two'
   mesh_hdrs(1)%meshType                   = 'strand'
   mesh_hdrs(1)%meshGenerator              = 'mixed_write'
   mesh_hdrs(1)%coordinateSystem           = 'xByUzL'
   mesh_hdrs(1)%modelScale                 = 1.0
   mesh_hdrs(1)%gridUnits                  = 'm'
   mesh_hdrs(1)%referenceLength(:)          = (/1.0,1.0,1.0/)
   mesh_hdrs(1)%referenceArea            = 1.0
   mesh_hdrs(1)%referencePoint(:)          = (/4.0,5.0,6.0/)
   mesh_hdrs(1)%referencePointDescription = 'origin'
   mesh_hdrs(1)%meshDescription = 'A nonsensical strand mesh that just demonstrates the format.'

   strandHeader%surfaceOnly        = 0 ! FALSE
   strandHeader%nNodes             = nNodesL
   strandHeader%nStrands           = nStrandsL
   strandHeader%nTriFaces          = nTriFacesL
   strandHeader%nQuadFaces         = nQuadFacesL
   strandHeader%nPolyFaces         = nPolyFacesL
   strandHeader%polyFacesSize      = polyFacesSizeL
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

   mesh_hdrs(2)%meshName = 'three'
   mesh_hdrs(2)%meshType = 'unstruc'
   mesh_hdrs(2)%meshGenerator = 'mixed_write'
   mesh_hdrs(2)%coordinateSystem = 'xByUzL'
   mesh_hdrs(2)%modelScale = 1.0
   mesh_hdrs(2)%gridUnits = 'in'
   mesh_hdrs(2)%referenceLength(:) = (/2.0,2.0,2.0/)
   mesh_hdrs(2)%referenceArea = 1.0
   mesh_hdrs(2)%referencePoint(:) = (/7.0,8.0,9.0/)
   mesh_hdrs(2)%referencePointDescription = 'origin'
   mesh_hdrs(2)%meshDescription = 'A nonsensical unstruc mesh that just demonstrates the format.'

   unstruc_hdr%nNodes = 12
   unstruc_hdr%nFaces = 17
   unstruc_hdr%nCells = 4
   unstruc_hdr%nMaxNodesPerFace = 4
   unstruc_hdr%nMaxNodesPerCell = 8
   unstruc_hdr%nMaxFacesPerCell = 6
   unstruc_hdr%bndFacePolyOrder = 1
   unstruc_hdr%cellPolyOrder = 1
   unstruc_hdr%nPatches = 2
   unstruc_hdr%nHexCells = 1
   unstruc_hdr%nTetCells = 1
   unstruc_hdr%nPriCells = 1
   unstruc_hdr%nPyrCells = 1
   unstruc_hdr%nBndTriFaces = 8
   unstruc_hdr%nTriFaces = 9
   unstruc_hdr%nBndQuadFaces = 6
   unstruc_hdr%nQuadFaces = 8
   unstruc_hdr%nEdges = 12
   unstruc_hdr%nNodesOnGeometry = 4
   unstruc_hdr%nEdgesOnGeometry = 1
   unstruc_hdr%nFacesOnGeometry = 2
   unstruc_hdr%geomRegionId = -1

   do j = 1,nPatches
       write(unstruc_patchhdrs(j)%patchLabel,'(A,I0,A)')  'patch ',j,' label'
       write(unstruc_patchhdrs(j)%patch_bctype,'(A,I0,A)')  'patch ',j,' bctype'
       unstruc_patchhdrs(j)%patchId = -j
   end do

   allocate(xyz(3,unstruc_hdr%nNodes), &
            triFaces(5,unstruc_hdr%nTriFaces), &
            quadFaces(6,unstruc_hdr%nQuadFaces), &
            hexCells(8, unstruc_hdr%nHexCells), &
            tetCells(4, unstruc_hdr%nTetCells), &
            priCells(6, unstruc_hdr%nPriCells), &
            pyrCells(5, unstruc_hdr%nPyrCells), &
            edges(2, unstruc_hdr%nEdges), &
            nodesOnGeometry(unstruc_hdr%nNodesOnGeometry), &
            edgesOnGeometry(3,unstruc_hdr%nEdgesOnGeometry), &
            facesOnGeometry(3,unstruc_hdr%nFacesOnGeometry), &
            hexGeomIds(unstruc_hdr%nHexCells), &
            tetGeomIds(unstruc_hdr%nTetCells), &
            priGeomIds(unstruc_hdr%nPriCells), &
            pyrGeomIds(unstruc_hdr%nPyrCells))


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

    hexCells(:,1) = (/1, 4, 5, 6, 3, 8, 5, 4/)
    tetCells(:,1) = (/5, 12, 11, 3/)
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
    nodesOnGeometry(1)%u = 0.0
    nodesOnGeometry(1)%v = 0.0
    nodesOnGeometry(1)%dummy = 1
    nodesOnGeometry(2)%nodeIndex = 5
    nodesOnGeometry(2)%geomType = 1
    nodesOnGeometry(2)%geomTopoId = 3
    nodesOnGeometry(2)%u = 0.5
    nodesOnGeometry(2)%v = 0.0
    nodesOnGeometry(2)%dummy = 1
    nodesOnGeometry(3)%nodeIndex = 11
    nodesOnGeometry(3)%geomType = 2
    nodesOnGeometry(3)%geomTopoId = 5
    nodesOnGeometry(3)%u = 0.5
    nodesOnGeometry(3)%v = 0.7
    nodesOnGeometry(3)%dummy = 1
    nodesOnGeometry(4)%nodeIndex = 12
    nodesOnGeometry(4)%geomType = 1
    nodesOnGeometry(4)%geomTopoId = 7
    nodesOnGeometry(4)%u = 0.6
    nodesOnGeometry(4)%v = 0.0
    nodesOnGeometry(4)%dummy = 1

    edgesOnGeometry(:,1) = (/4, 1, 8/)

    facesOnGeometry(:,1) = (/18, 2, 12/)
    facesOnGeometry(:,2) = (/22, 2, 14/)

    hexGeomIds(1) = 1
    tetGeomIds(1) = 2
    priGeomIds(1) = 7
    pyrGeomIds(1) = 8

end subroutine init_unstruc_mesh

subroutine write_strand_mesh
   use data_mod
   implicit none
   integer :: n,k

   ! generate the mesh (nonsensical)
   ! set the values based on mesh number to check
   ! that we can get out what we write to disk

   do n=1,nTriFacesL
      do k=1,3
         strand_triFaces(k,n) = n+k
      end do
   end do

   do n=1,nQuadFacesL
      do k=1,4
         strand_quadFaces(k,n) = n+k
      end do
   end do
   do n=1,polyFacesSizeL
      strand_polyFaces(n) = n
   end do
   do n=1,nTriFacesL+nQuadFacesL+nPolyFacesL
      faceTag(n) = n
   end do
   bndEdges    = 1
   nodeID    = 1
   nodeClip    = 1
   edgeTag     = 1
   strand_xyz       = real(1)
   pointingVec = real(1)

   do n=0,nPtsPerStrandL-1
      xStrand(n) = real(n)
   end do

   call avm_write_strand_mesh(10, n, &
      strand_triFaces, strand_quadFaces, strand_polyFaces, bndEdges, nodeID, nodeClip, faceTag, &
      edgeTag, strand_xyz, pointingVec, xStrand, nTriFacesL, &
      nQuadFacesL, nPolyFacesL, polyFacesSizeL, nBndEdgesL, nNodesL, nStrandsL, nPtsPerStrandL)
end subroutine write_strand_mesh

subroutine write_unstruc_mesh
   use data_mod
   implicit none
   call avm_write_unstruc_mesh(10, &
           xyz, unstruc_hdr%nNodes, &
           triFaces, unstruc_hdr%nTriFaces, &
           quadFaces, unstruc_hdr%nQuadFaces, &
           hexCells, unstruc_hdr%nHexCells, &
           tetCells, unstruc_hdr%nTetCells, &
           priCells, unstruc_hdr%nPriCells, &
           pyrCells, unstruc_hdr%nPyrCells, &
           edges, unstruc_hdr%nEdges, &
           nodesOnGeometry, unstruc_hdr%nNodesOnGeometry, &
           edgesOnGeometry, unstruc_hdr%nEdgesOnGeometry, &
           facesOnGeometry, unstruc_hdr%nFacesOnGeometry, &
           hexGeomIds, tetGeomIds, priGeomIds, pyrGeomIds)

end subroutine write_unstruc_mesh
