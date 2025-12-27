#include "parser.hpp"
#include <stdexcept>

Parser::Parser(const std::vector<Token>& tokens)
    : tokens(tokens), position(0) {}

std::unique_ptr<TranslationUnit> Parser::parse() {
    return parseTranslationUnit();
}

Token& Parser::current() {
    if (position < tokens.size()) {
        return tokens[position];
    }
    /* Return EOF token (lexer guarantees last token is EOF) */
    if (tokens.size() > 0) {
        return tokens[tokens.size() - 1];
    }
    throw std::runtime_error("Parser error: no tokens available");
}

Token& Parser::peek() {
    if (position + 1 < tokens.size()) {
        return tokens[position + 1];
    }
    /* Return EOF token (lexer guarantees last token is EOF) */
    if (tokens.size() > 0) {
        return tokens[tokens.size() - 1];
    }
    throw std::runtime_error("Parser error: no tokens available");
}

bool Parser::isAtEnd() {
    return current().type == TokenType::END_OF_FILE;
}

void Parser::advance() {
    if (!isAtEnd()) {
        position++;
    }
}

bool Parser::check(TokenType type) {
    if (isAtEnd()) {
        return false;
    }
    return current().type == type;
}

bool Parser::match(TokenType type) {
    if (check(type)) {
        advance();
        return true;
    }
    return false;
}

void Parser::expect(TokenType type, const std::string& message) {
    if (!match(type)) {
        throw std::runtime_error(message);
    }
}

std::unique_ptr<TranslationUnit> Parser::parseTranslationUnit() {
    auto tu = std::make_unique<TranslationUnit>();
    
    while (!isAtEnd()) {
        tu->functions.push_back(parseFunctionDeclaration());
    }
    
    return tu;
}

std::unique_ptr<FunctionDeclaration> Parser::parseFunctionDeclaration() {
    /* Parse return type: int */
    expect(TokenType::INT, "Expected 'int'");
    std::string returnType = "int";
    
    /* Parse function name */
    expect(TokenType::IDENTIFIER, "Expected function name");
    std::string name = tokens[position - 1].lexeme;
    
    /* Parse parameter list */
    expect(TokenType::LPAREN, "Expected '('");
    expect(TokenType::RPAREN, "Expected ')'");
    
    /* Parse function body */
    auto body = parseCompoundStatement();
    
    return std::make_unique<FunctionDeclaration>(returnType, name, std::move(body));
}

std::unique_ptr<CompoundStatement> Parser::parseCompoundStatement() {
    expect(TokenType::LBRACE, "Expected '{'");
    
    auto compound = std::make_unique<CompoundStatement>();
    
    while (!check(TokenType::RBRACE) && !isAtEnd()) {
        compound->statements.push_back(parseStatement());
    }
    
    expect(TokenType::RBRACE, "Expected '}'");
    
    return compound;
}

std::unique_ptr<Statement> Parser::parseStatement() {
    if (check(TokenType::RETURN)) {
        return parseReturnStatement();
    }
    
    throw std::runtime_error("Unexpected statement");
}

std::unique_ptr<ReturnStatement> Parser::parseReturnStatement() {
    expect(TokenType::RETURN, "Expected 'return'");
    
    auto expr = parseExpression();
    
    expect(TokenType::SEMICOLON, "Expected ';'");
    
    return std::make_unique<ReturnStatement>(std::move(expr));
}

std::unique_ptr<Expression> Parser::parseExpression() {
    return parsePrimaryExpression();
}

std::unique_ptr<Expression> Parser::parsePrimaryExpression() {
    if (check(TokenType::CONSTANT)) {
        int value = std::stoi(current().lexeme);
        advance();
        return std::make_unique<IntegerLiteral>(value);
    }
    
    throw std::runtime_error("Expected expression");
}
