#include "Parser.h"
#include <cstdio>
#include <error.hpp>
#include <fstream>
#include <iostream>

using namespace std;
using namespace antlr4;

void CCErrorListener::syntaxError(Recognizer *recognizer, Token *offendingSymbol, size_t line,
                                  size_t charPositionInLine, const std::string &msg, std::exception_ptr e)
{
    ifstream file(recognizer->getInputStream()->getSourceName());

    string source = recognizer->getInputStream()->getSourceName();
    string lineText;
    for (size_t i = 0; i < line; i++)
        getline(file, lineText);

    printf("\033[1m%s:%zu:%zu: \033[1;31merror: \033[0;1m%s\033[0m\n", source.c_str(), line, charPositionInLine,
           msg.c_str());
    printf("%5zu | %s\n", line, lineText.c_str());
    printf("%*s | %*s\033[1;31m^\033[0m\n", 5, "", static_cast<int>(charPositionInLine), "");
}
