#pragma once
#include <string>
#include <vector>
#include <cstdint>
#include <map>
#include "Value.hpp"
#include "Type.hpp"

namespace GobLang::Struct
{

    enum class StructureFieldType
    {
        Char,
        Bool,
        Float,
        Int,
        UnsignedInt,
        MemoryObj,
        Array,
        String
    };

    struct Field
    {
        std::string name;
        StructureFieldType type;
        /// @brief Reference to the custom type if type is MemoryObj
        size_t customStructureTypeId;
    };

    struct Structure
    {
        std::string name;
        std::vector<Field> fields;
    };
}