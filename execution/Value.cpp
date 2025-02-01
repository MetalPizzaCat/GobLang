#include "Value.hpp"
#include "Memory.hpp"

bool SimpleLang::areEqual(MemoryValue const &a, MemoryValue const &b)
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
    case Type::Number:
        return std::get<float>(a.value) == std::get<float>(b.value);
    case Type::Int:
        return std::get<int32_t>(a.value) == std::get<int32_t>(b.value);
    case Type::UserData:
        return std::get<void *>(a.value) == std::get<void *>(b.value);
    case Type::MemoryObj:
        return std::get<MemoryNode *>(a.value)->equalsTo(std::get<MemoryNode *>(b.value));
    case Type::NativeFunction:
        // c++ has no equality check for std::function
        return false;
    }
    return false;
}