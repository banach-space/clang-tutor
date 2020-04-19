// RUN: clang -cc1 -load %shlibdir/libHelloWorld%shlibext -plugin hello-world %s 2>&1 | FileCheck %s

class Base1 {
};

class Base2 {
};

class Derived1 : public Base1 {
};

class Derived2 : public Base1, public Base2 {
};

// CHECK: (clang-tutor) Hello from: Base1
// CHECK: (clang-tutor)  location: 3:  1
// CHECK: (clang-tutor)  number of bases classes 0
// CHECK: (clang-tutor) Hello from: Base2
// CHECK: (clang-tutor)  location: 6:  1
// CHECK: (clang-tutor)  number of bases classes 0
// CHECK: (clang-tutor) Hello from: Derived1
// CHECK: (clang-tutor)  location: 9:  1
// CHECK: (clang-tutor)  number of bases classes 1
// CHECK: (clang-tutor) Hello from: Derived2
// CHECK: (clang-tutor)  location: 12:  1
// CHECK: (clang-tutor)  number of bases classes 2
