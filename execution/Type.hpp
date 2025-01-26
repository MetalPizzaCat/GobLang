#pragma once
#include <functional>
#include <variant>
#include <vector>
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
        /**
         * @brief End program execution
         */
        End
    };

    struct OperationData
    {
        Operation op;
        const char *text;
        int32_t argCount;
    };

    static const std::vector<OperationData> Operations = {
        OperationData{.op = Operation::None, .text = "noop", .argCount = 0},
        OperationData{.op = Operation::Add, .text = "add", .argCount = 0},
        OperationData{.op = Operation::Sub, .text = "sub", .argCount = 0},
        OperationData{.op = Operation::Call, .text = "call", .argCount = 0},
        OperationData{.op = Operation::Set, .text = "setvar", .argCount = 0},
        OperationData{.op = Operation::Get, .text = "getvar", .argCount = 0},
        OperationData{.op = Operation::PushConstInt, .text = "push_int", .argCount = 1},
        OperationData{.op = Operation::PushConstString, .text = "push_str", .argCount = 1},
        OperationData{.op = Operation::End, .text = "hlt", .argCount = 0}};

    using FunctionValue = std::function<void(Machine *)>;
    using Value = std::variant<float, int32_t, void *, MemoryNode *, FunctionValue>;

    struct MemoryValue
    {
        Type type;
        Value value;
    };
}