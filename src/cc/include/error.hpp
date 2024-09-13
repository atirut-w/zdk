#pragma once
#include "BaseErrorListener.h"

// Source name, line, column, color, type, message
#define CALLOUT_FMT "\033[1m%s:%zu:%zu: \033[1;%dm%s: \033[0;1m%s\033[0m\n"
// Line number, line text
#define CALLOUT_INFO_FMT "%5zu | %s\n"
// Gutter width, gutter text, arrow position, arrow text, color
#define CALLOUT_ARROW_FMT "%*s | %*s\033[1;%dm^\033[0m\n"

class CCErrorListener : public antlr4::BaseErrorListener
{
    virtual void syntaxError(antlr4::Recognizer *recognizer, antlr4::Token *offendingSymbol, size_t line,
                             size_t charPositionInLine, const std::string &msg, std::exception_ptr e) override;
};
