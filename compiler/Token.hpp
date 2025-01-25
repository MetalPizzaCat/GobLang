#pragma once
#include <map>
#include <string>
#include <algorithm>
#include <vector>
#include <cstdint>
#include "../execution/Machine.hpp"

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

    static const std::map<std::string, Keyword> Keywords = {
        {"int", Keyword::Int},
        {"float", Keyword::Float},
        {"func", Keyword::Function},
        {"return", Keyword::Return},
        {"null", Keyword::Null},
        {"true", Keyword::True},
        {"false", Keyword::False}};

    struct OperatorData
    {
        const char *symbol;
        Operator op;
        int32_t priority;
        Operation operation;
    };

    static const std::vector<OperatorData> Operators = {
        OperatorData{.symbol = "==", .op = Operator::Equals, .priority = 5, .operation = Operation::None},
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

    class Token
    {
    public:
        virtual std::string toString() = 0;
        virtual int32_t getPriority() const;
        explicit Token(size_t row, size_t column);

    private:
        size_t m_row;
        size_t m_column;
    };

    class KeywordToken : public Token
    {
    public:
        explicit KeywordToken(size_t row, size_t column, Keyword keyword) : Token(row, column), m_keyword(keyword) {}
        Keyword getKeyword() const { return m_keyword; }
        std::string toString() override;

    private:
        Keyword m_keyword;
    };

    class OperatorToken : public Token
    {
    public:
        explicit OperatorToken(size_t row, size_t column, Operator oper);
        Operator getOperator() const { return m_data->op; }
        Operation getOperation() const { return m_data->operation; }
        virtual int32_t getPriority() const override;
        std::string toString() override;

    private:
        OperatorData const *m_data;
    };

    class IdToken : public Token
    {
    public:
        explicit IdToken(size_t row, size_t column, size_t id) : Token(row, column), m_id(id) {}

        std::string toString() override;

        size_t getId() const { return m_id; }

    private:
        size_t m_id;
    };

    class IntToken : public Token
    {
    public:
        explicit IntToken(size_t row, size_t column, size_t id) : Token(row, column), m_id(id) {}
        std::string toString() override;

        size_t getId() const { return m_id; }

    private:
        size_t m_id;
    };
}