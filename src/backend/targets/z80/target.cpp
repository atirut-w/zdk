#include "target.hpp"
#include "backend/target_registry.hpp"
#include "codegen.hpp"

TargetRegistry::Register<Z80Target> z80_target;

Z80Target::Z80Target() {
  name = "z80";
}

std::unique_ptr<TargetCodegen> Z80Target::create_codegen(std::ostream &os, llvm::Module &module) const {
  return std::make_unique<Z80Codegen>(os, module);
}
