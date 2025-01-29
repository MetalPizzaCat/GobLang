#pragma once

namespace SimpleLang
{

    enum class Type
    {
        Null,
        Number,
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