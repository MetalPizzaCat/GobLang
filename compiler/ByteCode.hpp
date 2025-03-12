#pragma once
#include <vector>
#include <string>
#include <cstdint>
#include "../execution/Function.hpp"
#include "../execution/Structure.hpp"
namespace GobLang::Compiler
{
    struct ByteCode
    {
        std::vector<std::string> ids;
        std::vector<uint8_t> operations;
        std::vector<Function> functions;
        std::vector<Struct::Structure> structures;
    };
}