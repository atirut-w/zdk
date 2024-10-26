#pragma once
#include "zir/module.hpp"
#include <map>
#include <ostream>
#include <string>

class AsmPrinter {
  ZIR::Module &module;
  std::ostream &os;

  struct {
    ZIR::Module::Function *current_function;
    std::map<std::string, int> offsets;
  } ctx;
  
  void generate_prologue();
  std::string get_ix(int offset);
  void load(const ZIR::Value &value);
  void store(const ZIR::Value &value);

public:
  AsmPrinter(ZIR::Module &module, std::ostream &os) : module(module), os(os) {}

  void print();
};
