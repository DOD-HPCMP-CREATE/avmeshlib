program unstruc_write
    use avmesh

    implicit none
    character*80, parameter :: filename='unstruc.avm'
    integer, parameter :: nMesh=1, nPatches=2, nNodes=12, nTriFaces=9, &
        nQuadFaces=8, nHexCells=1, nTetCells=2, nPriCells=1, nPyrCells=1, &
        nBndTriFaces=8, nBndQuadFaces=6, &
        nEdges=12, &
        nNodesOnGeometry=4,nEdgesOnGeometry=1,nFacesOnGeometry=2, geomRegionId=-1, &
        avmeshRev=2
    integer :: i, j, avmid, istat
    character*256 :: str
    real*8 :: refPt(3)
    real*8 :: refLength(3)
    real*8, allocatable :: xyz(:,:)
    integer, allocatable :: triFaces(:,:)
    integer, allocatable :: quadFaces(:,:)
    integer, allocatable :: hexCells(:,:)
    integer, allocatable :: tetCells(:,:)
    integer, allocatable :: priCells(:,:)
    integer, allocatable :: pyrCells(:,:)
    integer, allocatable :: edges(:,:)
    character*80 :: write_endian
    type(AVM_AMR_Node_Data), allocatable :: nodesOnGeometry(:)
    integer, allocatable :: edgesOnGeometry(:,:)
    integer, allocatable :: facesOnGeometry(:,:)
    integer, allocatable :: hexGeomIds(:)
    integer, allocatable :: tetGeomIds(:)
    integer, allocatable :: priGeomIds(:)
    integer, allocatable :: pyrGeomIds(:)

    write_endian = "native"
    if (command_argument_count() > 0) then
        call get_command_argument(1, write_endian)
    endif

 !
 ! initialization
 !
    call avm_new_filef(avmid, filename, avmeshRev, istat)
    if (istat.ne.0) stop 'ERROR: avm_new_file'

    call avm_set_strf(avmid, 'AVM_WRITE_ENDIAN', write_endian, istat)
    if (istat.ne.0) stop 'ERROR: avm_set_str("AVM_WRITE_ENDIAN")'

    refPt(:) = (/0.0,0.0,0.0/)
    refLength(:) = (/1.0,1.0,1.0/)

    call avm_selectf(avmid, 'header', 0, istat)
    call avm_set_intf(avmid, 'meshCount', nMesh, istat)
    call avm_set_strf(avmid, 'contactInfo', 'David McDaniel', istat)
    call avm_set_intf(avmid, 'precision', 2, istat)
    call avm_set_intf(avmid, 'dimensions', 3, istat)
    call avm_set_strf(avmid, 'description', &
                            'An example AVMesh file with some unstruc grids.', istat)

    do i = 1,nMesh
        call avm_selectf(avmid, 'mesh', i, istat)

        write(str, '(A,I0)') 'unstruc mesh # ', i

        ! generic mesh fields
        call avm_set_strf(avmid, 'meshName', str, istat)
        call avm_set_strf(avmid, 'meshType', 'unstruc', istat)
        call avm_set_strf(avmid, 'meshGenerator', 'unstruc_write', istat)
        call avm_set_strf(avmid, 'coordinateSystem', 'xByUzL', istat)
        call avm_set_realf(avmid, 'modelScale', 1.0d0, istat)
        call avm_set_strf(avmid, 'gridUnits', 'in', istat)
        call avm_set_real_arrayf(avmid, 'referenceLength', refLength, 3, istat)
        call avm_set_realf(avmid, 'referenceArea', 1.0d0, istat)
        call avm_set_real_arrayf(avmid, 'referencePoint', refPt, 3, istat)
        call avm_set_strf(avmid, 'referencePointDescription', 'origin', istat)
        call avm_set_strf(avmid, 'meshDescription', &
                  'A nonsensical unstruc mesh that just demonstrates the format.', istat)

        ! unstruc mesh fields
        call avm_set_intf(avmid, 'nNodes', nNodes, istat)
        call avm_set_intf(avmid, 'nFaces', 17, istat)
        call avm_set_intf(avmid, 'nCells', 4, istat)
        call avm_set_intf(avmid, 'nMaxNodesPerFace', 4, istat)
        call avm_set_intf(avmid, 'nMaxNodesPerCell', 8, istat)
        call avm_set_intf(avmid, 'nMaxFacesPerCell', 6, istat)
        call avm_set_intf(avmid, 'nPatches', nPatches, istat)
        call avm_set_intf(avmid, 'nHexCells', nHexCells, istat)
        call avm_set_intf(avmid, 'nTetCells', nTetCells, istat)
        call avm_set_intf(avmid, 'nPriCells', nPriCells, istat)
        call avm_set_intf(avmid, 'nPyrCells', nPyrCells, istat)
        call avm_set_intf(avmid, 'nBndTriFaces', nBndTriFaces, istat)
        call avm_set_intf(avmid, 'nTriFaces', nTriFaces, istat)
        call avm_set_intf(avmid, 'nBndQuadFaces', nBndQuadFaces, istat)
        call avm_set_intf(avmid, 'nQuadFaces', nQuadFaces, istat)
        call avm_set_intf(avmid, 'nEdges', nEdges, istat)
        call avm_set_intf(avmid, 'nNodesOnGeometry', nNodesOnGeometry, istat)
        call avm_set_intf(avmid, 'nEdgesOnGeometry', nEdgesOnGeometry, istat)
        call avm_set_intf(avmid, 'nFacesOnGeometry', nFacesOnGeometry, istat)
        call avm_set_intf(avmid, 'geomRegionId', geomRegionId, istat)

        do j = 1,nPatches
            call avm_selectf(avmid, 'patch', j, istat)
            write(str,'(A,I0,A)') 'patch ',j,' label'
            call avm_set_strf(avmid, 'patchLabel', str, istat)
            write(str,'(A,I0,A)') 'patch ',j,' bctype'
            call avm_set_strf(avmid, 'patchType', str, istat)
            call avm_set_intf(avmid, 'patchId', -j, istat)
        end do

    end do

    !
    ! unstruc mesh data initialization
    !
    allocate(xyz(3,nNodes), &
             quadFaces(5,nQuadFaces), &
             triFaces(4,nTriFaces), &
             hexCells(8, nHexCells), &
             tetCells(4, nTetCells), &
             priCells(6, nPriCells), &
             pyrCells(5, nPyrCells), &
             edges(2, nEdges), &
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
    nodesOnGeometry(1)%u = 0.0
    nodesOnGeometry(1)%v = 0.0
    nodesOnGeometry(2)%nodeIndex = 5
    nodesOnGeometry(2)%geomType = 1
    nodesOnGeometry(2)%geomTopoId = 3
    nodesOnGeometry(2)%u = 0.5
    nodesOnGeometry(2)%v = 0.0
    nodesOnGeometry(3)%nodeIndex = 11
    nodesOnGeometry(3)%geomType = 2
    nodesOnGeometry(3)%geomTopoId = 5
    nodesOnGeometry(3)%u = 0.5
    nodesOnGeometry(3)%v = 0.7
    nodesOnGeometry(4)%nodeIndex = 12
    nodesOnGeometry(4)%geomType = 1
    nodesOnGeometry(4)%geomTopoId = 7
    nodesOnGeometry(4)%u = 0.6
    nodesOnGeometry(4)%v = 0.0

    edgesOnGeometry(:,1) = (/4, 1, 8/)

    facesOnGeometry(:,1) = (/11, 2, 12/)
    facesOnGeometry(:,2) = (/14, 2, 14/)

    hexGeomIds(1) = 1
    tetGeomIds(1) = 2
    priGeomIds(1) = 7
    pyrGeomIds(1) = 8

    !
    ! write the file data
    !
    call avm_write_headersf(avmid, istat)
    if (istat.ne.0) stop 'ERROR: avm_write_headers'

    ! mesh data
    do i = 1,nMesh
        call avm_selectf(avmid, 'mesh', i, istat)
        call avm_unstruc_write_amr_volumeidsf(avmid, &
               hexGeomIds, nHexCells, &
               tetGeomIds, nTetCells, &
               priGeomIds, nPriCells, &
               pyrGeomIds, nPyrCells, &
               istat)
        if (istat.ne.0) stop 'ERROR: avm_unstruc_write_amr_volumeids'
        ! faces MUST be written before amr so face reordering happens
        call avm_unstruc_write_facesf(avmid, &
               triFaces,  4*nTriFaces, &
               quadFaces, 5*nQuadFaces, &
               istat)
        if (istat.ne.0) stop 'ERROR: avm_unstruc_write_faces'
        call avm_unstruc_write_amrf(avmid, &
               nodesOnGeometry, nNodesOnGeometry, &
               edgesOnGeometry, 3*nEdgesOnGeometry, &
               facesOnGeometry, 3*nFacesOnGeometry, &
               istat)
        if (istat.ne.0) stop 'ERROR: avm_unstruc_write_amr'
        call avm_unstruc_write_edgesf(avmid, &
               edges, 2*nEdges, &
               istat)
        if (istat.ne.0) stop 'ERROR: avm_unstruc_write_edges'
        call avm_unstruc_write_cellsf(avmid, &
               hexCells, 8*nHexCells, &
               tetCells, 4*nTetCells, &
               priCells, 6*nPriCells, &
               pyrCells, 5*nPyrCells, &
               istat)
        if (istat.ne.0) stop 'ERROR: avm_unstruc_write_cells'
        call avm_unstruc_write_nodes_r8f(avmid, xyz, 3*nNodes, istat)
        if (istat.ne.0) stop 'ERROR: avm_unstruc_write_r8'
    end do

    call avm_closef(avmid, istat)
    if (istat.ne.0) stop 'ERROR: avm_close'

    deallocate(xyz, triFaces, quadFaces)
    deallocate(hexCells, tetCells, priCells, pyrCells)
    deallocate(edges, nodesOnGeometry, edgesOnGeometry, facesOnGeometry)
    deallocate(hexGeomIds, tetGeomIds, priGeomIds, pyrGeomIds)

    print '(1X,A)',  'Successfully wrote: ' // trim(filename)
end program
