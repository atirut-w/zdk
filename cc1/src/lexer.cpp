#include "lexer.hpp"
#include <cctype>
#include <utility>

static const std::pair<std::string, Token::Kind> keyword_map[] = {
    {"auto", Token::Auto},         {"break", Token::Break},
    {"case", Token::Case},         {"char", Token::Char},
    {"const", Token::Const},       {"continue", Token::Continue},
    {"default", Token::Default},   {"do", Token::Do},
    {"double", Token::Double},     {"else", Token::Else},
    {"enum", Token::Enum},         {"extern", Token::Extern},
    {"float", Token::Float},       {"for", Token::For},
    {"goto", Token::Goto},         {"if", Token::If},
    {"int", Token::Int},           {"long", Token::Long},
    {"register", Token::Register}, {"return", Token::Return},
    {"short", Token::Short},       {"signed", Token::Signed},
    {"sizeof", Token::Sizeof},     {"static", Token::Static},
    {"struct", Token::Struct},     {"switch", Token::Switch},
    {"typedef", Token::Typedef},   {"union", Token::Union},
    {"unsigned", Token::Unsigned}, {"void", Token::Void},
    {"volatile", Token::Volatile}, {"while", Token::While}};

static const std::pair<std::string, Token::Kind> punct_map[] = {
    {"...", Token::Ellipsis},   {">>=", Token::RightAssign},
    {"<<=", Token::LeftAssign}, {"+=", Token::AddAssign},
    {"-=", Token::SubAssign},   {"*=", Token::MulAssign},
    {"/=", Token::DivAssign},   {"%=", Token::ModAssign},
    {"&=", Token::AndAssign},   {"^=", Token::XorAssign},
    {"|=", Token::OrAssign},    {">>", Token::RightOp},
    {"<<", Token::LeftOp},      {"++", Token::IncOp},
    {"--", Token::DecOp},       {"->", Token::PtrOp},
    {"&&", Token::AndOp},       {"||", Token::OrOp},
    {"<=", Token::LeOp},        {">=", Token::GeOp},
    {"==", Token::EqOp},        {"!=", Token::NeOp},
    {";", Token::Semi},         {"{", Token::LBrace},
    {"<%", Token::LBrace},      {"}", Token::RBrace},
    {"%>", Token::RBrace},      {",", Token::Comma},
    {":", Token::Colon},        {"=", Token::Assign},
    {"(", Token::LParen},       {")", Token::RParen},
    {"[", Token::LBracket},     {"<:", Token::LBracket},
    {"]", Token::RBracket},     {":>", Token::RBracket},
    {".", Token::Period},       {"&", Token::Ampersand},
    {"!", Token::Exclamation},  {"~", Token::Tilde},
    {"-", Token::Minus},        {"+", Token::Plus},
    {"*", Token::Asterisk},     {"/", Token::Slash},
    {"%", Token::Percent},      {"<", Token::LAngle},
    {">", Token::RAngle},       {"^", Token::Caret},
    {"|", Token::Pipe},         {"?", Token::Question}};

std::optional<Token> Lexer::next_token() {
  while (peek() && std::isspace(*peek())) {
    get();
  }

  unsigned int start_line = line;
  unsigned int start_column = column;

  for (auto const &[str, kind] : keyword_map) {
    if (accept(str)) {
      return Token(kind, start_line, start_column);
    }
  }

  if (peek()) {
    if (std::isalpha(*peek()) || *peek() == '_') {
      std::string ident;

      while (peek() && (std::isalnum(*peek()) || *peek() == '_')) {
        ident.push_back(*get());
      }

      // TODO: Check if identifier is a typedef name
      return Token(Token::Identifier, start_line, start_column, ident);
    } else if (std::isdigit(*peek())) {
      std::string number;

      while (peek() && std::isdigit(*peek())) {
        number.push_back(*get());
      }

      return Token(Token::Constant, start_line, start_column, number);
    }
  }

  for (auto const &[str, kind] : punct_map) {
    if (accept(str)) {
      return Token(kind, start_line, start_column);
    }
  }

  // TODO: Emit diag and recover
  return std::nullopt;
}
