#pragma once
#include <vector>
#include <string>
#include <cstdint>
#include "Function.hpp"
#include "../execution/Structure.hpp"
namespace GobLang::Codegen
{
    struct ByteCode
    {
        std::vector<Function> functions;
    };
}