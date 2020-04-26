// RUN: clang++ -Xclang -load -Xclang %shlibdir/libHelloWorld%shlibext -Xclang -plugin -Xclang hello-world -c %s 2>&1 | FileCheck %s

#include <vector>

// There's no straightforward way to write this test so that it works reliably
// across different platforms. Instead, this just makes sure that header
// <vector> is processed without errors.

// CHECK-NOT: (clang-tutor)  no declarations found
// CHECK: (clang-tutor)
