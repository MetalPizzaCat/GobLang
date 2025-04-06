#pragma once
#include "Parser.hpp"
#include "CodeNode.hpp"

namespace GobLang::Codegen
{

    /// @brief Generates an AST based on the parser tokens and then generates bytecode by recursively calling the codegen function
    class CodeGenerator
    {
    public:
        explicit CodeGenerator(Parser const &parser);

        void generate();

        /// @brief Block of code with n expressions separated by ;
        /// @return
        std::unique_ptr<SequenceNode> parseBody();

        std::unique_ptr<VariableCreationNode> parseVarCreation();

        std::unique_ptr<CodeNode> parseStandaloneExpression();

        /// @brief Parse the condition + body of a branch. Could be used for loops
        /// @return 
        std::unique_ptr<BranchNode> parseBranch();

        /// @brief Parse if-elif-else block
        /// @return 
        std::unique_ptr<BranchChainNode> parseBranchChain();

        std::unique_ptr<FloatNode> parseFloat();

        std::unique_ptr<IntNode> parseInt();

        std::unique_ptr<CodeNode> parseId();

        std::vector<std::unique_ptr<CodeNode>> parseFunctionCallArguments();

        std::unique_ptr<CodeNode> parsePrimary();

        std::unique_ptr<CodeNode> parseGrouped();

        std::unique_ptr<CodeNode> parseExpression();

        

        inline void advance() { m_it++; }

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

        template <class T>
        T *getTokenOrError(std::string const &msg)
        {
            if (T *t = dynamic_cast<T *>(getCurrent()); t != nullptr)
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

        void consumeOperator(Operator op, std::string const &err);

        bool isSeparator(Separator sep);

        bool isKeyword(Keyword keyword);

        template <class T>
        inline bool isOfType()
        {
            return dynamic_cast<T *>(getCurrent()) != nullptr;
        }

        Token *getCurrent() { return isAtTheEnd() ? nullptr : m_it->get(); }

        bool isAtTheEnd() { return m_it == m_parser.getTokens().end(); }

    private:
        Parser const &m_parser;
        std::vector<std::unique_ptr<Token>>::const_iterator m_it;
    };
} // namespace CodeGen
