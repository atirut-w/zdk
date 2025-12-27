#ifndef PARSER_HPP
#define PARSER_HPP

#include "token.hpp"
#include "ast.hpp"
#include <vector>
#include <memory>

class Parser {
public:
    explicit Parser(const std::vector<Token>& tokens);
    std::unique_ptr<TranslationUnit> parse();
    
private:
    std::vector<Token> tokens;
    size_t position;
    
    Token& current();
    Token& peek();
    bool isAtEnd();
    void advance();
    bool check(TokenType type);
    bool match(TokenType type);
    void expect(TokenType type, const std::string& message);
    
    std::unique_ptr<TranslationUnit> parseTranslationUnit();
    std::unique_ptr<FunctionDeclaration> parseFunctionDeclaration();
    std::unique_ptr<CompoundStatement> parseCompoundStatement();
    std::unique_ptr<Statement> parseStatement();
    std::unique_ptr<ReturnStatement> parseReturnStatement();
    std::unique_ptr<Expression> parseExpression();
    std::unique_ptr<Expression> parsePrimaryExpression();
};

#endif /* PARSER_HPP */
