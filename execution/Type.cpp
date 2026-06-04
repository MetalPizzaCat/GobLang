#include "Type.hpp"

#define CASE_ENUM_TO_STR(enumName, enumValue) \
    case enumName::enumValue:            \
        return #enumValue;

const char *GobLang::typeToString(Type type)
{
    switch (type)
    {
        CASE_ENUM_TO_STR(Type, Null)
        CASE_ENUM_TO_STR(Type, Bool)
        CASE_ENUM_TO_STR(Type, Char)
        CASE_ENUM_TO_STR(Type, Float)
        CASE_ENUM_TO_STR(Type, Int)
        CASE_ENUM_TO_STR(Type, UnsignedInt)
        CASE_ENUM_TO_STR(Type, String)
        CASE_ENUM_TO_STR(Type, Array)
        CASE_ENUM_TO_STR(Type, Structure)
        CASE_ENUM_TO_STR(Type, Closure)
        CASE_ENUM_TO_STR(Type, Prototype)
        CASE_ENUM_TO_STR(Type, UserData)
    }
    return nullptr;
}