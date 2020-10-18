// RUN: clang -cc1 -verify  -load %shlibdir/libCodeStyleChecker%shlibext -plugin CSC %s 2>&1

// Verify that conversion operators are not checked

// expected-no-diagnostics
class SomeClass {
public:
  operator bool();
  operator int();
  operator char();
};
