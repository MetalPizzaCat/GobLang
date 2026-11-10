#include "Value.hpp"
#include "Memory.hpp"
#include "Closure.hpp"
#include "String.hpp"
#include "Array.hpp"
#include "StructureObject.hpp"
#include "Closure.hpp"
#include "Error.hpp"
#include <format>
#include <iostream>

#define OPERATION_RETURN(a, b, op)                                                    \
    if ((Type)a.index() == Type::Int && (Type)b.index() == Type::Int)                 \
    {                                                                                 \
        return std::get<IntegerType>(a) op std::get<IntegerType>(b);                  \
    }                                                                                 \
    if ((Type)a.index() == Type::Int && (Type)b.index() == Type::Float)               \
    {                                                                                 \
        return std::get<IntegerType>(a) op(IntegerType) std::get<NumberType>(b);      \
    }                                                                                 \
    if ((Type)a.index() == Type::Int && (Type)b.index() == Type::UnsignedInt)         \
    {                                                                                 \
        return std::get<IntegerType>(a) op(IntegerType) std::get<UIntegerType>(b);    \
    }                                                                                 \
    if ((Type)a.index() == Type::Float && (Type)b.index() == Type::Float)             \
    {                                                                                 \
        return std::get<NumberType>(a) op std::get<NumberType>(b);                    \
    }                                                                                 \
    if ((Type)a.index() == Type::Float && (Type)b.index() == Type::Int)               \
    {                                                                                 \
        return std::get<NumberType>(a) op(NumberType) std::get<NumberType>(b);        \
    }                                                                                 \
    if ((Type)a.index() == Type::UnsignedInt && (Type)b.index() == Type::UnsignedInt) \
    {                                                                                 \
        return std::get<UIntegerType>(a) op std::get<UIntegerType>(b);                \
    }                                                                                 \
    if ((Type)a.index() == Type::UnsignedInt && (Type)b.index() == Type::UnsignedInt) \
    {                                                                                 \
        return std::get<UIntegerType>(a) op std::get<UIntegerType>(b);                \
    }                                                                                 \
    if ((Type)a.index() == Type::UnsignedInt && (Type)b.index() == Type::Int)         \
    {                                                                                 \
        return std::get<UIntegerType>(a) op(IntegerType) std::get<IntegerType>(b);    \
    }

bool GobLang::ValueOperations::areEqual(Value const &a, Value const &b)
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
        return std::get<NumberType>(a) == std::get<NumberType>(b);
    case Type::Int:
        return std::get<IntegerType>(a) == std::get<IntegerType>(b);
    case Type::UnsignedInt:
        return std::get<UIntegerType>(a) == std::get<UIntegerType>(b);
    case Type::Char:
        return std::get<char>(a) == std::get<char>(b);
    // while these are different types, they all have same base type so this doesn't matter
    // each type can override their own version of equals
    case Type::String:
        return std::get<StringObject *>(a)->equalsTo(std::get<StringObject *>(b));
    case Type::Array:
        return std::get<ArrayObject *>(a)->equalsTo(std::get<ArrayObject *>(b));
    case Type::Structure:
        return std::get<StructureObject *>(a)->equalsTo(std::get<StructureObject *>(b));
    default:
        // c++ has no equality check for std::function and it is easier to just assume things can't be compared
        // TODO: Consider adding equality checkes to memory based objects to compare their pointers
        return false;
    }
    return false;
}

std::string GobLang::ValueOperations::toString(Value const &val)
{
    switch ((Type)val.index())
    {
    case Type::Null:
        return "null";
    case Type::Bool:
        return std::get<bool>(val) ? "true" : "false";
    case Type::Float:
        return std::to_string(std::get<NumberType>(val));
    case Type::Int:
        return std::to_string(std::get<IntegerType>(val));
    case Type::UnsignedInt:
        return std::to_string(std::get<UIntegerType>(val));
    // while these are different types, they all have same base type so this doesn't matter
    case Type::String:
        return std::get<StringObject *>(val)->toString();
    case Type::Array:
        return std::get<ArrayObject *>(val)->toString();
    case Type::Structure:
        return std::get<StructureObject *>(val)->toString();
    case Type::Char:
        return std::string{std::get<char>(val)};
    case Type::Closure:
        return std::get<Closure const *>(val)->toString();
    default:
        return "Invalid datatype";
    }
}

void GobLang::ValueOperations::increaseValueRefCount(Value const &v)
{
    switch ((Type)v.index())
    {
    case Type::String:
        std::get<StringObject *>(v)->increaseRefCount();
        break;
    case Type::Array:
        std::get<ArrayObject *>(v)->increaseRefCount();
        break;
    case Type::Structure:
        std::get<StructureObject *>(v)->increaseRefCount();
        break;
    default:
        break;
    }
}

void GobLang::ValueOperations::decreaseValueRefCount(Value const &v)
{
    switch ((Type)v.index())
    {
    case Type::String:
        std::get<StringObject *>(v)->decreaseRefCount();
        break;
    case Type::Array:
        std::get<ArrayObject *>(v)->decreaseRefCount();
        break;
    case Type::Structure:
        std::get<StructureObject *>(v)->decreaseRefCount();
        break;
    default:
        break;
    }
}

bool GobLang::ValueOperations::less(Value const &a, Value const &b)
{
    OPERATION_RETURN(a, b, <)
    throw Errors::ExecutionError(std::format("Tried compare {} with {}", typeToString((Type)a.index()), typeToString((Type)b.index())));
}

bool GobLang::ValueOperations::more(Value const &a, Value const &b)
{
    OPERATION_RETURN(a, b, >)
    throw Errors::ExecutionError(std::format("Tried compare {} with {}", typeToString((Type)a.index()), typeToString((Type)b.index())));
}

bool GobLang::ValueOperations::lessEqual(Value const &a, Value const &b)
{
    OPERATION_RETURN(a, b, <=)
    return false;
}
