#pragma once
#include <stdexcept>

struct Positional
{
    int line = 1, col = 1;
};

struct PositionalError : std::runtime_error, Positional
{
    PositionalError(Positional &ctx, const std::string &message) : std::runtime_error(message), Positional(ctx)
    {
    }
};
