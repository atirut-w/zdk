#include "diagnostic.h"
#include "lexer.h"
#include <ctype.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

typedef struct {
  const char *str;
  TokenKind kind;
} KindMap;

KindMap keywords[] = {{"auto", TOKEN_AUTO},
                      {"break", TOKEN_BREAK},
                      {"case", TOKEN_CASE},
                      {"char", TOKEN_CHAR},
                      {"const", TOKEN_CONST},
                      {"continue", TOKEN_CONTINUE},
                      {"default", TOKEN_DEFAULT},
                      {"do", TOKEN_DO},
                      {"double", TOKEN_DOUBLE},
                      {"else", TOKEN_ELSE},
                      {"enum", TOKEN_ENUM},
                      {"extern", TOKEN_EXTERN},
                      {"float", TOKEN_FLOAT},
                      {"for", TOKEN_FOR},
                      {"goto", TOKEN_GOTO},
                      {"if", TOKEN_IF},
                      {"int", TOKEN_INT},
                      {"long", TOKEN_LONG},
                      {"register", TOKEN_REGISTER},
                      {"return", TOKEN_RETURN},
                      {"short", TOKEN_SHORT},
                      {"signed", TOKEN_SIGNED},
                      {"sizeof", TOKEN_SIZEOF},
                      {"static", TOKEN_STATIC},
                      {"struct", TOKEN_STRUCT},
                      {"switch", TOKEN_SWITCH},
                      {"typedef", TOKEN_TYPEDEF},
                      {"union", TOKEN_UNION},
                      {"unsigned", TOKEN_UNSIGNED},
                      {"void", TOKEN_VOID},
                      {"volatile", TOKEN_VOLATILE},
                      {"while", TOKEN_WHILE},
                      {0, 0}};

KindMap puncts[] = {{"...", TOKEN_ELLIPSIS},
                    {">>=", TOKEN_RIGHT_ASSIGN},
                    {"<<=", TOKEN_LEFT_ASSIGN},
                    {"+=", TOKEN_ADD_ASSIGN},
                    {"-=", TOKEN_SUB_ASSIGN},
                    {"*=", TOKEN_MUL_ASSIGN},
                    {"/=", TOKEN_DIV_ASSIGN},
                    {"%=", TOKEN_MOD_ASSIGN},
                    {"&=", TOKEN_AND_ASSIGN},
                    {"^=", TOKEN_XOR_ASSIGN},
                    {"|=", TOKEN_OR_ASSIGN},
                    {">>", TOKEN_RIGHT_OP},
                    {"<<", TOKEN_LEFT_OP},
                    {"++", TOKEN_INC_OP},
                    {"--", TOKEN_DEC_OP},
                    {"->", TOKEN_PTR_OP},
                    {"&&", TOKEN_AND_OP},
                    {"||", TOKEN_OR_OP},
                    {"<=", TOKEN_LE_OP},
                    {">=", TOKEN_GE_OP},
                    {"==", TOKEN_EQ_OP},
                    {"!=", TOKEN_NE_OP},
                    {";", ';'},
                    {"{", '{'},
                    {"<%", '{'},
                    {"}", '}'},
                    {"%>", '}'},
                    {",", ','},
                    {":", ':'},
                    {"=", '='},
                    {"(", '('},
                    {")", ')'},
                    {"[", '['},
                    {"<:", '['},
                    {"]", ']'},
                    {":>", ']'},
                    {".", '.'},
                    {"&", '&'},
                    {"!", '!'},
                    {"~", '~'},
                    {"-", '-'},
                    {"+", '+'},
                    {"*", '*'},
                    {"/", '/'},
                    {"%", '%'},
                    {"^", '^'},
                    {"|", '|'},
                    {"<", '<'},
                    {">", '>'},
                    {0, 0}};

/**
 * Returns the number of remaining characters in the lexer's buffer.
 */
size_t lexer_get_remaining(Lexer *lexer) {
  return lexer->limit - lexer->cursor;
}

/**
 * Fills the lexer's buffer with more data from the input stream.
 */
void lexer_fill(Lexer *lexer) {
  size_t remaining = lexer_get_remaining(lexer);
  size_t to_read;
  size_t read;

  if (lexer->cursor > lexer->buffer) {
    /* size_t offset = lexer->cursor - lexer->buffer; */
    memmove(lexer->buffer, lexer->cursor, remaining);
    /* lexer->start -= offset; */
    lexer->cursor = lexer->buffer;
    lexer->limit = lexer->buffer + remaining;
  }

  to_read = LEXER_BUFFER_SIZE - remaining;
  read = fread(lexer->limit, 1, to_read, lexer->input);
  lexer->limit += read;
}

/**
 * Peeks at the next character in the lexer's buffer without advancing the
 * cursor.
 */
int lexer_peek(Lexer *lexer) {
  if (lexer->cursor >= lexer->limit) {
    lexer_fill(lexer);
    if (lexer->cursor >= lexer->limit) {
      return EOF;
    }
  }
  return *(unsigned char *)lexer->cursor;
}

/**
 * Gets the next character in the lexer's buffer and advances the cursor.
 */
int lexer_get(Lexer *lexer) {
  int ch = lexer_peek(lexer);
  if (ch != EOF) {
    lexer->cursor++;

    /* TODO: Normalize line endings */
    switch (ch) {
    case '\n':
      lexer->line++;
      lexer->column = 1;
      break;
    case '\t':
      lexer->column += 8 - ((lexer->column - 1) % 8);
      break;
    default:
      lexer->column++;
      break;
    }
  }
  return ch;
}

/**
 * Accepts a given string and advances the lexer's cursor. Silently fails if
 * the string is larger than the buffer.
 */
int lexer_accept(Lexer *lexer, const char *str) {
  if (lexer_get_remaining(lexer) < strlen(str)) {
    lexer_fill(lexer);
    if (lexer_get_remaining(lexer) < strlen(str)) {
      return 0;
    }
  }

  if (strncmp(lexer->cursor, str, strlen(str)) == 0) {
    size_t i;
    for (i = 0; i < strlen(str); i++) {
      /* TODO: Normalize line endings */
      switch (lexer->cursor[i]) {
      case '\n':
        lexer->line++;
        lexer->column = 1;
        break;
      case '\t':
        lexer->column += 8 - ((lexer->column - 1) % 8);
        break;
      default:
        lexer->column++;
        break;
      }
    }

    lexer->cursor += strlen(str);
    return 1;
  }

  return 0;
}

Lexer *lexer_new(CompilationCtx *ctx, FILE *input) {
  Lexer *lexer = (Lexer *)malloc(sizeof(Lexer));
  if (!lexer) {
    return NULL;
  }

  lexer->ctx = ctx;
  lexer->input = input;
  /* lexer->start = lexer->buffer; */
  lexer->cursor = lexer->buffer;
  lexer->limit = lexer->buffer;
  lexer->line = 1;
  lexer->column = 1;

  return lexer;
}

void lexer_free(Lexer *lexer) {
  if (lexer) {
    free(lexer);
  }
}

int lexer_next_token(Lexer *lexer, Token *token) {
  KindMap *map;
  char *start;

  while (lexer_peek(lexer) != EOF && isspace(lexer_peek(lexer))) {
    lexer_get(lexer);
  }

  /* lexer->start = lexer->cursor; */
  token->line = lexer->line;
  token->column = lexer->column;

  map = keywords;
  while (map->str) {
    if (lexer_accept(lexer, map->str)) {
      token->kind = map->kind;
      return 1;
    }
    map++;
  }

  /* Something of unknown length is coming, so fill the buffer just in case */
  lexer_fill(lexer);
  start = lexer->cursor;
  
  if (isalpha(lexer_peek(lexer)) || lexer_peek(lexer) == '_') {
    while (isalnum(lexer_peek(lexer)) || lexer_peek(lexer) == '_') {
      lexer_get(lexer);
    }

    token->kind = TOKEN_IDENT;
    /* TODO: Intern identifier and set lexeme */
    return 1;
  }

  map = puncts;
  while (map->str) {
    if (lexer_accept(lexer, map->str)) {
      token->kind = map->kind;
      return 1;
    }
    map++;
  }

  if (lexer_peek(lexer) == EOF) {
    return 0;
  }

  queue_enqueue(lexer->ctx->diagnostics,
                diagnostic_new(DIAG_ERROR, "Garbage character", token->line,
                               token->column));
  /* Naive recovery: skip the garbage character */
  lexer_get(lexer);
  return lexer_next_token(lexer, token);
}
