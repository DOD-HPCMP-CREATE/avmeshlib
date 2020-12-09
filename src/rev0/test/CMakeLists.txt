enable_language (Fortran)

# gfortran note..
# Required gfortran > 4.3 
set (SUPPORTED_COMPILER TRUE)

SET(CMAKE_CXX_FLAGS "-DGTEST_HAS_TR1_TUPLE=0 ${CMAKE_CXX_FLAGS}")

####
# misc tests
####

if (BUILD_UNIT_TESTS)
   add_executable (rev0_misc_tests.exe misc_tests.cpp)
   target_link_libraries (rev0_misc_tests.exe avmesh_shared
                          gtest gtest_main pthread)
   add_test (rev0_misc_tests rev0_misc_tests.exe)
endif (BUILD_UNIT_TESTS)

# setup endian write flag for testing
set (WRITE_ENDIAN "native")
if (TEST_BIGENDIAN)
   set (WRITE_ENDIAN "big")
endif (TEST_BIGENDIAN)

####
# bfstruc tests
####

add_executable (rev0_bfstruc_write.exe bfstruc_write.f90)
target_link_libraries (rev0_bfstruc_write.exe avmesh_shared)
set_target_properties (rev0_bfstruc_write.exe
PROPERTIES LINKER_LANGUAGE Fortran)

add_executable (rev0_bfstruc_read.exe bfstruc_read.f90)

add_test (rev0_bfstruc_write_with_avmeshlib rev0_bfstruc_write.exe ${WRITE_ENDIAN})
set_tests_properties (rev0_bfstruc_write_with_avmeshlib
PROPERTIES PASS_REGULAR_EXPRESSION "Successfully wrote")

if (TEST_NATIVE_FORTRAN_READ)
   add_test (rev0_bfstruc_read_with_pure_fortran rev0_bfstruc_read.exe)
   set_tests_properties (rev0_bfstruc_read_with_pure_fortran
   PROPERTIES PASS_REGULAR_EXPRESSION "Successfully read")
endif (TEST_NATIVE_FORTRAN_READ)

if (BUILD_UNIT_TESTS)
   add_executable (rev0_bfstruc_tests.exe bfstruc_tests.cpp)
   target_link_libraries (rev0_bfstruc_tests.exe avmesh_shared
                          gtest gtest_main pthread)
   add_test (rev0_bfstruc_tests rev0_bfstruc_tests.exe)
endif (BUILD_UNIT_TESTS)

####
# unstruc tests
####

# executables
add_executable (rev0_unstruc_write_avm.exe unstruc_write_avm.f90)
target_link_libraries (rev0_unstruc_write_avm.exe avmesh_shared)
set_target_properties (rev0_unstruc_write_avm.exe
PROPERTIES LINKER_LANGUAGE Fortran)

add_executable (rev0_unstruc_write_ftn.exe unstruc_write_ftn.f90)

add_executable (rev0_unstruc_read.exe unstruc_read.f90)

if (BUILD_UNIT_TESTS)
   add_executable (rev0_unstruc_tests.exe unstruc_tests.cpp)
   target_link_libraries (rev0_unstruc_tests.exe avmesh_shared
                          gtest gtest_main pthread)
endif (BUILD_UNIT_TESTS)

# pure fortran write tests
if (SUPPORTED_COMPILER)
   add_test (rev0_unstruc_write_with_pure_fortran rev0_unstruc_write_ftn.exe)
   set_tests_properties (rev0_unstruc_write_with_pure_fortran
   PROPERTIES PASS_REGULAR_EXPRESSION "Successfully wrote")

   add_test (rev0_unstruc_read_test1 rev0_unstruc_read.exe)
   set_tests_properties (rev0_unstruc_read_test1
   PROPERTIES PASS_REGULAR_EXPRESSION "Successfully read")

   if (BUILD_UNIT_TESTS)
      add_test (rev0_unstruc_tests_with_fortran_written_file rev0_unstruc_tests.exe)
   endif (BUILD_UNIT_TESTS)
   if (ENABLE_PYTHON)
      add_test (rev0_unstruc_tests_with_fortran_written_file_w_pythonAPI python3 unstruc_tests.py)
   endif (ENABLE_PYTHON)
endif (SUPPORTED_COMPILER)

# avmeshlib write tests
add_test (rev0_unstruc_write_with_avmeshlib rev0_unstruc_write_avm.exe ${WRITE_ENDIAN})
set_tests_properties (rev0_unstruc_write_with_avmeshlib
PROPERTIES PASS_REGULAR_EXPRESSION "Successfully wrote")

if (TEST_NATIVE_FORTRAN_READ)
   add_test (rev0_unstruc_read_test2 rev0_unstruc_read.exe)
   set_tests_properties (rev0_unstruc_read_test2
   PROPERTIES PASS_REGULAR_EXPRESSION "Successfully read")
endif (TEST_NATIVE_FORTRAN_READ)

if (BUILD_UNIT_TESTS)
   add_test (rev0_unstruc_tests_with_avmeshlib_written_file rev0_unstruc_tests.exe)
endif (BUILD_UNIT_TESTS)
if (ENABLE_PYTHON)
   add_test (rev0_unstruc_tests_with_avmeshlib_written_file_w_pythonAPI python3 unstruc_tests.py)
endif (ENABLE_PYTHON)

####
# strand tests
####

add_executable (rev0_strand_write.exe strand_write.f90)
target_link_libraries (rev0_strand_write.exe avmesh_shared)
set_target_properties (rev0_strand_write.exe
PROPERTIES LINKER_LANGUAGE Fortran)

add_executable (rev0_strand_read.exe strand_read.f90)

add_test (rev0_strand_write_with_avmeshlib rev0_strand_write.exe ${WRITE_ENDIAN})
set_tests_properties (rev0_strand_write_with_avmeshlib
PROPERTIES PASS_REGULAR_EXPRESSION "Successfully wrote")

if (TEST_NATIVE_FORTRAN_READ)
   add_test (rev0_strand_read_with_pure_fortran rev0_strand_read.exe)
   set_tests_properties (rev0_strand_read_with_pure_fortran
   PROPERTIES PASS_REGULAR_EXPRESSION "Successfully read")
endif (TEST_NATIVE_FORTRAN_READ)

if (BUILD_UNIT_TESTS)
   add_executable (rev0_strand_tests.exe strand_tests.cpp)
   target_link_libraries (rev0_strand_tests.exe avmesh_shared
                          gtest gtest_main pthread)
   add_test (rev0_strand_tests rev0_strand_tests.exe)
endif (BUILD_UNIT_TESTS)

####
# cart tests
####

add_executable (rev0_cart_write.exe cart_write.f90)
target_link_libraries (rev0_cart_write.exe avmesh_shared)
set_target_properties (rev0_cart_write.exe
PROPERTIES LINKER_LANGUAGE Fortran)

add_executable (rev0_cart_read.exe cart_read.f90)

add_test (rev0_cart_write_with_avmeshlib rev0_cart_write.exe ${WRITE_ENDIAN})
set_tests_properties (rev0_cart_write_with_avmeshlib
PROPERTIES PASS_REGULAR_EXPRESSION "Successfully wrote")

if (TEST_NATIVE_FORTRAN_READ)
   add_test (rev0_cart_read_with_pure_fortran rev0_cart_read.exe)
   set_tests_properties (rev0_cart_read_with_pure_fortran
   PROPERTIES PASS_REGULAR_EXPRESSION "Successfully read")
endif (TEST_NATIVE_FORTRAN_READ)

if (BUILD_UNIT_TESTS)
   add_executable (rev0_cart_tests.exe cart_tests.cpp)
   target_link_libraries (rev0_cart_tests.exe avmesh_shared
                          gtest gtest_main pthread)
   add_test (rev0_cart_tests rev0_cart_tests.exe)
endif (BUILD_UNIT_TESTS)

####
# mixed mesh tests
####

if (SUPPORTED_COMPILER)
   add_executable (rev0_mixed_write.exe mixed_write.f90)

   add_test (rev0_mixed_write_with_pure_fortran rev0_mixed_write.exe)
   set_tests_properties (rev0_mixed_write_with_pure_fortran
   PROPERTIES PASS_REGULAR_EXPRESSION "Successfully wrote")

   if (BUILD_UNIT_TESTS)
      add_executable (rev0_mixed_tests.exe mixed_tests.cpp)
      target_link_libraries (rev0_mixed_tests.exe avmesh_shared
                             gtest gtest_main pthread)
      add_test (rev0_mixed_tests rev0_mixed_tests.exe)
   endif (BUILD_UNIT_TESTS)

   if (ENABLE_PYTHON)
      add_test (rev0_mixed_tests.py python3 mixed_tests.py)
   endif (ENABLE_PYTHON)
endif (SUPPORTED_COMPILER)

####
# examples
####

# these tests depend on mixed.avm, so don't run them
# if we dont construct that file

add_executable (rev0_example_reader_c.exe example_reader.c)
target_link_libraries (rev0_example_reader_c.exe avmesh_shared)
if (SUPPORTED_COMPILER)
add_test (rev0_example_reader.c rev0_example_reader_c.exe)
set_tests_properties (rev0_example_reader.c
PROPERTIES PASS_REGULAR_EXPRESSION
"Mesh Count is 4")
endif (SUPPORTED_COMPILER)

add_executable (rev0_example_reader_f90.exe example_reader.f90)
target_link_libraries (rev0_example_reader_f90.exe avmesh_shared)
set_target_properties (rev0_example_reader_f90.exe
PROPERTIES LINKER_LANGUAGE Fortran)
if (SUPPORTED_COMPILER)
add_test (rev0_example_reader.f90 rev0_example_reader_f90.exe)
set_tests_properties (rev0_example_reader.f90
PROPERTIES PASS_REGULAR_EXPRESSION
"Mesh Count is 4")
endif (SUPPORTED_COMPILER)

if (SUPPORTED_COMPILER)
if (ENABLE_PYTHON)
   add_test (rev0_example_reader.py python3 example_reader.py)
   set_tests_properties (rev0_example_reader.py
   PROPERTIES PASS_REGULAR_EXPRESSION
   "Mesh Count is 4")
endif (ENABLE_PYTHON)
endif (SUPPORTED_COMPILER)

add_executable (rev0_example_offset.exe example_offset.c)
target_link_libraries (rev0_example_offset.exe avmesh_shared)
if (SUPPORTED_COMPILER)
add_test (rev0_example_offset.c rev0_example_offset.exe mixed.avm)
set_tests_properties (rev0_example_offset.c
PROPERTIES PASS_REGULAR_EXPRESSION
"File size is 14526")
endif (SUPPORTED_COMPILER)

add_executable (rev0_print_descriptions.exe print_descriptions.cpp)
target_link_libraries (rev0_print_descriptions.exe avmesh_shared)
