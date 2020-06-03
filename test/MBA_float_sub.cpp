// RUN: clang -cc1 -load %shlibdir/libObfuscator%shlibext -add-plugin Obfuscator %s 2>&1 | FileCheck --allow-empty %s --match-full-lines
// TODO We need --match-full-lines to make sure that FileCheck doesn't match
// against the CHECK lines. Normally, the comments in the input file can be
// stripped by the preprocessor. However, if I add `-E -P` to strip the
// comments the plugin is not run.

// The output from this test should be empty
// CHECK-NOT: return a + b;

float foo(float a, float b) {
  return a - b;
}

double bar(double a, double b) {
  return a - b;
}

int bez(float a, float b) {
  return a - b;
}
