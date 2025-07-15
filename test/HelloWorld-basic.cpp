// RUN: clang -cc1 -load %shlibdir/libHelloWorld%shlibext -plugin hello-world %s 2>&1 | FileCheck %s

class Foo {
  int i;
};

struct Bar {
  int j;
};

union Bez {
  int k;
  float l;
};

// CHECK: (clang-tutor) file: {{.*}}/test/HelloWorld-basic.cpp
// CHECK-NEXT: (clang-tutor)  count: 3
