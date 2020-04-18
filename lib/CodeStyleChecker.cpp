//==============================================================================
// FILE:
//    CodeStyleChecker.cpp
//
// DESCRIPTION:
//
// USAGE:
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
  nameStartsWithLowercase(Decl);
  noUnderscoreInName(Decl);
  return true;
}

bool CodeStyleChecker::VisitFunctionDecl(FunctionDecl *Decl) {
  nameStartsWithUppercase(Decl);
  noUnderscoreInName(Decl);
  return true;
}

bool CodeStyleChecker::VisitVarDecl(VarDecl *Decl) {
  nameStartsWithLowercase(Decl);
  noUnderscoreInName(Decl);
  return true;
}

bool CodeStyleChecker::VisitFieldDecl(FieldDecl *Decl) {
  nameStartsWithLowercase(Decl);
  noUnderscoreInName(Decl);
  return true;
}

void CodeStyleChecker::noUnderscoreInName(NamedDecl *Decl) {
  StringRef Name = Decl->getName();
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

void CodeStyleChecker::nameStartsWithLowercase(NamedDecl *Decl) {
  StringRef Name = Decl->getName();
  char FirstChar = Name[0];

  if (!isLowercase(FirstChar))
    return;

  std::string Hint = Name;
  Hint[0] = toUppercase(FirstChar);

  FixItHint FixItHint = FixItHint::CreateReplacement(
      SourceRange(Decl->getLocation(),
                  Decl->getLocation().getLocWithOffset(Name.size())),
      Hint);

  DiagnosticsEngine &DiagEngine = Ctx->getDiagnostics();
  unsigned DiagID = DiagEngine.getCustomDiagID(
      DiagnosticsEngine::Warning,
      "Function names should start with lower-case letter");
  DiagEngine.Report(Decl->getLocation(), DiagID).AddFixItHint(FixItHint);
}

void CodeStyleChecker::nameStartsWithUppercase(NamedDecl *Decl) {
  StringRef Name = Decl->getName();
  char FirstChar = Name[0];
  if (!isUppercase(FirstChar))
    return;

  std::string Hint = Name;
  Hint[0] = toLowercase(FirstChar);

  FixItHint FixItHint = FixItHint::CreateReplacement(
      SourceRange(Decl->getLocation(),
                  Decl->getLocation().getLocWithOffset(Name.size())),
      Hint);

  DiagnosticsEngine &DiagEngine = Ctx->getDiagnostics();
  unsigned DiagID = DiagEngine.getCustomDiagID(
      DiagnosticsEngine::Warning,
      "Type and variable names should start with upper-case letter");
  DiagEngine.Report(Decl->getLocation(), DiagID).AddFixItHint(FixItHint);
}

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
    X(/*Name=*/"CodeStyleChecker",
      /*Description=*/"Checks whether class, variable and function names "
                      "adhere to LLVM's guidelines");
