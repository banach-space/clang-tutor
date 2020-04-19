//==============================================================================
// FILE:
//    LACommenter.cpp
//
// DESCRIPTION:
//    Literal argument commenter - adds in-line C-style comments as reccomended
//    in LLVM's coding guideline:
//      * https://llvm.org/docs/CodingStandards.html#comment-formatting
//    This plugin will comment the following literal argument types:
//      * integer, character, floating, boolean, string
//
//    Below is an example for a function that takes one integer argument:
//    ```c
//    extern void foo(int some_arg);
//
//    void(bar) {
//      foo(/*some_arg=*/123);
//    }
//    ```
//
//    TODO: Update the following
//    This plugin modifies the input file in-place.
//
// USAGE:
//    1. As a loadable Clang plugin:
//      clang -cc1 -load <BUILD_DIR>/lib/libLACommenter.dylib \
//        -plugin LACPlugin <input-cpp-file>
//    2. As a standalone tool:
//      <BUILD_DIR>/bin/lacommenter <input-cpp-file>
//
// REFERENCES:
//    Based on an example by Peter Smith:
//      * https://s3.amazonaws.com/connect.linaro.org/yvr18/presentations/yvr18-223.pdf
//
// License: The Unlicense
//==============================================================================
#include "LACommenter.h"

#include "clang/AST/RecursiveASTVisitor.h"
#include "clang/Frontend/CompilerInstance.h"
#include "clang/Frontend/FrontendPluginRegistry.h"
#include "llvm/Support/raw_ostream.h"

using namespace clang;
using namespace ast_matchers;

//-----------------------------------------------------------------------------
// LACommenter - implementation
//-----------------------------------------------------------------------------
void LACommenter::run(const MatchFinder::MatchResult &Result) {
  // ASTContext is used to retrieve the source location
  ASTContext *Ctx = Result.Context;

  // Callee and caller are accessed via .bind("callee") and .bind("caller"),
  // respecitvely, from the ASTMatcher
  const FunctionDecl *CalleeDecl =
      Result.Nodes.getNodeAs<clang::FunctionDecl>("callee");
  const CallExpr *TheCall = Result.Nodes.getNodeAs<clang::CallExpr>("caller");

  // Basic sanity checking
  assert(TheCall && CalleeDecl &&
         "The matcher matched, so callee and caller should be non-null");

  // No arguments means there's nothing to comment
  if (CalleeDecl->parameters().empty())
    return;

  // Get the arguments
  Expr const *const *Args = TheCall->getArgs();
  size_t NumArgs = TheCall->getNumArgs();

  // If this is a call to an overloaded operator (e.g. `+`), then the first
  // parameter is the object itself (i.e. `this` pointer). Skip it.
  if (isa<CXXOperatorCallExpr>(TheCall)) {
    Args++;
    NumArgs--;
  }

  // For each argument match it with the callee parameter. If it is an integer,
  // float, boolean, character or string literal insert a comment.
  for (unsigned Idx = 0; Idx < NumArgs; Idx++) {
    const Expr *AE = Args[Idx]->IgnoreParenCasts();

    if (!dyn_cast<IntegerLiteral>(AE) && !dyn_cast<CXXBoolLiteralExpr>(AE) &&
        !dyn_cast<FloatingLiteral>(AE) && !dyn_cast<StringLiteral>(AE) &&
        !dyn_cast<CharacterLiteral>(AE))
      continue;

    // Parameter declaration
    ParmVarDecl *ParamDecl = CalleeDecl->parameters()[Idx];

    // Source code locations (parameter and argument)
    FullSourceLoc ParamLocation = Ctx->getFullLoc(ParamDecl->getBeginLoc());
    FullSourceLoc ArgLoc = Ctx->getFullLoc(AE->getBeginLoc());

    if (ParamLocation.isValid() && !ParamDecl->getDeclName().isEmpty() &&
        EditedLocations.insert(ArgLoc).second)
      // Insert the comment immediately before the argument
      LACRewriter.InsertText(
          ArgLoc,
          (Twine("/*") + ParamDecl->getDeclName().getAsString() + "=*/").str());
  }
}

void LACommenter::onEndOfTranslationUnit() {
  // Replace in place
  // LACRewriter.overwriteChangedFiles();

  // Output to stdout
  LACRewriter.getEditBuffer(LACRewriter.getSourceMgr().getMainFileID())
      .write(llvm::outs());

}

LACASTConsumer::LACASTConsumer(Rewriter &R) : LAC(R) {
  // We use almost the same syntax as the ASTMatcher prototyped in
  // clang-query. The changes are the .bind(string) additions so that we
  // can access these once the match has occurred.
  StatementMatcher CallSiteMatcher =
      callExpr(
          allOf(callee(functionDecl(unless(isVariadic())).bind("callee")),
                unless(cxxMemberCallExpr(
                    on(hasType(substTemplateTypeParmType())))),
                anyOf(hasAnyArgument(ignoringParenCasts(cxxBoolLiteral())),
                      hasAnyArgument(ignoringParenCasts(integerLiteral())),
                      hasAnyArgument(ignoringParenCasts(stringLiteral())),
                      hasAnyArgument(ignoringParenCasts(characterLiteral())),
                      hasAnyArgument(ignoringParenCasts(floatLiteral()))
                      )
                )
          )
          .bind("caller");
  // LAC is the callback that will run when the ASTMatcher finds the pattern
  // above.
  Matcher.addMatcher(CallSiteMatcher, &LAC);
}

//-----------------------------------------------------------------------------
// Registration
//-----------------------------------------------------------------------------
// Implement PluginASTAction rather than ASTFrontendAction
class LACPluginAction : public PluginASTAction {
public:
  // Our plugin can alter behavior based on the command line options
  bool ParseArgs(const CompilerInstance &,
                 const std::vector<std::string> &) override {
    return true;
  }

  // Returns our ASTConsumer per translation unit.
  std::unique_ptr<ASTConsumer> CreateASTConsumer(CompilerInstance &CI,
                                                 StringRef file) override {
    LACRewriter.setSourceMgr(CI.getSourceManager(), CI.getLangOpts());
    return std::make_unique<LACASTConsumer>(LACRewriter);
  }

private:
  Rewriter LACRewriter;
};

static FrontendPluginRegistry::Add<LACPluginAction>
    X(/*Name=*/"LAC",
      /*Desc=*/"Literal Argument Commenter");
