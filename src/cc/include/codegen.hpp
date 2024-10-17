#pragma once
#include "CBaseVisitor.h"

class Codegen : public CBaseVisitor {
  std::ostream &os;

public:
  Codegen(std::ostream &os) : os(os) {}
};
