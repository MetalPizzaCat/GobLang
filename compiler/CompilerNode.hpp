#pragma once
#include <vector>
#include <cstdint>
#include "Token.hpp"
namespace GobLang::Compiler
{

    class CompilerNode
    {
    public:
        explicit CompilerNode() {}
        virtual std::vector<uint8_t> getOperationGetBytes() = 0;

        virtual std::vector<uint8_t> getOperationSetBytes() { return {}; }

        virtual ~CompilerNode() = default;

    private:
    };

    class OperationCompilerNode : public CompilerNode
    {
    public:
        explicit OperationCompilerNode(std::vector<uint8_t> const &vec) : CompilerNode(), m_bytes(vec) {}

        std::vector<uint8_t> getOperationGetBytes() override { return m_bytes; }
        virtual ~OperationCompilerNode() = default;

    private:
        std::vector<uint8_t> m_bytes;
    };

    class TokenCompilerNode : public CompilerNode
    {
    public:
        explicit TokenCompilerNode(Token *token) : CompilerNode(), m_token(token) {}
        std::vector<uint8_t> getOperationGetBytes() override;

        std::vector<uint8_t> getOperationSetBytes() override;

        Token *getToken() { return m_token; }

    private:
        Token *m_token;
    };

    class LocalVarTokenCompilerNode : public TokenCompilerNode
    {
    public:
        explicit LocalVarTokenCompilerNode(Token *token) : TokenCompilerNode(token) {}
    };

    class FieldAccessNode : public CompilerNode
    {
    public:
        explicit FieldAccessNode(CompilerNode *object, CompilerNode *field) : m_object(object), m_field(field) {}

        std::vector<uint8_t> getOperationGetBytes() override
        {
            std::vector<uint8_t> out = m_field->getOperationGetBytes();
            std::vector<uint8_t> arrayGetBytes = m_object->getOperationGetBytes();
            out.insert(out.end(), arrayGetBytes.begin(), arrayGetBytes.end());
            out.push_back((uint8_t)Operation::GetField);
            return out;
        }

        std::vector<uint8_t> getOperationSetBytes() override
        {
            std::vector<uint8_t> out = m_field->getOperationGetBytes();
            std::vector<uint8_t> arrayGetBytes = m_object->getOperationGetBytes();
            out.insert(out.end(), arrayGetBytes.begin(), arrayGetBytes.end());
            return out;
        }

        ~FieldAccessNode()
        {
            delete m_object;
            delete m_field;
        }

    private:
        CompilerNode *m_object;
        CompilerNode *m_field;
    };

    class ArrayCompilerNode : public CompilerNode
    {
    public:
        explicit ArrayCompilerNode(CompilerNode *array,
                                   CompilerNode *index) : CompilerNode(), m_array(array), m_index(index) {}

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
        explicit BoolConstCompilerNode(bool value) : CompilerNode(), m_value(value) {}

        std::vector<uint8_t> getOperationGetBytes() override
        {
            return {(uint8_t)(m_value ? Operation::PushTrue : Operation::PushFalse)};
        }

    private:
        bool m_value;
    };
} // namespace SimpleLang::Compiler
