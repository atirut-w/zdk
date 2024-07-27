#pragma once
#include <map>
#include <string>
#include <vector>

struct Type
{
    int size;
    ~Type() {} // Force polymorphism
};

// TODO: Flesh this out
struct PrimitiveType : public Type
{
    std::vector<std::string> byte_layout;
    std::vector<std::string> word_layout;
    // TODO: Optional subroutine for name for sign extension
};

// TODO: Structs

extern std::map<std::string, PrimitiveType> primitives;
