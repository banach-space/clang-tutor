//==============================================================================
// FILE:
//    UnusedForLoopVar.h
//
// DESCRIPTION:
//
// License: The Unlicense
//==============================================================================
#ifndef CLANG_TUTOR_UFLV_H
#define CLANG_TUTOR_UFLV_H

#include "clang/AST/ASTConsumer.h"
#include "clang/ASTMatchers/ASTMatchFinder.h"
#include "clang/ASTMatchers/ASTMatchers.h"
#include "clang/AST/RecursiveASTVisitor.h"
#include "clang/Basic/SourceManager.h"
#include "llvm/ADT/SmallSet.h"
#include "llvm/ADT/SmallVector.h"

//-----------------------------------------------------------------------------
// ASTMatcher callback
//-----------------------------------------------------------------------------
class UnusedForLoopVarMatcher
    : public clang::ast_matchers::MatchFinder::MatchCallback {
public:
  UnusedForLoopVarMatcher() = default;
  // Callback that's executed whenever the Matcher in UnusedForLoopVarASTConsumer matches
  void run(const clang::ast_matchers::MatchFinder::MatchResult &) override;

private:
  void runRegularForLoop(const clang::ast_matchers::MatchFinder::MatchResult &,
                         clang::ASTContext *Ctx);
  void runRangeForLoop(const clang::ast_matchers::MatchFinder::MatchResult &,
                       clang::ASTContext *Ctx);
};

//-----------------------------------------------------------------------------
// RecursiveASTVisitor
//-----------------------------------------------------------------------------
class UnusedForLoopVarVisitor
    : public clang::RecursiveASTVisitor<UnusedForLoopVarVisitor> {
public:
  explicit UnusedForLoopVarVisitor(clang::ASTContext *Ctx) : Ctx(Ctx) {}
  bool TraverseForStmt(clang::ForStmt *S);
  bool VisitDeclRefExpr(clang::DeclRefExpr const *S);

private:
  clang::ASTContext *Ctx;

  llvm::SmallVector<clang::VarDecl const *, 4> VarsDeclaredInLoopInitStmt;
  llvm::SmallSet<clang::VarDecl const *, 4> UsedVars;
};

//-----------------------------------------------------------------------------
// ASTConsumer
//-----------------------------------------------------------------------------
class UnusedForLoopVarASTConsumer : public clang::ASTConsumer {
public:
  UnusedForLoopVarASTConsumer(clang::ASTContext &Ctx, clang::SourceManager &SM);
  void HandleTranslationUnit(clang::ASTContext &Ctx) override {
    Matcher.matchAST(Ctx);

    // Only visit declarations declared in in the input TU
    auto Decls = Ctx.getTranslationUnitDecl()->decls();
    for (auto &Decl : Decls) {
      const auto &FileID = SM.getFileID(Decl->getLocation());
      if (FileID != SM.getMainFileID())
        continue;
      UFLVVisitor.TraverseDecl(Decl);
    }
  }

private:
  clang::ast_matchers::MatchFinder Matcher;
  clang::SourceManager &SM;

  UnusedForLoopVarMatcher UFLVMatcher;
  UnusedForLoopVarVisitor UFLVVisitor;
};

#endif
