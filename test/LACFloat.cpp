// RUN: clang -cc1 -load %shlibdir/libLACommenter%shlibext -plugin LAC %s 2>&1 | FileCheck %s

// CHECK-LABEL: bar()
// CHECK-NEXT: foo(/*some_arg=*/1.0);

void foo(float some_arg);

void bar() {
  foo(1.0);
}
