#pragma once
#include "ast.hpp"
#include "lexer.hpp"

namespace deltora {

class Parser {
public:
    explicit Parser(const std::vector<Token>& tokens);
    Program parseProgram();

private:
    const Token& peek() const;
    const Token& advance();
    bool match(TokenKind k);
    bool check(TokenKind k) const;
    bool eof() const;

    Expr parseExpr(int prec = 0);
    Stmt parseStmt();
    Function parseFunction();
    Form parseForm();

    const std::vector<Token>& tokens;
    size_t pos = 0;
};

} // namespace deltora
