cmake_minimum_required (VERSION 2.8)
project (avmeshlib)

option (ENABLE_FORTRAN "Enable Fortran API. Requires Fortran compiler" OFF)
option (ENABLE_PYTHON "Enable Python API." OFF)
option (ENABLE_SHARED_LIBRARY
        "Enabling building AVMESH as a shared library"
        OFF)

if (ENABLE_FORTRAN)
    option (ENABLE_TESTS "Enable building of tests." ON)
endif (ENABLE_FORTRAN)

if (NOT ENABLE_SHARED_LIBRARY)
    set(AVMESH_BUILD_STATIC "true")
endif (NOT ENABLE_SHARED_LIBRARY)

if (ENABLE_FORTRAN)
    enable_language (Fortran)
    enable_testing ()
    set(TEST_NATIVE_FORTRAN_READ YES)
endif (ENABLE_FORTRAN)

if (EXISTS "${GTEST_DIR}")
   SET (BUILD_UNIT_TESTS YES)
   include_directories ("${GTEST_DIR}/include")
   link_directories ("${GTEST_DIR}/lib")
endif (EXISTS "${GTEST_DIR}")

if (ENABLE_PYTHON)
   find_package (SWIG REQUIRED)
   include (UseSWIG)
   include (FindPythonInterp)
   include (FindPythonLibs)
   include_directories (${PYTHON_INCLUDE_DIRS})
endif (ENABLE_PYTHON)

IF(COMPILER_FAMILY STREQUAL "intel")
   SET(CMAKE_Fortran_FLAGS "-DINTEL -fpe0 -traceback -fPIC -O3 -fpp -align sequence -DPP_GRAVITY -DPP_DOUBLE -DPP_GRIDDEFORM -recursive -heap-arrays 10 -vec-report0 -diag-disable 8291,8293")
ELSEIF(CMPILER_FAMILY STREQUAL "gnu")
   SET(CMAKE_Fortran_FLAGS "-ffree-line-length-none -frounding-math")
ENDIF()


#add_subdirectory (doc)
add_subdirectory (src)
# No longer build mesh conversion tools (KES-2253)
#add_subdirectory (tools)
