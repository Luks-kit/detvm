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
        // TODO: detect top-level forms/functions
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

static int precedenceOf(const Token& tok) {
    switch (tok.kind) {
        case TokenKind::Plus:
        case TokenKind::Minus: return PREC_TERM;
        case TokenKind::Star:
        case TokenKind::Slash: return PREC_FACTOR;
        case TokenKind::Dot: return PREC_POSTFIX;
        case TokenKind::Arrow: return PREC_POSTFIX; // ->view/edit/move
        default: return PREC_LOWEST;
    }
}

// === Core Pratt ===
Expr Parser::parseExpr(int prec) {
    Expr left = parseUnary();

    while (true) {
        if (eof()) break;
        int p = precedenceOf(peek());
        if (p <= prec) break;

        Token op = advance();

        // Handle postfix like .member or ->view
        if (op.kind == TokenKind::Dot) {
            if (!check(TokenKind::Identifier))
                throw std::runtime_error("Expected member name after '.'");
            std::string member = advance().text;
            left = Expr{MemberAccess{std::make_unique<Expr>(left), member}};
            continue;
        }
        if (op.kind == TokenKind::Arrow) {
            // Expect view/edit/move
            if (!check(TokenKind::Identifier))
                throw std::runtime_error("Expected 'view', 'edit', or 'move' after '->'");
            std::string mode = advance().text;
            if (mode != "view" && mode != "edit" && mode != "move")
                throw std::runtime_error("Invalid reference mode after '->'");
            left = Expr{RefOp{mode[0], std::make_unique<Expr>(left)}};
            continue;
        }

        // Binary ops
        Expr rhs = parseExpr(p);
        left = make::binary(op.text[0], std::move(left), std::move(rhs));
    }

    return left;
}

Expr Parser::parseUnary() {
    if (match(TokenKind::Minus)) {
        Expr operand = parseUnary();
        return Expr{UnaryOp{'-', std::make_unique<Expr>(operand)}};
    }
    if (match(TokenKind::Bang)) {
        Expr operand = parseUnary();
        return Expr{UnaryOp{'!', std::make_unique<Expr>(operand)}};
    }
    return parsePrimary();
}

Expr Parser::parsePrimary() {
    const Token& t = advance();

    switch (t.kind) {
        case TokenKind::Number:
            return make::literal(std::stol(t.text));
        case TokenKind::String:
            return make::literal(t.text);
        case TokenKind::Identifier:
            // handle function calls
            if (match(TokenKind::LParen)) {
                std::vector<std::pair<std::string, Expr>> args;
                if (!check(TokenKind::RParen)) {
                    do {
                        Expr arg = parseExpr();
                        args.push_back({"", arg});
                    } while (match(TokenKind::Comma));
                }
                if (!match(TokenKind::RParen))
                    throw std::runtime_error("Expected ')' after call");
                return make::call(t.text, std::move(args));
            }
            return make::ident(t.text);
        case TokenKind::LParen: {
            Expr e = parseExpr();
            if (!match(TokenKind::RParen))
                throw std::runtime_error("Expected ')'");
            return e;
        }
        default:
            throw std::runtime_error("Unexpected token in expression");
    }
}

} // namespace deltora