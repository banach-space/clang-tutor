# THE LIST OF TOOLS AND THE CORRESPONDING SOURCE FILES
# ====================================================
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

# DECIDE WHAT LIBRARIES TO LINK IN
#=================================
# The list of the libraries below is the required minimum. However, when using
# LLVM from Homebrew your system linker might complain when using these:
# ```
#   (...) Opaque pointers are only supported in -opaque-pointers mode (Producer: 'LLVM15.0.0' Reader: 'LLVM APPLE_1_1400.0.29.102_0')
# ```
# To work around this, you can use `libclang-cpp` and `libLLVM` instead. That
# solves the problem because:
#  * `libclang-cpp`/`libLLVM` are always a dynamic library (and the problem above is only
#    triggered for static libs)
#  * `libclang-cpp`/`libLLVM` include all Clang/LLVM libraries (i.e. all the
#    libraries listed below _and more_)
#
# TODO: Once LLVM 16 is released, replace `try_compile` with a test for
# LLVM_ENABLE_LTO, see https://reviews.llvm.org/D134936.
set(libs "clangTooling")

try_compile(testMinimalCompilation "${CMAKE_BINARY_DIR}/temp" SOURCES
  ${ct-la-commenter_SOURCES})

if(NOT ${testMinimalCompilation})
  set(libs "clang-cpp" "LLVM")
endif()

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
      ${libs}
    )

    # Configure include directories for 'tool'
    target_include_directories(
      ${tool}
      PRIVATE
      "${CMAKE_CURRENT_SOURCE_DIR}/../include"
    )
endforeach()
