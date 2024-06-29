#pragma once
#include <memory>
#include <ostream>

#include <llvm/IR/Module.h>

class CodeGen
{
private:
    std::unique_ptr<llvm::Module> &module;
    std::ostream &output;

    void enter();
    void leave();

public:
    CodeGen(std::unique_ptr<llvm::Module> &module, std::ostream &output);

    void generate();
};
