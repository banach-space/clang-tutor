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
// USAGE:
//   clang -cc1 -load libCodeStyleChecker.dylib -plugin code-style-checker\
//    test/CodeStyleCheckerUnderscore.cpp
//
// License: The Unlicense
//==============================================================================
#include "CodeStyleChecker.h"

#include "clang/AST/AST.h"
#include "clang/AST/ASTConsumer.h"
#include "clang/AST/RecursiveASTVisitor.h"
#include "clang/Frontend/CompilerInstance.h"
#include "clang/Frontend/FrontendPluginRegistry.h"

using namespace clang;

//-----------------------------------------------------------------------------
// CodeStyleChecker implementation
//-----------------------------------------------------------------------------
bool CodeStyleChecker::VisitCXXRecordDecl(CXXRecordDecl *Decl) {
  // Skip anonymous records, e.g. unions:
  //    * https://en.cppreference.com/w/cpp/language/union
  if (0 == Decl->getNameAsString().size())
    return true;

  checkNameStartsWithUpperCase(Decl);
  checkNoUnderscoreInName(Decl);
  return true;
}

bool CodeStyleChecker::VisitFunctionDecl(FunctionDecl *Decl) {
  // Skip user-defined conversion operators/functions:
  //    * https://en.cppreference.com/w/cpp/language/cast_operator
  if (isa<CXXConversionDecl>(Decl))
    return true;

  checkNameStartsWithLowerCase(Decl);
  checkNoUnderscoreInName(Decl);
  return true;
}

bool CodeStyleChecker::VisitVarDecl(VarDecl *Decl) {
  // Skip anonymous function parameter declarations
  if (isa<ParmVarDecl>(Decl) &&  (0 == Decl->getNameAsString().size()))
    return true;

  checkNameStartsWithUpperCase(Decl);
  checkNoUnderscoreInName(Decl);
  return true;
}

bool CodeStyleChecker::VisitFieldDecl(FieldDecl *Decl) {
  // Skip anonymous bit-fields:
  //  * https://en.cppreference.com/w/c/language/bit_field
  if (0 == Decl->getNameAsString().size())
    return true;

  checkNameStartsWithUpperCase(Decl);
  checkNoUnderscoreInName(Decl);

  return true;
}

void CodeStyleChecker::checkNoUnderscoreInName(NamedDecl *Decl) {
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

void CodeStyleChecker::checkNameStartsWithLowerCase(NamedDecl *Decl) {
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

void CodeStyleChecker::checkNameStartsWithUpperCase(NamedDecl *Decl) {
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
// ASTConsumer
//-----------------------------------------------------------------------------
class CSCConsumer : public ASTConsumer {
public:
  explicit CSCConsumer(ASTContext *Context) : Visitor(Context) {}

  void HandleTranslationUnit(ASTContext &Ctx) {
    Visitor.TraverseDecl(Ctx.getTranslationUnitDecl());
  }

private:
  CodeStyleChecker Visitor;
};

//-----------------------------------------------------------------------------
// FrotendAction
//-----------------------------------------------------------------------------
class CSCASTAction : public PluginASTAction {
public:
  std::unique_ptr<ASTConsumer>
  CreateASTConsumer(CompilerInstance &Compiler,
                    llvm::StringRef InFile) override {
    return std::make_unique<CSCConsumer>(&Compiler.getASTContext());
  }

  bool ParseArgs(const CompilerInstance &CI,
                 const std::vector<std::string> &args) override {
    return true;
  }
};

//-----------------------------------------------------------------------------
// Registration
//-----------------------------------------------------------------------------
static clang::FrontendPluginRegistry::Add<CSCASTAction>
    X(/*Name=*/"code-style-checker",
      /*Description=*/"Checks whether class, variable and function names "
                      "adhere to LLVM's guidelines");
