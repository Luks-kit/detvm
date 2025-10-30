#include "parser.hpp"
#include <stdexcept>

namespace deltora {

// === helpers ===
const Token& Parser::peek() const { return tokens[pos]; }
const Token& Parser::advance() { return tokens[pos++]; }
bool Parser::check(TokenKind k) const { return !eof() && peek().kind == k; }
bool Parser::match(TokenKind k) { if (check(k)) { advance(); return true; } return false; }
bool Parser::eof() const { return peek().kind == TokenKind::Eof; }

Parser::Parser(const std::vector<Token>& t) : tokens(t) {}

Program Parser::parseProgram() {
    Program p;
    while (!eof()) {
        // TODO: top-level form/function detection
        advance();
    }
    return p;
}

// === Pratt parsing setup ===

enum Precedence {
    PREC_LOWEST = 0,
    PREC_ASSIGN = 1,
    PREC_OR = 2,
    PREC_AND = 3,
    PREC_EQUALITY = 4,
    PREC_COMPARE = 5,
    PREC_TERM = 6,     // + -
    PREC_FACTOR = 7,   // * /
    PREC_UNARY = 8,
    PREC_POSTFIX = 9,
    PREC_PRIMARY = 10
};

// prefix parse functions
Expr parsePrimary(Parser& P);
Expr parseUnary(Parser& P);

// infix parse functions
Expr parseBinary(Parser& P, Expr left, int prec);

static int precedenceOf(const Token& tok) {
    switch (tok.kind) {
        case TokenKind::Plus:
        case TokenKind::Minus: return PREC_TERM;
        case TokenKind::Star:
        case TokenKind::Slash: return PREC_FACTOR;
        case TokenKind::Equal: return PREC_ASSIGN;
        case TokenKind::Dot: return PREC_POSTFIX;
        default: return PREC_LOWEST;
    }
}

Expr Parser::parseExpr(int prec) {
    // prefix
    Expr left = parsePrimary(*this);

    // infix
    while (true) {
        if (eof()) break;
        int p = precedenceOf(peek());
        if (p <= prec) break;

        Token op = advance();
        Expr rhs = parseExpr(p);
        left = make::binary(op.text[0], std::move(left), std::move(rhs));
    }
    return left;
}

Expr parsePrimary(Parser& P) {
    const Token& t = P.advance();

    switch (t.kind) {
        case TokenKind::Number:
            return make::literal(std::stol(t.text));
        case TokenKind::Identifier:
            return make::ident(t.text);
        case TokenKind::LParen: {
            Expr e = P.parseExpr();
            if (!P.match(TokenKind::RParen))
                throw std::runtime_error("Expected ')'");
            return e;
        }
        default:
            throw std::runtime_error("Unexpected token in expression");
    }
}

} // namespace deltora
