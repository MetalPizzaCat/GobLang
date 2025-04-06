#pragma once
#include <cstdint>
#include <string>
#include <memory>
#include "Lexems.hpp"

namespace GobLang::Codegen
{
    class CodeNode
    {
    public:
        virtual ~CodeNode() = default;

        virtual std::string toString() = 0;

    private:
    };

    class IdNode : public CodeNode
    {
    public:
        explicit IdNode(size_t id);

        std::string toString() override;

    private:
        size_t m_id;
    };

    class FloatNode : public CodeNode
    {
    public:
        explicit FloatNode(float val);

        std::string toString() override;

    private:
        float m_val;
    };

    class IntNode : public CodeNode
    {
    public:
        explicit IntNode(int32_t val);

        std::string toString() override;

    private:
        int32_t m_val;
    };

    class UnsignedIntNode : public CodeNode
    {
    public:
        explicit UnsignedIntNode(uint32_t val);

        std::string toString() override;

    private:
        uint32_t m_val;
    };

    class StringNode : public CodeNode
    {
    public:
        explicit StringNode(size_t id);

        std::string toString() override;

    private:
        size_t m_id;
    };

    class SequenceNode : public CodeNode
    {
    public:
        explicit SequenceNode(std::vector<std::unique_ptr<CodeNode>> seq);

        std::string toString() override;

    private:
        std::vector<std::unique_ptr<CodeNode>> m_sequence;
    };

    class BinaryOperationNode : public CodeNode
    {
    public:
        explicit BinaryOperationNode(Operator op, std::unique_ptr<CodeNode> left, std::unique_ptr<CodeNode> right);
        std::string toString() override;

    private:
        std::unique_ptr<CodeNode> m_left;
        std::unique_ptr<CodeNode> m_right;
        Operator m_op;
    };

    class FunctionCallNode : public CodeNode
    {
    public:
        explicit FunctionCallNode(size_t id, std::vector<std::unique_ptr<CodeNode>> args);
        std::string toString() override;

    private:
        size_t m_id;
        std::vector<std::unique_ptr<CodeNode>> m_args;
    };

    class VariableCreationNode : public CodeNode
    {
    public:
        explicit VariableCreationNode(size_t id, std::unique_ptr<CodeNode> body);
        std::string toString() override;

    private:
        size_t m_id;
        std::unique_ptr<CodeNode> m_body;
    };
} // namespace GobLang::Codegen
