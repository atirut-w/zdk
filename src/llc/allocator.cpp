#include "allocator.hpp"
#include <cstdint>
#include <map>
#include <stdexcept>

using namespace std;

map<uint8_t, string> Allocator::register_names = {
    {R8_A, "a"}, {R8_B, "b"}, {R8_C, "c"},    {R8_D, "d"},    {R8_E, "e"},
    {R8_H, "h"}, {R8_L, "l"}, {R16_BC, "bc"}, {R16_DE, "de"}, {R16_HL, "hl"}};

uint8_t Allocator::allocate_r8() {
  for (uint8_t i = 0; i < 7; i++) {
    uint8_t mask = 1 << i;
    if ((state & mask) == 0) {
      state |= mask;
      return mask;
    }
  }
  throw runtime_error("out of registers");
}

uint8_t Allocator::allocate_r16() {
  for (uint8_t i = 0; i < 3; i++) {
    uint8_t mask = 0b11 << (i * 2);
    if ((state & mask) == 0) {
      state |= mask;
      return mask;
    }
  }
  throw runtime_error("out of registers");
}

bool Allocator::allocate(uint8_t regs) {
  if ((state & regs) == 0) {
    state |= regs;
    return true;
  }
  return false;
}

void Allocator::free(uint8_t regs) { state &= ~regs; }
