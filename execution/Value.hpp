#pragma once
#include <vector>
#include <cstdint>
#include <functional>
#include <variant>
#include "Type.hpp"

namespace SimpleLang
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
}