#pragma once

enum TokenType { T_PLUS, T_MINUS, T_STAR, T_SLASH, T_INTLIT };

typedef struct Token {
  enum TokenType type;
  int int_value;
} Token;
