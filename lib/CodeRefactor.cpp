//==============================================================================
// FILE:
//    CodeRefactor.cpp
//
// DESCRIPTION:
//
// License: The Unlicense
//==============================================================================
#include "CodeRefactor.h"

#include "clang/AST/Expr.h"
#include "clang/AST/ExprCXX.h"
#include "clang/AST/RecursiveASTVisitor.h"
#include "clang/ASTMatchers/ASTMatchers.h"
#include "clang/Frontend/CompilerInstance.h"
#include "clang/Frontend/FrontendPluginRegistry.h"
#include "clang/Tooling/Refactoring/Rename/RenamingAction.h"
#include "llvm/Support/raw_ostream.h"
#include <string>

using namespace clang;
using namespace ast_matchers;

//-----------------------------------------------------------------------------
// CodeRefactorMatcher - implementation
//-----------------------------------------------------------------------------
void CodeRefactorMatcher::run(const MatchFinder::MatchResult &Result) {
  const MemberExpr *MemberAccess =
      Result.Nodes.getNodeAs<clang::MemberExpr>("MemberAccess");

  if (MemberAccess) {
    SourceRange CallExprSrcRange = MemberAccess->getMemberLoc();
    CodeRefactorRewriter.ReplaceText(CallExprSrcRange, NewName);
  }

  const NamedDecl *MemberDecl =
      Result.Nodes.getNodeAs<clang::NamedDecl>("MemberDecl");

  if (MemberDecl) {
    SourceRange MemberDeclSrcRange = MemberDecl->getLocation();
    CodeRefactorRewriter.ReplaceText(
        CharSourceRange::getTokenRange(MemberDeclSrcRange), NewName);
  }
}

void CodeRefactorMatcher::onEndOfTranslationUnit() {
  // Output to stdout
  CodeRefactorRewriter
      .getEditBuffer(CodeRefactorRewriter.getSourceMgr().getMainFileID())
      .write(llvm::outs());
}

CodeRefactorASTConsumer::CodeRefactorASTConsumer(Rewriter &R,
                                                 std::string ClassName,
                                                 std::string OldName,
                                                 std::string NewName)
    : CodeRefactorHandler(R, NewName), ClassName(ClassName), OldName(OldName),
      NewName(NewName) {

  const auto MatcherForMemberAccess = cxxMemberCallExpr(
      callee(memberExpr(member(hasName(OldName))).bind("MemberAccess")),
      thisPointerType(cxxRecordDecl(isSameOrDerivedFrom(hasName(ClassName)))));

  Finder.addMatcher(MatcherForMemberAccess, &CodeRefactorHandler);

  const auto MatcherForMemberDecl = cxxRecordDecl(
      allOf(isSameOrDerivedFrom(hasName(ClassName)),
            hasMethod(decl(namedDecl(hasName(OldName))).bind("MemberDecl"))));

  Finder.addMatcher(MatcherForMemberDecl, &CodeRefactorHandler);
}

//-----------------------------------------------------------------------------
// FrotendAction
//-----------------------------------------------------------------------------
class CodeRefactorAddPluginAction : public PluginASTAction {
public:
  bool ParseArgs(const CompilerInstance &CI,
                 const std::vector<std::string> &Args) override {
    // Example error handling.
    DiagnosticsEngine &D = CI.getDiagnostics();
    for (unsigned I = 0, E = Args.size(); I != E; ++I) {
      llvm::errs() << "CodeRefactor arg = " << Args[I] << "\n";

      if (Args[I] == "-class-name") {
        if (I + 1 >= E) {
          D.Report(D.getCustomDiagID(DiagnosticsEngine::Error,
                                     "missing -class-name argument"));
          return false;
        }
        ++I;
        ClassName = Args[I];
      } else if (Args[I] == "-old-name") {
        if (I + 1 >= E) {
          D.Report(D.getCustomDiagID(DiagnosticsEngine::Error,
                                     "missing -old-name argument"));
          return false;
        }
        ++I;
        OldName = Args[I];
      } else if (Args[I] == "-new-name") {
        if (I + 1 >= E) {
          D.Report(D.getCustomDiagID(DiagnosticsEngine::Error,
                                     "missing -new-name argument"));
          return false;
        }
        ++I;
        NewName = Args[I];
      }
      if (!Args.empty() && Args[0] == "help")
        PrintHelp(llvm::errs());
    }

    if (NewName == "") {
      D.Report(D.getCustomDiagID(DiagnosticsEngine::Error,
                                 "missing -new-name argument"));
      return false;
    }
    if (OldName == "") {
      D.Report(D.getCustomDiagID(DiagnosticsEngine::Error,
                                 "missing -old-name argument"));
      return false;
    }
    if (ClassName == "") {
      D.Report(D.getCustomDiagID(DiagnosticsEngine::Error,
                                 "missing -class-name argument"));
      return false;
    }

    return true;
  }
  void PrintHelp(llvm::raw_ostream &ros) {
    ros << "Help for CodeRefactor plugin goes here\n";
  }

  // Returns our ASTConsumer per translation unit.
  std::unique_ptr<ASTConsumer> CreateASTConsumer(CompilerInstance &CI,
                                                 StringRef file) override {
    RewriterForCodeRefactor.setSourceMgr(CI.getSourceManager(),
                                         CI.getLangOpts());
    return std::make_unique<CodeRefactorASTConsumer>(
        RewriterForCodeRefactor, ClassName, OldName, NewName);
  }

private:
  Rewriter RewriterForCodeRefactor;
  std::string ClassName;
  std::string OldName;
  std::string NewName;
};

//-----------------------------------------------------------------------------
// Registration
//-----------------------------------------------------------------------------
static FrontendPluginRegistry::Add<CodeRefactorAddPluginAction>
    X(/*Name=*/"CodeRefactor",
      /*Desc=*/"Change the name of a class method");
