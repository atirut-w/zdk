#pragma once

enum class Type {
  None,
  Void,
  Char,
  Int,

  // Special indicator for converting between primitive types
  Widen,
};
