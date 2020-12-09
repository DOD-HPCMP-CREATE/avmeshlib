enable_language (Fortran)

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
# bfstruc tests
####

add_executable (bfstruc_write.exe bfstruc_write.f90)
target_link_libraries (bfstruc_write.exe avmesh_shared)
set_target_properties (bfstruc_write.exe
PROPERTIES LINKER_LANGUAGE Fortran)

add_executable (bfstruc_read.exe bfstruc_read.f90)

add_test (bfstruc_write_with_avmeshlib bfstruc_write.exe ${WRITE_ENDIAN})
set_tests_properties (bfstruc_write_with_avmeshlib
PROPERTIES PASS_REGULAR_EXPRESSION "Successfully wrote")

if (TEST_NATIVE_FORTRAN_READ)
   add_test (bfstruc_read_with_pure_fortran bfstruc_read.exe)
   set_tests_properties (bfstruc_read_with_pure_fortran
   PROPERTIES PASS_REGULAR_EXPRESSION "Successfully read"
   DEPENDS bfstruc_write_with_avmeshlib)
endif (TEST_NATIVE_FORTRAN_READ)

if (BUILD_UNIT_TESTS)
   add_executable (bfstruc_tests.exe bfstruc_tests.cpp)
   target_link_libraries (bfstruc_tests.exe avmesh_shared
                          gtest gtest_main pthread)
   add_test (bfstruc_tests bfstruc_tests.exe)
   set_tests_properties(bfstruc_tests
   PROPERTIES DEPENDS bfstruc_write_with_avmeshlib)
endif (BUILD_UNIT_TESTS)

####
# unstruc tests
####

# executables
add_executable (unstruc_write_avm.exe unstruc_write_avm.f90)
target_link_libraries (unstruc_write_avm.exe avmesh_shared)
set_target_properties (unstruc_write_avm.exe
PROPERTIES LINKER_LANGUAGE Fortran)

add_executable (unstruc_write_ftn.exe unstruc_write_ftn.f90)

add_executable (unstruc_read.exe unstruc_read.f90)
target_link_libraries (unstruc_read.exe avmesh_shared)
set_target_properties (unstruc_read.exe
PROPERTIES LINKER_LANGUAGE Fortran)

add_executable (unstruc_read_bnd.exe unstruc_read_bnd.f90)
target_link_libraries (unstruc_read_bnd.exe avmesh_shared)
set_target_properties (unstruc_read_bnd.exe
PROPERTIES LINKER_LANGUAGE Fortran)

add_executable (unstruc_write_out_of_order.exe unstruc_write_out_of_order.f90)
target_link_libraries (unstruc_write_out_of_order.exe avmesh_shared)
set_target_properties (unstruc_write_out_of_order.exe
PROPERTIES LINKER_LANGUAGE Fortran)

add_executable (unstruc_read_out_of_order.exe unstruc_read_out_of_order.f90)
target_link_libraries (unstruc_read_out_of_order.exe avmesh_shared)
set_target_properties (unstruc_read_out_of_order.exe
PROPERTIES LINKER_LANGUAGE Fortran)

if (BUILD_UNIT_TESTS)
   add_executable (unstruc_tests.exe unstruc_tests.cpp)
   target_link_libraries (unstruc_tests.exe avmesh_shared
                          gtest gtest_main pthread)
endif (BUILD_UNIT_TESTS)

# pure fortran write tests
if (SUPPORTED_COMPILER)
   add_test (unstruc_write_with_pure_fortran unstruc_write_ftn.exe)
   set_tests_properties (unstruc_write_with_pure_fortran
   PROPERTIES PASS_REGULAR_EXPRESSION "Successfully wrote")

   add_test (unstruc_read_test1 unstruc_read.exe)
   set_tests_properties (unstruc_read_test1
   PROPERTIES PASS_REGULAR_EXPRESSION "Successfully read"
   DEPENDS unstruc_write_with_pure_fortran)

   add_test (unstruc_read_bnd_test1 unstruc_read_bnd.exe)
   set_tests_properties (unstruc_read_bnd_test1
   PROPERTIES PASS_REGULAR_EXPRESSION "Successfully read"
   DEPENDS unstruc_write_with_pure_fortran)

   if (BUILD_UNIT_TESTS)
      add_test (unstruc_tests_with_fortran_written_file unstruc_tests.exe)
      set_tests_properties(unstruc_tests_with_fortran_written_file
      PROPERTIES DEPENDS unstruc_write_with_pure_fortran)
   endif (BUILD_UNIT_TESTS)
   if (ENABLE_PYTHON)
      add_test (unstruc_tests_with_fortran_written_file_w_pythonAPI python3 unstruc_tests.py)
      set_tests_properties(
      unstruc_tests_with_fortran_written_file_w_pythonAPI
      PROPERTIES DEPENDS unstruc_write_with_pure_fortran)
   endif (ENABLE_PYTHON)
endif (SUPPORTED_COMPILER)

# avmeshlib write tests
add_test (unstruc_write_with_avmeshlib unstruc_write_avm.exe ${WRITE_ENDIAN})
set_tests_properties (unstruc_write_with_avmeshlib
PROPERTIES PASS_REGULAR_EXPRESSION "Successfully wrote")

if (TEST_NATIVE_FORTRAN_READ)
   add_test (unstruc_read_test2 unstruc_read.exe)
   set_tests_properties (unstruc_read_test2
   PROPERTIES PASS_REGULAR_EXPRESSION "Successfully read"
   DEPENDS unstruc_write_with_avmeshlib)

   add_test (unstruc_read_bnd_test2 unstruc_read_bnd.exe)
   set_tests_properties (unstruc_read_bnd_test2
   PROPERTIES PASS_REGULAR_EXPRESSION "Successfully read"
   DEPENDS unstruc_write_with_avmeshlib)

endif (TEST_NATIVE_FORTRAN_READ)

if (BUILD_UNIT_TESTS)
   add_test (unstruc_tests_with_avmeshlib_written_file unstruc_tests.exe)
   set_tests_properties(unstruc_tests_with_avmeshlib_written_file
   PROPERTIES DEPENDS unstruc_write_with_avmeshlib)
endif (BUILD_UNIT_TESTS)
if (ENABLE_PYTHON)
   add_test (unstruc_tests_with_avmeshlib_written_file_w_pythonAPI python3 unstruc_tests.py)
   set_tests_properties(
   unstruc_tests_with_avmeshlib_written_file_w_pythonAPI
   PROPERTIES DEPENDS unstruc_write_with_avmeshlib)
endif (ENABLE_PYTHON)

add_test (unstruc_write_out_of_order unstruc_write_out_of_order.exe ${WRITE_ENDIAN})
set_tests_properties (unstruc_write_out_of_order
PROPERTIES PASS_REGULAR_EXPRESSION "Successfully wrote")

add_test (unstruc_read_out_of_order unstruc_read_out_of_order.exe)
set_tests_properties (unstruc_read_out_of_order
PROPERTIES PASS_REGULAR_EXPRESSION "Successfully read"
DEPENDS unstruc_write_out_of_order)

####
# strand tests
####

add_executable (strand_write.exe strand_write.f90)
target_link_libraries (strand_write.exe avmesh_shared)
set_target_properties (strand_write.exe
PROPERTIES LINKER_LANGUAGE Fortran)

add_executable (strand_read.exe strand_read.f90)

add_test (strand_write_with_avmeshlib strand_write.exe ${WRITE_ENDIAN})
set_tests_properties (strand_write_with_avmeshlib
PROPERTIES PASS_REGULAR_EXPRESSION "Successfully wrote")

if (TEST_NATIVE_FORTRAN_READ)
   add_test (strand_read_with_pure_fortran strand_read.exe)
   set_tests_properties (strand_read_with_pure_fortran
   PROPERTIES PASS_REGULAR_EXPRESSION "Successfully read"
   DEPENDS strand_write_with_avmeshlib)
endif (TEST_NATIVE_FORTRAN_READ)

if (BUILD_UNIT_TESTS)
   add_executable (strand_tests.exe strand_tests.cpp)
   target_link_libraries (strand_tests.exe avmesh_shared
                          gtest gtest_main pthread)
   add_test (strand_tests strand_tests.exe)
   set_tests_properties(strand_tests
   PROPERTIES DEPENDS strand_write_with_avmeshlib)
endif (BUILD_UNIT_TESTS)

####
# cart tests
####

add_executable (cart_write.exe cart_write.f90)
target_link_libraries (cart_write.exe avmesh_shared)
set_target_properties (cart_write.exe
PROPERTIES LINKER_LANGUAGE Fortran)

add_executable (cart_read.exe cart_read.f90)

add_test (cart_write_with_avmeshlib cart_write.exe ${WRITE_ENDIAN})
set_tests_properties (cart_write_with_avmeshlib
PROPERTIES PASS_REGULAR_EXPRESSION "Successfully wrote")

if (TEST_NATIVE_FORTRAN_READ)
   add_test (cart_read_with_pure_fortran cart_read.exe)
   set_tests_properties (cart_read_with_pure_fortran
   PROPERTIES PASS_REGULAR_EXPRESSION "Successfully read"
   DEPENDS cart_write_with_avmeshlib)
endif (TEST_NATIVE_FORTRAN_READ)

if (BUILD_UNIT_TESTS)
   add_executable (cart_tests.exe cart_tests.cpp)
   target_link_libraries (cart_tests.exe avmesh_shared
                          gtest gtest_main pthread)
   add_test (cart_tests cart_tests.exe)
   set_tests_properties(cart_tests
   PROPERTIES DEPENDS cart_write_with_avmeshlib)
endif (BUILD_UNIT_TESTS)

####
# mixed mesh tests
####

if (SUPPORTED_COMPILER)
   add_executable (mixed_write.exe mixed_write.f90)
   add_test (mixed_write_with_pure_fortran mixed_write.exe)
   target_link_libraries (mixed_write.exe avmesh_shared)
   set_target_properties (mixed_write.exe
   PROPERTIES LINKER_LANGUAGE Fortran)
   set_tests_properties (mixed_write_with_pure_fortran
   PROPERTIES PASS_REGULAR_EXPRESSION "Successfully wrote")

   if (BUILD_UNIT_TESTS)
      add_executable (mixed_tests.exe mixed_tests.cpp)
      target_link_libraries (mixed_tests.exe avmesh_shared
                             gtest gtest_main pthread)
      add_test (mixed_tests mixed_tests.exe)
      set_tests_properties(mixed_tests
      PROPERTIES DEPENDS mixed_write_with_pure_fortran)
   endif (BUILD_UNIT_TESTS)

   if (ENABLE_PYTHON)
      add_test (mixed_tests.py python3 mixed_tests.py)
      set_tests_properties(mixed_tests.py
      PROPERTIES DEPENDS mixed_write_with_pure_fortran)
   endif (ENABLE_PYTHON)
endif (SUPPORTED_COMPILER)

####
# examples
####

# these tests depend on mixed.avm, so don't run them
# if we dont construct that file

add_executable (example_reader_c.exe example_reader.c)
target_link_libraries (example_reader_c.exe avmesh_shared)
if (SUPPORTED_COMPILER)
add_test (example_reader.c example_reader_c.exe)
set_tests_properties (example_reader.c
PROPERTIES PASS_REGULAR_EXPRESSION
"Mesh Count is 4"
DEPENDS mixed_write_with_pure_fortran)
endif (SUPPORTED_COMPILER)

add_executable (example_reader_f90.exe example_reader.f90)
target_link_libraries (example_reader_f90.exe avmesh_shared)
set_target_properties (example_reader_f90.exe
PROPERTIES LINKER_LANGUAGE Fortran)
if (SUPPORTED_COMPILER)
add_test (example_reader.f90 example_reader_f90.exe)
set_tests_properties (example_reader.f90
PROPERTIES PASS_REGULAR_EXPRESSION
"Mesh Count is 4"
DEPENDS mixed_write_with_pure_fortran)
endif (SUPPORTED_COMPILER)

if (SUPPORTED_COMPILER)
if (ENABLE_PYTHON)
   add_test (example_reader.py python3 example_reader.py)
   set_tests_properties (example_reader.py
   PROPERTIES PASS_REGULAR_EXPRESSION
   "Mesh Count is 4")
endif (ENABLE_PYTHON)
endif (SUPPORTED_COMPILER)

add_executable (example_offset.exe example_offset.c)
target_link_libraries (example_offset.exe avmesh_shared)
if (SUPPORTED_COMPILER)
add_test (example_offset.c example_offset.exe mixed.avm)
set_tests_properties (example_offset.c
PROPERTIES PASS_REGULAR_EXPRESSION
"File size is 13754"
DEPENDS mixed_write_with_pure_fortran)
endif (SUPPORTED_COMPILER)

add_executable (print_descriptions.exe print_descriptions.cpp)
target_link_libraries (print_descriptions.exe avmesh_shared)
