#include "Parser.h"
#include <error.hpp>
#include <iostream>

using namespace std;
using namespace antlr4;

void CCErrorListener::syntaxError(Recognizer *recognizer, Token *offendingSymbol, size_t line,
                                  size_t charPositionInLine, const std::string &msg, std::exception_ptr e)
{
    string source = recognizer->getInputStream()->getSourceName();

    cout << "\033[1m" << source << ":" << line << ":" << charPositionInLine << ": \033[31merror: \033[0;1m" << msg
         << "\033[0m" << endl;
}
