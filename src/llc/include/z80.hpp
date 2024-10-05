#pragma once

#include <map>
#include <string>
namespace Z80 {
  enum R8 {
    R8_A = 1 << 6,
    R8_B = 1 << 5,
    R8_C = 1 << 4,
    R8_D = 1 << 3,
    R8_E = 1 << 2,
    R8_H = 1 << 1,
    R8_L = 1 << 0,
  };

  enum R16 {
    R16_BC = R8_B | R8_C,
    R16_DE = R8_D | R8_E,
    R16_HL = R8_H | R8_L,
  };

  extern std::map<int, std::string> register_names;
}
