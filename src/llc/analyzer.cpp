#include "LLVMIRParser.h"
#include <analyzer.hpp>
#include <iostream>

using namespace std;

string unquote(const string &str)
{
    return str.substr(1, str.size() - 2);
}

any Analyzer::visitCompilationUnit(LLVMIRParser::CompilationUnitContext *ctx)
{
    module_info = ModuleInfo();
    
    for (auto ent : ctx->topLevelEntity())
    {
        if (auto func_def = ent->funcDef())
        {
            auto info = any_cast<FunctionInfo>(visitFuncDef(func_def));
            module_info.functions[info.name] = info;
        }
        else if (auto attr_ctx = ent->attrGroupDef())
        {
            int id = stoi(attr_ctx->AttrGroupId()->getText().substr(1));
            auto group = any_cast<AttributeGroup>(visitAttrGroupDef(attr_ctx));
            module_info.attribute_groups[id] = group;
        }
    }

    return module_info;
}

any Analyzer::visitFuncDef(LLVMIRParser::FuncDefContext *ctx)
{
    FunctionInfo info;

    auto header = ctx->funcHeader();
    info.type = header->type()->getText();
    info.name = header->GlobalIdent()->getText().substr(1);

    for (auto field_ctx : header->funcHdrField())
    {
        if (auto attr_ctx = field_ctx->funcAttribute())
        {
            info.attribute_group = stoi(attr_ctx->AttrGroupId()->getText().substr(1));
            // cout << "Assigned attribute group " << info.attribute_group << " to function " << info.name << endl;
        }
    }

    // TODO: Iterate over args and locals for stack manip

    return info;
}

any Analyzer::visitAttrGroupDef(LLVMIRParser::AttrGroupDefContext *ctx)
{
    AttributeGroup group;
    // cout << "Visiting attribute group definition" << endl;

    for (auto attr_ctx : ctx->funcAttribute())
    {
        if (auto pair_ctx = attr_ctx->attrPair())
        {
            string key = unquote(pair_ctx->StringLit()[0]->getText());
            string value = unquote(pair_ctx->StringLit()[1]->getText());
            group[key] = value;
            // cout << "\t" << key << " = " << value << endl;
        }
        else
        {
            group[attr_ctx->getText()] = "";
            // cout << "\t" << attr_ctx->getText() << endl;
        }
    }

    return group;
}
