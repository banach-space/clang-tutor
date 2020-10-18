// RUN: clang -cc1 -verify -load %shlibdir/libUnusedForLoopVar%shlibext -plugin UFLV %s 2>&1

// Unused for loop variables are are not reported by the plugin, because their
// name match: [U|u][N|n][U|u][S|s][E|e][D|d]

// expected-no-diagnostics

int foo() {
  int a = 10;

  for (int Unused = 0; Unused < 20; Unused++) {
    a++;
    a--;
    a += 123;
  }

  for (int unuseD = 0, unusED = 0; unusED < 20; unusED++) {
    a++;
    a--;
    a += 123;
  }

  return a;
}
