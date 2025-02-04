#include "Type.hpp"

const char *GobLang::typeToString(Type type)
{
    switch (type)
    {
    case Type::Null:
        return "Null";
    case Type::Bool:
        return "Bool";
    case Type::Number:
        return "Float";
    case Type::Int:
        return "Int";
    case Type::UserData:
        return "UserData";
    case Type::MemoryObj:
        return "Object";
    case Type::NativeFunction:
        return "NativeFunction";
    case Type::Char:
        return "Char";
    }
    return nullptr;
}