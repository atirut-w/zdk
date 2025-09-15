#pragma once
#include "source_position.hpp"
#include <stdexcept>
#include <string>

namespace cparse {

struct Error : std::runtime_error {
  Error(const SourcePosition &position, const std::string &message)
      : std::runtime_error(message), position(position) {}

  SourcePosition position;
};

} // namespace cparse
