//==============================================================================
// FILE:
//    CodeStyleChecker.cpp
//
// DESCRIPTION:
//    Checks whether function, variable and type names follow the LLVM's coding
//    style guide. If not, issue a warning and generate a FixIt hint. The
//    following items are exempt from the above rules and are ignored:
//      * anonymous fields in classes and structs
//      * anonymous unions
//      * anonymous function parameters
//      * conversion operators
//    These exemptions are further documented in the source code below.
//
//    This plugin is complete in the sense that it successfully processes
//    vector.h from STL. Also, note that it implements only a small subset of
//    LLVM's coding guidelines.
//
//    By default this plugin will only run on the main translation unit. Use
//    `-main-tu-only=false` to make it run on e.g. included header files too.
//
// USAGE:
//    1. As a loadable Clang plugin:
//    Main TU only:
//      * clang -cc1 -load <BUILD_DIR>/lib/libCodeStyleChecker.dylib -plugin '\'
//        CSC test/CodeStyleCheckerVector.cpp
//    All TUs (the main file and the #includ-ed header files)
//      * clang -cc1 -load <BUILD_DIR>/lib/libCodeStyleChecker.dylib '\'
//        -plugin CSC -plugin-arg-CSC -main-tu-only=false '\'
//        test/CodeStyleCheckerVector.cpp
//    2. As a standalone tool:
//        <BUILD_DIR>/bin/ct-code-style-checker '\'
//        test/ct-code-style-checker-basic.cpp
//
// License: The Unlicense
//==============================================================================
#include "CodeStyleChecker.h"

#include "clang/AST/AST.h"
#include "clang/AST/RecursiveASTVisitor.h"
#include "clang/Frontend/CompilerInstance.h"
#include "clang/Frontend/FrontendPluginRegistry.h"

using namespace clang;

//-----------------------------------------------------------------------------
// CodeStyleCheckerVisitor implementation
//-----------------------------------------------------------------------------
bool CodeStyleCheckerVisitor::VisitCXXRecordDecl(CXXRecordDecl *Decl) {
  // Skip anonymous records, e.g. unions:
  //    * https://en.cppreference.com/w/cpp/language/union
  if (0 == Decl->getNameAsString().size())
    return true;

  checkNameStartsWithUpperCase(Decl);
  checkNoUnderscoreInName(Decl);
  return true;
}

bool CodeStyleCheckerVisitor::VisitFunctionDecl(FunctionDecl *Decl) {
  // Skip user-defined conversion operators/functions:
  //    * https://en.cppreference.com/w/cpp/language/cast_operator
  if (isa<CXXConversionDecl>(Decl))
    return true;

  checkNameStartsWithLowerCase(Decl);
  checkNoUnderscoreInName(Decl);
  return true;
}

bool CodeStyleCheckerVisitor::VisitVarDecl(VarDecl *Decl) {
  // Skip anonymous function parameter declarations
  if (isa<ParmVarDecl>(Decl) && (0 == Decl->getNameAsString().size()))
    return true;

  checkNameStartsWithUpperCase(Decl);
  checkNoUnderscoreInName(Decl);
  return true;
}

bool CodeStyleCheckerVisitor::VisitFieldDecl(FieldDecl *Decl) {
  // Skip anonymous bit-fields:
  //  * https://en.cppreference.com/w/c/language/bit_field
  if (0 == Decl->getNameAsString().size())
    return true;

  checkNameStartsWithUpperCase(Decl);
  checkNoUnderscoreInName(Decl);

  return true;
}

void CodeStyleCheckerVisitor::checkNoUnderscoreInName(NamedDecl *Decl) {
  auto Name = Decl->getNameAsString();
  size_t underscorePos = Name.find('_');

  if (underscorePos == StringRef::npos)
    return;

  std::string Hint = Name;
  auto end_pos = std::remove(Hint.begin(), Hint.end(), '_');
  Hint.erase(end_pos, Hint.end());

  FixItHint FixItHint = FixItHint::CreateReplacement(
      SourceRange(Decl->getLocation(),
                  Decl->getLocation().getLocWithOffset(Name.size())),
      Hint);

  DiagnosticsEngine &DiagEngine = Ctx->getDiagnostics();
  unsigned DiagID = DiagEngine.getCustomDiagID(DiagnosticsEngine::Warning,
                                               "`_` in names is not allowed");
  SourceLocation UnderscoreLoc =
      Decl->getLocation().getLocWithOffset(underscorePos);
  DiagEngine.Report(UnderscoreLoc, DiagID).AddFixItHint(FixItHint);
}

void CodeStyleCheckerVisitor::checkNameStartsWithLowerCase(NamedDecl *Decl) {
  auto Name = Decl->getNameAsString();
  char FirstChar = Name[0];

  // The actual check
  if (isLowercase(FirstChar))
    return;

  // Construct the hint
  std::string Hint = Name;
  Hint[0] = toLowercase(FirstChar);
  FixItHint FixItHint = FixItHint::CreateReplacement(
      SourceRange(Decl->getLocation(),
                  Decl->getLocation().getLocWithOffset(Name.size())),
      Hint);

  // Construct the diagnostic
  DiagnosticsEngine &DiagEngine = Ctx->getDiagnostics();
  unsigned DiagID = DiagEngine.getCustomDiagID(
      DiagnosticsEngine::Warning,
      "Function names should start with lower-case letter");
  DiagEngine.Report(Decl->getLocation(), DiagID) << FixItHint;
}

void CodeStyleCheckerVisitor::checkNameStartsWithUpperCase(NamedDecl *Decl) {
  auto Name = Decl->getNameAsString();
  char FirstChar = Name[0];

  // The actual check
  if (isUppercase(FirstChar))
    return;

  // Construct the hint
  std::string Hint = Name;
  Hint[0] = toUppercase(FirstChar);
  FixItHint FixItHint = FixItHint::CreateReplacement(
      SourceRange(Decl->getLocation(),
                  Decl->getLocation().getLocWithOffset(Name.size())),
      Hint);

  // Construct the diagnostic
  DiagnosticsEngine &DiagEngine = Ctx->getDiagnostics();
  unsigned DiagID = DiagEngine.getCustomDiagID(
      DiagnosticsEngine::Warning,
      "Type and variable names should start with upper-case letter");
  DiagEngine.Report(Decl->getLocation(), DiagID) << FixItHint;
}

//-----------------------------------------------------------------------------
// FrontendAction
//-----------------------------------------------------------------------------
class CSCASTAction : public PluginASTAction {
public:
  std::unique_ptr<ASTConsumer>
  CreateASTConsumer(CompilerInstance &Compiler,
                    llvm::StringRef InFile) override {
    return std::make_unique<CodeStyleCheckerASTConsumer>(
        &Compiler.getASTContext(), MainTuOnly, Compiler.getSourceManager());
  }

  bool ParseArgs(const CompilerInstance &CI,
                 const std::vector<std::string> &Args) override {
    for (StringRef Arg : Args) {
      if (Arg.starts_with("-main-tu-only="))
        MainTuOnly =
            Arg.substr(strlen("-main-tu-only=")).equals_insensitive("true");
      else if (Arg.starts_with("-help"))
        PrintHelp(llvm::errs());
      else
        return false;
    }

    return true;
  }

  void PrintHelp(llvm::raw_ostream &ros) {
    ros << "Help for CodeStyleChecker plugin goes here\n";
  }

  PluginASTAction::ActionType getActionType() override {
#ifndef TARGET_CLANG_TOOL
    return AddBeforeMainAction;
#else
    return CmdlineAfterMainAction;
#endif
  }

private:
  bool MainTuOnly = true;
};

//-----------------------------------------------------------------------------
// Registration
//-----------------------------------------------------------------------------
static clang::FrontendPluginRegistry::Add<CSCASTAction>
    X(/*Name=*/"CSC",
      /*Description=*/"Checks whether class, variable and function names "
                      "adhere to LLVM's guidelines");
