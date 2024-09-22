#pragma once
#include <cstdint>
#include <map>
#include <string>

namespace Z80
{
enum R8 : uint8_t
{
    R8_A = 0b01000000,
    R8_B = 0b00100000,
    R8_C = 0b00010000,
    R8_D = 0b00001000,
    R8_E = 0b00000100,
    R8_H = 0b00000010,
    R8_L = 0b00000001,
};

enum R16 : uint8_t
{
    R16_BC = R8_B | R8_C,
    R16_DE = R8_D | R8_E,
    R16_HL = R8_H | R8_L,
};

extern std::map<uint8_t, std::string> register_names;
} // namespace Z80
