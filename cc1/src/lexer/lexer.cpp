#include "lexer.hpp"
#include <cctype>

Lexer::Lexer(std::istream& input)
    : input(input), bufferPos(0), bufferEnd(0), line(1), column(1), eof(false) {
    fillBuffer();
}

std::vector<Token> Lexer::tokenize() {
    std::vector<Token> tokens;
    
    while (!eof || bufferPos < bufferEnd) {
        skipWhitespace();
        
        if (eof && bufferPos >= bufferEnd) {
            break;
        }
        
        char c = current();
        
        /* Handle comments */
        if (c == '/' && peek() == '*') {
            skipComment();
            continue;
        }
        
        /* Handle single character tokens */
        if (c == '(') {
            tokens.push_back(makeToken(TokenType::LPAREN, "("));
            advance();
        } else if (c == ')') {
            tokens.push_back(makeToken(TokenType::RPAREN, ")"));
            advance();
        } else if (c == '{') {
            tokens.push_back(makeToken(TokenType::LBRACE, "{"));
            advance();
        } else if (c == '}') {
            tokens.push_back(makeToken(TokenType::RBRACE, "}"));
            advance();
        } else if (c == ';') {
            tokens.push_back(makeToken(TokenType::SEMICOLON, ";"));
            advance();
        } else if (std::isalpha(c) || c == '_') {
            tokens.push_back(readIdentifierOrKeyword());
        } else if (std::isdigit(c)) {
            tokens.push_back(readNumber());
        } else {
            /* Invalid character */
            advance();
        }
    }
    
    tokens.push_back(makeToken(TokenType::END_OF_FILE, ""));
    return tokens;
}

void Lexer::fillBuffer() {
    if (eof) {
        return;
    }
    
    /* Move any remaining data to the beginning of the buffer for lookahead */
    if (bufferPos < bufferEnd) {
        size_t remaining = bufferEnd - bufferPos;
        for (size_t i = 0; i < remaining; i++) {
            buffer[i] = buffer[bufferPos + i];
        }
        bufferEnd = remaining;
        bufferPos = 0;
    } else {
        bufferEnd = 0;
        bufferPos = 0;
    }
    
    /* Read more data from input stream */
    input.read(buffer + bufferEnd, BUFFER_SIZE - bufferEnd);
    size_t bytesRead = input.gcount();
    bufferEnd += bytesRead;
    
    if (bytesRead == 0 || input.eof()) {
        eof = true;
    }
}

char Lexer::current() {
    if (bufferPos >= bufferEnd) {
        return '\0';
    }
    return buffer[bufferPos];
}

char Lexer::peek() {
    /* Use buffer for lookahead */
    if (bufferPos + 1 >= bufferEnd) {
        return '\0';
    }
    return buffer[bufferPos + 1];
}

void Lexer::advance() {
    if (bufferPos < bufferEnd) {
        if (buffer[bufferPos] == '\n') {
            line++;
            column = 1;
        } else {
            column++;
        }
        bufferPos++;
        
        /* Refill buffer when we've consumed enough data */
        if (bufferPos >= BUFFER_SIZE / 2 && !eof) {
            fillBuffer();
        }
    }
}

void Lexer::skipWhitespace() {
    while ((bufferPos < bufferEnd || !eof) && std::isspace(current())) {
        advance();
    }
}

void Lexer::skipComment() {
    /* Skip / and * */
    advance();
    advance();
    
    while (bufferPos < bufferEnd || !eof) {
        if (current() == '\0' && eof) {
            /* Unterminated comment - just return */
            return;
        }
        if (current() == '*' && peek() == '/') {
            advance();
            advance();
            break;
        }
        advance();
    }
}

Token Lexer::makeToken(TokenType type, const std::string& lexeme) {
    return Token(type, lexeme, line, column);
}

Token Lexer::readIdentifierOrKeyword() {
    int startCol = column;
    std::string lexeme;
    
    while ((bufferPos < bufferEnd || !eof) && 
           (std::isalnum(current()) || current() == '_')) {
        lexeme += current();
        advance();
    }
    
    TokenType type = TokenType::IDENTIFIER;
    if (lexeme == "int") {
        type = TokenType::INT;
    } else if (lexeme == "return") {
        type = TokenType::RETURN;
    }
    
    return Token(type, lexeme, line, startCol);
}

Token Lexer::readNumber() {
    int startCol = column;
    std::string lexeme;
    
    while ((bufferPos < bufferEnd || !eof) && std::isdigit(current())) {
        lexeme += current();
        advance();
    }
    
    return Token(TokenType::CONSTANT, lexeme, line, startCol);
}
