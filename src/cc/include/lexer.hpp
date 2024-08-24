#pragma once
#include <any>
#include <istream>
#include <positional.hpp>
#include <stdexcept>
#include <string>
#include <vector>

struct Token : Positional
{
    enum class Type
    {
        Identifier,
        Constant,
        Keyword,
        LParen,
        RParen,
        LBrace,
        RBrace,
        Semicolon,
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
    void error(const std::string &message);

public:
    Lexer(std::istream &input) : input(input)
    {
    }

    std::vector<Token> tokenize();
};
