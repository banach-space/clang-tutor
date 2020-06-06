add_executable(lacommenter
  LACommenterMain.cpp
  ../lib/LACommenter.cpp
)

add_executable(ct-code-refactor
  CodeRefactorMain.cpp
  ../lib/CodeRefactor.cpp
)

target_link_libraries(lacommenter
  clangTooling
  )

target_link_libraries(ct-code-refactor
  clangTooling
  )

target_include_directories(
  lacommenter
  PRIVATE
  "${CMAKE_CURRENT_SOURCE_DIR}/../include")

target_include_directories(
  ct-code-refactor
  PRIVATE
  "${CMAKE_CURRENT_SOURCE_DIR}/../include")
