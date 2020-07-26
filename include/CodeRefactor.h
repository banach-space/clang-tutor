//==============================================================================
// FILE:
//    CodeRefactor.h
//
// DESCRIPTION:
//
// License: The Unlicense
//==============================================================================
#ifndef CLANG_TUTOR_CODEREFACTOR_H
#define CLANG_TUTOR_CODEREFACTOR_H

#include "clang/AST/ASTConsumer.h"
#include "clang/ASTMatchers/ASTMatchFinder.h"
#include "clang/Rewrite/Core/Rewriter.h"
#include "clang/Rewrite/Frontend/FixItRewriter.h"
#include "clang/Tooling/CommonOptionsParser.h"

//-----------------------------------------------------------------------------
// ASTFinder callback
//-----------------------------------------------------------------------------
class CodeRefactorMatcher
    : public clang::ast_matchers::MatchFinder::MatchCallback {
public:
  explicit CodeRefactorMatcher(clang::Rewriter &RewriterForCodeRefactor,
                               std::string NewName)
      : CodeRefactorRewriter(RewriterForCodeRefactor), NewName(NewName) {}
  void onEndOfTranslationUnit() override;

  void run(const clang::ast_matchers::MatchFinder::MatchResult &) override;

private:
  clang::Rewriter CodeRefactorRewriter;
  // The new name of the matched member expression.
  // NOTE: This matcher already knows *what* to replace (which method in which
  // class/struct), because it _matched_ a member expression that corresponds to
  // the command line arguments.
  std::string NewName;
};

//-----------------------------------------------------------------------------
// ASTConsumer
//-----------------------------------------------------------------------------
class CodeRefactorASTConsumer : public clang::ASTConsumer {
public:
  CodeRefactorASTConsumer(clang::Rewriter &R, std::string ClassName,
                          std::string OldName, std::string NewName);
  void HandleTranslationUnit(clang::ASTContext &Ctx) override {
    Finder.matchAST(Ctx);
  }

private:
  clang::ast_matchers::MatchFinder Finder;
  CodeRefactorMatcher CodeRefactorHandler;
  // The name of the class to match. Use base class name to rename in all
  // derived classes.
  std::string ClassName;
  // The name of the member method to match
  std::string OldName;
  // The new name of the member method
  std::string NewName;
};

#endif
