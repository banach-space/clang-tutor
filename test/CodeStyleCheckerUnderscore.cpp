// RUN: clang -cc1 -verify -load %shlibdir/libCodeStyleChecker%shlibext -add-plugin CSC %s 2>&1

// RUN: clang -Xclang -load -Xclang %shlibdir/libCodeStyleChecker%shlibext -Xclang -add-plugin -Xclang CSC -c %s -o %t.o
// RUN: test -f %t.o
// RUN: rm %t.o

// RUN: clang -fplugin=%shlibdir/libCodeStyleChecker%shlibext -c %s -o %t.o
// RUN: test -f %t.o
// RUN: rm %t.o

// Verify that underscare in types, variables and function names are reported
// as invalid

// expected-warning@+1 {{`_` in names is not allowed}}
class ClangTutorClass_Bad;
class ClangTutorClassOK;

// expected-warning@+1 {{`_` in names is not allowed}}
struct ClangTutorStruct_Bad;
struct ClangTutorStructOK;

// expected-warning@+1 {{`_` in names is not allowed}}
union ClangTutorUnion_Bad;
union ClangTutorUnionOK;

// expected-warning@+1 {{`_` in names is not allowed}}
int ClangTutorInt_Bad;
int ClangTutorIntOK;

// expected-warning@+1 {{`_` in names is not allowed}}
void clangTutor_Bad();
void clangTutorOK();

struct SomeClass {
  // expected-warning@+1 {{`_` in names is not allowed}}
  void clangTutorMemberMethod_Bad();
  void clangTutorMemberMethodOK();

  // expected-warning@+1 {{`_` in names is not allowed}}
  int ClangTutorInt_Bad;
  int ClangTutorIntOK;
};
