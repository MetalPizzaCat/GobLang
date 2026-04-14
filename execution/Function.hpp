#pragma once
#include <string>
#include <vector>
#include "Type.hpp"
namespace GobLang
{

    enum NativeType
    {
        Null,
        Char,
        Bool,
        Number,
        Int,
        Array,
        String
    };

    struct NativeTypeData
    {
        NativeType type;
        std::string text;
        bool isValidArgumentType;
    };

    static const std::vector<NativeTypeData> NativeTypes = {
        NativeTypeData{.type = NativeType::Null, .text = "none", .isValidArgumentType = false},
        NativeTypeData{.type = NativeType::Char, .text = "char", .isValidArgumentType = true},
        NativeTypeData{.type = NativeType::Bool, .text = "bool", .isValidArgumentType = true},
        NativeTypeData{.type = NativeType::Number, .text = "float", .isValidArgumentType = true},
        NativeTypeData{.type = NativeType::Int, .text = "int", .isValidArgumentType = true},
        NativeTypeData{.type = NativeType::Array, .text = "array", .isValidArgumentType = true},
        NativeTypeData{.type = NativeType::String, .text = "string", .isValidArgumentType = true},
    };

} // namespace GobLang
