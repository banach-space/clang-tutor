// RUN: %clang_cxx -Xclang -verify -Xclang -load -Xclang %shlibdir/libUnusedForLoopVar%shlibext -Xclang -plugin -Xclang UFLV -c %s 2>&1

#include <vector>

int bar(int var_a) {
  std::vector<int> var_b{1, 2, 3}; 

  // expected-warning@+1 {{(AST Matcher) range for-loop variable not used}}
  for (auto some_integer: var_b)
    var_a++;

  return var_a;
}
