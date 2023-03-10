if (ENABLE_FORTRAN)
   enable_language (Fortran)
   set(F90_SRCS avmesh.f90)
   include_directories ("${CMAKE_CURRENT_BINARY_DIR}")
endif (ENABLE_FORTRAN)

set(CPP_SRCS avmesh.cpp byte_swap.c avmesh_file.cpp)

include_directories ("${CMAKE_CURRENT_SOURCE_DIR}")

add_subdirectory (rev0)
add_subdirectory (rev1)
add_subdirectory (rev2)

add_library (avmesh_obj OBJECT ${CPP_SRCS} ${F90_SRCS})
set_property (TARGET avmesh_obj PROPERTY POSITION_INDEPENDENT_CODE 1)

if (ENABLE_SHARED_LIBRARY)
   add_library (avmesh_shared SHARED $<TARGET_OBJECTS:avmesh_obj>)
   install (TARGETS avmesh_shared DESTINATION lib)
endif (ENABLE_SHARED_LIBRARY)
add_library (avmesh_static STATIC $<TARGET_OBJECTS:avmesh_obj>)

install (TARGETS avmesh_static DESTINATION lib)
install (FILES avmesh.h DESTINATION include)

if (ENABLE_PYTHON)
   swig_add_library (avmeshlib LANGUAGE python SOURCES avmeshlib.i ${CPP_SRCS})
   target_link_libraries (avmeshlib ${Python_LIBRARIES})
   add_custom_command(TARGET avmesh_shared
                      COMMAND ${Python_EXECUTABLE} -O -m compileall -f -q ${CMAKE_CURRENT_BINARY_DIR}/avmeshlib.py)
   add_custom_command(TARGET avmesh_shared
                      COMMAND ${Python_EXECUTABLE} -m compileall -f -q ${CMAKE_CURRENT_BINARY_DIR}/avmeshlib.py)
   install (FILES avmeshlib.py _avmeshlib.so DESTINATION python)
   install (FILES avmeshlib.pyc avmeshlib.pyo DESTINATION python OPTIONAL)
endif (ENABLE_PYTHON)

