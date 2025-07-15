// RUN: %clang_cc -cc1 -load %shlibdir/libHelloWorld%shlibext -plugin hello-world %s 2>&1 | %FileCheck %s

int i;

void foo();

auto glambda = []() { return 10; };

// CHECK: (clang-tutor)  no declarations found
