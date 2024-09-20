#pragma once
#include "zir/module.hpp"
#include <ostream>

class Codegen
{
    std::ostream &out;
    const ZIR::Module &module;

public:
    Codegen(std::ostream &out, const ZIR::Module &module);

    void generate();
};
