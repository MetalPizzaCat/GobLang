#pragma once
#include "Token.hpp"
#include <string>
#include <cstdint>
namespace SimpleLang::Compiler
{
    class FunctionCallToken : public Token
    {
    public:
        explicit FunctionCallToken(size_t row, size_t column) : Token(row, column) {}

        std::string toString() override { return "CALL_" + std::to_string(m_argCount); }
        void increaseArgCount();
        int32_t getArgCount() const { return m_argCount; }

    private:
        int32_t m_argCount = 0;
    };

    class ArrayIndexToken : public Token
    {
    public:
        explicit ArrayIndexToken(size_t row, size_t column) : Token(row, column) {}

        std::string toString() override { return "AIO"; }
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
}