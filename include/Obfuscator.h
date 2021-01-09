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

#include <memory>

//-----------------------------------------------------------------------------
// ASTMatcher callback (add instructions)
//-----------------------------------------------------------------------------
class ObfuscatorMatcherForAdd
    : public clang::ast_matchers::MatchFinder::MatchCallback {
public:
  explicit ObfuscatorMatcherForAdd(
      std::shared_ptr<clang::Rewriter> RewriterForObfuscator)
      : ObfuscatorRewriter{std::move(RewriterForObfuscator)} {}

  void run(const clang::ast_matchers::MatchFinder::MatchResult &) override;

private:
  std::shared_ptr<clang::Rewriter> ObfuscatorRewriter;
};

//-----------------------------------------------------------------------------
// ASTMatcher callback (sub instructions)
//-----------------------------------------------------------------------------
class ObfuscatorMatcherForSub
    : public clang::ast_matchers::MatchFinder::MatchCallback {
public:
  explicit ObfuscatorMatcherForSub(
      std::shared_ptr<clang::Rewriter> RewriterForObfuscatorSub)
      : ObfuscatorRewriter{std::move(RewriterForObfuscatorSub)} {}

  void run(const clang::ast_matchers::MatchFinder::MatchResult &) override;

private:
  std::shared_ptr<clang::Rewriter> ObfuscatorRewriter;
};

//-----------------------------------------------------------------------------
// ASTConsumer
//-----------------------------------------------------------------------------
class ObfuscatorASTConsumer : public clang::ASTConsumer {
public:
  ObfuscatorASTConsumer(std::shared_ptr<clang::Rewriter> R);
  void HandleTranslationUnit(clang::ASTContext &Ctx) override {
    Matcher.matchAST(Ctx);
  }

private:
  clang::ast_matchers::MatchFinder Matcher;
  ObfuscatorMatcherForAdd AddHandler;
  ObfuscatorMatcherForSub SubHandler;
};

#endif
