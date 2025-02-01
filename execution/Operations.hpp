#pragma once

#include <vector>
#include <cstdint>
namespace GobLang
{
    enum class Operation
    {
        None,
        Add,
        Sub,
        Call,
        Set,
        Get,
        /**
         * @brief Get value of the nth element of an array
         */
        GetArray,
        /**
         * @brief Set value of the nth element of an array
         */
        SetArray,
        PushConstInt,
        PushConstString,
        PushTrue,
        PushFalse,
        Equals,
        /**
         * @brief Unconditionally jump. Uses sizeof(size_t) bytes to get the address to jump to
         */
        Jump,
        /**
         * @brief Jump only if the last operation on stack is false. Uses sizeof(size_t) bytes to get the address to jump to
         */
        JumpIfNot,
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
        OperationData{.op = Operation::SetArray, .text = "set_arr", .argCount = 0},
        OperationData{.op = Operation::GetArray, .text = "get_arr", .argCount = 0},
        OperationData{.op = Operation::PushConstInt, .text = "push_int", .argCount = 1},
        OperationData{.op = Operation::PushConstString, .text = "push_str", .argCount = 1},
        OperationData{.op = Operation::PushTrue, .text = "push_true", .argCount = 0},
        OperationData{.op = Operation::PushFalse, .text = "push_false", .argCount = 0},
        OperationData{.op = Operation::Equals, .text = "eq", .argCount = 0},
        OperationData{.op = Operation::Jump, .text = "goto", .argCount = sizeof(size_t)},
        OperationData{.op = Operation::JumpIfNot, .text = "goto_if_not", .argCount = sizeof(size_t)},
        OperationData{.op = Operation::End, .text = "hlt", .argCount = 0}};
} // namespace SimpleLang
