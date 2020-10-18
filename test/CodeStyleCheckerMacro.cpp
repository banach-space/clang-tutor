// RUN: clang -cc1 -verify -load %shlibdir/libCodeStyleChecker%shlibext -plugin CSC %s 2>&1

#define clang_tutor_class_ok(class_name) class ClangTutor##class_name
#define clang_tutor_class_underscore(class_name) class Clang_TutorClass##class_name
#define clang_tutor_class_lower_case(class_name) class clangTutorClass##class_name

#define clang_tutor_func_ok(func_name) void clangTutorFunc##func_name()
#define clang_tutor_func_underscore(func_name) void clangTutor_Func##func_name()
#define clang_tutor_func_upper_case(func_name) void ClangTutorFunc##func_name()

clang_tutor_class_ok(Foo) {
};
// expected-warning@+1 {{`_` in names is not allowed}}
clang_tutor_class_underscore(Bar) {
};
// expected-warning@+1 {{Type and variable names should start with upper-case letter}}
clang_tutor_class_lower_case(Bar) {
};

clang_tutor_func_ok(Foo) {
};
// expected-warning@+1 {{`_` in names is not allowed}}
clang_tutor_func_underscore(Bar) {
};
// expected-warning@+1 {{Function names should start with lower-case letter}}
clang_tutor_func_upper_case(Bar) {
};
