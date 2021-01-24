// RUN:  ../bin/ct-code-style-checker %s 2>&1 -- | FileCheck -implicit-check-not warning: %s
// RUN:  ../bin/ct-code-style-checker -main-tu-only=true %s 2>&1 -- | FileCheck -implicit-check-not warning: %s

// This is a rather simplified test for CodeStyleChecker that focuses on the
// standalone wrapper tool: ct-code-style-checker. 
//
// Unlike `clang -cc1`, ct-code-style-checker does not support `-verify` and we
// cannot re-use the tests implemented for the plugin (which do rely on
// `-verify`). Instead, this test is added as a small sanity-check. For proper
// testing of CodeStyleChecker see the other tests that use the plugin version.

// CHECK: ct-code-style-checker-basic.cpp:[[@LINE+1]]:6: warning: Function names should start with lower-case letter 
void ClangTutorFuncBad();
void clangTutorFuncOK();

// CHECK: ct-code-style-checker-basic.cpp:[[@LINE+1]]:7: warning: Type and variable names should start with upper-case letter
class clangTutorClassBad;
class ClangTutorClassOK;

// CHECK: ct-code-style-checker-basic.cpp:[[@LINE+1]]:22: warning: `_` in names is not allowed
class ClangTutorClass_Bad;
class ClangTutorClassOK;
