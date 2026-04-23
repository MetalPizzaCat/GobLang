#pragma once
#include <cstdint>
#include <string>
#include <memory>
#include "Lexems.hpp"
#include "Builder.hpp"
#include "FunctionContext.hpp"

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

    class NumberNode : public CodeNode
    {
    public:
        explicit NumberNode(NumberType val);

        std::unique_ptr<CodeGenValue> generateCode(Builder &builder) override;

        std::string toString() override;

    private:
        NumberType m_val;
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
        std::unique_ptr<CodeGenValue> generateOrCode(Builder &builder);
        std::unique_ptr<CodeGenValue> generateAndCode(Builder &builder);

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
        /**
         * @brief Construct a new Function Call Node object
         *
         * @param value Code block that returns callable object
         * @param args Arguments to pass into the called function
         */
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
        explicit FunctionPrototypeNode(std::string const &name, std::vector<size_t> args);
        std::unique_ptr<CodeGenValue> generateCode(Builder &builder) override { return generateFunction(builder); }

        std::unique_ptr<FunctionPrototypeCodeGenValue> generateFunction(Builder &builder);
        std::string toString() override;

        std::vector<size_t> const &getArgumentNameStringIds() const { return m_argIds; }

        std::string const &getName() const { return m_name; }

    private:
        std::string m_name;
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
        explicit FunctionNode() = default;
        /**
         * @brief Construct a new function node
         *
         * @param context Information for the constant function object, such as constant strings
         * @param proto Prototype of the function which stores details such as name and argument count
         * @param body Node containing body of the function
         */
        explicit FunctionNode(FunctionContext context, std::unique_ptr<FunctionPrototypeNode> proto, std::unique_ptr<CodeNode> body);
        std::unique_ptr<CodeGenValue> generateCode(Builder &builder) override { return nullptr; }

        /**
         * @brief Create new function instance from parsed code and store it in the state memory
         *
         * @param builder Builder for generating bytecode
         * @param state State to save the function in
         * @return GobFunction* Pointer to the function object MANAGED by the state
         */
        GobFunction *generateFunction(Builder &builder, State &state);
        std::string toString() override;

    private:
        std::unique_ptr<FunctionPrototypeNode> m_proto;
        std::unique_ptr<CodeNode> m_body;

        FunctionContext m_context;
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
