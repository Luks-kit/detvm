#include "parser.hpp"
#include <stdexcept>

using namespace det;

ExprPtr Parser::parse_expr() {
    auto lhs = parse_unary();
    return parse_binary_rhs(0, std::move(lhs));
}

ExprPtr Parser::parse_primary() {
    const Token& t = peek();

    switch (t.kind) {
    case TokenKind::Int: {
        advance();
        long value = std::stol(t.lexeme);
        return make_LiteralExpr(make_LongLiteral(value));
    }
    case TokenKind::Double: {
        advance();
        double value = std::stod(t.lexeme);
        return make_LiteralExpr(make_DoubleLiteral(value));
    }
    case TokenKind::String: {
        advance();
        return make_LiteralExpr(make_StringLiteral(t.lexeme));
    }
    case TokenKind::Ident: {
        advance();
        return make_VariableExpr(t.lexeme);
    }
    case TokenKind::LParen: {
        advance();
        auto expr = parse_expr();
        if (!match(TokenKind::RParen))
            throw std::runtime_error("expected ')'");
        return expr;
    }
    default:
        throw std::runtime_error("unexpected token: " + t.lexeme);
    }
}

ExprPtr Parser::parse_unary() {
    if (match(TokenKind::Minus))
        return make_UnaryExpr(parse_unary(), '-');
    if (match(TokenKind::Bang))
        return make_UnaryExpr(parse_unary(), '!');
    return parse_primary();
}

int Parser::get_precedence(TokenKind k) {
    switch (k) {
        case TokenKind::Star:
        case TokenKind::Slash: return 5;
        case TokenKind::Plus:
        case TokenKind::Minus: return 4;
        case TokenKind::EqualEqual:
        case TokenKind::BangEqual: return 3;
        case TokenKind::Less:
        case TokenKind::LessEqual:
        case TokenKind::Greater:
        case TokenKind::GreaterEqual: return 2;
        case TokenKind::Equal: return 1;
        default: return 0;
    }
}

ExprPtr Parser::parse_binary_rhs(int prec, ExprPtr lhs) {
    while (true) {
        TokenKind op = peek().kind;
        int opPrec = get_precedence(op);

        if (opPrec <= prec)
            break;

        advance(); // consume op

        auto rhs = parse_unary();

        TokenKind nextOp = peek().kind;
        int nextPrec = get_precedence(nextOp);

        if (opPrec < nextPrec)
            rhs = parse_binary_rhs(opPrec + 1, std::move(rhs));

        char opChar = '?';
        switch (op) {
            case TokenKind::Plus: opChar = '+'; break;
            case TokenKind::Minus: opChar = '-'; break;
            case TokenKind::Star: opChar = '*'; break;
            case TokenKind::Slash: opChar = '/'; break;
            default: break;
        }

        lhs = make_BinaryExpr(std::move(lhs), std::move(rhs), opChar);
    }

    return lhs;
}
