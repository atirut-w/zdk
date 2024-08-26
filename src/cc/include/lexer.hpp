#pragma once
#include <any>
#include <istream>
#include <optional>
#include <positional.hpp>
#include <string>
#include <vector>

struct Token : Positional
{
    enum class Type
    {
        // Some important constructs
        Identifier,
        Constant,
        Keyword,

        // Misc
        LParen,
        RParen,
        LBrace,
        RBrace,
        Semicolon,

        // Idk what to call these
        Tilde,
        Minus,
        MinusMinus,
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

    std::optional<Token> next();
    std::vector<Token> tokenize();
};
