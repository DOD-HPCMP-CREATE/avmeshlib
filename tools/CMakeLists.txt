enable_language (Fortran)

add_executable (over2avmesh over2avmesh.f90)
target_link_libraries (over2avmesh avmesh_shared)
set_target_properties (over2avmesh PROPERTIES LINKER_LANGUAGE Fortran)

add_executable (avmesh2fvp3d avmesh2fvp3d.f90)
target_link_libraries (avmesh2fvp3d avmesh_shared)
set_target_properties (avmesh2fvp3d PROPERTIES LINKER_LANGUAGE Fortran)

install (TARGETS over2avmesh DESTINATION bin)
install (TARGETS avmesh2fvp3d DESTINATION bin)
