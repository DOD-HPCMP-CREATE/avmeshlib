cmake_minimum_required (VERSION 3.12)
project (avmeshlib)

if(POLICY CMP0078)
    # CMP0078: ``UseSWIG`` generated standard target name.
    # cmake 3.13.5 warns if this policy is not set, and defaults to OLD.
    cmake_policy(SET CMP0078 NEW)
endif()

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
endif (ENABLE_FORTRAN)

if (EXISTS "${GTEST_DIR}")
   SET (BUILD_UNIT_TESTS YES)
   include_directories ("${GTEST_DIR}/include")
   link_directories ("${GTEST_DIR}/lib64")
endif (EXISTS "${GTEST_DIR}")

if (ENABLE_PYTHON)
   find_package (SWIG REQUIRED)
   include (UseSWIG)
   find_package (Python 3 COMPONENTS Interpreter Development REQUIRED)
   set(CMAKE_SWIG_FLAGS -py3)
   include_directories (${Python_INCLUDE_DIRS})
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
