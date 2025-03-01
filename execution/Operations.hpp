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
        Mul,
        Div,
        Modulo,
        Call,
        /**
         * @brief Call a function defined by the user
         */
        CallLocal,
        Set,
        Get,
        GetLocal,
        SetLocal,
        /**
         * @brief Get value of the nth element of an array
         */
        GetArray,
        /**
         * @brief Set value of the nth element of an array
         */
        SetArray,
        PushConstInt,
        PushConstFloat,
        PushConstChar,
        PushConstString,
        PushTrue,
        PushFalse,
        PushNull,
        Equals,
        Less,
        More,
        LessOrEq,
        MoreOrEq,
        NotEq,
        And,
        Or,
        Not,
        BitAnd,
        BitOr,
        BitXor,
        BitNot,
        ShiftLeft,
        ShiftRight,
        Negate,
        /**
         * @brief Unconditionally jump. Uses sizeof(size_t) bytes to get the address to jump to
         */
        Jump,
        /**
         * @brief Jump only if the last operation on stack is false. Uses sizeof(size_t) bytes to get the address to jump to
         */
        JumpIfNot,
        /**
         * @brief Shrink local variable array by n variables
         */
        ShrinkLocal,
        /**
         * @brief Exit function and return to prev pos in call stack
         */
        Return,
        /**
         * @brief Exit function and return to prev pos in call stack, while also pushing value to the stack
         */
        ReturnValue,
        /**
         * @brief Create an array of size n using values from stack. Exists to provide a native way to make arrays
         */
        CreateArray,
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
        OperationData{.op = Operation::BitAnd, .text = "bit_and", .argCount = 0},
        OperationData{.op = Operation::BitOr, .text = "bit_or", .argCount = 0},
        OperationData{.op = Operation::BitXor, .text = "bit_xor", .argCount = 0},
        OperationData{.op = Operation::BitNot, .text = "bit_not", .argCount = 0},
        OperationData{.op = Operation::ShiftLeft, .text = "shift_left", .argCount = 0},
        OperationData{.op = Operation::ShiftRight, .text = "shift_right", .argCount = 0},
        OperationData{.op = Operation::Add, .text = "add", .argCount = 0},
        OperationData{.op = Operation::Sub, .text = "sub", .argCount = 0},
        OperationData{.op = Operation::Mul, .text = "mul", .argCount = 0},
        OperationData{.op = Operation::Div, .text = "div", .argCount = 0},
        OperationData{.op = Operation::Modulo, .text = "mod", .argCount = 0},
        OperationData{.op = Operation::Call, .text = "call", .argCount = 0},
        OperationData{.op = Operation::CallLocal, .text = "call_local", .argCount = 1},
        OperationData{.op = Operation::CreateArray, .text = "create_array", .argCount = 1},
        OperationData{.op = Operation::Set, .text = "set_global", .argCount = 0},
        OperationData{.op = Operation::Get, .text = "get_global", .argCount = 0},
        OperationData{.op = Operation::SetLocal, .text = "set", .argCount = 1},
        OperationData{.op = Operation::GetLocal, .text = "get", .argCount = 1},
        OperationData{.op = Operation::SetArray, .text = "set_arr", .argCount = 0},
        OperationData{.op = Operation::GetArray, .text = "get_arr", .argCount = 0},
        OperationData{.op = Operation::PushConstInt, .text = "push_int", .argCount = 1},
        OperationData{.op = Operation::PushConstFloat, .text = "push_float", .argCount = 1},
        OperationData{.op = Operation::PushConstChar, .text = "push_char", .argCount = 1},
        OperationData{.op = Operation::PushConstString, .text = "push_str", .argCount = 1},
        OperationData{.op = Operation::PushTrue, .text = "push_true", .argCount = 0},
        OperationData{.op = Operation::PushFalse, .text = "push_false", .argCount = 0},
        OperationData{.op = Operation::PushNull, .text = "push_null", .argCount = 0},
        OperationData{.op = Operation::Equals, .text = "eq", .argCount = 0},
        OperationData{.op = Operation::NotEq, .text = "neq", .argCount = 0},
        OperationData{.op = Operation::Not, .text = "not", .argCount = 0},
        OperationData{.op = Operation::Negate, .text = "negate", .argCount = 0},
        OperationData{.op = Operation::More, .text = "more", .argCount = 0},
        OperationData{.op = Operation::Less, .text = "less", .argCount = 0},
        OperationData{.op = Operation::MoreOrEq, .text = "eqmore", .argCount = 0},
        OperationData{.op = Operation::LessOrEq, .text = "eqless", .argCount = 0},
        OperationData{.op = Operation::Jump, .text = "goto", .argCount = sizeof(size_t)},
        OperationData{.op = Operation::JumpIfNot, .text = "goto_if_not", .argCount = sizeof(size_t)},
        OperationData{.op = Operation::ShrinkLocal, .text = "local_free", .argCount = 1},
        OperationData{.op = Operation::Return, .text = "ret", .argCount = 0},
        OperationData{.op = Operation::ReturnValue, .text = "ret_val", .argCount = 0},
        OperationData{.op = Operation::End, .text = "hlt", .argCount = 0},
    };
} // namespace SimpleLang
