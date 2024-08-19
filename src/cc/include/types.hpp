#pragma once
#include <map>
#include <memory>
#include <string>
#include <vector>

struct Type
{
    int size;
    virtual ~Type() = default; // Force polymorphism
};

// TODO: Flesh this out
struct PrimitiveType : public Type
{
    std::vector<std::string> byte_layout;
    std::vector<std::string> word_layout;
    bool is_signed = false;
    // TODO: Optional subroutine for name for sign extension
};

// TODO: Structs

typedef std::shared_ptr<Type> ParsedType;
