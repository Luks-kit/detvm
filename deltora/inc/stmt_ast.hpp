#pragma once
#include "expr_ast.hpp"

enum class StmtKind { ExprStmt, VarDecl, If, While, Return, Block };
DEFINE_NODE_BASE(Stmt, StmtKind)
DEFINE_NODE_PTR(Stmt)

DEFINE_NODE_TYPE(Stmt, StmtKind, ExprStmt, StmtKind::ExprStmt,
    ExprPtr expr;
)
DEFINE_NODE_TYPE(Stmt, StmtKind, VarDeclStmt, StmtKind::VarDecl,
    std::string name;
    ExprPtr initializer;
)
DEFINE_NODE_TYPE(Stmt, StmtKind, IfStmt, StmtKind::If,
    ExprPtr condition;
    StmtPtr thenBranch;
    StmtPtr elseBranch;
)
DEFINE_NODE_TYPE(Stmt, StmtKind, BlockStmt, StmtKind::Block,
    std::vector<StmtPtr> statements;
)

BEGIN_NODE_TABLE(Stmt)
    NODE_ENTRY(ExprStmt)
    NODE_ENTRY(VarDeclStmt)
    NODE_ENTRY(IfStmt)
    NODE_ENTRY(BlockStmt)
END_NODE_TABLE()
