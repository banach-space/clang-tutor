// RUN: clang -cc1 -verify -load %shlibdir/libCodeStyleChecker%shlibext -plugin CSC %s 2>&1

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
