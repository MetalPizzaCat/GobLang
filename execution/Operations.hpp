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
        GetLocalFunction,
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
        GetField,
        SetField,
        CallMethod,
        PushConstInt,
        PushConstUnsignedInt,
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
         * @brief Create a new object from a structure. Uses 
         */
        New,
        /**
         * @brief End program execution
         */
        End
    };

    enum class OperatorArgType
    {
        None,
        Char,
        Byte,
        Address,
        Int,
        UnsignedInt,
        Float
    };

    struct OperationData
    {
        Operation op;
        const char *text;
        OperatorArgType argType;
    };

    static const std::vector<OperationData> Operations = {
        OperationData{.op = Operation::None, .text = "noop", .argType = OperatorArgType::None},
        OperationData{.op = Operation::BitAnd, .text = "bit_and", .argType = OperatorArgType::None},
        OperationData{.op = Operation::BitOr, .text = "bit_or", .argType = OperatorArgType::None},
        OperationData{.op = Operation::BitXor, .text = "bit_xor", .argType = OperatorArgType::None},
        OperationData{.op = Operation::BitNot, .text = "bit_not", .argType = OperatorArgType::None},
        OperationData{.op = Operation::ShiftLeft, .text = "shift_left", .argType = OperatorArgType::None},
        OperationData{.op = Operation::ShiftRight, .text = "shift_right", .argType = OperatorArgType::None},
        OperationData{.op = Operation::Add, .text = "add", .argType = OperatorArgType::None},
        OperationData{.op = Operation::Sub, .text = "sub", .argType = OperatorArgType::None},
        OperationData{.op = Operation::Mul, .text = "mul", .argType = OperatorArgType::None},
        OperationData{.op = Operation::Div, .text = "div", .argType = OperatorArgType::None},
        OperationData{.op = Operation::Modulo, .text = "mod", .argType = OperatorArgType::None},
        OperationData{.op = Operation::Call, .text = "call", .argType = OperatorArgType::None},
        OperationData{.op = Operation::GetLocalFunction, .text = "get_local_func", .argType = OperatorArgType::Byte},
        OperationData{.op = Operation::CreateArray, .text = "create_array", .argType = OperatorArgType::Byte},
        OperationData{.op = Operation::Set, .text = "set_global", .argType = OperatorArgType::None},
        OperationData{.op = Operation::Get, .text = "get_global", .argType = OperatorArgType::None},
        OperationData{.op = Operation::SetLocal, .text = "set", .argType = OperatorArgType::Byte},
        OperationData{.op = Operation::GetLocal, .text = "get", .argType = OperatorArgType::Byte},
        OperationData{.op = Operation::SetArray, .text = "set_arr", .argType = OperatorArgType::None},
        OperationData{.op = Operation::GetArray, .text = "get_arr", .argType = OperatorArgType::None},
        OperationData{.op = Operation::SetField, .text = "set_field", .argType = OperatorArgType::None},
        OperationData{.op = Operation::GetField, .text = "get_field", .argType = OperatorArgType::None},
        OperationData{.op = Operation::CallMethod, .text = "call_method", .argType = OperatorArgType::None},
        OperationData{.op = Operation::PushConstInt, .text = "push_int", .argType = OperatorArgType::Int},
        OperationData{.op = Operation::PushConstUnsignedInt, .text = "push_uint", .argType = OperatorArgType::UnsignedInt},
        OperationData{.op = Operation::PushConstFloat, .text = "push_float", .argType = OperatorArgType::Float},
        OperationData{.op = Operation::PushConstChar, .text = "push_char", .argType = OperatorArgType::Char},
        OperationData{.op = Operation::PushConstString, .text = "push_str", .argType = OperatorArgType::Byte},
        OperationData{.op = Operation::PushTrue, .text = "push_true", .argType = OperatorArgType::None},
        OperationData{.op = Operation::PushFalse, .text = "push_false", .argType = OperatorArgType::None},
        OperationData{.op = Operation::PushNull, .text = "push_null", .argType = OperatorArgType::None},
        OperationData{.op = Operation::Equals, .text = "eq", .argType = OperatorArgType::None},
        OperationData{.op = Operation::NotEq, .text = "neq", .argType = OperatorArgType::None},
        OperationData{.op = Operation::Not, .text = "not", .argType = OperatorArgType::None},
        OperationData{.op = Operation::Negate, .text = "negate", .argType = OperatorArgType::None},
        OperationData{.op = Operation::More, .text = "more", .argType = OperatorArgType::None},
        OperationData{.op = Operation::Less, .text = "less", .argType = OperatorArgType::None},
        OperationData{.op = Operation::MoreOrEq, .text = "eqmore", .argType = OperatorArgType::None},
        OperationData{.op = Operation::LessOrEq, .text = "eqless", .argType = OperatorArgType::None},
        OperationData{.op = Operation::Jump, .text = "goto", .argType = OperatorArgType::Address},
        OperationData{.op = Operation::JumpIfNot, .text = "goto_if_not", .argType = OperatorArgType::Address},
        OperationData{.op = Operation::ShrinkLocal, .text = "local_free", .argType = OperatorArgType::Byte},
        OperationData{.op = Operation::Return, .text = "ret", .argType = OperatorArgType::None},
        OperationData{.op = Operation::ReturnValue, .text = "ret_val", .argType = OperatorArgType::None},
        OperationData{.op = Operation::New, .text = "new", .argType = OperatorArgType::Byte},
        OperationData{.op = Operation::End, .text = "hlt", .argType = OperatorArgType::None},
    };
} // namespace SimpleLang
