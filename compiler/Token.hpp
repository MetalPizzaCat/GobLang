#pragma once
#include <map>
#include <string>
#include <algorithm>
#include <vector>
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
    };

    static const std::vector<OperatorData> Operators = {
        OperatorData{.symbol = "==", .op = Operator::Equals, .priority = 5},
        OperatorData{.symbol = "=", .op = Operator::Equals, .priority = 2},
        OperatorData{.symbol = "!=", .op = Operator::Equals, .priority = 5},
        OperatorData{.symbol = "<", .op = Operator::Equals, .priority = 5},
        OperatorData{.symbol = ">", .op = Operator::Equals, .priority = 5},
        OperatorData{.symbol = "+", .op = Operator::Equals, .priority = 6},
        OperatorData{.symbol = "-", .op = Operator::Equals, .priority = 6},
        OperatorData{.symbol = "*", .op = Operator::Equals, .priority = 7},
        OperatorData{.symbol = "/", .op = Operator::Equals, .priority = 7},
        OperatorData{.symbol = "and", .op = Operator::Equals, .priority = 2},
        OperatorData{.symbol = "&&", .op = Operator::Equals, .priority = 2},
        OperatorData{.symbol = "or", .op = Operator::Equals, .priority = 2},
        OperatorData{.symbol = "||", .op = Operator::Equals, .priority = 2}};

    class Token
    {
    public:
        virtual std::string toString() = 0;
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
        explicit OperatorToken(size_t row, size_t column, Operator op) : Token(row, column), m_op(op) {}
        Operator getOperator() const { return m_op; }
        std::string toString() override;

    private:
        Operator m_op;
    };

    class IdToken : public Token
    {
    public:
        explicit IdToken(size_t row, size_t column, size_t id) : Token(row, column), m_id(id) {}
        std::string toString() override;
    private:
        size_t m_id;
    };

    class IntToken : public Token
    {
    public:
        explicit IntToken(size_t row, size_t column, size_t id) : Token(row, column), m_id(id) {}
        std::string toString() override;
    private:
        size_t m_id;
    };
}