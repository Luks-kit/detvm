#pragma once
#include "expr_ast.hpp"

enum class StmtKind { Expression, VarDecl, If, While, Return, Block };

// ----------------- Base -----------------
struct StmtBase {
    const StmtKind kind;
    StmtBase(StmtKind k) : kind(k) {}
};

using StmtPtr = Ptr<StmtBase>;

// ----------------- Derived -----------------
struct ExprStmt    { StmtBase base; ExprPtr expr;
    ExprStmt(ExprPtr e) : base(StmtKind::Expression), expr(std::move(e)) {} };

struct VarDeclStmt { StmtBase base; std::string name; ExprPtr initializer;
    VarDeclStmt(std::string n, ExprPtr init) : base(StmtKind::VarDecl), name(std::move(n)), initializer(std::move(init)) {} };

struct IfStmt      { StmtBase base; ExprPtr condition; StmtPtr thenBranch; StmtPtr elseBranch;
    IfStmt(ExprPtr cond, StmtPtr t, StmtPtr e)
        : base(StmtKind::If), condition(std::move(cond)), thenBranch(std::move(t)), elseBranch(std::move(e)) {} };

struct WhileStmt   { StmtBase base; ExprPtr condition; StmtPtr body;
    WhileStmt(ExprPtr cond, StmtPtr b) : base(StmtKind::While), condition(std::move(cond)), body(std::move(b)) {} };

struct ReturnStmt  { StmtBase base; ExprPtr value;
    ReturnStmt(ExprPtr v) : base(StmtKind::Return), value(std::move(v)) {} };

struct BlockStmt   { StmtBase base; std::vector<StmtPtr> statements;
    BlockStmt(std::vector<StmtPtr> stmts) : base(StmtKind::Block), statements(std::move(stmts)) {} };

// ----------------- Factories -----------------
inline StmtPtr makeExprStmt(ExprPtr e) 
{ 
    auto* s = new ExprStmt(std::move(e));
    return StmtPtr(&s->base); 
}

inline StmtPtr makeVarDeclStmt(const std::string& name, ExprPtr init)
{ 
    auto* s = new VarDeclStmt(name, std::move(init));
    return StmtPtr(&s->base); 
}

inline StmtPtr makeIfStmt(ExprPtr cond, StmtPtr t, StmtPtr e)
{ 
    auto* s = new IfStmt(std::move(cond), std::move(t), std::move(e));
    return StmtPtr(&s->base);
 }

inline StmtPtr makeWhileStmt(ExprPtr cond, StmtPtr body)
{  
    auto* s = (new WhileStmt(std::move(cond), std::move(body)));
    return StmtPtr(&s->base); 
}

inline StmtPtr makeReturnStmt(ExprPtr val)
{  
    auto* s = (new ReturnStmt(std::move(val)));
    return StmtPtr(&s->base);
}

inline StmtPtr makeBlockStmt(std::vector<StmtPtr> stmts)
{   
    auto* s = (new BlockStmt(std::move(stmts))); 
    return StmtPtr(&s->base);
}
