#pragma once

#include <vector>
#include <cstdint>
namespace GobLang
{
    // TODO: Cleanup instruction set
    enum class Instruction
    {
        None,
        Add,
        Sub,
        Mul,
        Div,
        Modulo,
        Call,
        SetGlobal,
        GetGlobal,
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
        /// @brief  Unconditionally jump to PC + n bytes forward
        Jump,
        /// @brief  Unconditionally jump to PC + n bytes backward
        JumpBack,
        /// @brief Jump by n bytes, but only if the last value on stack is false
        JumpIfNot,
        /// @brief Jump by n bytes, but only if the last value on stack is true
        JumpIf,
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
        Instruction op;
        const char *text;
        OperatorArgType argType;
    };

    static const std::vector<OperationData> Operations = {
        OperationData{.op = Instruction::None, .text = "noop", .argType = OperatorArgType::None},
        OperationData{.op = Instruction::BitAnd, .text = "bit_and", .argType = OperatorArgType::None},
        OperationData{.op = Instruction::BitOr, .text = "bit_or", .argType = OperatorArgType::None},
        OperationData{.op = Instruction::BitXor, .text = "bit_xor", .argType = OperatorArgType::None},
        OperationData{.op = Instruction::BitNot, .text = "bit_not", .argType = OperatorArgType::None},
        OperationData{.op = Instruction::ShiftLeft, .text = "shift_left", .argType = OperatorArgType::None},
        OperationData{.op = Instruction::ShiftRight, .text = "shift_right", .argType = OperatorArgType::None},
        OperationData{.op = Instruction::Add, .text = "add", .argType = OperatorArgType::None},
        OperationData{.op = Instruction::Sub, .text = "sub", .argType = OperatorArgType::None},
        OperationData{.op = Instruction::Mul, .text = "mul", .argType = OperatorArgType::None},
        OperationData{.op = Instruction::Div, .text = "div", .argType = OperatorArgType::None},
        OperationData{.op = Instruction::Modulo, .text = "mod", .argType = OperatorArgType::None},
        OperationData{.op = Instruction::Call, .text = "call", .argType = OperatorArgType::None},
        OperationData{.op = Instruction::CreateArray, .text = "create_array", .argType = OperatorArgType::Byte},
        OperationData{.op = Instruction::SetGlobal, .text = "set_global", .argType = OperatorArgType::None},
        OperationData{.op = Instruction::GetGlobal, .text = "get_global", .argType = OperatorArgType::None},
        OperationData{.op = Instruction::SetLocal, .text = "set", .argType = OperatorArgType::Byte},
        OperationData{.op = Instruction::GetLocal, .text = "get", .argType = OperatorArgType::Byte},
        OperationData{.op = Instruction::SetArray, .text = "set_arr", .argType = OperatorArgType::None},
        OperationData{.op = Instruction::GetArray, .text = "get_arr", .argType = OperatorArgType::None},
        OperationData{.op = Instruction::SetField, .text = "set_field", .argType = OperatorArgType::None},
        OperationData{.op = Instruction::GetField, .text = "get_field", .argType = OperatorArgType::None},
        OperationData{.op = Instruction::CallMethod, .text = "call_method", .argType = OperatorArgType::None},
        OperationData{.op = Instruction::PushConstInt, .text = "push_int", .argType = OperatorArgType::Int},
        OperationData{.op = Instruction::PushConstUnsignedInt, .text = "push_uint", .argType = OperatorArgType::UnsignedInt},
        OperationData{.op = Instruction::PushConstFloat, .text = "push_float", .argType = OperatorArgType::Float},
        OperationData{.op = Instruction::PushConstChar, .text = "push_char", .argType = OperatorArgType::Char},
        OperationData{.op = Instruction::PushConstString, .text = "push_str", .argType = OperatorArgType::Byte},
        OperationData{.op = Instruction::PushTrue, .text = "push_true", .argType = OperatorArgType::None},
        OperationData{.op = Instruction::PushFalse, .text = "push_false", .argType = OperatorArgType::None},
        OperationData{.op = Instruction::PushNull, .text = "push_null", .argType = OperatorArgType::None},
        OperationData{.op = Instruction::Equals, .text = "eq", .argType = OperatorArgType::None},
        OperationData{.op = Instruction::NotEq, .text = "neq", .argType = OperatorArgType::None},
        OperationData{.op = Instruction::Not, .text = "not", .argType = OperatorArgType::None},
        OperationData{.op = Instruction::Negate, .text = "negate", .argType = OperatorArgType::None},
        OperationData{.op = Instruction::More, .text = "more", .argType = OperatorArgType::None},
        OperationData{.op = Instruction::Less, .text = "less", .argType = OperatorArgType::None},
        OperationData{.op = Instruction::MoreOrEq, .text = "eqmore", .argType = OperatorArgType::None},
        OperationData{.op = Instruction::LessOrEq, .text = "eqless", .argType = OperatorArgType::None},
        OperationData{.op = Instruction::Or, .text = "or", .argType = OperatorArgType::None},
        OperationData{.op = Instruction::And, .text = "and", .argType = OperatorArgType::None},
        OperationData{.op = Instruction::Jump, .text = "jmp_by", .argType = OperatorArgType::Address},
        OperationData{.op = Instruction::JumpBack, .text = "jump_back_by", .argType = OperatorArgType::Address},
        OperationData{.op = Instruction::JumpIfNot, .text = "jmp_by_if_not", .argType = OperatorArgType::Address},
        OperationData{.op = Instruction::JumpIf, .text = "jmp_by_if", .argType = OperatorArgType::Address},
        OperationData{.op = Instruction::ShrinkLocal, .text = "local_free", .argType = OperatorArgType::Byte},
        OperationData{.op = Instruction::Return, .text = "ret", .argType = OperatorArgType::None},
        OperationData{.op = Instruction::ReturnValue, .text = "ret_val", .argType = OperatorArgType::None},
        OperationData{.op = Instruction::New, .text = "new", .argType = OperatorArgType::Byte},
        OperationData{.op = Instruction::End, .text = "hlt", .argType = OperatorArgType::None},
    };
} // namespace SimpleLang
