#pragma once
#include <map>
#include <string>
#include <algorithm>
#include <vector>
#include <cstdint>
#include "../execution/Machine.hpp"
#include "Lexems.hpp"

namespace GobLang::Compiler
{

    class Token
    {
    public:
        virtual std::string toString() = 0;
        virtual int32_t getPriority() const;
        explicit Token(size_t row, size_t column);

        size_t getRow() const { return m_row; }
        size_t getColumn() const { return m_column; }

        virtual ~Token() = default;

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
        Operation getOperation() const;
        virtual int32_t getPriority() const override;
        std::string toString() override;

        bool isUnary() const { return m_unary; }
        void setIsUnary(bool unary) { m_unary = unary; }

    private:
        OperatorData const *m_data;
        bool m_unary;
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

    class CharToken : public Token
    {
    public:
        explicit CharToken(size_t row, size_t column, char ch) : Token(row, column), m_char(ch) {}

        std::string toString() override
        {
            return "CHAR_" + std::to_string(m_char);
        }
        char getChar() const { return m_char; }

    private:
        char m_char;
    };

    class IntToken : public Token
    {
    public:
        explicit IntToken(size_t row, size_t column, int32_t value) : Token(row, column), m_value(value) {}
        std::string toString() override;

        int32_t getValue() const { return m_value; }

    private:
        int32_t m_value;
    };

    class FloatToken : public Token
    {
    public:
        explicit FloatToken(size_t row, size_t column, float value) : Token(row, column), m_value(value) {}
        std::string toString() override;

        float getValue() const { return m_value; }

    private:
        float m_value;
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

    class GotoToken : public Token
    {
    public:
        explicit GotoToken(size_t row, size_t column, size_t mark = 0) : Token(row, column), m_mark(mark) {}
        std::string toString() override;
        virtual size_t getMark() const { return m_mark; }
        void setMark(size_t mark) { m_mark = mark; }

        virtual ~GotoToken() = default;

    private:
        size_t m_mark;
    };

    class IfToken : public GotoToken
    {
    public:
        explicit IfToken(size_t row, size_t column, bool elif, size_t mark = 0) : GotoToken(row, column, mark), m_elif(elif) {}
        std::string toString() override;
        bool isElif() const { return m_elif; }

    private:
        bool m_elif;
    };

    class WhileToken : public GotoToken
    {
    public:
        explicit WhileToken(size_t row, size_t column, size_t returnMark = 0, size_t mark = 0) : GotoToken(row, column, mark), m_returnMark(returnMark) {}
        std::string toString() override;
        size_t getReturnMark() const { return m_returnMark; }
        void setReturnMark(size_t mark) { m_returnMark = mark; }

    private:
        size_t m_returnMark;
    };

    class JumpDestinationToken : public Token
    {
    public:
        explicit JumpDestinationToken(size_t row, size_t column, size_t id) : Token(row, column), m_id(id) {}
        std::string toString() override;

        size_t getId() const { return m_id; }

    private:
        size_t m_id;
    };

    class BoolConstToken : public Token
    {
    public:
        explicit BoolConstToken(size_t row, size_t column, bool value) : Token(row, column), m_value(value) {}

        bool getValue() const { return m_value; }

        std::string toString() override { return m_value ? "True" : "False"; }

    private:
        bool m_value;
    };

    /**
     * @brief Token representing a null constant     *
     */
    class NullConstToken : public Token
    {
    public:
        explicit NullConstToken(size_t row, size_t column) : Token(row, column) {}

        std::string toString() override;
    };
}