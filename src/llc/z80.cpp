
#include "z80.hpp"
#include <map>
#include <string>
using namespace std;

map<int, string> Z80::register_names = {
    {R8_A, "a"},
    {R8_B, "b"},
    {R8_C, "c"},
    {R8_D, "d"},
    {R8_E, "e"},
    {R8_H, "h"},
    {R8_L, "l"},
    {R16_BC, "bc"},
    {R16_DE, "de"},
    {R16_HL, "hl"},
    {R16_BC | R16_DE, "bcde"},
    {R16_DE | R16_HL, "dehl"},
};
