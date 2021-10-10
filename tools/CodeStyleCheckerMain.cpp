//==============================================================================
// FILE:
//    CodeStyleCheckerMain.cpp
//
// DESCRIPTION:
//    A standalone tool that runs the CodeStyleChecker plugin. See
//    CodeStyleChecker.cpp for a complete description.
//
// USAGE:
//  Main TU only:
//    * ct-code-style-checker input-file.cpp
//  All TUs (the main file and the #includ-ed header files)
//    * ct-code-style-checker -main-tu-only=false input-file.cpp
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

static cl::opt<bool> MainTuOnly{
    "main-tu-only",
    cl::desc("Only run on the main translation unit "
             "(e.g. ignore included header files)"),
    cl::init(true), cl::cat(CSCCategory)};

//===----------------------------------------------------------------------===//
// PluginASTAction
//===----------------------------------------------------------------------===//
class CSCPluginAction : public PluginASTAction {
public:
  bool ParseArgs(const CompilerInstance &CI,
                 const std::vector<std::string> &args) override {
    return true;
  }

  std::unique_ptr<ASTConsumer> CreateASTConsumer(CompilerInstance &CI,
                                                 StringRef file) override {
    return std::make_unique<CodeStyleCheckerASTConsumer>(
        &CI.getASTContext(), MainTuOnly, CI.getSourceManager());
  }
};

//===----------------------------------------------------------------------===//
// Main driver code.
//===----------------------------------------------------------------------===//
int main(int Argc, const char **Argv) {
  Expected<tooling::CommonOptionsParser> eOptParser = clang::tooling::CommonOptionsParser::create(Argc, Argv, CSCCategory);
  if (auto E = eOptParser.takeError()) {
    errs() << "Problem constructing CommonOptionsParser " << toString(std::move(E)) << '\n';
    return EXIT_FAILURE;
  }
  clang::tooling::ClangTool Tool(eOptParser->getCompilations(),
                                 eOptParser->getSourcePathList());

  return Tool.run(
      clang::tooling::newFrontendActionFactory<CSCPluginAction>().get());
}
