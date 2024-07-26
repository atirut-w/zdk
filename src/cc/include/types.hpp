#pragma once
#include <map>
#include <string>

struct Type
{
    int size;
    ~Type() {} // Force polymorphism
};

// TODO: Flesh this out
struct PrimitiveType : public Type
{

};

// TODO: Structs

extern std::map<std::string, PrimitiveType> primitives;
