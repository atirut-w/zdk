#ifndef LEXER_HPP
#define LEXER_HPP

#include "token.hpp"
#include <vector>
#include <string>
#include <istream>

class Lexer {
public:
    explicit Lexer(std::istream& input);
    std::vector<Token> tokenize();
    
private:
    std::istream& input;
    static const size_t BUFFER_SIZE = 4096;
    char buffer[BUFFER_SIZE];
    size_t bufferPos;
    size_t bufferEnd;
    int line;
    int column;
    bool eof;
    
    char current();
    char peek();
    void advance();
    void fillBuffer();
    void skipWhitespace();
    void skipComment();
    
    Token makeToken(TokenType type, const std::string& lexeme);
    Token readIdentifierOrKeyword();
    Token readNumber();
};

#endif /* LEXER_HPP */
