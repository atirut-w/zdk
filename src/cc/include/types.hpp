#pragma once
#include <memory>
#include <vector>

struct Type
{
    virtual ~Type() = default; // Force polymorphism
};

// TODO: Make a drop-in replacement for this typedef. Custom class, maybe?
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

struct FunctionType : public Type
{
    ParsedType return_type;
    std::vector<ParsedType> parameters;
};

// TODO: Structs
