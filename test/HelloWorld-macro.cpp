// RUN: clang -cc1 -load %shlibdir/libHelloWorld%shlibext -plugin hello-world %s 2>&1 | FileCheck %s

#define clang_tutor_class(class_name) class ClangTutor##class_name

clang_tutor_class(Foo) {
};

clang_tutor_class(Bar) {
};

// CHECK: (clang-tutor) file: {{.*}}/test/HelloWorld-macro.cpp
// CHECK-NEXT: (clang-tutor)  count: 2
