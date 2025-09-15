#pragma once
#include <cstdint>

namespace cparse {

struct SourcePosition {
  uint32_t offset = 0;
  int line = 1;
  int column = 1;
};

} // namespace cparse
