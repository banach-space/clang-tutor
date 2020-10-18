// RUN: clang -cc1 -verify -load %shlibdir/libUnusedForLoopVar%shlibext -plugin UFLV %s 2>&1

int foo() {
  int a = 10;

  // expected-warning@+1 {{(Recursive AST Visitor) regular for-loop variable not used}}
  for (int j = 0; j < 20; j++) {
    a++;
    a--;
    a += 123;
  }

  // expected-warning@+3 {{(AST Matcher) regular for-loop variable not used}}
  // expected-warning@+2 {{(Recursive AST Visitor) regular for-loop variable not used}}
  // expected-warning@+1 {{(Recursive AST Visitor) regular for-loop variable not used}}
  for (int j = 0, i = 0; j < 20; j++) {
    a++;
    a--;
    a += 123;
  }

  return a;
}
