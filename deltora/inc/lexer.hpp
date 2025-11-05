#pragma once
#include "token.hpp"
#include <vector>

namespace det {

class Lexer {
public:
    explicit Lexer(const std::string&);
    const Token& peek() const;     // look at current token
    const Token& advance();        // move to next
    bool match(TokenKind);    // if matches, consume
    bool check(TokenKind) const;
    bool eof() const;

private:
    std::vector<Token> tokens;
    std::size_t pos = 0;
    std::string src;
};

} // namespace det
