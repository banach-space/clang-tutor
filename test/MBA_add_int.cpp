// RUN: clang -cc1 -load %shlibdir/libObfuscator%shlibext -plugin Obfuscator %s 2>&1 | FileCheck %s --match-full-lines
// TODO We need --match-full-lines to make sure that FileCheck doesn't match
// against the CHECK lines. Normally, the comments in the input file can be
// stripped by the preprocessor. However, if I add `-E -P` to strip the
// comments the plugin is not run.

// CHECK-LABEL: int foo(int a, int b) {
// CHECK-NEXT: return (a ^ b) + 2 * (a & b);

// CHECK-LABEL: short bar(short a, short b) {
// CHECK-NEXT: return (a ^ b) + 2 * (a & b);

// CHECK-LABEL: long bez(long a, long b) {
// CHECK-NEXT: return (a ^ b) + 2 * (a & b);

// CHECK-LABEL: int fez(long a, short b) {
// CHECK-NEXT: return (a ^ b) + 2 * (a & b);

// CHECK-LABEL: int fer(int a) {
// CHECK-NEXT: return (a ^ 123) + 2 * (a & 123);

// CHECK-LABEL: int ber() {
// CHECK-NEXT: return (321 ^ 123) + 2 * (321 & 123);

int foo(int a, int b) {
  return a + b;
}

short bar(short a, short b) {
  return a + b;
}

long bez(long a, long b) {
  return a + b;
}

int fez(long a, short b) {
  return a + b;
}

int fer(int a) {
  return a + 123;
}

int ber() {
  return 321 + 123;
}
