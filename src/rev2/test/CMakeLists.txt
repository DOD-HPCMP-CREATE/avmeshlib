enable_language (Fortran)
include_directories ("${PROJECT_BINARY_DIR}/src")

# gfortran note..
set (SUPPORTED_COMPILER TRUE)

SET(CMAKE_CXX_FLAGS "-DGTEST_HAS_TR1_TUPLE=0 ${CMAKE_CXX_FLAGS}")

####
# misc tests
####

if (BUILD_UNIT_TESTS)
   add_executable (misc_tests.exe misc_tests.cpp)
   target_link_libraries (misc_tests.exe avmesh_shared
                          gtest gtest_main pthread)
   add_test (misc_tests misc_tests.exe)
endif (BUILD_UNIT_TESTS)

# setup endian write flag for testing
set (WRITE_ENDIAN "native")
if (TEST_BIGENDIAN)
   set (WRITE_ENDIAN "big")
endif (TEST_BIGENDIAN)

####
# unstruc tests
####

# executables
add_executable (rev2_unstruc_write_avm.exe unstruc_write_avm.f90)
target_link_libraries (rev2_unstruc_write_avm.exe avmesh_shared)
set_target_properties (rev2_unstruc_write_avm.exe
PROPERTIES LINKER_LANGUAGE Fortran)

add_executable (rev2_unstruc_write_ftn.exe unstruc_write_ftn.f90)

add_executable (rev2_unstruc_read.exe unstruc_read.f90)
target_link_libraries (rev2_unstruc_read.exe avmesh_shared)
set_target_properties (rev2_unstruc_read.exe
PROPERTIES LINKER_LANGUAGE Fortran)

add_executable (rev2_unstruc_read_bnd.exe unstruc_read_bnd.f90)
target_link_libraries (rev2_unstruc_read_bnd.exe avmesh_shared)
set_target_properties (rev2_unstruc_read_bnd.exe
PROPERTIES LINKER_LANGUAGE Fortran)

add_executable (rev2_unstruc_write_out_of_order.exe unstruc_write_out_of_order.f90)
target_link_libraries (rev2_unstruc_write_out_of_order.exe avmesh_shared)
set_target_properties (rev2_unstruc_write_out_of_order.exe
PROPERTIES LINKER_LANGUAGE Fortran)

add_executable (rev2_unstruc_read_out_of_order.exe unstruc_read_out_of_order.f90)
target_link_libraries (rev2_unstruc_read_out_of_order.exe avmesh_shared)
set_target_properties (rev2_unstruc_read_out_of_order.exe
PROPERTIES LINKER_LANGUAGE Fortran)

if (BUILD_UNIT_TESTS)
   add_executable (rev2_unstruc_tests.exe unstruc_tests.cpp)
   target_link_libraries (rev2_unstruc_tests.exe avmesh_shared
                          gtest gtest_main pthread)
endif (BUILD_UNIT_TESTS)

# pure fortran write tests
if (SUPPORTED_COMPILER)
   add_test (rev2_unstruc_write_with_pure_fortran unstruc_write_ftn.exe)
   set_tests_properties (rev2_unstruc_write_with_pure_fortran
   PROPERTIES PASS_REGULAR_EXPRESSION "Successfully wrote")

   add_test (rev2_unstruc_read_test1 unstruc_read.exe)
   set_tests_properties (rev2_unstruc_read_test1
   PROPERTIES PASS_REGULAR_EXPRESSION "Successfully read"
   DEPENDS rev2_unstruc_write_with_pure_fortran)

   add_test (rev2_unstruc_read_bnd_test1 unstruc_read_bnd.exe)
   set_tests_properties (rev2_unstruc_read_bnd_test1
   PROPERTIES PASS_REGULAR_EXPRESSION "Successfully read"
   DEPENDS rev2_unstruc_write_with_pure_fortran)

   if (BUILD_UNIT_TESTS)
      add_test (rev2_unstruc_tests_with_fortran_written_file unstruc_tests.exe)
      set_tests_properties(rev2_unstruc_tests_with_fortran_written_file
      PROPERTIES DEPENDS rev2_unstruc_write_with_pure_fortran)
   endif (BUILD_UNIT_TESTS)
   if (ENABLE_PYTHON)
      add_test (rev2_unstruc_tests_with_fortran_written_file_w_pythonAPI python unstruc_tests.py)
      set_tests_properties(
      rev2_unstruc_tests_with_fortran_written_file_w_pythonAPI
      PROPERTIES DEPENDS rev2_unstruc_write_with_pure_fortran)
   endif (ENABLE_PYTHON)
endif (SUPPORTED_COMPILER)

# avmeshlib write tests
add_test (rev2_unstruc_write_with_avmeshlib unstruc_write_avm.exe ${WRITE_ENDIAN})
set_tests_properties (rev2_unstruc_write_with_avmeshlib
PROPERTIES PASS_REGULAR_EXPRESSION "Successfully wrote")

if (TEST_NATIVE_FORTRAN_READ)
   add_test (rev2_unstruc_read_test2 unstruc_read.exe)
   set_tests_properties (rev2_unstruc_read_test2
   PROPERTIES PASS_REGULAR_EXPRESSION "Successfully read"
   DEPENDS rev2_unstruc_write_with_avmeshlib)

   add_test (rev2_unstruc_read_bnd_test2 unstruc_read_bnd.exe)
   set_tests_properties (rev2_unstruc_read_bnd_test2
   PROPERTIES PASS_REGULAR_EXPRESSION "Successfully read"
   DEPENDS rev2_unstruc_write_with_avmeshlib)

endif (TEST_NATIVE_FORTRAN_READ)

if (BUILD_UNIT_TESTS)
   add_test (rev2_unstruc_tests_with_avmeshlib_written_file unstruc_tests.exe)
   set_tests_properties(rev2_unstruc_tests_with_avmeshlib_written_file
   PROPERTIES DEPENDS rev2_unstruc_write_with_avmeshlib)
endif (BUILD_UNIT_TESTS)
if (ENABLE_PYTHON)
   add_test (rev2_unstruc_tests_with_avmeshlib_written_file_w_pythonAPI python unstruc_tests.py)
   set_tests_properties(
   rev2_unstruc_tests_with_avmeshlib_written_file_w_pythonAPI
   PROPERTIES DEPENDS rev2_unstruc_write_with_avmeshlib)
endif (ENABLE_PYTHON)

add_test (rev2_unstruc_write_out_of_order unstruc_write_out_of_order.exe ${WRITE_ENDIAN})
set_tests_properties (rev2_unstruc_write_out_of_order
PROPERTIES PASS_REGULAR_EXPRESSION "Successfully wrote")

add_test (rev2_unstruc_read_out_of_order unstruc_read_out_of_order.exe)
set_tests_properties (rev2_unstruc_read_out_of_order
PROPERTIES PASS_REGULAR_EXPRESSION "Successfully read"
DEPENDS rev2_unstruc_write_out_of_order)

####
# strand tests
####

add_executable (rev2_strand_write.exe strand_write.f90)
target_link_libraries (rev2_strand_write.exe avmesh_shared)
set_target_properties (rev2_strand_write.exe
PROPERTIES LINKER_LANGUAGE Fortran)

add_executable (rev2_strand_read.exe strand_read.f90)

add_test (rev2_strand_write_with_avmeshlib strand_write.exe ${WRITE_ENDIAN})
set_tests_properties (rev2_strand_write_with_avmeshlib
PROPERTIES PASS_REGULAR_EXPRESSION "Successfully wrote")

if (TEST_NATIVE_FORTRAN_READ)
   add_test (rev2_strand_read_with_pure_fortran strand_read.exe)
   set_tests_properties (rev2_strand_read_with_pure_fortran
   PROPERTIES PASS_REGULAR_EXPRESSION "Successfully read"
   DEPENDS rev2_strand_write_with_avmeshlib)
endif (TEST_NATIVE_FORTRAN_READ)

if (BUILD_UNIT_TESTS)
   add_executable (rev2_strand_tests.exe strand_tests.cpp)
   target_link_libraries (rev2_strand_tests.exe avmesh_shared
                          gtest gtest_main pthread)
   add_test (rev2_strand_tests strand_tests.exe)
   set_tests_properties(rev2_strand_tests
   PROPERTIES DEPENDS rev2_strand_write_with_avmeshlib)
endif (BUILD_UNIT_TESTS)

####
# mixed mesh tests
####

if (SUPPORTED_COMPILER)
   add_executable (rev2_mixed_write.exe mixed_write.f90)
   add_test (rev2_mixed_write_with_pure_fortran mixed_write.exe)
   target_link_libraries (rev2_mixed_write.exe avmesh_shared)
   set_target_properties (rev2_mixed_write.exe
   PROPERTIES LINKER_LANGUAGE Fortran)
   set_tests_properties (rev2_mixed_write_with_pure_fortran
   PROPERTIES PASS_REGULAR_EXPRESSION "Successfully wrote")

   if (BUILD_UNIT_TESTS)
      add_executable (rev2_mixed_tests.exe mixed_tests.cpp)
      target_link_libraries (rev2_mixed_tests.exe avmesh_shared
                             gtest gtest_main pthread)
      add_test (rev2_mixed_tests mixed_tests.exe)
      set_tests_properties(rev2_mixed_tests
      PROPERTIES DEPENDS rev2_mixed_write_with_pure_fortran)
   endif (BUILD_UNIT_TESTS)

   if (ENABLE_PYTHON)
      add_test (rev2_mixed_tests.py python mixed_tests.py)
      set_tests_properties(rev2_mixed_tests.py
      PROPERTIES DEPENDS rev2_mixed_write_with_pure_fortran)
   endif (ENABLE_PYTHON)
endif (SUPPORTED_COMPILER)

####
# examples
####

# these tests depend on mixed.avm, so don't run them
# if we dont construct that file

add_executable (rev2_example_reader_c.exe example_reader.c)
target_link_libraries (rev2_example_reader_c.exe avmesh_shared)
if (SUPPORTED_COMPILER)
add_test (rev2_example_reader.c example_reader_c.exe)
set_tests_properties (rev2_example_reader.c
PROPERTIES PASS_REGULAR_EXPRESSION
"Mesh Count is 2"
DEPENDS rev2_mixed_write_with_pure_fortran)
endif (SUPPORTED_COMPILER)

add_executable (rev2_example_reader_f90.exe example_reader.f90)
target_link_libraries (rev2_example_reader_f90.exe avmesh_shared)
set_target_properties (rev2_example_reader_f90.exe
PROPERTIES LINKER_LANGUAGE Fortran)
if (SUPPORTED_COMPILER)
add_test (rev2_example_reader.f90 example_reader_f90.exe)
set_tests_properties (rev2_example_reader.f90
PROPERTIES PASS_REGULAR_EXPRESSION
"Mesh Count is 2"
DEPENDS rev2_mixed_write_with_pure_fortran)
endif (SUPPORTED_COMPILER)

if (SUPPORTED_COMPILER)
if (ENABLE_PYTHON)
   add_test (rev2_example_reader.py python example_reader.py)
   set_tests_properties (rev2_example_reader.py
   PROPERTIES PASS_REGULAR_EXPRESSION
   "Mesh Count is 2")
endif (ENABLE_PYTHON)
endif (SUPPORTED_COMPILER)

add_executable (rev2_example_offset.exe example_offset.c)
target_link_libraries (rev2_example_offset.exe avmesh_shared)
if (SUPPORTED_COMPILER)
add_test (rev2_example_offset.c example_offset.exe mixed.avm)
set_tests_properties (rev2_example_offset.c
PROPERTIES PASS_REGULAR_EXPRESSION
"File size is 13754"
DEPENDS rev2_mixed_write_with_pure_fortran)
endif (SUPPORTED_COMPILER)

add_executable (rev2_print_descriptions.exe print_descriptions.cpp)
target_link_libraries (rev2_print_descriptions.exe avmesh_shared)
