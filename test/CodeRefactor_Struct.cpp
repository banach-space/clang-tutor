// RUN: clang -cc1 -load %shlibdir/libCodeRefactor%shlibext -plugin CodeRefactor -plugin-arg-CodeRefactor -class-name \
// RUN: -plugin-arg-CodeRefactor Base  -plugin-arg-CodeRefactor -old-name -plugin-arg-CodeRefactor run \
// RUN: -plugin-arg-CodeRefactor -new-name -plugin-arg-CodeRefactor walk %s 2>&1 | FileCheck %s --match-full-lines
// RUN:  ../bin/ct-code-refactor --class-name=Base --new-name=walk --old-name=run %s | FileCheck %s --match-full-lines

// Verify that the method `run` is renamed as `walk` (in both Base and Derived)

// CHECK-LABEL: struct Base {
// CHECK: virtual void walk() {};

// CHECK-LABEL: struct Derived : public Base {
// CHECK: void walk() override {};

// CHECK-LABEL: int main() {
// CHECK:  B1.walk();
// CHECK:  D1.walk();
// CHECK:  B2->walk();
// CHECK:  D2->walk();

struct Base {
  public:
    virtual void run() {};
};

struct Derived : public Base {
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
