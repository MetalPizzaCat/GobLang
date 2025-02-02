#pragma once
#include <vector>
#include <cstdint>
#include <functional>
#include <variant>
#include <string>
#include "Type.hpp"

namespace GobLang
{
    class Machine;
    class MemoryNode;
    using FunctionValue = std::function<void(Machine *)>;
    using Value = std::variant<bool, float, int32_t, void *, MemoryNode *, FunctionValue>;

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
    bool areEqual(MemoryValue const& a, MemoryValue const & b);

    /**
     * @brief Create a string representation of a given value
     * 
     * @param val 
     * @return std::string 
     */
    std::string valueToString(MemoryValue const& val);
}