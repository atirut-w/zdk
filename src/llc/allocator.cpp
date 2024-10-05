#include "allocator.hpp"
#include <cstdint>
#include <map>
#include <stdexcept>

using namespace std;

int Allocator::allocate(int size) {
  int mask = (1 << size) - 1;
  for (int i = 0; i < 7; i++) {
    if (usage & mask) {
      mask <<= size;
    } else {
      usage |= mask;
      return mask;
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
