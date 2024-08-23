#pragma once
#include <memory>

struct Type
{
    virtual ~Type() = default; // Force polymorphism
};

typedef std::shared_ptr<Type> ParsedType;

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
