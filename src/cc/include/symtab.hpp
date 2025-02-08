#pragma once
#include "type.hpp"
#include <map>
#include <string>

struct Symbol {
  enum Kind {
    Variable,
    Function,
  };

  Kind kind;
  Type type;
};

class Symtab {
public:
  std::map<std::string, Symbol> gsym;
};
