#include <algorithm>
#include <iostream>
#include <lexer.hpp>

using namespace std;

char Lexer::peek() const
{
    char ch = input.peek();
    if (ch == '\r')
        return '\n';
    return ch;
}

char Lexer::get()
{
    char c = input.get();

    switch (c)
    {
    case '\r':
    case '\n':
        line++;
        col = 1;
        return '\n';
    default:
        col++;
        break;
    }

    return c;
}

bool Lexer::is_boundary(char c) const
{
    vector<char> boundaries = {
        '(',
        ')',
    };
    return isspace(c) || find(boundaries.begin(), boundaries.end(), c) != boundaries.end();
}

vector<Token> Lexer::tokenize()
{
    vector<Token> tokens;

    while (peek() != EOF)
    {
        Token token = {line, col};
        char ch = peek();

        if (isspace(ch))
        {
            get();
            continue;
        }

        if (isalpha(ch) || ch == '_')
        {
            string text;

            while (isalnum(ch) || ch == '_')
            {
                text += get();
                ch = peek();
            }

            if (!is_boundary(ch))
                throw runtime_error("Unexpected character: " + string(1, ch));

            if (text == "void" || text == "int" || text == "return")
                token.type = Token::Type::Keyword;
            else
                token.type = Token::Type::Identifier;

            token.text = text;
        }
        else
        {
            throw runtime_error("Unrecognized character: " + string(1, ch));
        }

        tokens.push_back(token);
    }

    return tokens;
}
