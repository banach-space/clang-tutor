//==============================================================================
// FILE:
//    LACommenterMain.cpp
//
// DESCRIPTION:
//    A standalone tool that runs the LACommenter plugin. See
//    LACommenter.cpp for a complete description.
//
// USAGE:
//    * ct-la-commenter input-file.cpp
//
// REFERENCES:
//    Based on an example by Peter Smith:
//      * https://s3.amazonaws.com/connect.linaro.org/yvr18/presentations/yvr18-223.pdf
//
// License: The Unlicense
//==============================================================================
#include "LACommenter.h"

#include "clang/Frontend/CompilerInstance.h"
#include "clang/Frontend/FrontendPluginRegistry.h"
#include "clang/Tooling/CommonOptionsParser.h"
#include "clang/Tooling/Tooling.h"

using namespace llvm;
using namespace clang;

//===----------------------------------------------------------------------===//
// Command line options
//===----------------------------------------------------------------------===//
static llvm::cl::OptionCategory LACCategory("lacommenter options");

//===----------------------------------------------------------------------===//
// PluginASTAction
//===----------------------------------------------------------------------===//
class LACPluginAction : public PluginASTAction {
public:
  bool ParseArgs(const CompilerInstance &CI,
                 const std::vector<std::string> &args) override {
    return true;
  }

  std::unique_ptr<ASTConsumer> CreateASTConsumer(CompilerInstance &CI,
                                                 StringRef file) override {
    LACRewriter.setSourceMgr(CI.getSourceManager(), CI.getLangOpts());
    return std::make_unique<LACommenterASTConsumer>(LACRewriter);
  }

private:
  Rewriter LACRewriter;
};

//===----------------------------------------------------------------------===//
// Main driver code.
//===----------------------------------------------------------------------===//
int main(int Argc, const char **Argv) {
  clang::tooling::CommonOptionsParser OptionsParser(Argc, Argv, LACCategory);
  clang::tooling::ClangTool Tool(OptionsParser.getCompilations(),
                                 OptionsParser.getSourcePathList());

  return Tool.run(
      clang::tooling::newFrontendActionFactory<LACPluginAction>().get());
}
