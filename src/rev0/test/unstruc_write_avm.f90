program unstruc_write
    implicit none
    character*80, parameter :: filename='unstruc.avm'
    integer, parameter :: nMesh=1, nPatches=2, nNodes=12, nTriFaces=9, &
        nQuadFaces=8, nHexCells=1, nTetCells=1, nPriCells=1, nPyrCells=1, &
        nBndTriFaces=8, nBndQuadFaces=6, &
        polyFacesSize=0 !polyFacesSize must be 0 for avmesh rev0
    integer :: i, j, k, n, avmid, istat, avmeshRev=0
    character*256 :: str
    real*8 :: value, refPt(3)
    real, allocatable :: xyz(:,:)
    integer, allocatable :: triFaces(:,:)
    integer, allocatable :: quadFaces(:,:)
    integer, allocatable :: polyFaces(:)
    ! Use this in place of empty polyFaces so no empty array is passed
    integer :: polyFaces_dummy
    character*80 :: write_endian
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

    call avm_selectf(avmid, 'header', 0, istat)
    call avm_set_strf(avmid, 'formatVersion', '1.0', istat)
    call avm_set_strf(avmid, 'meshRevision', '1.0', istat)
    call avm_set_intf(avmid, 'meshCount', nMesh, istat)
    call avm_set_strf(avmid, 'contactName', 'David McDaniel', istat)
    call avm_set_strf(avmid, 'contactOrg', 'UAB', istat)
    call avm_set_strf(avmid, 'contactEmail', 'david.mcdaniel.ctr@eglin.af.mil', istat)
    call avm_set_intf(avmid, 'precision', 1, istat)
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
        call avm_set_strf(avmid, 'changedDate', 'June 15, 2010', istat)
        call avm_set_strf(avmid, 'coordinateSystem', 'xByUzL', istat)
        call avm_set_realf(avmid, 'modelScale', 1.0d0, istat)
        call avm_set_strf(avmid, 'gridUnits', 'in', istat)
        call avm_set_realf(avmid, 'reynoldsNumber', 1.0d0, istat)
        call avm_set_realf(avmid, 'referenceLength', 1.0d0, istat)
        call avm_set_realf(avmid, 'wallDistance', 1.0d0, istat)
        call avm_set_real_arrayf(avmid, 'referencePoint', refPt, 3, istat)
        call avm_set_strf(avmid, 'referencePointDescription', 'origin', istat)
        call avm_set_realf(avmid, 'periodicity', 1.0d0, istat)
        call avm_set_strf(avmid, 'periodicityDescription', 'full-span', istat)
        call avm_set_intf(avmid, 'refinementLevel', 1, istat)
        call avm_set_strf(avmid, 'description', &
                  'A nonsensical unstruc mesh that just demonstrates the format.', istat)

        ! unstruc mesh fields
        call avm_set_intf(avmid, 'fullViscousLayerCount', 0, istat)
        call avm_set_intf(avmid, 'partialViscousLayerCount', 0, istat)
        call avm_set_intf(avmid, 'nNodes', nNodes, istat)
        call avm_set_intf(avmid, 'nEdges', 24, istat)
        call avm_set_intf(avmid, 'nFaces', 17, istat)
        call avm_set_intf(avmid, 'nMaxNodesPerFace', 4, istat)
        call avm_set_intf(avmid, 'nTriFaces', nTriFaces, istat)
        call avm_set_intf(avmid, 'nQuadFaces', nQuadFaces, istat)
        call avm_set_intf(avmid, 'nBndTriFaces', nBndTriFaces, istat)
        call avm_set_intf(avmid, 'nBndQuadFaces', nBndQuadFaces, istat)
        call avm_set_intf(avmid, 'nCells', 4, istat)
        call avm_set_intf(avmid, 'nMaxNodesPerCell', 8, istat)
        call avm_set_intf(avmid, 'nMaxFacesPerCell', 6, istat)
        call avm_set_intf(avmid, 'nHexCells', nHexCells, istat)
        call avm_set_intf(avmid, 'nTetCells', nTetCells, istat)
        call avm_set_intf(avmid, 'nPriCells', nPriCells, istat)
        call avm_set_intf(avmid, 'nPyrCells', nPyrCells, istat)
        call avm_set_intf(avmid, 'nPatches', nPatches, istat)
        call avm_set_intf(avmid, 'nPatchNodes', 12, istat)
        call avm_set_intf(avmid, 'nPatchFaces', 14, istat)

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

    !
    ! write the file data
    !
    call avm_write_headersf(avmid, istat) 
    if (istat.ne.0) stop 'ERROR: avm_write_headers'

    ! mesh data
    polyFaces_dummy = 0
    do i = 1,nMesh
        call avm_selectf(avmid, 'mesh', i, istat)
        call avm_unstruc_write_nodes_r4f(avmid, xyz, 3*nNodes, istat)
        call avm_unstruc_write_facesf(avmid, &
               triFaces,  5*nTriFaces, &
               quadFaces, 6*nQuadFaces, &
               polyFaces_dummy, polyFacesSize, &  !polyfaces must be zero for avmesh rev0
               istat)
        if (istat.ne.0) stop 'ERROR: avm_unstruc_write_r4'
    end do

    call avm_closef(avmid, istat)
    if (istat.ne.0) stop 'ERROR: avm_close'

    deallocate(xyz, triFaces, quadFaces) 

    print '(1X,A)',  'Successfully wrote: ' // trim(filename)
end program
