#include "allocator.hpp"
#include <cstdint>
#include <map>
#include <stdexcept>

using namespace std;

Allocator::Allocator(const vector<RegisterDefinition> &register_defs)
    : register_defs(register_defs) {}

const string &Allocator::get_register_name(int mask) const {
  for (const RegisterDefinition &def : register_defs) {
    if (def.mask == mask)
      return def.name;
  }
  throw runtime_error("register not found");
}

int Allocator::allocate(int size) {
  for (RegisterDefinition &def : register_defs) {
    if (def.size != size)
      continue;

    if ((usage & def.mask) == 0) {
      usage |= def.mask;
      return def.mask;
    }
  }
  return 0;
}

int Allocator::allocate_register(int mask) {
  if ((usage & mask) == 0) {
    usage |= mask;
    return mask;
  }
  return 0;
}

void Allocator::free(int mask) { usage &= ~mask; }

// vector<pair<int, int>> Allocator::vacate(int mask) {
//   vector<pair<int, int>> vacated;
//   usage |= mask; // Prevent re-allocating the same registers.

//   for (RegisterDefinition &def : register_defs) {
//     if ((def.mask & usage) == mask) {
//       vacated.push_back({def.mask, allocate(def.size)});
//     }
//   }

//   usage &= ~mask;
//   return vacated;
// }
