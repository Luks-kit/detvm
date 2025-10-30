#pragma once
#include <string>
#include <vector>
#include <optional>

namespace deltora {

enum class TokenKind {
    Identifier, Number, String, Char,
    LParen, RParen, LBrace, RBrace, LBracket, RBracket,
    Comma, Semicolon, Colon, Dot, Arrow,
    Plus, Minus, Star, Slash, Percent, Equal, Less, Greater,
    Bang, Amp, Pipe, Caret,
    Keyword, Eof
};

struct Token {
    TokenKind kind;
    std::string text;
    int line, col;
};

class Lexer {
public:
    explicit Lexer(const std::string& src);
    std::vector<Token> tokenize();

private:
    char peek() const;
    char advance();
    bool eof() const;
    void skipWhitespace();
    Token nextToken();

    std::string src;
    size_t pos = 0;
    int line = 1, col = 1;
};

} // namespace deltora
