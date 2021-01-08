//==============================================================================
// FILE:
//    Obfuscator.cpp
//
// DESCRIPTION:
//  Obfuscates the integer addition (`+`) and subtraction (`-`) expressions as
//  follows:
//
//    * a + b --> (a ^ b) + 2 * (a & b)
//    * a - b --> (a + ~b) + 1
//
//  Either of the operands can be a literal or a variable declared earlier. The
//  modified file is printed to stdout. Additions and subtractions are modified
//  seperately, i.e. the plugin runs twice over the input file. First time it
//  modifies the additions, second time it modifies the subtractions.
//
// USAGE:
//    * clang -cc1 -load <BUILD_DIR>/lib/libObfuscator.dylib `\`
//        -plugin Obfuscator <input-cpp-file>
//
// License: The Unlicense
//==============================================================================
#include "Obfuscator.h"

#include "clang/AST/Expr.h"
#include "clang/AST/RecursiveASTVisitor.h"
#include "clang/ASTMatchers/ASTMatchers.h"
#include "clang/Frontend/CompilerInstance.h"
#include "clang/Frontend/FrontendPluginRegistry.h"
#include "clang/Rewrite/Frontend/FixItRewriter.h"
#include "llvm/ADT/APInt.h"
#include "llvm/Support/Casting.h"
#include "llvm/Support/raw_ostream.h"

using namespace clang;
using namespace ast_matchers;

//-----------------------------------------------------------------------------
// ObfuscatorMatcherForSub - implementation
//-----------------------------------------------------------------------------
void ObfuscatorMatcherForSub::run(const MatchFinder::MatchResult &Result) {
  const auto &Op = Result.Nodes.getNodeAs<clang::BinaryOperator>("op");

  std::string LHSAsStr, RHSAsStr;
  SourceRange RangeLHS, RangeRHS;

  // Grab the left-hand-side of the expression
  if (const auto *LHSWithIC =
          Result.Nodes.getNodeAs<clang::ImplicitCastExpr>("lhs")) {
    const auto *LHS = LHSWithIC->IgnoreImplicit();

    LHSAsStr = dyn_cast<clang::DeclRefExpr>(LHS)->getDecl()->getName().str();
    RangeLHS = LHS->getSourceRange();
  } else if (const auto *LHS =
                 Result.Nodes.getNodeAs<clang::IntegerLiteral>("lhs")) {
    LHSAsStr = std::to_string(LHS->getValue().getZExtValue());
    RangeLHS = LHS->getSourceRange();
  } else {
    llvm_unreachable("Unsupported case in the handler for `-`");
  }

  // Grab the right-hand-side of the expression
  if (const auto *RHSWithIC =
          Result.Nodes.getNodeAs<clang::ImplicitCastExpr>("rhs")) {
    const auto *RHS = RHSWithIC->IgnoreImplicit();

    RHSAsStr = dyn_cast<clang::DeclRefExpr>(RHS)->getDecl()->getName().str();
    RangeRHS = RHS->getSourceRange();
  } else if (const auto *RHS =
                 Result.Nodes.getNodeAs<clang::IntegerLiteral>("rhs")) {
    RHSAsStr = std::to_string(RHS->getValue().getZExtValue());
    RangeRHS = RHS->getSourceRange();
  } else {
    llvm_unreachable("Unsupported case in the handler for `-`");
  }

  // Rewrite the expression
  ObfuscatorRewriter.ReplaceText(RangeLHS,
                                 "(" + LHSAsStr + " + ~" + RHSAsStr + ")");
  ObfuscatorRewriter.ReplaceText(Op->getOperatorLoc(), "+");
  ObfuscatorRewriter.ReplaceText(RangeRHS, "1");

  Changed = true;
}

void ObfuscatorMatcherForSub::onEndOfTranslationUnit() {
  if (Changed == false)
    return;

  // Output to stdout
  ObfuscatorRewriter
      .getEditBuffer(ObfuscatorRewriter.getSourceMgr().getMainFileID())
      .write(llvm::outs());
}

//-----------------------------------------------------------------------------
// ObfuscatorMatcherForAdd - implementation
//-----------------------------------------------------------------------------
void ObfuscatorMatcherForAdd::run(const MatchFinder::MatchResult &Result) {
  std::string LHSAsStr, RHSAsStr;
  SourceRange RangeLHS, RangeRHS;

  if (Result.Nodes.getMap().size() == 0)
    llvm_unreachable("Unsupported case in the handler for `+`");

  // Grab the left-hand-side of the expression
  if (const auto *LHSWithIC =
          Result.Nodes.getNodeAs<clang::ImplicitCastExpr>("lhs")) {
    const auto *LHS = LHSWithIC->IgnoreImplicit();

    LHSAsStr = dyn_cast<clang::DeclRefExpr>(LHS)->getDecl()->getName().str();
    RangeLHS = LHS->getSourceRange();
  } else if (const auto *LHS =
                 Result.Nodes.getNodeAs<clang::IntegerLiteral>("lhs")) {
    LHSAsStr = std::to_string(LHS->getValue().getZExtValue());
    RangeLHS = LHS->getSourceRange();
  } else {
    llvm_unreachable("Unsupported case in the handler for `+`");
  }

  // Grab the right-hand-side of the expression
  if (const auto *RHSWithIC =
          Result.Nodes.getNodeAs<clang::ImplicitCastExpr>("rhs")) {
    const auto *RHS = RHSWithIC->IgnoreImplicit();

    RHSAsStr = dyn_cast<clang::DeclRefExpr>(RHS)->getDecl()->getName().str();
    RangeRHS = RHS->getSourceRange();
  } else if (const auto *RHS =
                 Result.Nodes.getNodeAs<clang::IntegerLiteral>("rhs")) {
    RHSAsStr = std::to_string(RHS->getValue().getZExtValue());
    RangeRHS = RHS->getSourceRange();
  } else {
    llvm_unreachable("Unsupported case in the handler for `+`");
  }

  // Rewrite the expression
  ObfuscatorRewriter.ReplaceText(RangeLHS,
                                 "(" + LHSAsStr + " ^ " + RHSAsStr + ")");
  ObfuscatorRewriter.ReplaceText(RangeRHS,
                                 "2 * (" + LHSAsStr + " & " + RHSAsStr + ")");

  Changed = true;
}

void ObfuscatorMatcherForAdd::onEndOfTranslationUnit() {
  if (Changed == false)
    return;

  // Output to stdout
  ObfuscatorRewriter
      .getEditBuffer(ObfuscatorRewriter.getSourceMgr().getMainFileID())
      .write(llvm::outs());
}

//-----------------------------------------------------------------------------
// ObfuscatorASTConsumer - implementation
//-----------------------------------------------------------------------------
ObfuscatorASTConsumer::ObfuscatorASTConsumer(Rewriter &R)
    : AddHandler(R), SubHandler(R) {
  const auto MatcherAdd =
      binaryOperator(
          hasOperatorName("+"),
          hasLHS(anyOf(implicitCastExpr(hasType(isSignedInteger())).bind("lhs"),
                       integerLiteral().bind("lhs"))),
          hasRHS(anyOf(implicitCastExpr(hasType(isSignedInteger())).bind("rhs"),
                       integerLiteral().bind("rhs"))))
          .bind("op");

  const auto MatcherSub =
      binaryOperator(
          hasOperatorName("-"),
          hasLHS(anyOf(implicitCastExpr(hasType(isSignedInteger())).bind("lhs"),
                       integerLiteral().bind("lhs"))),
          hasRHS(anyOf(implicitCastExpr(hasType(isSignedInteger())).bind("rhs"),
                       integerLiteral().bind("rhs"))))
          .bind("op");

  Matcher.addMatcher(MatcherAdd, &AddHandler);
  Matcher.addMatcher(MatcherSub, &SubHandler);
}

//-----------------------------------------------------------------------------
// FrotendAction
//-----------------------------------------------------------------------------
class ObfuscatorPluginAction : public PluginASTAction {
public:
  // Our plugin can alter behavior based on the command line options
  bool ParseArgs(const CompilerInstance &,
                 const std::vector<std::string> &) override {
    return true;
  }

  // Returns our ASTConsumer per translation unit.
  std::unique_ptr<ASTConsumer> CreateASTConsumer(CompilerInstance &CI,
                                                 StringRef file) override {
    RewriterForObfuscator.setSourceMgr(CI.getSourceManager(), CI.getLangOpts());
    return std::make_unique<ObfuscatorASTConsumer>(RewriterForObfuscator);
  }

private:
  Rewriter RewriterForObfuscator;
};

//-----------------------------------------------------------------------------
// Registration
//-----------------------------------------------------------------------------
static FrontendPluginRegistry::Add<ObfuscatorPluginAction>
    X(/*Name=*/"Obfuscator",
      /*Desc=*/"Mixed Boolean Algebra Transformations");
