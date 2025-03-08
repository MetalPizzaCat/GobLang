#include "Type.hpp"

const char *GobLang::typeToString(Type type)
{
    switch (type)
    {
    case Type::Null:
        return "Null";
    case Type::Bool:
        return "Bool";
    case Type::Float:
        return "Float";
    case Type::Int:
        return "Int";
    case Type::UnsignedInt:
        return "UnsignedInt";
    case Type::MemoryObj:
        return "Object";
    case Type::NativeFunction:
        return "NativeFunction";
    case Type::Char:
        return "Char";
    }
    return nullptr;
}