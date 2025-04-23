#pragma once
#include <vector>
#include <map>
#include <cstdint>
#include <string>
#include "../execution/Operations.hpp"
namespace GobLang::Codegen
{
    enum class Keyword
    {
        Int,
        Float,
        Let,
        Function,
        Return,
        True,
        False,
        If,
        Elif,
        Else,
        While,
        Continue,
        Break,
        Struct,
        FunctionReturnType
    };

    enum class Operator
    {
        Equals,
        Assign,
        NotEqual,
        Less,
        More,
        LessEq,
        MoreEq,
        Add,
        Sub,
        Mul,
        Div,
        AddAssign,
        SubAssign,
        MulAssign,
        DivAssign,
        ModuloAssign,
        Modulo,
        And,
        Or,
        Not,
        BitAnd,
        BitOr,
        BitXor,
        BitNot,
        BitLeftShift,
        BitRightShift,
        BitAndAssign,
        BitOrAssign,
        BitXorAssign,
        BitNotAssign,
        BitLeftShiftAssign,
        BitRightShiftAssign,
    };

    enum class Separator
    {
        BracketOpen,  // (
        BracketClose, // )
        BlockOpen,    // {
        BlockClose,   // }
        ArrayOpen,    // [
        ArrayClose,   // ]
        Dot,
        Colon, // :
        Comma, // ,
        End,   // ;
    };

    /**
     * @brief Data for the operators that can be used in the language
     *
     */
    struct OperatorData
    {
        /// @brief Character sequence representing the operation
        const char *symbol;
        /// @brief What operator it represents
        Operator op;
        /**
         * @brief Priority used for construction of reverse polish notation.
         *  Lower priority operators will be processed later.
         *  If '*' has priority of 2 and '+' has priority of '1', then without any brackets '*'
         *  will be processed first
         *
         */
        int32_t priority;
        /// @brief  What bytecode corresponds to this operation
        Operation operation;
        /// @brief If true this operator will represent operations like '+=' where operation is performed on itself
        bool isCombinedAssignment;
    };

    struct SeparatorData
    {
        char symbol;
        Separator separator;
        int32_t priority;
    };

    /**
     * @brief Info about all keywords used in the parser
     *
     */
    static const std::map<std::string, Keyword,  std::less<>> Keywords = {
        //{"int", Keyword::Int},
        //{"float", Keyword::Float},
        {"func", Keyword::Function},
        {"return", Keyword::Return},
        {"if", Keyword::If},
        {"while", Keyword::While},
        {"continue", Keyword::Continue},
        {"break", Keyword::Break},
        {"elif", Keyword::Elif},
        {"else", Keyword::Else},
        // this is kinda cheating but it's also less clunky than whole special parser
        {"->", Keyword::FunctionReturnType},
        {"let", Keyword::Let},
        {"type", Keyword::Struct}};

    static const std::map<std::string, bool,  std::less<>> Booleans = {
        {"true", true},
        {"false", false},
        {"True", true},
        {"False", false}};

    static const std::map<Operator, Operator> AssignmentAndActionOperatorOrigins = {
        {Operator::BitRightShiftAssign, Operator::BitRightShift},
        {Operator::BitLeftShiftAssign, Operator::BitLeftShift},
        {Operator::AddAssign, Operator::Add},
        {Operator::SubAssign, Operator::Sub},
        {Operator::MulAssign, Operator::Mul},
        {Operator::DivAssign, Operator::Div},
        {Operator::BitAndAssign, Operator::BitAnd},
        {Operator::BitNotAssign, Operator::BitNot},
        {Operator::BitXorAssign, Operator::BitXor},
        {Operator::ModuloAssign, Operator::Modulo},
    };

    /**
     * @brief Static array containing info about all operators used in the Codegen
     *
     */
    static const std::vector<OperatorData> Operators = {
        // logic operators
        OperatorData{.symbol = "==", .op = Operator::Equals, .priority = 5, .operation = Operation::Equals},
        OperatorData{.symbol = ">=", .op = Operator::LessEq, .priority = 5, .operation = Operation::MoreOrEq},
        OperatorData{.symbol = "<=", .op = Operator::MoreEq, .priority = 5, .operation = Operation::LessOrEq},
        // assignment
        OperatorData{.symbol = "=", .op = Operator::Assign, .priority = 1, .operation = Operation::Set},
        // bool operators
        OperatorData{.symbol = "!=", .op = Operator::NotEqual, .priority = 5, .operation = Operation::NotEq},
        OperatorData{.symbol = "!", .op = Operator::Not, .priority = 5, .operation = Operation::Not},
        OperatorData{.symbol = ">>=", .op = Operator::BitRightShiftAssign, .priority = 1, .operation = Operation::ShiftRight},
        OperatorData{.symbol = "<<=", .op = Operator::BitLeftShiftAssign, .priority = 1, .operation = Operation::ShiftLeft},
        OperatorData{.symbol = ">>", .op = Operator::BitRightShift, .priority = 2, .operation = Operation::ShiftRight},
        OperatorData{.symbol = "<<", .op = Operator::BitLeftShift, .priority = 2, .operation = Operation::ShiftLeft},
        OperatorData{.symbol = "<", .op = Operator::Less, .priority = 5, .operation = Operation::Less},
        OperatorData{.symbol = ">", .op = Operator::More, .priority = 5, .operation = Operation::More},
        // math
        OperatorData{.symbol = "+=", .op = Operator::AddAssign, .priority = 1, .operation = Operation::Add},
        OperatorData{.symbol = "-=", .op = Operator::SubAssign, .priority = 1, .operation = Operation::Sub},
        OperatorData{.symbol = "*=", .op = Operator::MulAssign, .priority = 1, .operation = Operation::Mul},
        OperatorData{.symbol = "/=", .op = Operator::DivAssign, .priority = 1, .operation = Operation::Div},
        OperatorData{.symbol = "+", .op = Operator::Add, .priority = 6, .operation = Operation::Add},
        OperatorData{.symbol = "-", .op = Operator::Sub, .priority = 6, .operation = Operation::Sub},
        OperatorData{.symbol = "*", .op = Operator::Mul, .priority = 7, .operation = Operation::Mul},
        OperatorData{.symbol = "/", .op = Operator::Div, .priority = 7, .operation = Operation::Div},
        // boolean operators
        OperatorData{.symbol = "&&", .op = Operator::And, .priority = 4, .operation = Operation::And},
        OperatorData{.symbol = "||", .op = Operator::Or, .priority = 3, .operation = Operation::Or},
        OperatorData{.symbol = "and", .op = Operator::And, .priority = 4, .operation = Operation::And},
        OperatorData{.symbol = "or", .op = Operator::Or, .priority = 3, .operation = Operation::Or},
        // bit math
        OperatorData{.symbol = "&=", .op = Operator::BitAndAssign, .priority = 1, .operation = Operation::BitAnd},
        OperatorData{.symbol = "~=", .op = Operator::BitNotAssign, .priority = 1, .operation = Operation::BitNot},
        OperatorData{.symbol = "^=", .op = Operator::BitXorAssign, .priority = 1, .operation = Operation::BitXor},
        OperatorData{.symbol = "%=", .op = Operator::ModuloAssign, .priority = 1, .operation = Operation::Modulo},
        OperatorData{.symbol = "|", .op = Operator::BitOr, .priority = 2, .operation = Operation::BitOr},
        OperatorData{.symbol = "&", .op = Operator::BitAnd, .priority = 2, .operation = Operation::BitAnd},
        OperatorData{.symbol = "~", .op = Operator::BitNot, .priority = 2, .operation = Operation::BitNot},
        OperatorData{.symbol = "^", .op = Operator::BitXor, .priority = 2, .operation = Operation::BitXor},
        OperatorData{.symbol = "%", .op = Operator::Modulo, .priority = 6, .operation = Operation::Modulo},
    };

    /**
     * @brief Static array containing info about all separators and similar elements used in the language
     *
     */
    static const std::vector<SeparatorData> Separators = {
        SeparatorData{.symbol = '(', .separator = Separator::BracketOpen, .priority = -1},
        SeparatorData{.symbol = ')', .separator = Separator::BracketClose, .priority = -1},
        SeparatorData{.symbol = '{', .separator = Separator::BlockOpen, .priority = 1},
        SeparatorData{.symbol = '}', .separator = Separator::BlockClose, .priority = 2},
        SeparatorData{.symbol = '[', .separator = Separator::ArrayOpen, .priority = -1},
        SeparatorData{.symbol = ']', .separator = Separator::ArrayClose, .priority = -1},
        SeparatorData{.symbol = '.', .separator = Separator::Dot, .priority = -1},
        SeparatorData{.symbol = ',', .separator = Separator::Comma, .priority = -1},
        SeparatorData{.symbol = ';', .separator = Separator::End, .priority = -1},
        SeparatorData{.symbol = ':', .separator = Separator::Colon, .priority = -1}

    };
}