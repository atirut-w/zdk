#include <algorithm>
#include <iostream>
#include <lexer.hpp>
#include <optional>

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

optional<Token> Lexer::next()
{
    Token current = {line, col};
    string &text = current.text;
    
    while (isspace(peek()))
    {
        get();
    }
    if (eof())
    {
        return nullopt;
    }
    char ch = peek();

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
            current.type = Token::Type::Keyword;
        else
            current.type = Token::Type::Identifier;
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

        current.type = Token::Type::Constant;
        current.value = stoi(text);
    }
    else
    {
        // Process punctuation
        text = get();
        
        switch (ch)
        {
        default:
            error("unexpected character: " + text);
        case '(':
            current.type = Token::Type::LParen;
            break;
        case ')':
            current.type = Token::Type::RParen;
            break;
        case '{':
            current.type = Token::Type::LBrace;
            break;
        case '}':
            current.type = Token::Type::RBrace;
            break;
        case ';':
            current.type = Token::Type::Semicolon;
            break;
        case '~':
            current.type = Token::Type::Tilde;
            break;
        case '-':
            ch = peek();

            if (ch == '-')
            {
                current.type = Token::Type::MinusMinus;
                text += get();
            }
            else
            {
                current.type = Token::Type::Minus;
            }
            break;
        }
    }

    return current;
}

vector<Token> Lexer::tokenize()
{
    vector<Token> tokens;

    while (peek() != EOF)
    {
        if (auto token = next())
        {
            tokens.push_back(*token);
        }
    }

    return tokens;
}
