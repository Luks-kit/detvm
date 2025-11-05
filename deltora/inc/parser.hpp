#pragma once
#include "lexer.hpp"
#include "expr_ast.hpp"

namespace det {

class Parser {
public:
    explicit Parser(Lexer& lexer) : lexer(lexer) {}

    ExprPtr parse_expr();

private:
    Lexer& lexer;

    // Pratt
    int get_precedence(TokenKind kind);
    ExprPtr parse_primary();
    ExprPtr parse_unary();
    ExprPtr parse_binary_rhs(int prec, ExprPtr lhs);

    // helpers
    const Token& peek() const { return lexer.peek(); }
    const Token& advance() { return lexer.advance(); }
    bool match(TokenKind k) { return lexer.match(k); }
    bool check(TokenKind k) const { return lexer.check(k); }
    bool eof() const { return lexer.eof(); }
};

} // namespace det
