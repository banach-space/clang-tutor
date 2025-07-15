// RUN: %clang_cc -cc1 -load %shlibdir/libLACommenter%shlibext -plugin LAC %s 2>&1 | %FileCheck %s
// RUN: ../bin/ct-la-commenter %s 2>&1 -- | %FileCheck %s

// CHECK-LABEL: bar()
// CHECK-NEXT: foo(/*a=*/1, /*b=*/1.0, /*c=*/'1', /*d=*/"one", /*e=*/true);

void foo(int a, float b, char c, const char* d, bool e);

void bar() {
  foo(1, 1.0, '1', "one", true);
}
