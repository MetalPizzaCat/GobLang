#include "Value.hpp"
#include "Memory.hpp"
#include "Closure.hpp"
#include <iostream>
bool GobLang::areEqual(Value const &a, Value const &b)
{
    if (a.index() != b.index())
    {
        return false;
    }
    switch ((Type)a.index())
    {
    case Type::Null:
        return true;
    case Type::Bool:
        return std::get<bool>(a) == std::get<bool>(b);
    case Type::Float:
        return std::get<float>(a) == std::get<float>(b);
    case Type::Int:
        return std::get<int32_t>(a) == std::get<int32_t>(b);
    case Type::UnsignedInt:
        return std::get<uint32_t>(a) == std::get<uint32_t>(b);
    case Type::Char:
        return std::get<char>(a) == std::get<char>(b);
    case Type::MemoryObj:
        return std::get<Object *>(a)->equalsTo(std::get<Object *>(b));
    default:
        // c++ has no equality check for std::function and it is easier to just assume things can't be comparef
        // TODO: Consider adding equality checkes to memory based objects to compare their pointers
        return false;
    }
    return false;
}

std::string GobLang::valueToString(Value const &val)
{
    switch ((Type)val.index())
    {
    case Type::Null:
        return "null";
    case Type::Bool:
        return std::get<bool>(val) ? "true" : "false";
    case Type::Float:
        return std::to_string(std::get<float>(val));
    case Type::Int:
        return std::to_string(std::get<int32_t>(val));
    case Type::UnsignedInt:
        return std::to_string(std::get<uint32_t>(val));
    case Type::MemoryObj:
        return std::get<Object *>(val)->toString();
    case Type::Char:
        return std::string{std::get<char>(val)};
    case Type::Closure:
        return std::get<Closure const *>(val)->toString();
    }
    return "Invalid datatype";
}
