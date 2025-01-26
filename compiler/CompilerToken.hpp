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
}