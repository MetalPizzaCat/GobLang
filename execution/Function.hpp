#pragma once
#include <string>
#include <vector>
#include "Type.hpp"
namespace GobLang
{
    struct FunctionArgInfo
    {
        size_t nameId;
        Type type;
        /**
         * @brief If true this argument should be wrapped into an object which would pass all data to the original value
         *
         */
        bool reference;
    };

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

    /**
     * @brief Data structure storing info for functions and procedures that are written by user and can be called in the code
     *
     */
    struct Function
    {
        size_t nameId;
        /**
         * @brief Which type this function returns with Null meaning nothing
         */
        Type returnType;
        std::vector<FunctionArgInfo> arguments;
        /**
         * @brief Which address to jump to when calling this function
         */
        size_t start;
    };
} // namespace GobLang
