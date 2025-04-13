#pragma once
#include <vector>
#include <cstdint>
#include <functional>
#include <variant>
#include <string>
#include "Type.hpp"

namespace GobLang
{
     /**
     * @brief Type used to store jump addresses in the code
     *
     */
    using ProgramAddressType = size_t;
    static size_t MAX_PRINT_RECURSION_DEPTH = MAX_PRINT_DEPTH;
    class Machine;
    class MemoryNode;
    using FunctionValue = void (*)(Machine *);
    using Value = std::variant<bool, char, float, int32_t, uint32_t, MemoryNode *, FunctionValue>;

    struct MemoryValue
    {
        Type type;
        Value value;
    };

    /**
     * @brief Compare two memory values and validate that both are equal
     *
     * @param a
     * @param b
     * @return true
     * @return false
     */
    bool areEqual(MemoryValue const &a, MemoryValue const &b);

    /**
     * @brief Create a string representation of a given value
     *
     * @param val Value to convert to string
     * @param pretty Whether to add decorators. Only is relevant for strings during printing
     * @return std::string
     */
    std::string valueToString(MemoryValue const &val, bool pretty, size_t depth);
}