//==============================================================================
// FILE:
//    CodeStyleChecker.h
//
// DESCRIPTION:
//
// License: The Unlicense
//==============================================================================
#ifndef CLANG_TUTOR_CSC_H
#define CLANG_TUTOR_CSC_H

#include "clang/AST/ASTConsumer.h"
#include "clang/AST/RecursiveASTVisitor.h"

class CodeStyleChecker : public clang::RecursiveASTVisitor<CodeStyleChecker> {
public:
  explicit CodeStyleChecker(clang::ASTContext *Ctx) : Ctx(Ctx) {}
  bool VisitCXXRecordDecl(clang::CXXRecordDecl *Decl);
  bool VisitFunctionDecl(clang::FunctionDecl *Decl);
  bool VisitVarDecl(clang::VarDecl *Decl);
  bool VisitFieldDecl(clang::FieldDecl *Decl);

private:
  clang::ASTContext *Ctx;
  void noUnderscoreInName(clang::NamedDecl *Decl);
  void nameStartsWithLowercase(clang::NamedDecl *Decl);
  void nameStartsWithUppercase(clang::NamedDecl *Decl);
};

#endif
