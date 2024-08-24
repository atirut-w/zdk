#include <parser.hpp>

using namespace std;

Token *Parser::expect(Token::Type type)
{
    if (index >= tokens.size())
        throw PositionalError(*this, "Unexpected end of input");

    if (tokens[index].type != type)
        throw PositionalError(tokens[index], "Expected token type " + to_string(static_cast<int>(type)) + ", got " +
                                                 to_string(static_cast<int>(tokens.front().type)));

    return &tokens[index++];
}

Token *Parser::expect(const string &text)
{
    if (index >= tokens.size())
        throw PositionalError(*this, "Unexpected end of input");

    if (tokens[index].text != text)
        throw PositionalError(tokens[index], "Expected '" + text + "', got '" + tokens.front().text + "'");

    return &tokens[index++];
}

Expression *Parser::parse_expression()
{
    auto expression = new Expression;
    expression->integer = expect(Token::Type::Constant);
    return expression;
}

Statement *Parser::parse_statement()
{
    auto statement = new Statement;
    expect("return");
    statement->exp = parse_expression();
    statement->children.push_back(unique_ptr<Expression>(statement->exp));
    expect(Token::Type::Semicolon);
    return statement;
}

FunctionDefinition *Parser::parse_function()
{
    auto function = new FunctionDefinition;
    expect("int");
    function->identifier = expect(Token::Type::Identifier);
    expect(Token::Type::LParen);
    expect("void");
    expect(Token::Type::RParen);

    expect(Token::Type::LBrace);
    function->body = parse_statement();
    function->children.push_back(unique_ptr<Statement>(function->body));
    expect(Token::Type::RBrace);

    return function;
}

unique_ptr<Program> Parser::parse()
{
    auto program = make_unique<Program>();
    program->function = parse_function();
    program->children.push_back(unique_ptr<FunctionDefinition>(program->function));

    if (index < tokens.size())
        throw PositionalError(tokens[index], "Unexpected token");
    return program;
}
