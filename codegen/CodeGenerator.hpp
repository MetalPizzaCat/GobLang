#pragma once
#include "Parser.hpp"
#include "CodeNode.hpp"
#include "ByteCode.hpp"

namespace GobLang::Codegen
{

    /// @brief Generates an AST based on the parser tokens and then generates bytecode by recursively calling the codegen function
    class CodeGenerator
    {
    public:
        explicit CodeGenerator(Parser const &parser);

        void generate();

        ByteCode getByteCode();

        std::unique_ptr<FunctionNode> parseFunctionDefinition();

        std::unique_ptr<FunctionPrototypeNode> parseFunctionPrototype();

        /// @brief Parse structure data
        /// @return
        std::unique_ptr<TypeDefinitionNode> parseStructureDefinition();

        /// @brief Block of code with n expressions separated by ;
        /// @return
        std::unique_ptr<SequenceNode> parseBody();

        std::unique_ptr<VariableCreationNode> parseVarCreation();

        std::unique_ptr<CodeNode> parseStandaloneExpression();

        std::unique_ptr<ArrayLiteralNode> parseArrayLiteral();

        /// @brief Parse the condition + body of a branch. Could be used for loops
        /// @return
        std::unique_ptr<BranchNode> parseBranch();

        std::unique_ptr<WhileLoopNode> parseLoop();

        /// @brief Parse if-elif-else block
        /// @return
        std::unique_ptr<BranchChainNode> parseBranchChain();

        std::unique_ptr<FloatNode> parseFloat();

        std::unique_ptr<StringNode> parseString();

        std::unique_ptr<IntNode> parseInt();

        std::unique_ptr<BoolNode> parseBool();

        std::unique_ptr<CharacterNode> parseChar();

        std::unique_ptr<CodeNode> parseId();

        std::unique_ptr<CodeNode> parseIdExpression(std::unique_ptr<CodeNode> left);

        std::vector<std::unique_ptr<CodeNode>> parseFunctionCallArguments();

        std::unique_ptr<CodeNode> parseArrayAccess();

        std::unique_ptr<CodeNode> parsePrimary();

        std::unique_ptr<CodeNode> parseGrouped();

        std::unique_ptr<CodeNode> parseExpression();

        std::unique_ptr<CodeNode> parseUnary();

        std::unique_ptr<CodeNode> parseBreak();

        std::unique_ptr<CodeNode> parseContinue();

        std::unique_ptr<CodeNode> parseReturn();

        std::unique_ptr<ConstructorCallNode> parseConstructor();

        /// @brief Advance iterator by one
        inline void advance() { m_it++; }

        /// @brief Move iterator back by one
        inline void retract() { m_it--; }

        /// @brief Parse the binary operation. binopright = (op primary)*
        /// @param priority
        /// @param leftSide
        /// @return
        std::unique_ptr<CodeNode> parseBinaryOperationRightSide(int32_t priority, std::unique_ptr<CodeNode> leftSide);

        int32_t getBinaryOperationPriority();
        /// @brief Throw an error using current token as the reference point
        /// @param message Message to display
        std::unique_ptr<CodeNode> error(std::string const &message);

        /// @brief Cast current token to a given type or display an error if cast failed
        /// @tparam T Type to cast into
        /// @param msg Message to display on failure
        /// @return Constant pointer to the token converted into the requested type
        template <class T>
        T const *getTokenOrError(std::string const &msg)
        {
            if (T const *t = dynamic_cast<T const *>(getCurrent()); t != nullptr)
            {
                return t;
            }
            error(msg);
            return nullptr;
        }

        /// @brief Check if current token is valid and a separator and if not print an error. On success pointer will advance
        /// @param sep Separator to check
        /// @param err Message to display
        void consumeSeparator(Separator sep, std::string const &err);

        /// @brief Check if current token is valid and a separator and if not print an error. On success pointer will advance
        /// @param sep Separator to check
        /// @param err Message to display
        void consumeKeyword(Keyword keyword, std::string const &err);

        /// @brief Check if current token is valid and an operator, otherwise print an error. Advance pointer on success
        /// @param op Operator to consume
        /// @param err Message to display on failure
        void consumeOperator(Operator op, std::string const &err);

        /// @brief Is current token a given separator
        /// @param sep Separator to check
        /// @return False if not that separator or null
        bool isSeparator(Separator sep);

        /// @brief Is current token a given keyword
        /// @param keyword
        /// @return False if not that keyword or null
        bool isKeyword(Keyword keyword);

        /// @brief Check if current token is one of the unary operators(+,-,~,!)
        /// @return
        bool isUnaryOperator();

        /// @brief Check if the given token is an operator and stores any of the assignment operators in it
        /// @return False if not assignment or null
        bool isAssignment();

        /// @brief Is current token of a given type. Performs a dynamic cast and checks it against `nullptr`
        /// @tparam T Type to check against
        /// @return True if of that type, false if not or null
        template <class T>
        inline bool isOfType()
        {
            return dynamic_cast<T const *>(getCurrent()) != nullptr;
        }

        void printTree();
        Token const *getCurrent() { return isAtTheEnd() ? nullptr : m_it->get(); }

        bool isAtTheEnd() { return m_it == m_parser.getTokens().end(); }

    private:
        Parser const &m_parser;
        std::vector<std::unique_ptr<Token>>::const_iterator m_it;

        std::unique_ptr<SequenceNode> m_rootSequence;

        std::vector<std::unique_ptr<FunctionNode>> m_functions;

        std::vector<std::unique_ptr<TypeDefinitionNode>> m_structs;
    };
} // namespace CodeGen
