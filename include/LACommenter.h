//==============================================================================
// FILE:
//    LACommenter.h
//
// DESCRIPTION:
//
// License: The Unlicense
//==============================================================================
#ifndef CLANG_TUTOR_LAC_H
#define CLANG_TUTOR_LAC_H

#include "clang/AST/ASTConsumer.h"
#include "clang/ASTMatchers/ASTMatchFinder.h"
#include "clang/ASTMatchers/ASTMatchers.h"
#include "clang/Rewrite/Core/Rewriter.h"

class LACommenter : public clang::ast_matchers::MatchFinder::MatchCallback {
public:
  LACommenter(clang::Rewriter &LACRewriter) : LACRewriter(LACRewriter) {}
  // Callback that's executed whenever the Matcher in LACASTConsumer matches.
  void run(const clang::ast_matchers::MatchFinder::MatchResult &) override;
  // Callback that's executed at the end of the translation unit
  void onEndOfTranslationUnit() override;

private:
  clang::Rewriter LACRewriter;
  llvm::SmallSet<clang::FullSourceLoc, 8> EditedLocations;
};

class LACASTConsumer : public clang::ASTConsumer {
public:
  LACASTConsumer(clang::Rewriter &R);
  void HandleTranslationUnit(clang::ASTContext &Ctx) override {
    Matcher.matchAST(Ctx);
  }

private:
  clang::ast_matchers::MatchFinder Matcher;
  LACommenter LAC;
};

#endif
