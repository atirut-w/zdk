#pragma once
#include "langkit/lexer.hpp"
#include "token.hpp"

class Lexer : public langkit::Lexer<Token> {};
