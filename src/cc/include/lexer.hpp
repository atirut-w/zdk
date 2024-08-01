#pragma once
#include <any>
#include <istream>
#include <string>
#include <vector>

struct Positional
{
    int line, col;
};

struct Token : Positional
{
    enum class Type
    {
        Identifier,
        Keyword,
        LParen,
        RParen,
        LBrace,
        RBrace,
        Semicolon,
        Constant,
    } type;
    std::string text;
    std::any value;
};

class Lexer : Positional
{
    std::istream &input;

    bool eof() const
    {
        return input.peek() == EOF;
    }
    char peek() const;
    char get();
    bool is_boundary(char c) const;

public:
    Lexer(std::istream &input) : input(input)
    {
    }

    std::vector<Token> tokenize();
};
