//==============================================================================
// FILE:
//    CodeRefactorommenterMain.cpp
//
// DESCRIPTION:
//    The processing of input arguments (defined with the llvm::cl API) so that
//    they can be used inside CodeRefactorPluginAction was inspired by the
//    implementation of clang-check:
//    https://github.com/llvm/llvm-project/blob/release/10.x/clang/tools/clang-check/ClangCheck.cpp#L137
//    It relies on using variables that are global inside this TU. Perhaps this
//    is not idea, but I could not find a better way (neither in documentation
//    nor in code).
//
// USAGE:
//
//
// License: The Unlicense
//==============================================================================
#include "CodeRefactor.h"

#include "clang/Frontend/CompilerInstance.h"
#include "clang/Frontend/FrontendPluginRegistry.h"
#include "clang/Tooling/CommonOptionsParser.h"
#include "clang/Tooling/Refactoring.h"
#include "llvm/Support/CommandLine.h"

using namespace llvm;
using namespace clang;

//===----------------------------------------------------------------------===//
// Command line options
//===----------------------------------------------------------------------===//
static cl::OptionCategory CodeRefactorCategory("ct-code-refactor options");

static cl::opt<std::string> ClassNameOpt{
    "class-name",
    cl::desc("The name of the class/struct that the method to be renamed "
             "belongs to"),
    cl::Required, cl::init(""), cl::cat(CodeRefactorCategory)};
static cl::opt<std::string> OldNameOpt{
    "old-name", cl::desc("The current name of the method to be renamed"),
    cl::Required, cl::init(""), cl::cat(CodeRefactorCategory)};
static cl::opt<std::string> NewNameOpt{
    "new-name", cl::desc("The new name of the method to be renamed"),
    cl::Required, cl::init(""), cl::cat(CodeRefactorCategory)};

//===----------------------------------------------------------------------===//
// PluginASTAction
//===----------------------------------------------------------------------===//
class CodeRefactorPluginAction : public PluginASTAction {
public:
  explicit CodeRefactorPluginAction(){};
  // Not used
  bool ParseArgs(const CompilerInstance &CI,
                 const std::vector<std::string> &args) override {
    return true;
  }

  // Output the edit buffer for this translation unit
  // void EndSourceFileAction() override {
  //   RewriterForCodeRefactor
  //       .getEditBuffer(RewriterForCodeRefactor.getSourceMgr().getMainFileID())
  //       .write(llvm::outs());
  // }

  std::unique_ptr<ASTConsumer> CreateASTConsumer(CompilerInstance &CI,
                                                 StringRef file) override {
    RewriterForCodeRefactor.setSourceMgr(CI.getSourceManager(),
                                         CI.getLangOpts());
    return std::make_unique<CodeRefactorASTConsumer>(
        RewriterForCodeRefactor, ClassNameOpt, OldNameOpt, NewNameOpt);
  }

private:
  Rewriter RewriterForCodeRefactor;
};

//===----------------------------------------------------------------------===//
// Main driver code.
//===----------------------------------------------------------------------===//
int main(int Argc, const char **Argv) {
  clang::tooling::CommonOptionsParser OptionsParser(Argc, Argv,
                                                    CodeRefactorCategory);
  clang::tooling::RefactoringTool Tool(OptionsParser.getCompilations(),
                                 OptionsParser.getSourcePathList());

  return Tool.runAndSave(
      clang::tooling::newFrontendActionFactory<CodeRefactorPluginAction>()
          .get());
}
