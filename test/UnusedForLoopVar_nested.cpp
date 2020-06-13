// RUN: clang -cc1 -verify -load %shlibdir/libUnusedForLoopVar%shlibext -plugin unused-for-loop-variable %s 2>&1

int foo() {
  int a = 10;

  // expected-warning@+1 {{(Recursive AST Visitor) regular for-loop variable not used}}
  for (int i = 0; i < 10; i++) {
    a++;
    for (int j = 0; j < 10; j++) {
      a+=j;
      // expected-warning@+1 {{(Recursive AST Visitor) regular for-loop variable not used}}
      for (int k = 0; k < 10; k++) {
        a--;
      }
    }
  }

  return a;
}
