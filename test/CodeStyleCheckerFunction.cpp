// RUN: clang -cc1 -verify -load %shlibdir/libCodeStyleChecker%shlibext -add-plugin CSC %s 2>&1
// RUN: clang -Xclang -load -Xclang %shlibdir/libCodeStyleChecker%shlibext -Xclang -add-plugin -Xclang CSC -c %s 2>&1
// RUN: clang -fplugin=%shlibdir/libCodeStyleChecker%shlibext -c %s 2>&1

// Verify that function names starting with upper case are reported as invalid

// expected-warning@+1 {{Function names should start with lower-case letter}}
void ClangTutorFuncBad();

void clangTutorFuncOK();

struct ClangTutorStruct {
  // expected-warning@+1 {{Function names should start with lower-case letter}}
  void ClangTutorMemberMethodBad();
  void clangTutorMemberMethodOK();
};
