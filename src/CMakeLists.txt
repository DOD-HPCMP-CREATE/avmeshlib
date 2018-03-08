enable_language (Fortran)

set(CPP_SRCS avmesh.cpp byte_swap.c avmesh_file.cpp)
set(F90_SRCS avmesh.f90)

add_subdirectory (rev0)
add_subdirectory (rev1)
add_subdirectory (rev2)

add_library (avmesh_obj OBJECT ${CPP_SRCS} ${F90_SRCS})
set_property (TARGET avmesh_obj PROPERTY POSITION_INDEPENDENT_CODE 1)

add_library (avmesh_shared SHARED $<TARGET_OBJECTS:avmesh_obj>)
add_library (avmesh_static STATIC $<TARGET_OBJECTS:avmesh_obj>)

install (TARGETS avmesh_shared DESTINATION lib)
install (TARGETS avmesh_static DESTINATION lib)
install (FILES avmesh.h DESTINATION include)

if (ENABLE_PYTHON)
   swig_add_module (avmeshlib python avmeshlib.i ${CPP_SRCS})
   swig_link_libraries (avmeshlib ${PYTHON_LIBRARIES})
   add_custom_command(TARGET avmesh_shared
                      COMMAND ${PYTHON_EXECUTABLE} -O -m compileall -f -q ${CMAKE_CURRENT_BINARY_DIR}/avmeshlib.py)
   add_custom_command(TARGET avmesh_shared
                      COMMAND ${PYTHON_EXECUTABLE} -m compileall -f -q ${CMAKE_CURRENT_BINARY_DIR}/avmeshlib.py)
   install (FILES avmeshlib.py _avmeshlib.so DESTINATION python)
   install (FILES avmeshlib.pyc avmeshlib.pyo DESTINATION python OPTIONAL)
endif (ENABLE_PYTHON)

