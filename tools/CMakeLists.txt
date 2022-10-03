# TODO: Once LLVM 16 is released, replace `try_compile` with a test for
# LLVM_ENABLE_LTO, see https://reviews.llvm.org/D134936.

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
endforeach()

# DECIDE WHAT LIBRARIES TO LINK IN
#=================================
# The minimal list of the required LLVM libraries.
set(libs "clangTooling")

# The list above won't work when using LLVM from Homebrew. Indeed, your system
# linker will most likely complain:
# ```
#   (...) Opaque pointers are only supported in -opaque-pointers mode (Producer: 'LLVM15.0.0' Reader: 'LLVM APPLE_1_1400.0.29.102_0')
# ```
# That's more or less due to Homebrew packages being built with LTO (i.e.
# LLVM_ENABLE_LTO set to "ON") To work around this, you can use `libLLVM` and
# `libclang-cpp` instead. That solves the problem because:
#  * `libclang-cpp`/`libLLVM` are always a dynamic library (and the problem above is only
#    triggered for static libs)
#  * `libclang-cpp`/`libLLVM` include all Clang/LLVM libraries (i.e. all the
#    libraries listed below _and more_)

# Check whether the default set-up is sufficient ...
try_compile(testMinimalCompilation "${CMAKE_BINARY_DIR}/temp"
  SOURCES ${ct-la-commenter_SOURCES}
  LINK_LIBRARIES ${libs}
  CMAKE_FLAGS 
  "-DINCLUDE_DIRECTORIES=${CMAKE_CURRENT_SOURCE_DIR}/../include;${LLVM_INCLUDE_DIRS};${CLANG_INCLUDE_DIRS}"
    "-DLINK_DIRECTORIES=${LLVM_LIBRARY_DIRS}")

# ... if not, use libLLVM and libclang-cpp
if(NOT ${testMinimalCompilation})
  set(libs "clang-cpp" "LLVM")
endif()

# ADD THE LIBRARIES TO LINK
#==========================
foreach( tool ${CLANG_TUTOR_TOOLS} )
    # Link with libclangTooling
    target_link_libraries(
      ${tool}
      ${libs}
    )
endforeach()
