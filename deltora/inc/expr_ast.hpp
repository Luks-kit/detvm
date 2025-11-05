#pragma once

#include "node_macros.hpp"
#include "literal.hpp"

enum class ExprKind { Literal, Variable, Binary, Unary, Call };

// ----------------- Base -----------------
struct ExprBase {
    const ExprKind kind;
    ExprBase(ExprKind k) : kind(k) {}
};

using ExprPtr = Ptr<ExprBase>;

// ----------------- Derived -----------------
struct LiteralExpr  { ExprBase base; LiteralPtr value;
    LiteralExpr(LiteralPtr v) : base(ExprKind::Literal), value(std::move(v)) {} };

struct VariableExpr { ExprBase base; std::string name;
    VariableExpr(const std::string& n) : base(ExprKind::Variable), name(n) {} };

struct BinaryExpr   { ExprBase base; ExprPtr left, right; std::string op;
    BinaryExpr(ExprPtr l, ExprPtr r, std::string o)
        : base(ExprKind::Binary), left(std::move(l)), right(std::move(r)), op(std::move(o)) {} };

struct UnaryExpr    { ExprBase base; ExprPtr operand; std::string op;
    UnaryExpr(ExprPtr opnd, std::string o)
        : base(ExprKind::Unary), operand(std::move(opnd)), op(std::move(o)) {} };

struct CallExpr     { ExprBase base; std::string callee; std::vector<ExprPtr> args;
    CallExpr(std::string c, std::vector<ExprPtr> a)
        : base(ExprKind::Call), callee(std::move(c)), args(std::move(a)) {} };

// ----------------- Factories -----------------
inline ExprPtr makeLiteralExpr(LiteralPtr v)
{
    auto* e = new LiteralExpr(std::move(v));
    return ExprPtr(&e->base);
}

inline ExprPtr makeVariableExpr(const std::string& n)
{
    auto* e = new VariableExpr(n);
    return ExprPtr(&e->base);
}

inline ExprPtr makeBinaryExpr(ExprPtr l, ExprPtr r, const std::string& op)
{
    auto* e = new BinaryExpr(std::move(l), std::move(r), op);
    return ExprPtr(&e->base);
}

inline ExprPtr makeUnaryExpr(ExprPtr operand, const std::string& op)
{
    auto* e = new UnaryExpr(std::move(operand), op);
    return ExprPtr(&e->base);
}

inline ExprPtr makeCallExpr(const std::string& callee, std::vector<ExprPtr> args)
{
    auto* e = new CallExpr(callee, std::move(args));
    return ExprPtr(&e->base);
}
