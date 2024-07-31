#include <lexer.hpp>

using namespace std;

vector<unique_ptr<Token>> Lexer::tokenize()
{
    vector<unique_ptr<Token>> tokens;

    while (input.peek() != EOF)
    {
        char c = input.get();
    }

    return tokens;
}
