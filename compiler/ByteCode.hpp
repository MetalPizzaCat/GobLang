#pragma once
#include <vector>
#include <string>
#include <cstdint>
#include "../execution/Function.hpp"
namespace GobLang::Compiler
{
    struct ByteCode
    {
        std::vector<std::string> ids;
        std::vector<int32_t> ints;
        std::vector<float> floats;
        std::vector<uint8_t> operations;
        std::vector<Function> functions;
    };
}