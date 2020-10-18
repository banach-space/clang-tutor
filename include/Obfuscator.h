//==============================================================================
// FILE:
//    Obfuscator.h
//
// DESCRIPTION:
//
// License: The Unlicense
//==============================================================================
#ifndef CLANG_TUTOR_OBFUSCATOR_H
#define CLANG_TUTOR_OBFUSCATOR_H

#include "clang/AST/ASTConsumer.h"
#include "clang/ASTMatchers/ASTMatchFinder.h"
#include "clang/ASTMatchers/ASTMatchers.h"
#include "clang/Rewrite/Core/Rewriter.h"
#include "clang/Rewrite/Frontend/FixItRewriter.h"
#include "clang/Tooling/CommonOptionsParser.h"

//-----------------------------------------------------------------------------
// ASTMatcher callback (add instructions)
//-----------------------------------------------------------------------------
class ObfuscatorMatcherForAdd
    : public clang::ast_matchers::MatchFinder::MatchCallback {
public:
  explicit ObfuscatorMatcherForAdd(clang::Rewriter &RewriterForObfuscator)
      : ObfuscatorRewriter(RewriterForObfuscator) {}
  void onEndOfTranslationUnit() override;

  void run(const clang::ast_matchers::MatchFinder::MatchResult &) override;

private:
  clang::Rewriter ObfuscatorRewriter;
  bool Changed = false;
};

//-----------------------------------------------------------------------------
// ASTMatcher callback (sub instructions)
//-----------------------------------------------------------------------------
class ObfuscatorMatcherForSub : public clang::ast_matchers::MatchFinder::MatchCallback {
public:
  explicit ObfuscatorMatcherForSub(clang::Rewriter &RewriterForObfuscatorSub)
      : ObfuscatorRewriter(RewriterForObfuscatorSub) {}
  void onEndOfTranslationUnit() override;

  void run(const clang::ast_matchers::MatchFinder::MatchResult &) override;

private:
  clang::Rewriter ObfuscatorRewriter;
  bool Changed = false;
};

//-----------------------------------------------------------------------------
// ASTConsumer
//-----------------------------------------------------------------------------
class ObfuscatorASTConsumer : public clang::ASTConsumer {
public:
  ObfuscatorASTConsumer(clang::Rewriter &R);
  void HandleTranslationUnit(clang::ASTContext &Ctx) override {
    Matcher.matchAST(Ctx);
  }

private:
  clang::ast_matchers::MatchFinder Matcher;
  ObfuscatorMatcherForAdd AddHandler;
  ObfuscatorMatcherForSub SubHandler;
};

#endif
