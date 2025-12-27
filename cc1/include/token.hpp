#ifndef TOKEN_HPP
#define TOKEN_HPP

#include <string>

enum class TokenType {
    /* Keywords */
    INT,
    RETURN,
    
    /* Identifiers and Literals */
    IDENTIFIER,
    CONSTANT,
    
    /* Operators and Punctuation */
    LPAREN,      /* ( */
    RPAREN,      /* ) */
    LBRACE,      /* { */
    RBRACE,      /* } */
    SEMICOLON,   /* ; */
    
    /* Special */
    END_OF_FILE,
    INVALID
};

struct Token {
    TokenType type;
    std::string lexeme;
    int line;
    int column;
    
    Token(TokenType t, const std::string& lex, int l, int c)
        : type(t), lexeme(lex), line(l), column(c) {}
};

#endif /* TOKEN_HPP */
