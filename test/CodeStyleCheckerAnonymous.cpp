// RUN: clang -cc1 -verify -load %shlibdir/libCodeStyleChecker%shlibext -plugin CSC %s 2>&1

// 1. Verify that anonymous unions are not flagged as invalid (no name ->
// nothing to check). However, the member variables _are_ verified.

static union {
// expected-warning@+1 {{Type and variable names should start with upper-case letter}}
  int clangTutorBad;
  const char *ClangTutorOk;
};

// 2. Verify that anonymous function parameters are not flagged as invalid (no name
// -> nothing to check). However, the function name's _is_ verified.

void clangTutorFunc(void *);

// expected-warning@+1 {{Function names should start with lower-case letter}}
void ClangTutorFunc(void *);

// 3. Verify that anonymous bit-fields are not flagged as invalid (no name
// -> nothing to check). However, the named fields _are_ verified.
struct ClangTutorStruct {
  unsigned short 
    // expected-warning@+1 {{Type and variable names should start with upper-case letter}}
    clangTutorBitFieldBad : 1,
    ClangTutorBitFieldOK  : 1,
                          : 1;
};
