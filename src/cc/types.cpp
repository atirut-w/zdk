#include <types.hpp>

using namespace std;

map<string, PrimitiveType> primitives = {
    {"void", {0, {}, {}}},
    {"char", {1, {"a"}, {}}},
    {"short", {2, {"l", "h"}, {"hl"}}},
    {"int", {4, {"l", "h", "e", "d"}, {"hl", "de"}}},
};
