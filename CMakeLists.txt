cmake_minimum_required(VERSION 3.20)
project(clang-tutor)

#===============================================================================
# 1. FIND CLANG/LLVM INSTALLATION
#===============================================================================
# Uses CMake's `find_package` to locate a Clang/LLVM installation.
# To specify a custom installation path, run CMake with:
#
#   cmake -D CMAKE_PREFIX_PATH=/path/to/your/llvm/install ..
#   (or)
#   cmake -D LLVM_DIR=/path/to/your/llvm/install ..
#
# `find_package` will then automatically find the necessary configuration files.

find_package(Clang 19.1 REQUIRED CONFIG)

# The `find_package` command with a version number (e.g., 19) makes the
# manual version check below redundant. It will fail automatically if a
# compatible version isn't found.

message(STATUS "Found Clang/LLVM ${LLVM_PACKAGE_VERSION}")

message("CLANG STATUS:
  Includes (clang)    ${CLANG_INCLUDE_DIRS}
  Includes (llvm)     ${LLVM_INCLUDE_DIRS}"
)

# Set the LLVM and Clang header and library paths
include_directories(SYSTEM "${LLVM_INCLUDE_DIRS}" "${CLANG_INCLUDE_DIRS}")

#===============================================================================
# 2. CLANG-TUTOR BUILD CONFIGURATION
#===============================================================================
# Use the same C++ standard as LLVM does
set(CMAKE_CXX_STANDARD 17 CACHE STRING "")

# Build type
if(NOT CMAKE_BUILD_TYPE)
  set(CMAKE_BUILD_TYPE Debug CACHE
      STRING "Build type (default Debug):" FORCE)
endif()

# Compiler flags
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wall\
    -fdiagnostics-color=always")

# LLVM/Clang is normally built without RTTI. Be consistent with that.
if(NOT LLVM_ENABLE_RTTI)
  set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -fno-rtti")
endif()

# -fvisibility-inlines-hidden is set when building LLVM and on Darwin warnings
# are triggered if llvm-tutor is built without this flag (though otherwise it
# builds fine). For consistency, add it here too.
include(CheckCXXCompilerFlag)
check_cxx_compiler_flag("-fvisibility-inlines-hidden"
  SUPPORTS_FVISIBILITY_INLINES_HIDDEN_FLAG)
if(${SUPPORTS_FVISIBILITY_INLINES_HIDDEN_FLAG} EQUAL "1")
  set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -fvisibility-inlines-hidden")
endif()

# Set the build directories
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY "${PROJECT_BINARY_DIR}/bin")
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY "${PROJECT_BINARY_DIR}/lib")

#===============================================================================
# 3. ADD SUB-TARGETS
# Doing this at the end so that all definitions and link/include paths are
# available for the sub-projects.
#===============================================================================
add_subdirectory(test)
add_subdirectory(lib)
add_subdirectory(tools)
add_subdirectory(HelloWorld)
