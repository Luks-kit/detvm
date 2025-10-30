#include "lexer.hpp"
#include <cctype>

namespace deltora {

Lexer::Lexer(const std::string& s) : src(s) {}

std::vector<Token> Lexer::tokenize() {
    std::vector<Token> tokens;
    while (!eof()) {
        skipWhitespace();
        tokens.push_back(nextToken());
    }
    tokens.push_back({TokenKind::Eof, "", line, col});
    return tokens;
}

void Lexer::skipWhitespace() {
    while (!eof() && std::isspace(peek())) advance();
}

char Lexer::peek() const { return pos < src.size() ? src[pos] : '\0'; }
char Lexer::advance() { return src[pos++]; }
bool Lexer::eof() const { return pos >= src.size(); }

Token Lexer::nextToken() {
    char c = advance();
    Token t{TokenKind::Eof, std::string(1, c), line, col};
    // TODO: implement identifiers, numbers, strings, operators
    return t;
}

} // namespace deltora
