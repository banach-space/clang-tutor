// RUN: %clang_cc -cc1 -load %shlibdir/libLACommenter%shlibext -plugin LAC %s 2>&1 | %FileCheck %s
// RUN: ../bin/ct-la-commenter %s 2>&1 -- | %FileCheck %s

// CHECK-LABEL: bar()
// CHECK-NEXT: foo(/*some_arg=*/'1');

void foo(char some_arg);

void bar() {
  foo('1');
}
