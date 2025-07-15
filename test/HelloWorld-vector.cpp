// RUN: %clang_cxx -Xclang -load -Xclang %shlibdir/libHelloWorld%shlibext -Xclang -plugin -Xclang hello-world -c %s 2>&1 | %FileCheck %s

#include <vector>

// Stress test the plugin. This test makes sure that a very complex input
// translation unit is processed successfully (i.e. the plugin does not crash).
//
// There's no straightforward way to write a test for an STL header. Indeed,
// the expected output depends on the actual implementation of the header file.
// However, STL header files are great inputs for stress tests.

// CHECK-NOT: (clang-tutor)  no declarations found
// CHECK: (clang-tutor)
