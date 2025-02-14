#pragma once

namespace GobLang
{

    enum class Type
    {
        Null,
        Char,
        Bool,
        Float,
        Int,
        UserData,
        MemoryObj,
        NativeFunction,
    };

    /**
     * @brief Convert type enum value into a readable string
     * 
     * @param type 
     * @return const char* C string with the type name 
     */
    const char* typeToString(Type type);
}