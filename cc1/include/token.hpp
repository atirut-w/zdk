#pragma once
#include <optional>
#include <string>

struct Token {
  enum Kind {
    // Keywords
    Auto,     // `auto`
    Break,    // `break`
    Case,     // `case`
    Char,     // `char`
    Const,    // `const`
    Continue, // `continue`
    Default,  // `default`
    Do,       // `do`
    Double,   // `double`
    Else,     // `else`
    Enum,     // `enum`
    Extern,   // `extern`
    Float,    // `float`
    For,      // `for`
    Goto,     // `goto`
    If,       // `if`
    Int,      // `int`
    Long,     // `long`
    Register, // `register`
    Return,   // `return`
    Short,    // `short`
    Signed,   // `signed`
    Sizeof,   // `sizeof`
    Static,   // `static`
    Struct,   // `struct`
    Switch,   // `switch`
    Typedef,  // `typedef`
    Union,    // `union`
    Unsigned, // `unsigned`
    Void,     // `void`
    Volatile, // `volatile`
    While,    // `while`

    // Keywords, typedef names, constants, and string literals
    Identifier,    // Identifiers
    TypeName,      // Typedef names
    Constant,      // Numeric constants
    StringLiteral, // String literals

    // Operators and punctuators
    Ellipsis,    // `...`
    RightAssign, // `>>=`
    LeftAssign,  // `<<=`
    AddAssign,   // `+=`
    SubAssign,   // `-=`
    MulAssign,   // `*=`
    DivAssign,   // `/=`
    ModAssign,   // `%=`
    AndAssign,   // `&=`
    XorAssign,   // `^=`
    OrAssign,    // `|=`
    RightOp,     // `>>`
    LeftOp,      // `<<`
    IncOp,       // `++`
    DecOp,       // `--`
    PtrOp,       // `->`
    AndOp,       // `&&`
    OrOp,        // `||`
    LeOp,        // `<=`
    GeOp,        // `>=`
    EqOp,        // `==`
    NeOp,        // `!=`
    Semi,        // `;`
    LBrace,      // `{` or `<%`
    RBrace,      // `}` or `%>`
    Comma,       // `,`
    Colon,       // `:`
    Assign,      // `=`
    LParen,      // `(`
    RParen,      // `)`
    LBracket,    // `[` or `<:`
    RBracket,    // `]` or `:>`
    Period,      // `.`
    Ampersand,   // `&`
    Exclamation, // `!`
    Tilde,       // `~`
    Minus,       // `-`
    Plus,        // `+`
    Asterisk,    // `*`
    Slash,       // `/`
    Percent,     // `%`
    LAngle,      // `<`
    RAngle,      // `>`
    Caret,       // `^`
    Pipe,        // `|`
    Question,    // `?`
  };

  Kind kind;
  unsigned int line;
  unsigned int column;
  std::optional<std::string> lexeme;

  explicit Token(Kind kind, unsigned int line, unsigned int column,
                 std::optional<std::string> lexeme = std::nullopt)
      : kind(kind), line(line), column(column), lexeme(std::move(lexeme)) {}
};
