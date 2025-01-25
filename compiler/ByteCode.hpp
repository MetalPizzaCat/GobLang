#pragma once
#include <vector>
#include <string>
#include <cstdint>
namespace SimpleLang::Compiler
{
    struct ByteCode
    {
        std::vector<std::string> ids;
        std::vector<int32_t> ints;
        std::vector<uint8_t> operations;
    };
}