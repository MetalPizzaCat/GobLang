#pragma once
#include <vector>
#include <map>
#include <cstdint>
namespace GobLang::Compiler
{
    enum class Keyword
    {
        Int,
        Float,
        Let,
        Function,
        Return,
        Null,
        True,
        False,
        If,
        Elif,
        Else,
        While,
        Continue,
        Break,
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
        {"if", Keyword::If},
        {"while", Keyword::While},
        {"continue", Keyword::Continue},
        {"break", Keyword::Break},
        {"elif", Keyword::Elif},
        {"else", Keyword::Else},
        {"let", Keyword::Let}};

    static const std::map<std::string, bool> Booleans = {
        {"true", true},
        {"false", false},
        {"True", true},
        {"False", false}};
        
    /**
     * @brief Static array containing info about all operators used in the compiler
     *
     */
    static const std::vector<OperatorData> Operators = {
        OperatorData{.symbol = "==", .op = Operator::Equals, .priority = 5, .operation = Operation::Equals},
        OperatorData{.symbol = ">=", .op = Operator::LessEq, .priority = 5, .operation = Operation::MoreOrEq},
        OperatorData{.symbol = "<=", .op = Operator::MoreEq, .priority = 5, .operation = Operation::LessOrEq},
        OperatorData{.symbol = "=", .op = Operator::Assign, .priority = 1, .operation = Operation::Set},
        OperatorData{.symbol = "!", .op = Operator::Not, .priority = 5, .operation = Operation::Not},
        OperatorData{.symbol = "!=", .op = Operator::NotEqual, .priority = 5, .operation = Operation::NotEq},
        OperatorData{.symbol = "<", .op = Operator::Less, .priority = 5, .operation = Operation::Less},
        OperatorData{.symbol = ">", .op = Operator::More, .priority = 5, .operation = Operation::More},
        OperatorData{.symbol = "+", .op = Operator::Add, .priority = 6, .operation = Operation::Add},
        OperatorData{.symbol = "-", .op = Operator::Sub, .priority = 6, .operation = Operation::Sub},
        OperatorData{.symbol = "*", .op = Operator::Mul, .priority = 7, .operation = Operation::None},
        OperatorData{.symbol = "/", .op = Operator::Div, .priority = 7, .operation = Operation::None},
        OperatorData{.symbol = "and", .op = Operator::And, .priority = 4, .operation = Operation::Add},
        OperatorData{.symbol = "&&", .op = Operator::And, .priority = 4, .operation = Operation::And},
        OperatorData{.symbol = "or", .op = Operator::Or, .priority = 3, .operation = Operation::Or},
        OperatorData{.symbol = "||", .op = Operator::Or, .priority = 3, .operation = Operation::Or}};

    /**
     * @brief Static array containing info about all separators and similar elements used in the language
     *
     */
    static const std::vector<SeparatorData> Separators = {
        SeparatorData{.symbol = '(', .separator = Separator::BracketOpen, .priority = 1},
        SeparatorData{.symbol = ')', .separator = Separator::BracketClose, .priority = 2},
        SeparatorData{.symbol = '{', .separator = Separator::BlockOpen, .priority = 1},
        SeparatorData{.symbol = '}', .separator = Separator::BlockClose, .priority = 2},
        SeparatorData{.symbol = '[', .separator = Separator::ArrayOpen, .priority = 1},
        SeparatorData{.symbol = ']', .separator = Separator::ArrayClose, .priority = 2},
        SeparatorData{.symbol = '.', .separator = Separator::Dot, .priority = -1},
        SeparatorData{.symbol = ',', .separator = Separator::Comma, .priority = -1},
        SeparatorData{.symbol = ';', .separator = Separator::End, .priority = -1}

    };
}