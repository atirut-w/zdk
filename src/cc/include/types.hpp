#pragma once
#include <map>
#include <string>
#include <vector>

struct Type
{
    int size;

    Type() = default;
    Type(int size) : size{size} {}
    virtual ~Type() = default; // Force polymorphism
};

// TODO: Flesh this out
struct PrimitiveType : public Type
{
    std::vector<std::string> byte_layout;
    std::vector<std::string> word_layout;
    // TODO: Optional subroutine for name for sign extension

    PrimitiveType() = default;
    PrimitiveType(int size, std::vector<std::string> byte_layout, std::vector<std::string> word_layout)
        : Type{size}, byte_layout{byte_layout}, word_layout{word_layout}
    {
    }
};

// TODO: Structs

extern std::map<std::string, PrimitiveType> primitives;
