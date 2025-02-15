#include "Value.hpp"
#include "Memory.hpp"
#include <iostream>
bool GobLang::areEqual(MemoryValue const &a, MemoryValue const &b)
{
    if (a.type != b.type)
    {
        return false;
    }
    switch (a.type)
    {
    case Type::Null:
        return true;
    case Type::Bool:
        return std::get<bool>(a.value) == std::get<bool>(b.value);
    case Type::Float:
        return std::get<float>(a.value) == std::get<float>(b.value);
    case Type::Int:
        return std::get<int32_t>(a.value) == std::get<int32_t>(b.value);
    case Type::Char:
        return std::get<char>(a.value) == std::get<char>(b.value);
    case Type::MemoryObj:
        return std::get<MemoryNode *>(a.value)->equalsTo(std::get<MemoryNode *>(b.value));
    case Type::NativeFunction:
        // c++ has no equality check for std::function
        return false;
    }
    return false;
}

std::string GobLang::valueToString(MemoryValue const &val)
{
    switch (val.type)
    {
    case Type::Null:
        return "null";
    case Type::Bool:
        return std::get<bool>(val.value) ? "true" : "false";
    case Type::Float:
        return std::to_string(std::get<float>(val.value));
    case Type::Int:
        return std::to_string(std::get<int32_t>(val.value));
    case Type::MemoryObj:
        return std::get<MemoryNode *>(val.value)->toString();
    case Type::Char:
        return std::string{std::get<char>(val.value)};
    case Type::NativeFunction:
        // c++ has no equality check for std::function
        return "Native function";
    }
    return "Invalid datatype";
}
