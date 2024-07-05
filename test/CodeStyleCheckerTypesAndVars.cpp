// RUN: clang -cc1 -verify -load %shlibdir/libCodeStyleChecker%shlibext -add-plugin CSC %s 2>&1
// RUN: clang -Xclang -load -Xclang %shlibdir/libCodeStyleChecker%shlibext -Xclang -add-plugin -Xclang CSC -c %s 2>&1
// RUN: clang -fplugin=%shlibdir/libCodeStyleChecker%shlibext -c %s 2>&1

// Verify that type and variable names starting with lower case are reported as
// invalid

// expected-warning@+1 {{Type and variable names should start with upper-case letter}}
class clangTutorClassBad;
class ClangTutorClassOK;

// expected-warning@+1 {{Type and variable names should start with upper-case letter}}
struct clangTutorStructBad;
struct ClangTutorStructOK;

// expected-warning@+1 {{Type and variable names should start with upper-case letter}}
union clangTutorUnionBad;
union ClangTutorUnionOK;

// expected-warning@+1 {{Type and variable names should start with upper-case letter}}
int clangTutorIntBad;
int ClangTutorIntOK;

struct SomeClass {
  // expected-warning@+1 {{Type and variable names should start with upper-case letter}}
  int clangTutorMemberVarBad;
  int ClangTutorMemberVarOK;
};
