#pragma once
#include "literal.hpp"
#include "node_macros.hpp"

// ---------- Expression base ----------

DEFINE_NODE_BASE(Expr, ExprKind {Literal, Variable, Binary, Unary, Call})

// ---------- Derived types ----------
DEFINE_NODE_TYPE(Expr, LiteralExpr, Kind::Literal,
    LiteralPtr value;
)
DEFINE_NODE_TYPE(Expr, VariableExpr, Kind::Variable,
    std::string name;
)
DEFINE_NODE_TYPE(Expr, BinaryExpr, Kind::Binary,
    ExprPtr left;
    std::string op;
    ExprPtr right;
)
DEFINE_NODE_TYPE(Expr,  UnaryExpr, Kind::Unary,
    std::string op;
    ExprPtr expr;
)
DEFINE_NODE_TYPE(Expr,CallExpr, Kind::Call,
    std::string callee;
    std::vector<ExprPtr> args;
)

// ---------- Deleter table ----------
BEGIN_NODE_TABLE(Expr)
    NODE_ENTRY(LiteralExpr)
    NODE_ENTRY(VariableExpr)
    NODE_ENTRY(BinaryExpr)
    NODE_ENTRY(UnaryExpr)
    NODE_ENTRY(CallExpr)
END_NODE_TABLE(Expr)

// ---------- Factories ----------
DEFINE_NODE_FACTORY(Expr, LiteralExpr, Kind::Literal,  ;)
DEFINE_NODE_FACTORY(Expr, VariableExpr, Kind::Variable, /*INIT*/ ;)
DEFINE_NODE_FACTORY(Expr, BinaryExpr, Kind::Binary, /*INIT*/ ;)
DEFINE_NODE_FACTORY(Expr, UnaryExpr, Kind::Unary, /*INIT*/ ;)
DEFINE_NODE_FACTORY(Expr, CallExpr, Kind::Call, /*INIT*/ ;)
