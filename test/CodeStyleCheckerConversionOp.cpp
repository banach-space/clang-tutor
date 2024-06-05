// RUN: clang -cc1 -verify -load %shlibdir/libCodeStyleChecker%shlibext -add-plugin CSC %s 2>&1
// RUN: clang -Xclang -load -Xclang %shlibdir/libCodeStyleChecker%shlibext -Xclang -add-plugin -Xclang CSC -c %s 2>&1
// RUN: clang -fplugin=%shlibdir/libCodeStyleChecker%shlibext -c %s 2>&1

// Verify that conversion operators are not checked

// expected-no-diagnostics
class SomeClass {
public:
  operator bool();
  operator int();
  operator char();
};
