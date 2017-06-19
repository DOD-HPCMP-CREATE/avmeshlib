
# Prepare configuration options

configure_file("Doxyfile.in" "Doxyfile")

# See if we can find doxygen, if so, build the docs

find_program(DOXYGEN doxygen)

if(EXISTS ${DOXYGEN})
  add_custom_target(avm_docs ALL DEPENDS html latex/refman.pdf)
  add_custom_command(OUTPUT html latex
    COMMAND doxygen > /dev/null 2>&1
    MAIN_DEPENDENCY Doxyfile)
  add_custom_command(OUTPUT latex/refman.pdf
    COMMAND cp "${PROJECT_SOURCE_DIR}/doc/refman.tex" .
    COMMAND cp "${PROJECT_SOURCE_DIR}/doc/titlepage.tex" .
    COMMAND cp "${PROJECT_SOURCE_DIR}/doc/createLogo.pdf" .
    COMMAND cp "${PROJECT_SOURCE_DIR}/doc/createAVLogo.pdf" .
    COMMAND make pdf > /dev/null 2>&1
    COMMAND cp refman.pdf ../../avmeshlib_user_manual.pdf
    WORKING_DIRECTORY ${PROJECT_SOURCE_DIR}/doc/latex
    DEPENDS latex
    MAIN_DEPENDENCY Doxyfile)
  #install(FILES "${PROJECT_BINARY_DIR}/doc/latex/refman.pdf"
  #  DESTINATION share/avmesh RENAME avmesh_rev1.pdf)
  #install(DIRECTORY "${PROJECT_BINARY_DIR}/doc/html"
  #  DESTINATION share/avmesh)
endif(EXISTS ${DOXYGEN})
