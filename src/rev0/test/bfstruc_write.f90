program bfstruc_write
   implicit none
   character*80, parameter :: filename='bfstruc.avm'
   integer, parameter :: n_mesh=3, nPatches=4, n_patch_params=5
   integer, parameter :: jdim=2, kdim=3, ldim=4
   integer :: i_mesh, i_patch, i_patch_param
   integer :: j, k, l, avmid, istat, avmeshRev=0
   character*256 :: str

   real*8 :: value, refPt(3)

   real x(jdim, kdim, ldim), &
        y(jdim, kdim, ldim), &
        z(jdim, kdim, ldim)
   integer iblanks(jdim, kdim, ldim)

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
   call avm_set_strf(avmid, 'meshRevision', '1.0', istat)
   call avm_set_intf(avmid, 'meshCount', n_mesh, istat)
   call avm_set_strf(avmid, 'contactName', 'Josh Calahan', istat)
   call avm_set_strf(avmid, 'contactOrg', 'Aerospace Testing Alliance', istat)
   call avm_set_strf(avmid, 'contactEmail', 'joshua.calahan@arnold.af.mil', istat)
   call avm_set_intf(avmid, 'precision', 1, istat)
   call avm_set_intf(avmid, 'dimensions', 3, istat)
   call avm_set_strf(avmid, 'description', &
                            'An example AVMesh file with some bfstruc grids.', istat)

   do i_mesh=1,n_mesh
      call avm_selectf(avmid, 'mesh', i_mesh, istat)

      write(str, '(A,I0)') 'bfstruc mesh # ', i_mesh

      ! generic mesh fields
      call avm_set_strf(avmid, 'meshName', str, istat)
      call avm_set_strf(avmid, 'meshType', 'bfstruc', istat)
      call avm_set_strf(avmid, 'meshGenerator', 'write_test001', istat)
      call avm_set_strf(avmid, 'changedDate', 'April 22, 2010', istat)
      call avm_set_strf(avmid, 'coordinateSystem', 'xByUzL', istat)
      call avm_set_realf(avmid, 'modelScale', 1.0d0, istat)
      call avm_set_strf(avmid, 'gridUnits', 'ft', istat)
      call avm_set_realf(avmid, 'reynoldsNumber', 5.0d0, istat)
      call avm_set_realf(avmid, 'referenceLength', 1.0d0, istat)
      call avm_set_realf(avmid, 'wallDistance', 1.0d0, istat)
      call avm_set_real_arrayf(avmid, 'referencePoint', refPt, 3, istat)
      call avm_set_strf(avmid, 'referencePointDescription', 'origin', istat)
      call avm_set_realf(avmid, 'periodicity', 1.0d0, istat)
      call avm_set_strf(avmid, 'periodicityDescription', 'full-span', istat)
      call avm_set_intf(avmid, 'refinementLevel', 1, istat)
      call avm_set_strf(avmid, 'description', &
                'A nonsensical bfstruc mesh that just demonstrates the format.', istat)

      ! bfstruc mesh fields
      call avm_set_intf(avmid, 'jmax', jdim, istat)
      call avm_set_intf(avmid, 'kmax', kdim, istat)
      call avm_set_intf(avmid, 'lmax', ldim, istat)
      call avm_set_intf(avmid, 'iblank', 1, istat)
      call avm_set_intf(avmid, 'nPatches', nPatches, istat)
      call avm_set_intf(avmid, 'nPatchIntParams', n_patch_params, istat)
      call avm_set_intf(avmid, 'nPatchR8Params', n_patch_params, istat)
      call avm_set_intf(avmid, 'nPatchC80Params', n_patch_params, istat)

      do i_patch=1,nPatches
         call avm_selectf(avmid, 'patch', i_patch, istat)

         write(str, '(A,I0,A)') 'patch ', i_patch, ' description'

         call avm_set_intf(avmid, 'bctype', i_patch, istat)
         call avm_set_intf(avmid, 'direction', i_patch, istat)
         call avm_set_intf(avmid, 'jbegin', i_patch, istat)
         call avm_set_intf(avmid, 'jend', i_patch, istat)
         call avm_set_intf(avmid, 'kbegin', i_patch, istat)
         call avm_set_intf(avmid, 'kend', i_patch, istat)
         call avm_set_intf(avmid, 'lbegin', i_patch, istat)
         call avm_set_intf(avmid, 'lend', i_patch, istat)
         call avm_set_strf(avmid, 'patch_description', str, istat)

         do i_patch_param=1,n_patch_params
            call avm_selectf(avmid, 'patchParam', i_patch_param, istat)

            write(str, '(A,I0,A,I0,A,I0,A)') &
               'bc character parameter (mesh=', i_mesh, &
               ', bc=', i_patch, ', param=', i_patch_param, ')'

            call avm_set_intf(avmid, 'patch_int_param', i_patch_param, istat)
            value = real(i_patch_param)
            call avm_set_realf(avmid, 'patch_r8_param', value, istat)
            call avm_set_strf(avmid, 'patch_c80_param', str, istat)
         end do
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

 !
 ! write
 ! 
   call avm_write_headersf(avmid, istat) 
   if (istat.ne.0) stop 'ERROR: avm_write_headers'

   do i_mesh=1,n_mesh
      call avm_selectf(avmid, 'mesh', i_mesh, istat)
      call avm_bfstruc_write_xyz_r4f(avmid, x,y,z, jdim*kdim*ldim, istat)
      if (istat.ne.0) stop 'ERROR: avm_bfstruc_write_xyz_r4'
      call avm_bfstruc_write_iblankf(avmid, iblanks, jdim*kdim*ldim, istat)
      if (istat.ne.0) stop 'ERROR: avm_bfstruc_write_iblank'
   end do

   call avm_closef(avmid, istat)
   if (istat.ne.0) stop 'ERROR: avm_close'

   write(6,'(A,A)')  'Successfully wrote: ', filename
end program bfstruc_write
