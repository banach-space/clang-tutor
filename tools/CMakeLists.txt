# THE LIST OF TOOLS AND THE CORRESPONDING SOURCE FILES
# ====================================================
set(CLANG_TUTOR_TOOLS
    ct-la-commenter
    ct-code-refactor
    ct-code-style-checker
)

set(ct-la-commenter_SOURCES
  LACommenterMain.cpp
  ../lib/LACommenter.cpp
)

set(ct-code-refactor_SOURCES
  CodeRefactorMain.cpp
  ../lib/CodeRefactor.cpp
)

set(ct-code-style-checker_SOURCES
  CodeStyleCheckerMain.cpp
  ../lib/CodeStyleChecker.cpp
)

# CONFIGURE THE TOOLS
# ===================
foreach( tool ${CLANG_TUTOR_TOOLS} )
    # Create a library corresponding to 'plugin'
    add_executable(
      ${tool}
      ${${tool}_SOURCES}
      )

    # Link with libclangTooling
    target_link_libraries(
      ${tool}
      clangTooling
    )

    # Configure include directories for 'tool'
    target_include_directories(
      ${tool}
      PRIVATE
      "${CMAKE_CURRENT_SOURCE_DIR}/../include"
    )
endforeach()
