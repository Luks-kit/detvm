#include "ast.hpp"

namespace deltora::make {

Expr literal(long v) { return Expr{Literal{v}}; }
Expr literal(double v) { return Expr{Literal{v}}; }
Expr literal(const std::string& v) { return Expr{Literal{v}}; }
Expr ident(const std::string& n) { return Expr{Ident{n}}; }
Expr binary(char op, Expr lhs, Expr rhs) { return Expr{BinaryOp{op, std::make_unique<Expr>(lhs), std::make_unique<Expr>(rhs)}}; }
Expr call(const std::string& name, std::vector<std::pair<std::string, Expr>> args) { return Expr{Call{name, args}}; }

} // namespace deltora::make