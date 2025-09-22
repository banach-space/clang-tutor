set(CLANG_TUTOR_TOOLS
    ct-la-commenter
    ct-code-refactor
    ct-code-style-checker
)

set(ct-la-commenter_SOURCES
 "${CMAKE_CURRENT_SOURCE_DIR}/LACommenterMain.cpp"
 "${CMAKE_CURRENT_SOURCE_DIR}/../lib/LACommenter.cpp"
)

set(ct-code-refactor_SOURCES
 "${CMAKE_CURRENT_SOURCE_DIR}/CodeRefactorMain.cpp"
 "${CMAKE_CURRENT_SOURCE_DIR}/../lib/CodeRefactor.cpp"
)

set(ct-code-style-checker_SOURCES
 "${CMAKE_CURRENT_SOURCE_DIR}/CodeStyleCheckerMain.cpp"
 "${CMAKE_CURRENT_SOURCE_DIR}/../lib/CodeStyleChecker.cpp"
)

# CONFIGURE THE TOOLS
# ===================
foreach( tool ${CLANG_TUTOR_TOOLS} )
    # Create a library corresponding to 'plugin'
    add_executable(
      ${tool}
      ${${tool}_SOURCES}
      )

    # Configure include directories for 'tool'
    target_include_directories(
      ${tool}
      PRIVATE
      "${CMAKE_CURRENT_SOURCE_DIR}/../include"
    )

    # Link in the required libraries 
    target_link_libraries(
      ${tool}
      "clangTooling"
    )

    # ct action type should be CmdlineAfterMainAction
    if(${tool} STREQUAL "ct-code-style-checker")
      target_compile_definitions(${tool} PRIVATE TARGET_CLANG_TOOL=1)
    endif()
endforeach()
