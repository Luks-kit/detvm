#pragma once
#include <string>
#include <vector>
#include <variant>
#include <optional>
#include <memory>

namespace deltora {

// === Expressions ===
struct Literal;
struct Ident;
struct UnaryOp;
struct RefOp;
struct BinaryOp;
struct MemberAccess;
struct ArrayAccess;
struct Call;
struct WhenExpr;



// === Expression Node Types ===
struct Literal {
    std::variant<long, double, char, std::string, bool> value;
};

struct Ident {
    std::string name;
};

struct UnaryOp {
    char op;
    std::unique_ptr<Expr> operand;
};

struct RefOp {
    char mode; // 'v','e','m'
    std::unique_ptr<Expr> operand;
};

struct BinaryOp {
    char op;
    std::unique_ptr<Expr> lhs;
    std::unique_ptr<Expr> rhs;
};

struct MemberAccess {
    std::unique_ptr<Expr> head;
    std::string member;
};

struct ArrayAccess {
    std::unique_ptr<Expr> head;
    std::unique_ptr<Expr> index;
};

struct Call {
    std::string name;
    std::vector<std::pair<std::string, Expr>> args;
};

struct Pattern {
    std::string variant_name;
    std::vector<std::string> binds;
};

struct WhenExpr {
    Expr target;
    std::vector<std::pair<Pattern, std::vector<Expr>>> cases;
};

struct Expr {
    using Variant = std::variant<
        Literal, Ident, UnaryOp, RefOp, BinaryOp,
        MemberAccess, ArrayAccess, Call, WhenExpr
    >;
    Variant node;
};

// === Statements ===
struct Decl;
struct Assign;
struct If;
struct While;
struct Check;
struct ExprStmt;
struct Return;
struct Break;
struct Continue;

struct Stmt {
    using Variant = std::variant<
        Decl, Assign, If, While, Check, ExprStmt, Return, Break, Continue
    >;
    Variant node;
};

struct Type {
    std::string name;
    std::vector<Type> args;
};

struct Decl {
    std::string name;
    Type type;
    std::optional<Expr> init;
};

struct Assign {
    Expr lhs;
    Expr rhs;
};

struct If {
    Expr cond;
    std::vector<Stmt> then_branch;
    std::vector<Stmt> else_branch;
};

struct While {
    Expr cond;
    std::vector<Stmt> body;
};

struct Check {
    Expr cond;
};

struct ExprStmt {
    Expr expr;
};

struct Return {
    std::optional<Expr> value;
};

struct Break {};
struct Continue {};

// === Declarations ===
struct Function {
    std::string name;
    std::optional<Type> return_type;
    std::vector<std::pair<std::string, Type>> params;
    std::vector<Stmt> body;
    bool is_method = false;
    bool is_special = false;
};

struct StateVariant {
    std::string name;
    std::optional<Type> type;
};

struct State {
    std::string name;
    std::vector<StateVariant> variants;
};

struct Form {
    std::string name;
    std::vector<Decl> fields;
    std::optional<State> state;
    std::vector<Function> methods;
};

struct Program {
    std::vector<std::variant<Form, Function>> decls;
};

// === Factory Helpers ===
namespace make {
    Expr literal(long v);
    Expr literal(double v);
    Expr literal(const std::string& v);
    Expr ident(const std::string& n);
    Expr binary(char op, Expr lhs, Expr rhs);
    Expr call(const std::string& name, std::vector<std::pair<std::string, Expr>> args);
}

} // namespace deltora
