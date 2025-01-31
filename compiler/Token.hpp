#pragma once
#include <map>
#include <string>
#include <algorithm>
#include <vector>
#include <cstdint>
#include "../execution/Machine.hpp"
#include "Lexems.hpp"

namespace SimpleLang::Compiler
{

    class Token
    {
    public:
        virtual std::string toString() = 0;
        virtual int32_t getPriority() const;
        explicit Token(size_t row, size_t column);

        size_t getRow() const { return m_row; }
        size_t getColumn() const { return m_column; }

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

    class StringToken : public Token
    {
    public:
        explicit StringToken(size_t row, size_t column, size_t id) : Token(row, column), m_id(id) {}

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

    class SeparatorToken : public Token
    {
    public:
        explicit SeparatorToken(size_t row, size_t column, Separator sep);
        Separator getSeparator() const { return m_data->separator; }
        virtual int32_t getPriority() const override { return m_data->priority; }
        std::string toString() override;

    private:
        SeparatorData const *m_data;
    };
}