#pragma once
#include <istream>
#include <vector>
#include <memory>

struct Token
{
    ~Token() = default; // Force polymorphism
};

class Lexer
{
    std::istream &input;

public:
    Lexer(std::istream &input) : input(input) {}

    std::vector<std::unique_ptr<Token>> tokenize();
};
