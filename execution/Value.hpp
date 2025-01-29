#pragma once
#include <vector>
#include <cstdint>
#include <functional>
#include <variant>

namespace SimpleLang
{
    class Machine;
    class MemoryNode;
    using FunctionValue = std::function<void(Machine *)>;
    using Value = std::variant<float, int32_t, void *, MemoryNode *, FunctionValue>;

    struct MemoryValue
    {
        Type type;
        Value value;
    };
}