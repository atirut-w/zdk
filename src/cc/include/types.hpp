#pragma once
#include <map>
#include <memory>
#include <string>
#include <vector>

struct Type
{
    virtual ~Type() = default; // Force polymorphism
};

// TODO: Flesh this out
struct PrimitiveType : public Type
{
    enum Kind
    {
        Void,
        Char,
        Short,
        Int,
    } kind;
    bool is_signed = false;
};

// TODO: Structs

typedef std::shared_ptr<Type> ParsedType;
