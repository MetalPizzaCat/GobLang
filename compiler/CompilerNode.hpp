#pragma once
#include <vector>
#include <cstdint>
#include "Token.hpp"
namespace GobLang::Compiler
{

    class CompilerNode
    {
    public:
        explicit CompilerNode(bool isDestination, size_t destinationId) : m_hasMark(isDestination), m_attachedMark(destinationId) {}
        virtual std::vector<uint8_t> getOperationGetBytes() = 0;

        virtual std::vector<uint8_t> getOperationSetBytes() { return {}; }

        void setMark(size_t mark);
        size_t getMark() const { return m_attachedMark; }
        bool hasMark() const { return m_hasMark; }

        virtual ~CompilerNode() = default;

    private:
        bool m_hasMark;
        size_t m_attachedMark = 0;
    };

    class OperationCompilerNode : public CompilerNode
    {
    public:
        explicit OperationCompilerNode(std::vector<uint8_t> const &vec,
                                       bool isDestination,
                                       size_t destinationId) : CompilerNode(isDestination, destinationId), m_bytes(vec) {}

        std::vector<uint8_t> getOperationGetBytes() override { return m_bytes; }
        virtual ~OperationCompilerNode() = default;

    private:
        std::vector<uint8_t> m_bytes;
    };

    class TokenCompilerNode : public CompilerNode
    {
    public:
        explicit TokenCompilerNode(Token *token,
                                   bool isDestination,
                                   size_t destinationId) : CompilerNode(isDestination, destinationId), m_token(token) {}
        std::vector<uint8_t> getOperationGetBytes() override;

        std::vector<uint8_t> getOperationSetBytes() override;

        Token *getToken() { return m_token; }

    private:
        Token *m_token;
    };

    class LocalVarTokenCompilerNode : public TokenCompilerNode
    {
    public:
        explicit LocalVarTokenCompilerNode(Token *token,
                                           bool isDestination,
                                           size_t destinationId) : TokenCompilerNode(token, isDestination, destinationId) {}
    };

    class ArrayCompilerNode : public CompilerNode
    {
    public:
        explicit ArrayCompilerNode(CompilerNode *array,
                                   CompilerNode *index,
                                   bool isDestination,
                                   size_t destinationId) : CompilerNode(isDestination, destinationId), m_array(array), m_index(index) {}

        std::vector<uint8_t> getOperationGetBytes() override
        {
            std::vector<uint8_t> out = m_index->getOperationGetBytes();
            std::vector<uint8_t> arrayGetBytes = m_array->getOperationGetBytes();
            out.insert(out.end(), arrayGetBytes.begin(), arrayGetBytes.end());
            out.push_back((uint8_t)Operation::GetArray);
            return out;
        }

        std::vector<uint8_t> getOperationSetBytes() override
        {
            std::vector<uint8_t> out = m_index->getOperationGetBytes();
            std::vector<uint8_t> arrayGetBytes = m_array->getOperationGetBytes();
            out.insert(out.end(), arrayGetBytes.begin(), arrayGetBytes.end());
            return out;
        }

        virtual ~ArrayCompilerNode()
        {
            delete m_array;
            delete m_index;
        }

    private:
        CompilerNode *m_array;

        CompilerNode *m_index;
    };

    class BoolConstCompilerNode : public CompilerNode
    {
    public:
        explicit BoolConstCompilerNode(
            bool isDestination,
            size_t destinationId,
            bool value) : CompilerNode(isDestination, destinationId), m_value(value) {}

        std::vector<uint8_t> getOperationGetBytes() override
        {
            return {(uint8_t)(m_value ? Operation::PushTrue : Operation::PushFalse)};
        }

    private:
        bool m_value;
    };
} // namespace SimpleLang::Compiler
