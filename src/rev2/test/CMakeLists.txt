enable_language (Fortran)
include_directories ("${PROJECT_BINARY_DIR}/src")

if (BUILD_UNIT_TESTS)
   SET(CMAKE_CXX_FLAGS "-DGTEST_HAS_TR1_TUPLE=0 ${CMAKE_CXX_FLAGS}")
endif (BUILD_UNIT_TESTS)

####
# misc tests
####

if (BUILD_UNIT_TESTS)
   add_executable (rev2_misc_tests.exe misc_tests.cpp)
   target_link_libraries (rev2_misc_tests.exe avmesh_shared
                          gtest gtest_main pthread)
   add_test (rev2_misc_tests rev2_misc_tests.exe)
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

#native fortran write tests
add_test (rev2_unstruc_write_with_pure_fortran rev2_unstruc_write_ftn.exe)
set_tests_properties (rev2_unstruc_write_with_pure_fortran
PROPERTIES PASS_REGULAR_EXPRESSION "Successfully wrote")

add_test (rev2_unstruc_read_test1 rev2_unstruc_read.exe unstruc_ftn.avm)
set_tests_properties (rev2_unstruc_read_test1
PROPERTIES PASS_REGULAR_EXPRESSION "Successfully read"
DEPENDS rev2_unstruc_write_with_pure_fortran)

add_test (rev2_unstruc_read_bnd_test1 rev2_unstruc_read_bnd.exe unstruc_ftn.avm)
set_tests_properties (rev2_unstruc_read_bnd_test1
PROPERTIES PASS_REGULAR_EXPRESSION "Successfully read"
DEPENDS rev2_unstruc_write_with_pure_fortran)

if (BUILD_UNIT_TESTS)
   add_test (rev2_unstruc_tests_with_fortran_written_file rev2_unstruc_tests.exe unstruc_ftn.avm)
   set_tests_properties(rev2_unstruc_tests_with_fortran_written_file
   PROPERTIES DEPENDS rev2_unstruc_write_with_pure_fortran)
endif (BUILD_UNIT_TESTS)
if (ENABLE_PYTHON)
   add_test (rev2_unstruc_tests_with_fortran_written_file_w_pythonAPI python unstruc_tests.py unstruc_ftn.avm)
   set_tests_properties(
   rev2_unstruc_tests_with_fortran_written_file_w_pythonAPI
   PROPERTIES DEPENDS rev2_unstruc_write_with_pure_fortran)
endif (ENABLE_PYTHON)

# avmeshlib write tests
add_test (rev2_unstruc_write_with_avmeshlib rev2_unstruc_write_avm.exe ${WRITE_ENDIAN})
set_tests_properties (rev2_unstruc_write_with_avmeshlib
PROPERTIES PASS_REGULAR_EXPRESSION "Successfully wrote")

add_test (rev2_unstruc_read_test2 rev2_unstruc_read.exe unstruc_avm.avm)
set_tests_properties (rev2_unstruc_read_test2
PROPERTIES PASS_REGULAR_EXPRESSION "Successfully read"
DEPENDS rev2_unstruc_write_with_avmeshlib)

add_test (rev2_unstruc_read_bnd_test2 rev2_unstruc_read_bnd.exe unstruc_avm.avm)
set_tests_properties (rev2_unstruc_read_bnd_test2
PROPERTIES PASS_REGULAR_EXPRESSION "Successfully read"
DEPENDS rev2_unstruc_write_with_avmeshlib)

if (BUILD_UNIT_TESTS)
   add_test (rev2_unstruc_tests_with_avmeshlib_written_file rev2_unstruc_tests.exe unstruc_avm.avm)
   set_tests_properties(rev2_unstruc_tests_with_avmeshlib_written_file
   PROPERTIES DEPENDS rev2_unstruc_write_with_avmeshlib)
endif (BUILD_UNIT_TESTS)
if (ENABLE_PYTHON)
   add_test (rev2_unstruc_tests_with_avmeshlib_written_file_w_pythonAPI python unstruc_tests.py unstruc_avm.avm)
   set_tests_properties(
   rev2_unstruc_tests_with_avmeshlib_written_file_w_pythonAPI
   PROPERTIES DEPENDS rev2_unstruc_write_with_avmeshlib)
endif (ENABLE_PYTHON)

add_test (rev2_unstruc_write_out_of_order rev2_unstruc_write_out_of_order.exe ${WRITE_ENDIAN})
set_tests_properties (rev2_unstruc_write_out_of_order
PROPERTIES PASS_REGULAR_EXPRESSION "Successfully wrote")

add_test (rev2_unstruc_read_out_of_order rev2_unstruc_read_out_of_order.exe unstruc_ooo.avm)
set_tests_properties (rev2_unstruc_read_out_of_order
PROPERTIES PASS_REGULAR_EXPRESSION "Successfully read"
DEPENDS rev2_unstruc_write_out_of_order)

if (ENABLE_PYTHON)
   add_test (rev2_higher_order_python_tests python higherOrder.py)
endif (ENABLE_PYTHON)

####
# strand tests
####

add_executable (rev2_strand_write.exe strand_write.f90)
target_link_libraries (rev2_strand_write.exe avmesh_shared)
set_target_properties (rev2_strand_write.exe
PROPERTIES LINKER_LANGUAGE Fortran)

add_executable (rev2_strand_read.exe strand_read.f90)

add_test (rev2_strand_write_with_avmeshlib rev2_strand_write.exe ${WRITE_ENDIAN})
set_tests_properties (rev2_strand_write_with_avmeshlib
PROPERTIES PASS_REGULAR_EXPRESSION "Successfully wrote")

add_test (rev2_strand_read_with_pure_fortran rev2_strand_read.exe)
set_tests_properties (rev2_strand_read_with_pure_fortran
PROPERTIES PASS_REGULAR_EXPRESSION "Successfully read"
DEPENDS rev2_strand_write_with_avmeshlib)

if (BUILD_UNIT_TESTS)
   add_executable (rev2_strand_tests.exe strand_tests.cpp)
   target_link_libraries (rev2_strand_tests.exe avmesh_shared
                          gtest gtest_main pthread)
   add_test (rev2_strand_tests rev2_strand_tests.exe)
   set_tests_properties(rev2_strand_tests
   PROPERTIES DEPENDS rev2_strand_write_with_avmeshlib)
endif (BUILD_UNIT_TESTS)

####
# mixed mesh tests
####

add_executable (rev2_mixed_write.exe mixed_write.f90)
add_test (rev2_mixed_write_with_pure_fortran rev2_mixed_write.exe)
target_link_libraries (rev2_mixed_write.exe avmesh_shared)
set_target_properties (rev2_mixed_write.exe
PROPERTIES LINKER_LANGUAGE Fortran)
set_tests_properties (rev2_mixed_write_with_pure_fortran
PROPERTIES PASS_REGULAR_EXPRESSION "Successfully wrote")

if (BUILD_UNIT_TESTS)
   add_executable (rev2_mixed_tests.exe mixed_tests.cpp)
   target_link_libraries (rev2_mixed_tests.exe avmesh_shared
                          gtest gtest_main pthread)
   add_test (rev2_mixed_tests rev2_mixed_tests.exe)
   set_tests_properties(rev2_mixed_tests
   PROPERTIES DEPENDS rev2_mixed_write_with_pure_fortran)
endif (BUILD_UNIT_TESTS)

if (ENABLE_PYTHON)
   add_test (rev2_mixed_tests.py python mixed_tests.py)
   set_tests_properties(rev2_mixed_tests.py
   PROPERTIES DEPENDS rev2_mixed_write_with_pure_fortran)

   add_test (rev2_mesh_size_tests.py python mesh_size_tests.py)
   set_tests_properties(rev2_mesh_size_tests.py
   PROPERTIES DEPENDS rev2_example_offset.exe)
endif (ENABLE_PYTHON)

####
# examples
####

# these tests depend on mixed.avm, so don't run them
# if we dont construct that file

add_executable (rev2_example_reader_c.exe example_reader.c)
target_link_libraries (rev2_example_reader_c.exe avmesh_shared)
add_test (rev2_example_reader.c rev2_example_reader_c.exe)
set_tests_properties (rev2_example_reader.c
PROPERTIES PASS_REGULAR_EXPRESSION
"Mesh Count is 2"
DEPENDS rev2_mixed_write_with_pure_fortran)

add_executable (rev2_example_reader_f90.exe example_reader.f90)
target_link_libraries (rev2_example_reader_f90.exe avmesh_shared)
set_target_properties (rev2_example_reader_f90.exe
PROPERTIES LINKER_LANGUAGE Fortran)
add_test (rev2_example_reader.f90 rev2_example_reader_f90.exe)
set_tests_properties (rev2_example_reader.f90
PROPERTIES PASS_REGULAR_EXPRESSION
"Mesh Count is 2"
DEPENDS rev2_mixed_write_with_pure_fortran)

if (ENABLE_PYTHON)
   add_test (rev2_example_reader.py python example_reader.py)
   set_tests_properties (rev2_example_reader.py
   PROPERTIES PASS_REGULAR_EXPRESSION
   "Mesh Count is 2")
endif (ENABLE_PYTHON)

add_executable (rev2_example_offset.exe example_offset.c)
target_link_libraries (rev2_example_offset.exe avmesh_shared)

add_executable (rev2_print_descriptions.exe print_descriptions.cpp)
target_link_libraries (rev2_print_descriptions.exe avmesh_shared)
