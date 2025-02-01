#pragma once
#include <vector>
#include <map>
#include <cstdint>
namespace SimpleLang::Compiler
{
    enum class Keyword
    {
        Int,
        Float,
        Function,
        Return,
        Null,
        True,
        False,
        If,
        Elif,
        Else
    };

    enum class Operator
    {
        Equals,
        Assign,
        NotEqual,
        Less,
        More,
        Add,
        Sub,
        Mul,
        Div,
        And,
        Or,
        Not,
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
        Comma, // ,
        End,   // ;
    };

    struct OperatorData
    {
        const char *symbol;
        Operator op;
        int32_t priority;
        Operation operation;
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
    static const std::map<std::string, Keyword> Keywords = {
        {"int", Keyword::Int},
        {"float", Keyword::Float},
        {"func", Keyword::Function},
        {"return", Keyword::Return},
        {"null", Keyword::Null},
        {"true", Keyword::True},
        {"false", Keyword::False},
        {"if", Keyword::If},
        {"elif", Keyword::Elif},
        {"else", Keyword::Else}};

    /**
     * @brief Static array containing info about all operators used in the compiler
     *
     */
    static const std::vector<OperatorData> Operators = {
        OperatorData{.symbol = "==", .op = Operator::Equals, .priority = 5, .operation = Operation::Equals},
        OperatorData{.symbol = "=", .op = Operator::Assign, .priority = 2, .operation = Operation::Set},
        OperatorData{.symbol = "!=", .op = Operator::NotEqual, .priority = 5, .operation = Operation::None},
        OperatorData{.symbol = "<", .op = Operator::Less, .priority = 5, .operation = Operation::None},
        OperatorData{.symbol = ">", .op = Operator::More, .priority = 5, .operation = Operation::None},
        OperatorData{.symbol = "+", .op = Operator::Add, .priority = 6, .operation = Operation::Add},
        OperatorData{.symbol = "-", .op = Operator::Sub, .priority = 6, .operation = Operation::Sub},
        OperatorData{.symbol = "*", .op = Operator::Mul, .priority = 7, .operation = Operation::None},
        OperatorData{.symbol = "/", .op = Operator::Div, .priority = 7, .operation = Operation::None},
        OperatorData{.symbol = "and", .op = Operator::And, .priority = 2, .operation = Operation::None},
        OperatorData{.symbol = "&&", .op = Operator::And, .priority = 2, .operation = Operation::None},
        OperatorData{.symbol = "or", .op = Operator::Or, .priority = 2, .operation = Operation::None},
        OperatorData{.symbol = "||", .op = Operator::Or, .priority = 2, .operation = Operation::None}};

    /**
     * @brief Static array containing info about all separators and similar elements used in the language
     *
     */
    static const std::vector<SeparatorData> Separators = {
        SeparatorData{.symbol = '(', .separator = Separator::BracketOpen, .priority = 0},
        SeparatorData{.symbol = ')', .separator = Separator::BracketClose, .priority = 1},
        SeparatorData{.symbol = '{', .separator = Separator::BlockOpen, .priority = 0},
        SeparatorData{.symbol = '}', .separator = Separator::BlockClose, .priority = 1},
        SeparatorData{.symbol = '[', .separator = Separator::ArrayOpen, .priority = 0},
        SeparatorData{.symbol = ']', .separator = Separator::ArrayClose, .priority = 1},
        SeparatorData{.symbol = '.', .separator = Separator::Dot, .priority = -1},
        SeparatorData{.symbol = ',', .separator = Separator::Comma, .priority = -1},
        SeparatorData{.symbol = ';', .separator = Separator::End, .priority = -1}

    };
}