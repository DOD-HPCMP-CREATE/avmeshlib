set(CPP_SRCS
    ${CPP_SRCS}
    ${CMAKE_CURRENT_SOURCE_DIR}/avmesh_rev2.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/avmhdrs.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/strand.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/unstruc.cpp
    PARENT_SCOPE
)

if (ENABLE_TESTS)
    # Copy the test files into the build directory
    IF(NOT ${CMAKE_CURRENT_SOURCE_DIR} EQUAL ${CMAKE_CURRENT_BINARY_DIR})
        IF(IS_DIRECTORY "${CMAKE_CURRENT_SOURCE_DIR}/test")
            FILE(COPY ${CMAKE_CURRENT_SOURCE_DIR}/test
                 DESTINATION ${CMAKE_CURRENT_BINARY_DIR}
                 PATTERN ".svn" EXCLUDE)
        ENDIF(IS_DIRECTORY "${CMAKE_CURRENT_SOURCE_DIR}/test")
    ENDIF(NOT ${CMAKE_CURRENT_SOURCE_DIR} EQUAL ${CMAKE_CURRENT_BINARY_DIR})

    add_subdirectory (test)
endif (ENABLE_TESTS)
