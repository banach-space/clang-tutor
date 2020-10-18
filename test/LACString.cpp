// RUN: clang -cc1 -load %shlibdir/libLACommenter%shlibext -plugin LAC %s 2>&1 | FileCheck %s
// RUN: ../bin/ct-la-commenter %s 2>&1 | FileCheck %s

// CHECK-LABEL: bar()
// CHECK-NEXT: foo(/*some_arg=*/"one");

void foo(const char* some_arg);

void bar() {
  foo("one");
}
