#pragma once

namespace langkit {

/**
 * Language-agnostic base class for lexers.
 */
template <typename TokenType, int BufferSize = 64> class Lexer {};

}
