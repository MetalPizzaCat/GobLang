#pragma once
#include "Token.hpp"
#include <string>
#include <cstdint>
namespace GobLang::Compiler
{
    /**
     * @brief Base token for all tokens that need to count the amount of arguments separated by comma. Should not be used in final compilation
     *
     */
    class MultiArgToken : public Token
    {
    public:
        explicit MultiArgToken(size_t row, size_t column) : Token(row, column) {}

        std::string toString() override;
        void increaseArgCount();
        int32_t getArgCount() const { return m_argCount; }

    private:
        size_t m_argCount = 0;
    };

    /**
     * @brief Class representing a function call with arguments separated by comma
     *
     */
    class FunctionCallToken : public MultiArgToken
    {
    public:
        explicit FunctionCallToken(size_t row, size_t column) : MultiArgToken(row, column) {}
        explicit FunctionCallToken(size_t row, size_t column, size_t funcId) : MultiArgToken(row, column), m_funcId(funcId), m_usesLocalFunc(true) {}
        std::string toString() override;

        size_t getFuncId() const { return m_funcId; }
        bool usesLocalFunction() const { return m_usesLocalFunc; }

    private:
        size_t m_funcId = 0;
        bool m_usesLocalFunc = false;
    };

    /**
     * @brief Class representing an array literal. For example a = [1,2,3] will create an array that will be pushed onto the stack
     *
     */
    class ArrayCreationToken : public MultiArgToken
    {
    public:
        explicit ArrayCreationToken(size_t row, size_t column) : MultiArgToken(row, column) {}

        std::string toString() override;

    private:
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

    /**
     * @brief Version of return token that can only be used in the "main" function which will end execution immediately
     */
    class HaltToken : public Token
    {
    public:
        explicit HaltToken(size_t row, size_t column) : Token(row, column) {}

        std::string toString() override { return "HALT"; }
    };
}