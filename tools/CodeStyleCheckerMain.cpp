//==============================================================================
// FILE:
//    CodeStyleCheckerMain.cpp
//
// DESCRIPTION:
//
// USAGE:
//
// REFERENCES:
//
// License: The Unlicense
//==============================================================================
#include "CodeStyleChecker.h"

#include "clang/Frontend/CompilerInstance.h"
#include "clang/Frontend/FrontendPluginRegistry.h"
#include "clang/Tooling/CommonOptionsParser.h"
#include "clang/Tooling/Tooling.h"

using namespace llvm;
using namespace clang;

//===----------------------------------------------------------------------===//
// Command line options
//===----------------------------------------------------------------------===//
static llvm::cl::OptionCategory CSCCategory("ct-code-style-checker options");

class CSCPluginAction : public PluginASTAction {
public:
  bool ParseArgs(const CompilerInstance &CI,
                 const std::vector<std::string> &args) override {
    return true;
  }

  std::unique_ptr<ASTConsumer> CreateASTConsumer(CompilerInstance &CI,
                                                 StringRef file) override {
    return std::make_unique<CSCConsumer>(&CI.getASTContext());
  }
};

//===----------------------------------------------------------------------===//
// Main driver code.
//===----------------------------------------------------------------------===//
int main(int Argc, const char **Argv) {
  clang::tooling::CommonOptionsParser OptionsParser(Argc, Argv, CSCCategory);
  clang::tooling::ClangTool Tool(OptionsParser.getCompilations(),
                                 OptionsParser.getSourcePathList());

  return Tool.run(
      clang::tooling::newFrontendActionFactory<CSCPluginAction>().get());
}
