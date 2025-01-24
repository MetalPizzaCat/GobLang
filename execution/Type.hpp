#pragma once
#include <functional>
#include <variant>
#include "Machine.hpp"
#include "Memory.hpp"

namespace SimpleLang
{
    class Machine;
    enum class Type
    {
        Null,
        Number,
        Int,
        UserData,
        MemoryObj,
        NativeFunction,
    };
    enum class Operation
    {
        None,
        Add,
        Sub,
        Call,
        Set,
        Get,
        PushConstInt,
        PushConstString,
    };

    using FunctionValue = std::function<void(Machine *)>;
    using Value = std::variant<float, int32_t, void *, MemoryNode *, FunctionValue>;

    struct MemoryValue
    {
        Type type;
        Value value;
    };
}