//==============================================================================
// FILE:
//    CodeStyleChecker.h
//
// DESCRIPTION:
//    Declares the CodeStyleChecker visitor
//
// License: The Unlicense
//==============================================================================
#ifndef CLANG_TUTOR_CSC_H
#define CLANG_TUTOR_CSC_H

#include "clang/AST/ASTConsumer.h"
#include "clang/AST/RecursiveASTVisitor.h"

//-----------------------------------------------------------------------------
// RecursiveASTVisitor
//-----------------------------------------------------------------------------
class CodeStyleCheckerVisitor
    : public clang::RecursiveASTVisitor<CodeStyleCheckerVisitor> {
public:
  explicit CodeStyleCheckerVisitor(clang::ASTContext *Ctx) : Ctx(Ctx) {}
  bool VisitCXXRecordDecl(clang::CXXRecordDecl *Decl);
  bool VisitFunctionDecl(clang::FunctionDecl *Decl);
  bool VisitVarDecl(clang::VarDecl *Decl);
  bool VisitFieldDecl(clang::FieldDecl *Decl);

private:
  clang::ASTContext *Ctx;

  // Checks whether the name in Decl contains any `_`. Issues a warnning if it
  // does.
  void checkNoUnderscoreInName(clang::NamedDecl *Decl);
  // Checks whether the name in Decl starts with a lower case letter. Issues a
  // warning if not.
  void checkNameStartsWithLowerCase(clang::NamedDecl *Decl);
  // Checks whether the name in Decl starts with an upper case letter. Issues a
  // warning if not.
  void checkNameStartsWithUpperCase(clang::NamedDecl *Decl);
};

//-----------------------------------------------------------------------------
// ASTConsumer
//-----------------------------------------------------------------------------
class CodeStyleCheckerASTConsumer : public clang::ASTConsumer {
public:
  explicit CodeStyleCheckerASTConsumer(clang::ASTContext *Context,
                                       bool MainFileOnly,
                                       clang::SourceManager &SM)
      : Visitor(Context), SM(SM), MainTUOnly(MainFileOnly) {}

  void HandleTranslationUnit(clang::ASTContext &Ctx) {
    if (!MainTUOnly)
      Visitor.TraverseDecl(Ctx.getTranslationUnitDecl());
    else {
      // Only visit declarations declared in in the input TU
      auto Decls = Ctx.getTranslationUnitDecl()->decls();
      for (auto &Decl : Decls) {
        const auto &FileID = SM.getFileID(Decl->getLocation());
        if (FileID != SM.getMainFileID())
          continue;
        Visitor.TraverseDecl(Decl);
      }
    }
  }

private:
  CodeStyleCheckerVisitor Visitor;
  clang::SourceManager &SM;
  // Should this plugin be only run on the main translation unit?
  bool MainTUOnly = true;
};

#endif
