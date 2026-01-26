#pragma once
#include <cstddef>
#include <cstring>
#include <istream>
#include <optional>
#include <string>

namespace langkit {

/**
 * Language-agnostic base class for lexers.
 */
template <typename TokenType, int BufferSize = 64> class Lexer {
  std::istream &input;
  char buffer[BufferSize];
  char *cursor = buffer;
  char *limit = buffer;

  /**
   * Gets the amount of remaining data in the buffer.
   */
  std::size_t remaining() const {
    return static_cast<std::size_t>(limit - cursor);
  }

  /**
   * Fills the buffer with more data from the input stream.
   */
  void fill_buffer() {
    if (cursor > buffer) {
      std::size_t rem = remaining();
      std::memmove(buffer, cursor, rem);
      cursor = buffer;
      limit = buffer + rem;
    }

    input.read(limit, BufferSize - remaining());
    limit += static_cast<std::size_t>(input.gcount());
  }

protected:
  unsigned int line = 1;
  unsigned int column = 1;

  /**
   * Peeks at the next character in the buffer without consuming it.
   */
  std::optional<char> peek() {
    if (remaining() == 0) {
      fill_buffer();
      if (remaining() == 0) {
        return std::nullopt; // EOF
      }
    }
    return *cursor;
  }

  /**
   * Consumes the next character in the buffer and advances the cursor.
   */
  std::optional<char> get() {
    auto ch = peek();
    if (ch) {
      cursor++;

      switch (*ch) {
      case '\r':
        if (peek() == '\n') {
          cursor++;
        }
        [[fallthrough]];
      case '\n':
        line++;
        column = 1;
        break;
      case '\t':
        column += 8 - ((column - 1) % 8);
        break;
      default:
        column++;
        break;
      }
    }
    return ch;
  }

  /**
   * Attempts to accept the given string from the input. If successful, advances
   * the cursor and updates line/column information accordingly. Returns true on
   * success, false otherwise.
   *
   * Do note that this function will silently fail if the buffer is not large
   * enough to hold the entire string. This behavior may be changed in the
   * future.
   */
  bool accept(const std::string &str) {
    if (remaining() < str.size()) {
      fill_buffer();
      if (remaining() < str.size()) {
        return false; // Either EOF or string too large for buffer
      }
    }

    if (std::memcmp(cursor, str.data(), str.size()) == 0) {
      for (std::size_t i = 0; i < str.size(); ++i) {
        switch (cursor[i]) {
        case '\r':
          if (i + 1 < str.size() && cursor[i + 1] == '\n') {
            i++;
          }
          [[fallthrough]];
        case '\n':
          line++;
          column = 1;
          break;
        case '\t':
          column += 8 - ((column - 1) % 8);
          break;
        default:
          column++;
          break;
        }
      }
      cursor += str.size();
      return true;
    }

    return false;
  }

public:
  struct State {
    std::streampos pos;
    char *cursor;
    char *limit;
    unsigned int line;
    unsigned int column;

    explicit State(std::streampos p, char *c, char *l, unsigned int ln,
                   unsigned int col)
        : pos(p), cursor(c), limit(l), line(ln), column(col) {}
  };

  explicit Lexer(std::istream &in) : input(in) {}
  virtual ~Lexer() = default;

  /**
   * Saves the current state of the lexer.
   */
  State save_state() const {
    return State(input.tellg(), cursor, limit, line, column);
  }

  /**
   * Restores the lexer to a previously saved state. Do note that there is a
   * performance penalty if the lexer has to seek the input stream.
   */
  void restore_state(const State &state) {
    if (input.tellg() != state.pos) {
      input.clear(); // Clear any EOF flags
      input.seekg(state.pos);
      cursor = buffer;
      limit = buffer;
      fill_buffer();
    } else {
      cursor = state.cursor;
      limit = state.limit;
    }
    line = state.line;
    column = state.column;
  }

  /**
   * Gets the next token from the input stream. Returns std::nullopt on EOF.
   */
  virtual std::optional<TokenType> next_token() = 0;
};

} // namespace langkit
