#pragma once
#include <cstdint>
#include <string>
#include <memory>
#include "Lexems.hpp"
#include "Builder.hpp"

namespace GobLang::Codegen
{
    class CodeNode
    {
    public:
        virtual ~CodeNode() = default;

        virtual std::unique_ptr<CodeGenValue> generateCode(Builder &builder) = 0;

        virtual std::string toString() = 0;
    };

    class IdNode : public CodeNode
    {
    public:
        explicit IdNode(size_t id);
        std::unique_ptr<CodeGenValue> generateCode(Builder &builder) override;
        std::string toString() override;

    private:
        size_t m_id;
    };

    class FloatNode : public CodeNode
    {
    public:
        explicit FloatNode(float val);

        std::unique_ptr<CodeGenValue> generateCode(Builder &builder) override;

        std::string toString() override;

    private:
        float m_val;
    };

    class IntNode : public CodeNode
    {
    public:
        explicit IntNode(int32_t val);
        std::unique_ptr<CodeGenValue> generateCode(Builder &builder) override;
        std::string toString() override;

    private:
        int32_t m_val;
    };

    class BoolNode : public CodeNode
    {
    public:
        explicit BoolNode(bool val);
        std::unique_ptr<CodeGenValue> generateCode(Builder &builder) override;
        std::string toString() override;

    private:
        bool m_val;
    };

    class NullNode : public CodeNode
    {
    public:
        explicit NullNode() {}
        std::unique_ptr<CodeGenValue> generateCode(Builder &builder) override;
        std::string toString() override;

    private:
    };

    class UnsignedIntNode : public CodeNode
    {
    public:
        explicit UnsignedIntNode(uint32_t val);
        std::unique_ptr<CodeGenValue> generateCode(Builder &builder) override;
        std::string toString() override;

    private:
        uint32_t m_val;
    };

    class CharacterNode : public CodeNode
    {
    public:
        explicit CharacterNode(char ch);
        std::unique_ptr<CodeGenValue> generateCode(Builder &builder) override;
        std::string toString() override;

    private:
        char m_char;
    };

    class StringNode : public CodeNode
    {
    public:
        explicit StringNode(size_t id);
        std::unique_ptr<CodeGenValue> generateCode(Builder &builder) override;
        std::string toString() override;

    private:
        size_t m_id;
    };

    class BreakNode : public CodeNode
    {
    public:
        explicit BreakNode() = default;
        std::unique_ptr<CodeGenValue> generateCode(Builder &builder) override;
        std::string toString() override;
    };

    class ContinueNode : public CodeNode
    {
    public:
        explicit ContinueNode() = default;
        std::unique_ptr<CodeGenValue> generateCode(Builder &builder) override;
        std::string toString() override;
    };

    class ReturnEmptyNode : public CodeNode
    {
    public:
        explicit ReturnEmptyNode() = default;
        std::unique_ptr<CodeGenValue> generateCode(Builder &builder) override;
        std::string toString() override;
    };

    class ReturnNode : public CodeNode
    {
    public:
        explicit ReturnNode(std::unique_ptr<CodeNode> val);
        std::unique_ptr<CodeGenValue> generateCode(Builder &builder) override;
        std::string toString() override;

    private:
        std::unique_ptr<CodeNode> m_val;
    };

    class SequenceNode : public CodeNode
    {
    public:
        explicit SequenceNode(std::vector<std::unique_ptr<CodeNode>> seq);
        std::unique_ptr<CodeGenValue> generateCode(Builder &builder) override { return generateBlockContext(builder); }
        /// @brief Generate block codegen data for the given sequence
        /// @param builder
        /// @param jumpStartOffset
        /// @return
        std::unique_ptr<BlockCodeGenValue> generateBlockContext(Builder &builder, size_t jumpStartOffset = 0, bool isLoop = false);
        std::string toString() override;

    private:
        std::vector<std::unique_ptr<CodeNode>> m_sequence;
    };

    class ArrayLiteralNode : public CodeNode
    {
    public:
        explicit ArrayLiteralNode(std::vector<std::unique_ptr<CodeNode>> values);
        explicit ArrayLiteralNode() = default;
        std::unique_ptr<CodeGenValue> generateCode(Builder &builder) override;
        std::string toString() override;

    private:
        std::vector<std::unique_ptr<CodeNode>> m_values;
    };

    class FieldAccessNode : public CodeNode
    {
    public:
        explicit FieldAccessNode(std::unique_ptr<CodeNode> left, std::unique_ptr<CodeNode> right);
        std::string toString() override;
        std::unique_ptr<CodeGenValue> generateCode(Builder &builder) override;

    private:
        std::unique_ptr<CodeNode> m_left;
        std::unique_ptr<CodeNode> m_right;
    };
    class BinaryOperationNode : public CodeNode
    {
    public:
        explicit BinaryOperationNode(Operator op, std::unique_ptr<CodeNode> left, std::unique_ptr<CodeNode> right);
        std::string toString() override;
        std::unique_ptr<CodeGenValue> generateCode(Builder &builder) override;

    private:
        std::unique_ptr<CodeNode> m_left;
        std::unique_ptr<CodeNode> m_right;
        Operator m_op;
    };

    class UnaryOperationNode : public CodeNode
    {
    public:
        explicit UnaryOperationNode(Operator op, std::unique_ptr<CodeNode> value);
        std::string toString() override;
        std::unique_ptr<CodeGenValue> generateCode(Builder &builder) override;

    private:
        std::unique_ptr<CodeNode> m_value;
        Operator m_op;
    };

    class ArrayAccessNode : public CodeNode
    {
    public:
        explicit ArrayAccessNode(std::unique_ptr<CodeNode> value, std::unique_ptr<CodeNode> address);
        std::string toString() override;
        std::unique_ptr<CodeGenValue> generateCode(Builder &builder) override;

    private:
        std::unique_ptr<CodeNode> m_array;
        std::unique_ptr<CodeNode> m_index;
    };

    class FunctionCallNode : public CodeNode
    {
    public:
        explicit FunctionCallNode(std::unique_ptr<CodeNode> value, std::vector<std::unique_ptr<CodeNode>> args);
        std::unique_ptr<CodeGenValue> generateCode(Builder &builder) override;
        std::string toString() override;

    private:
        std::unique_ptr<CodeNode> m_value;
        std::vector<std::unique_ptr<CodeNode>> m_args;
    };

    class FunctionPrototypeNode : public CodeNode
    {
    public:
        explicit FunctionPrototypeNode(size_t nameId, std::vector<size_t> args);
        std::unique_ptr<CodeGenValue> generateCode(Builder &builder) override { return generateFunction(builder); }

        std::unique_ptr<FunctionPrototypeCodeGenValue> generateFunction(Builder &builder);
        std::string toString() override;

    private:
        size_t m_nameId;
        std::vector<size_t> m_argIds;
    };

    class TypeDefinitionNode : public CodeNode
    {
    public:
        explicit TypeDefinitionNode(size_t nameId, std::vector<size_t> fields);
        std::unique_ptr<CodeGenValue> generateCode(Builder &builder) override { return nullptr; }

        void generateType(Builder &builder, std::vector<std::string> const &ids);
        std::string toString() override;

    private:
        size_t m_nameId;
        std::vector<size_t> m_fieldIds;
    };

    class ConstructorCallNode : public CodeNode
    {
    public:
        explicit ConstructorCallNode(size_t nameId, std::vector<std::unique_ptr<CodeNode>> args);
        std::unique_ptr<CodeGenValue> generateCode(Builder &builder) override;
        std::string toString() override;

    private:
        size_t m_typeNameId;
        std::vector<std::unique_ptr<CodeNode>> m_args;
    };

    class FunctionNode : public CodeNode
    {
    public:
        explicit FunctionNode(std::unique_ptr<FunctionPrototypeNode> proto, std::unique_ptr<CodeNode> body);
        std::unique_ptr<CodeGenValue> generateCode(Builder &builder) override { return generateFunction(builder); }

        std::unique_ptr<FunctionCodeGenValue> generateFunction(Builder &builder);
        std::string toString() override;

    private:
        std::unique_ptr<FunctionPrototypeNode> m_proto;
        std::unique_ptr<CodeNode> m_body;
    };

    class BranchNode : public CodeNode
    {
    public:
        explicit BranchNode(std::unique_ptr<CodeNode> cond, std::unique_ptr<SequenceNode> body);
        std::unique_ptr<CodeGenValue> generateCode(Builder &builder) override { return generateBranchCode(builder); }

        /// @brief Create a branch codegen value
        /// @param builder Bytecode builder
        /// @param prevBranchOffset Offset based on the size of the previous if/elif branches, used for proper break/continue handling
        /// @return
        std::unique_ptr<BranchCodeGenValue> generateBranchCode(Builder &builder, size_t prevBranchOffset = 0);
        std::string toString() override;

        std::unique_ptr<CodeNode> &getCond() { return m_cond; }
        std::unique_ptr<SequenceNode> &getBody() { return m_body; }

    private:
        std::unique_ptr<CodeNode> m_cond;
        std::unique_ptr<SequenceNode> m_body;
    };

    class WhileLoopNode : public BranchNode
    {
    public:
        explicit WhileLoopNode(std::unique_ptr<CodeNode> cond, std::unique_ptr<SequenceNode> body);
        std::unique_ptr<CodeGenValue> generateCode(Builder &builder) override;
        std::string toString() override;
    };

    class BranchChainNode : public CodeNode
    {
    public:
        explicit BranchChainNode(
            std::unique_ptr<BranchNode> primary,
            std::vector<std::unique_ptr<BranchNode>> secondary,
            std::unique_ptr<SequenceNode> elseBlock = nullptr);
        std::string toString() override;
        std::unique_ptr<CodeGenValue> generateCode(Builder &builder) override;

    private:
        std::unique_ptr<BranchNode> m_primary;
        std::vector<std::unique_ptr<BranchNode>> m_secondary;
        std::unique_ptr<SequenceNode> m_else;
    };

    class VariableCreationNode : public CodeNode
    {
    public:
        explicit VariableCreationNode(size_t id, std::unique_ptr<CodeNode> body);
        std::string toString() override;
        std::unique_ptr<CodeGenValue> generateCode(Builder &builder) override;

    private:
        size_t m_id;
        std::unique_ptr<CodeNode> m_body;
    };
} // namespace GobLang::Codegen
