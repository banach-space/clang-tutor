//==============================================================================
// FILE:
//    HelloWorld.cpp
//
// DESCRIPTION:
//
// USAGE:
//
// License: The Unlicense
//==============================================================================
#include "clang/Frontend/FrontendPluginRegistry.h"
#include "clang/AST/ASTConsumer.h"
#include "clang/AST/RecursiveASTVisitor.h"
#include "clang/Frontend/CompilerInstance.h"
#include "llvm/Support/raw_ostream.h"

using namespace clang;

//-----------------------------------------------------------------------------
// HelloWorld implementation
//-----------------------------------------------------------------------------
class HelloWorld : public RecursiveASTVisitor<HelloWorld> {
public:
  explicit HelloWorld(ASTContext *Context) : Context(Context) {}
  bool VisitCXXRecordDecl(CXXRecordDecl *Decl);

private:
  ASTContext *Context;
};

bool HelloWorld::VisitCXXRecordDecl(CXXRecordDecl *Declaration) {
  FullSourceLoc FullLocation = Context->getFullLoc(Declaration->getBeginLoc());
  if (FullLocation.isValid())
    llvm::outs() << "(clang-tutor) Hello from: " << Declaration->getName()
                 << "\n"
                 << "(clang-tutor)  location: "
                 << FullLocation.getSpellingLineNumber() << ":"
                 << "  " << FullLocation.getSpellingColumnNumber() << "\n"
                 << "(clang-tutor)  number of bases classes "
                 << Declaration->getNumBases() << "\n";
  return true;
}

class HelloWorldASTConsumer : public clang::ASTConsumer {
public:
  explicit HelloWorldASTConsumer(ASTContext *Ctx) : Visitor(Ctx) {}

  void HandleTranslationUnit(clang::ASTContext &Ctx) override {
    Visitor.TraverseDecl(Ctx.getTranslationUnitDecl());
  }

private:
  HelloWorld Visitor;
};

//-----------------------------------------------------------------------------
// FrotendAction
//-----------------------------------------------------------------------------
class FindNamedClassAction : public clang::PluginASTAction {
public:
  std::unique_ptr<clang::ASTConsumer>
  CreateASTConsumer(clang::CompilerInstance &Compiler,
                    llvm::StringRef InFile) override {
    return std::unique_ptr<clang::ASTConsumer>(
        std::make_unique<HelloWorldASTConsumer>(&Compiler.getASTContext()));
  }
  bool ParseArgs(const CompilerInstance &CI,
                 const std::vector<std::string> &args) override {
    return true;
  }
};

//-----------------------------------------------------------------------------
// Registration
//-----------------------------------------------------------------------------
static FrontendPluginRegistry::Add<FindNamedClassAction>
    X(/*Name=*/"hello-world", /*Description:=*/"The HelloWorld plugin");
