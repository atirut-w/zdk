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
        '{',
        '}',
        ';',
        '~',
        '-',
    };
    return isspace(c) || find(boundaries.begin(), boundaries.end(), c) != boundaries.end();
}

void Lexer::error(const string &message)
{
    throw PositionalError(*this, message);
}

vector<Token> Lexer::tokenize()
{
    vector<Token> tokens;

    while (peek() != EOF)
    {
        Token token = {line, col};
        string &text = token.text;
        char ch = peek();

        if (isspace(ch))
        {
            get();
            continue;
        }

        if (isalpha(ch) || ch == '_')
        {
            while (isalnum(ch) || ch == '_')
            {
                text += get();
                ch = peek();
            }

            if (!is_boundary(ch))
                error("unexpected character: " + string(1, ch));

            if (text == "void" || text == "int" || text == "return")
                token.type = Token::Type::Keyword;
            else
                token.type = Token::Type::Identifier;
        }
        else if (isdigit(ch))
        {
            while (isdigit(ch))
            {
                text += get();
                ch = peek();
            }

            if (!is_boundary(ch))
                error("unexpected character: " + string(1, ch));

            token.type = Token::Type::Constant;
            token.value = stoi(text);
        }
        else if (ch == '(')
        {
            token.type = Token::Type::LParen;
            text = get();
        }
        else if (ch == ')')
        {
            token.type = Token::Type::RParen;
            text = get();
        }
        else if (ch == '{')
        {
            token.type = Token::Type::LBrace;
            text = get();
        }
        else if (ch == '}')
        {
            token.type = Token::Type::RBrace;
            text = get();
        }
        else if (ch == ';')
        {
            token.type = Token::Type::Semicolon;
            text = get();
        }
        else if (ch == '~')
        {
            token.type = Token::Type::Tilde;
            text = get();
        }
        else if (ch == '-')
        {
            text = get();
            ch = peek();

            if (ch == '-')
            {
                token.type = Token::Type::MinusMinus;
                text += get();
            }
            else
            {
                token.type = Token::Type::Minus;
            }
        }
        else
        {
            error("unrecognized character: " + string(1, ch));
        }

        tokens.push_back(token);
    }

    return tokens;
}
