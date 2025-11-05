#pragma once
#include <string>
#include <string_view>
#include <cstdint>

namespace det {

// ---------- Token kinds ----------
enum class TokenKind {
    End, Error,

    // literals
    Int, Double, String, Ident,

    // operators
    Plus, Minus, Star, Slash,
    Equal, EqualEqual, Bang, BangEqual,
    Less, LessEqual, Greater, GreaterEqual,

    // punctuation
    LParen, RParen,
    LBrace, RBrace,
    Comma, Semicolon,

    // keywords
    If, Else, While, Return, Let,
};

// ---------- Token ----------
struct Token {
    TokenKind kind;
    std::string lexeme;
    std::uint32_t line;
    std::uint32_t column;

    Token() = default;
    Token(TokenKind k, std::string l, std::uint32_t ln, std::uint32_t col)
        : kind(k), lexeme(std::move(l)), line(ln), column(col) {}
};

} // namespace det
