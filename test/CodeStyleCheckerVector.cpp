// RUN: clang++ -Xclang -load -Xclang %shlibdir/libCodeStyleChecker%shlibext -Xclang -add-plugin -Xclang CSC -c %s -o %t1.o 2>&1
// RUN: test -f %t1.o
// RUN: rm %t1.o

// RUN: clang++ -Xclang -load -Xclang %shlibdir/libCodeStyleChecker%shlibext -Xclang -add-plugin -Xclang CSC -Xclang -plugin-arg-CSC -Xclang -main-tu-only=true -c %s -o %t2.o 2>&1
// RUN: test -f %t2.o
// RUN: rm %t2.o

// RUN: clang++ -Xclang -load -Xclang %shlibdir/libCodeStyleChecker%shlibext -Xclang -add-plugin -Xclang CSC -Xclang -plugin-arg-CSC -Xclang -main-tu-only=false -c %s -o %t3.o 2>&1
// RUN: test -f %t3.o
// RUN: rm %t3.o


#include <vector>

// Stress test the plugin. This test makes sure that a very complex input
// translation unit is processed successfully (i.e. the plugin does not crash).
//
// There's no straightforward way to write a test for an STL header. Indeed,
// the expected output depends on the actual implementation of the header file.
// However, STL header files are great inputs for stress tests.
