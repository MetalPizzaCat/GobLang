#pragma once

namespace GobLang
{

    enum class Type
    {
        /// @brief Representation of nothing value
        Null,
        /// @brief Value that can be true or false
        Bool,
        /// @brief Single character representation in the form of one byte
        Char,
        /// @brief Floating point number 
        Float,
        /// @brief Integer value with positive and negative range
        Int,
        /// @brief Unsigned value the size of the integer number with only the positive range
        UnsignedInt,
        /// @brief Dynamically resizable string
        String,
        /// @brief List representation
        Array,
        /// @brief Custom object that can contain custom fields
        Structure,
        /// @brief Object representing a callable instance, such as goblang code or native function
        Closure,
        /// @brief Representation of goblang executable code with all necessary data
        Prototype,
        /// @brief  Type that allows storing any value that could be needed in the native callback
        UserData
    };

    /**
     * @brief Convert type enum value into a readable string
     *
     * @param type
     * @return const char* C string with the type name
     */
    const char *typeToString(Type type);
}