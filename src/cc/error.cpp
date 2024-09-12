#include "IntStream.h"
#include "Recognizer.h"
#include <cstdio>
#include <error.hpp>
#include <fstream>

// Source name, line, column, color, type, message
#define CALLOUT_FMT "\033[1m%s:%zu:%zu: \033[1;%dm%s: \033[0;1m%s\033[0m\n"
// Line number, line text
#define CALLOUT_INFO_FMT "%5zu | %s\n"
// Gutter width, gutter text, arrow position, arrow text, color
#define CALLOUT_ARROW_FMT "%*s | %*s\033[1;%dm^\033[0m\n"

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

    printf(CALLOUT_FMT, source.c_str(), line, charPositionInLine, 31, "error", msg.c_str());
    printf(CALLOUT_INFO_FMT, line, lineText.c_str());
    printf(CALLOUT_ARROW_FMT, 5, "", static_cast<int>(charPositionInLine), "", 31);
}
