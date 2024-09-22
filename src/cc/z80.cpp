#include <z80.hpp>

using namespace std;
using namespace Z80;

map<uint8_t, string> Z80::register_names = {
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
};
