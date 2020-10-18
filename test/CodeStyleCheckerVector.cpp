// RUN: clang++ -Xclang -load -Xclang %shlibdir/libCodeStyleChecker%shlibext -Xclang -plugin -Xclang CSC -c %s
// RUN: clang++ -Xclang -load -Xclang %shlibdir/libCodeStyleChecker%shlibext -Xclang -plugin -Xclang CSC -Xclang -plugin-arg-CSC -Xclang -main-tu-only=true -c %s
// RUN: clang++ -Xclang -load -Xclang %shlibdir/libCodeStyleChecker%shlibext -Xclang -plugin -Xclang CSC -Xclang -plugin-arg-CSC -Xclang -main-tu-only=false -c %s

#include <vector>

// Stress test the plugin. This test makes sure that a very complex input
// translation unit is processed successfully (i.e. the plugin does not crash).
//
// There's no straightforward way to write a test for an STL header. Indeed,
// the expected output depends on the actual implementation of the header file.
// However, STL header files are great inputs for stress tests.
