// RUN: clang++ -Xclang -verify -Xclang -load -Xclang %shlibdir/libUnusedForLoopVar%shlibext -Xclang -plugin -Xclang unused-for-loop-variable -c %s 2>&1

// Unused for loop variables are are not reported by the plugin, because their
// name match: [U|u][N|n][U|u][S|s][E|e][D|d]

// expected-no-diagnostics

#include <vector>

int foo() {
  int a = 10;

  std::vector<int> var_b{1, 2, 3}; 
  for (auto unused: var_b)
    a++;

  for (auto UNUSED: var_b)
    a++;

  return a;
}
