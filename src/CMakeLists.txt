enable_language (Fortran)

set(CPP_SRCS avmesh.cpp byte_swap.c avmesh_file.cpp)
set(F90_SRCS avmesh.f90)

add_subdirectory (rev0)
add_subdirectory (rev1)
add_subdirectory (rev2)

add_library (avmesh_shared SHARED ${CPP_SRCS} ${F90_SRCS})
add_library (avmesh_static STATIC ${CPP_SRCS} ${F90_SRCS})

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
   install (FILES avmeshlib.py avmeshlib.pyc avmeshlib.pyo _avmeshlib.so DESTINATION python)
endif (ENABLE_PYTHON)

