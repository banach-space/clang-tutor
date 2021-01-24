// RUN: clang -cc1 -load %shlibdir/libCodeRefactor%shlibext -plugin CodeRefactor -plugin-arg-CodeRefactor -class-name \
// RUN: -plugin-arg-CodeRefactor Derived  -plugin-arg-CodeRefactor -old-name -plugin-arg-CodeRefactor run \
// RUN: -plugin-arg-CodeRefactor -new-name -plugin-arg-CodeRefactor walk %s 2>&1 | FileCheck %s --match-full-lines
// RUN:  ../bin/ct-code-refactor --class-name=Derived --new-name=walk --old-name=run %s -- | FileCheck %s --match-full-lines

// Verify that Derived::run() is renamed as Derived::walk() (i.e. only the
// derived class is refactored).

// CHECK-LABEL: int main() {
// CHECK:  B1.run();
// CHECK:  D1.walk();
// CHECK:  B2->run();
// CHECK:  D2->walk();

class Base {
  public:
    virtual void run() {};
};

class Derived : public Base {
  public:
    void run() override {};
};

int main() {
  Base B1;
  Derived D1;

  B1.run();
  D1.run();

  Base *B2 = new Base();
  Derived *D2 = new Derived();

  B2->run();
  D2->run();

  return 0;
}
