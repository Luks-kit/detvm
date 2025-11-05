#pragma once
#include "expr_ast.hpp"

DEFINE_NODE_BASE(Stmt, StmtKind { ExprStmt, VarDecl, If, While, Return, Block })

DEFINE_NODE_TYPE(Stmt, ExprStmt, StmtKind::ExprStmt,
    ExprPtr expr;
)
DEFINE_NODE_TYPE(Stmt, VarDeclStmt, StmtKind::VarDecl,
    std::string name;
    ExprPtr initializer;
)
DEFINE_NODE_TYPE(Stmt, IfStmt, StmtKind::If,
    ExprPtr condition;
    StmtPtr thenBranch;
    StmtPtr elseBranch;
)
DEFINE_NODE_TYPE(Stmt, BlockStmt, StmtKind::Block,
    std::vector<StmtPtr> statements;
)

BEGIN_NODE_TABLE(Stmt)
    NODE_ENTRY(ExprStmt)
    NODE_ENTRY(VarDeclStmt)
    NODE_ENTRY(IfStmt)
    NODE_ENTRY(BlockStmt)
END_NODE_TABLE(Stmt)
