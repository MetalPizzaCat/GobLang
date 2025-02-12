#pragma once
#include "Token.hpp"
#include <string>
#include <cstdint>
namespace GobLang::Compiler
{
    class FunctionCallToken : public Token
    {
    public:
        explicit FunctionCallToken(size_t row, size_t column) : Token(row, column) {}
        explicit FunctionCallToken(size_t row, size_t column, size_t funcId) : Token(row, column), m_funcId(funcId), m_usesLocalFunc(true) {}

        std::string toString() override;
        void increaseArgCount();
        int32_t getArgCount() const { return m_argCount; }

        size_t getFuncId() const { return m_funcId; }
        bool usesLocalFunction() const { return m_usesLocalFunc; }

    private:
        int32_t m_argCount = 0;
        size_t m_funcId = 0;
        bool m_usesLocalFunc = false;
    };

    class ArrayIndexToken : public Token
    {
    public:
        explicit ArrayIndexToken(size_t row, size_t column) : Token(row, column) {}

        std::string toString() override { return "AIO"; }
    };

    class LoopControlToken : public GotoToken
    {
    public:
        explicit LoopControlToken(size_t row, size_t column, bool isBreak, WhileToken *loop) : GotoToken(row, column, 0), m_loop(loop), m_isBreak(isBreak) {}

        std::string toString() override { return "GOTO_M" + std::to_string(m_isBreak ? m_loop->getMark() : m_loop->getReturnMark()); }

        size_t getMark() const override { return m_isBreak ? m_loop->getMark() : m_loop->getReturnMark(); }

    private:
        WhileToken *m_loop;
        bool m_isBreak;
    };

    class LocalVarToken : public Token
    {
    public:
        explicit LocalVarToken(size_t row, size_t column, size_t id) : Token(row, column), m_varId(id) {}
        size_t getId() const { return m_varId; }

        std::string toString() override { return "LOC" + std::to_string(m_varId); }

    private:
        size_t m_varId;
    };

    class LocalVarShrinkToken : public Token
    {
    public:
        explicit LocalVarShrinkToken(size_t row, size_t column, size_t amount) : Token(row, column), m_amount(amount) {}
        size_t getAmount() const { return m_amount; }

        std::string toString() override;

    private:
        size_t m_amount;
    };

    class ReturnToken : public Token
    {
    public:
        explicit ReturnToken(size_t row, size_t column, bool hasVal) : Token(row, column), m_hasVal(hasVal) {}

        std::string toString() override;

        bool hasValue() const { return m_hasVal; }

    private:
        bool m_hasVal;
    };
}