add_executable(lacommenter
  LACommenterMain.cpp
  ../lib/LACommenter.cpp
)

target_link_libraries(lacommenter
  clangTooling
  )

target_include_directories(
  lacommenter
  PRIVATE
  "${CMAKE_CURRENT_SOURCE_DIR}/../include")
