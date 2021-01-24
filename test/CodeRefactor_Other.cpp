// RUN: clang -cc1 -load %shlibdir/libCodeRefactor%shlibext -plugin CodeRefactor -plugin-arg-CodeRefactor -class-name \
// RUN: -plugin-arg-CodeRefactor Base  -plugin-arg-CodeRefactor -old-name -plugin-arg-CodeRefactor run \
// RUN: -plugin-arg-CodeRefactor -new-name -plugin-arg-CodeRefactor walk %s 2>&1 | FileCheck %s --match-full-lines
// RUN:  ../bin/ct-code-refactor --class-name=Base --new-name=walk --old-name=run %s -- | FileCheck %s --match-full-lines

// Verify that Base::Run() or base::run() are *not* renamed as {B|b}ase::Walk()
// (because there's no Base::Run())

// CHECK-LABEL: struct Base {
// CHECK: void Run() {};

// CHECK-LABEL: struct base {
// CHECK: void run() {};

// CHECK-LABEL: int main() {
// CHECK:  B1.Run();
// CHECK:  b1.run();
// CHECK:  B2->Run();
// CHECK:  b2->run();

struct Base {
  public:
    void Run() {};
};

struct base {
  public:
    void run() {};
};

int main() {
  Base B1;
  base b1;

  B1.Run();
  b1.run();

  Base *B2 = new Base();
  base *b2 = new base();

  B2->Run();
  b2->run();

  delete B2;
  delete b2;

  return 0;
}
