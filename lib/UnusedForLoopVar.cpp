//==============================================================================
// FILE:
//  UnusedForLoopVar.cpp
//
// DESCRIPTION:
//
// USAGE:
//   * clang -cc1 -load libUnusedForLoopVar%.dylib '\'
//      -plugin unused-for-loop-variable test/UnusedForLoopVar_nested.cpp
//
// License: The Unlicense
//==============================================================================
#include "UnusedForLoopVar.h"

#include "clang/AST/Decl.h"
#include "clang/AST/RecursiveASTVisitor.h"
#include "clang/AST/StmtCXX.h"
#include "clang/ASTMatchers/ASTMatchers.h"
#include "clang/Frontend/CompilerInstance.h"
#include "clang/Frontend/FrontendPluginRegistry.h"

using namespace clang;
using namespace ast_matchers;

static std::string NameToMatch("[U|u][N|n][U|u][S|s][E|e][D|d]");

namespace {

AST_MATCHER_P(NamedDecl, matchesAnyListedName, std::string, Name) {
  return llvm::Regex(Name).match(Node.getName());
}
} // namespace

//-----------------------------------------------------------------------------
// UnusedForLoopVarMatcher - implementation
//-----------------------------------------------------------------------------
void UnusedForLoopVarMatcher::run(const MatchFinder::MatchResult &Result) {
  // ASTContext is used to retrieve the source location
  ASTContext *Ctx = Result.Context;

  if (Result.Nodes.getNodeAs<clang::ForStmt>("RegularForLoop"))
    return runRegularForLoop(Result, Ctx);

  if (Result.Nodes.getNodeAs<clang::CXXForRangeStmt>("RangeForLoop"))
    return runRangeForLoop(Result, Ctx);
}

void UnusedForLoopVarMatcher::runRegularForLoop(
    const MatchFinder::MatchResult &Result, ASTContext *Ctx) {
  // Retrieve the matched nodes
  const ForStmt *RegularForLoop =
      Result.Nodes.getNodeAs<clang::ForStmt>("RegularForLoop");
  const DeclStmt *LoopVarRegularFor =
      Result.Nodes.getNodeAs<clang::DeclStmt>("LoopVarRegularFor");

  // Don't follow #include files
  if (RegularForLoop &&
      !Ctx->getSourceManager().isWrittenInMainFile(RegularForLoop->getForLoc()))
    return;

  // Loop over all variables declared in the init statement of the loop
  for (auto *LoopVar : LoopVarRegularFor->decls()) {
    // If LoopVar _is used_ there's nothing to report
    if (LoopVar->isUsed(true))
      continue;

    // LoopVar is not used -> create a warning
    DiagnosticsEngine &DE = Ctx->getDiagnostics();
    unsigned DiagID =
        DE.getCustomDiagID(DiagnosticsEngine::Warning,
                           "(AST Matcher) regular for-loop variable not used");
    auto DB = DE.Report(LoopVar->getLocation(), DiagID);
    DB.AddSourceRange(
        clang::CharSourceRange::getCharRange(LoopVar->getSourceRange()));
  }
}

void UnusedForLoopVarMatcher::runRangeForLoop(
    const MatchFinder::MatchResult &Result, ASTContext *Ctx) {
  // Retrieve the matched nodes
  const VarDecl *LoopVar = Result.Nodes.getNodeAs<clang::VarDecl>("LoopVar");
  const CXXForRangeStmt *RangeForLoop =
      Result.Nodes.getNodeAs<clang::CXXForRangeStmt>("RangeForLoop");

  // Don't follow #include files: range for-loop
  if (RangeForLoop &&
      !Ctx->getSourceManager().isWrittenInMainFile(RangeForLoop->getForLoc()))
    return;

  // If LoopVar _is used_ there's nothing to report
  if (LoopVar->isUsed(true))
    return;

  // LoopVar is not used -> create a warning
  DiagnosticsEngine &DE = Ctx->getDiagnostics();
  unsigned DiagID =
      DE.getCustomDiagID(DiagnosticsEngine::Warning,
                         "(AST Matcher) range for-loop variable not used");
  auto DB = DE.Report(LoopVar->getLocation(), DiagID);
  DB.AddSourceRange(
      clang::CharSourceRange::getCharRange(LoopVar->getSourceRange()));
}

//-----------------------------------------------------------------------------
// UnusedForLoopVarVisitor implementation
//-----------------------------------------------------------------------------
bool UnusedForLoopVarVisitor::TraverseForStmt(ForStmt *S) {
  // Get DeclStmt corresponding to init in 'for (init;cond;inc)'
  auto InitStmt = dyn_cast<DeclStmt>(S->getInit());

  // Loop over all variables declared in the init S and add them to
  // VarsDeclaredInLoopInitStmt. Note that we care only about indices that _are
  // declared_ in the init statement, e.g. loops like this one:
  //    for (int i = 0; cond; inc)
  // We ignore loop indices declared outside the init statement, e.g. in loops
  // like the following:
  //    int i;
  //    for (i = 0; cond; inc)
  // The former are identified by checking the type of the init nodes is
  // VarDecl.
  for (auto *InitVar : InitStmt->decls()) {
    VarDecl const *InitVarDecl = nullptr;
    InitVarDecl = dyn_cast<VarDecl>(InitVar);
    if (InitVarDecl && !llvm::Regex(NameToMatch).match(InitVarDecl->getName()))
      VarsDeclaredInLoopInitStmt.push_back(InitVarDecl);
  }

  // If there are no variables declared in the init statement, return true to
  // continue traversing the AST.
  if (VarsDeclaredInLoopInitStmt.empty())
    return true;

  // Traverse the body of the loop and record every use of the loop indices
  auto Ret = RecursiveASTVisitor::TraverseStmt(S->getBody());

  // For every loop variable declaration, LoopVarDecl, check whether the
  // corresponding variable is used?
  for (auto LoopVarDecl : VarsDeclaredInLoopInitStmt) {
    if (UsedVars.erase(LoopVarDecl) == 0) {
      DiagnosticsEngine &DE = Ctx->getDiagnostics();
      unsigned DiagID = DE.getCustomDiagID(
          DiagnosticsEngine::Warning,
          "(Recursive AST Visitor) regular for-loop variable not used");
      auto DB = DE.Report(LoopVarDecl->getLocation(), DiagID);
      DB.AddSourceRange(
          clang::CharSourceRange::getCharRange(LoopVarDecl->getSourceRange()));
    }
  }

  // Clean-up before visiting the next loop
  VarsDeclaredInLoopInitStmt.clear();
  UsedVars.clear();

  // TODO: What are we returning here?
  return Ret;
}

bool UnusedForLoopVarVisitor::VisitDeclRefExpr(DeclRefExpr const *Stmt) {
  // Get the declaration of the variable being referenced
  auto ReferencedVarDecl = dyn_cast<VarDecl>(Stmt->getDecl());

  // TODO - when do we not hit this?
  if (!ReferencedVarDecl)
    return true;

  // If this is a variable declared in the lopp's init statement, record this
  // for later.
  if (std::find(VarsDeclaredInLoopInitStmt.begin(),
                VarsDeclaredInLoopInitStmt.end(),
                ReferencedVarDecl) != VarsDeclaredInLoopInitStmt.end())
    UsedVars.insert(ReferencedVarDecl);

  return true;
}

//-----------------------------------------------------------------------------
// UnusedForLoopVarASTConsumer - implemenentation
//-----------------------------------------------------------------------------
UnusedForLoopVarASTConsumer::UnusedForLoopVarASTConsumer(ASTContext &CTX, SourceManager &InSM)
    : SM(InSM), UFLVVisitor(&CTX) {
  // ASTMatcher for a range for-loop
  auto LoopVar = varDecl(unless(matchesAnyListedName(NameToMatch)));
  StatementMatcher For =
      cxxForRangeStmt(hasLoopVariable(LoopVar.bind("LoopVar")))
          .bind("RangeForLoop");

  Matcher.addMatcher(For, &UFLVMatcher);

  // ASTMatcher for a regular for-loop
  const auto LoopVarsInit =
      declStmt(unless(forEach(varDecl(matchesAnyListedName(NameToMatch)))))
          .bind("LoopVarRegularFor");
  auto ForStmt = forStmt(hasLoopInit(LoopVarsInit)).bind("RegularForLoop");

  Matcher.addMatcher(ForStmt, &UFLVMatcher);

  // TODO: clang-tidy uses `this`
}

//-----------------------------------------------------------------------------
// FrotendAction
//-----------------------------------------------------------------------------
class UFLVMatcherPluginAction : public PluginASTAction {
public:
  // Our plugin can alter behavior based on the command line options
  bool ParseArgs(const CompilerInstance &,
                 const std::vector<std::string> &) override {
    return true;
  }

  // Returns our ASTConsumer per translation unit.
  std::unique_ptr<ASTConsumer> CreateASTConsumer(CompilerInstance &CI,
                                                 StringRef file) override {
    return std::make_unique<UnusedForLoopVarASTConsumer>(CI.getASTContext(),
                                                         CI.getSourceManager());
  }
};

//-----------------------------------------------------------------------------
// Registration
//-----------------------------------------------------------------------------
static FrontendPluginRegistry::Add<UFLVMatcherPluginAction>
    X(/*Name=*/"UFLV",
      /*Desc=*/"Find unused for-loop variables");
