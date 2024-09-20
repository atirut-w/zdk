#include "irgen.hpp"

using namespace std;
using namespace antlr4;
using namespace ZIR;

const Module &IRGen::get_module() const
{
    return module;
}
