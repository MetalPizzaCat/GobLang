#pragma once
#include <memory>
#include "Parser.hpp"
#include "../execution/Function.hpp"
#include "CompilerToken.hpp"
#include "FunctionTokenSequence.hpp"

namespace GobLang::Compiler
{
    class ReversePolishGenerator
    {
    public:
        explicit ReversePolishGenerator(Parser const &parser) : m_parser(parser) {}
        /**
         * @brief Convert given parsed data into reverse polish notation representation of code
         *
         */
        void compile();

        /**
         * @brief Dump all contents of the stack into the reverse polish notation array
         *
         */
        void dumpStack();

        void dumpStackWhile(std::function<bool(Token *)> const &cond);

        /**
         * @brief Get priority for the top item on the stack or -1 if stack is empty
         *
         * @return int32_t Priority
         */
        int32_t getTopStackPriority();

        /**
         * @brief Pop the last item from the stack and return it
         *
         * @return Token* Token from top of the stack or nullptr if stack is empty
         */
        Token *popStack();

        /**
         * @brief Prints the parsed sequence using the token data
         *
         */
        void printCode();

        /**
         * @brief Prints the parsed sequences for functions
         *
         */
        void printFunctions();

        /// @brief Prints available struct info
        void printStructs();

        void addToken(Token *token);

        /**
         * @brief Get token of a while loop that is closest to the top of the jump stack.
         *
         * @return WhileToken* Token of a while loop that is closest to the top of the jump stack or nullptr if none are found
         */
        WhileToken *getCurrentLoop();

        size_t getMarkCounterAndAdvance();

        std::vector<Token *> const &getCode() const { return m_code; }

        std::vector<std::string> const &getIds() const { return m_parser.getIds(); }

        std::vector<std::unique_ptr<Struct::Structure>> const &getStructs() const { return m_structs; }

        std::vector<FunctionTokenSequence *> const &getFuncs() const { return m_funcs; }

        ~ReversePolishGenerator();

    private:
        bool _doesVariableExist(size_t stringId);
        int32_t _getLocalVariableAccessId(size_t id);
        size_t _getFunctionAccessId(size_t nameId);
        void _appendVariableBlock();
        void _popVariableBlock();
        void _appendVariable(size_t stringId);
        void _compileSeparators(SeparatorToken *sepToken, std::vector<Token *>::const_iterator const &it);
        void _compileFunctionCall(IdToken const *name, SeparatorToken const *sep);

        bool _isIdUsedForFunctionCall(size_t id);
        bool _isFieldName(std::vector<Token *>::const_iterator const &it);
        bool _isMethodName(std::vector<Token *>::const_iterator const &it);

        /**
         * @brief Check if previous token is a valid array construct which could be addressed via an array access operation. This accounts for ], ) and ID
         *
         * @param it
         * @return true
         * @return false
         */
        bool _isPreviousTokenValidArrayParent(std::vector<Token *>::const_iterator const &it);

        void _compileKeywords(KeywordToken *keyToken, std::vector<Token *>::const_iterator const &it);

        /**
         * @brief Attempt to parse function header and save its data.
         *  Unlike other compiler functions this one has to operate outside of the usual parsing process to generate function info
         *
         * @param start
         * @param end End of the function declaration
         */
        void _compileFunction(std::vector<Token *>::const_iterator const &start, std::vector<Token *>::const_iterator &end);

        /**
         * @brief Parse structure declaration and store its data. Like function header parsing this is handled in one big swoop.
         * This is done to avoid polluting main code handler
         *
         * @param start
         * @param end End of the structure declaration
         */
        void _compileStructure(std::vector<Token *>::const_iterator const &start, std::vector<Token *>::const_iterator &end);

        bool _isElseChainToken(std::vector<Token *>::const_iterator const &it);

        bool _isElifChainToken(std::vector<Token *>::const_iterator const &it);
        void _printTokenStack();

        bool _isBranchKeyword(std::vector<Token *>::const_iterator const &it);

        void _addOperator(std::vector<Token *>::const_iterator const &it);

        void _handleBlockClose(SeparatorToken const *sepTok, std::vector<Token *>::const_iterator const &it);

        void _handleBracketClose(SeparatorToken const *sepTok, std::vector<Token *>::const_iterator const &it);

        void _compileIdToken(IdToken const *idToken, std::vector<Token *>::const_iterator &it);

        /**
         * @brief Check if this is valid binary operation
         *
         * @param it Iterator pointing to current operation token
         * @return true This operator can be considered binary operator
         * @return false This  operator is most likey an unary operator
         */
        bool _isValidBinaryOperation(std::vector<Token *>::const_iterator const &it);
        /**
         * @brief code representation in reverse polish notation
         *
         */
        std::vector<Token *> m_code;

        std::vector<Token *> m_stack;
        std::vector<SeparatorToken *> m_blockDepthStack;

        std::vector<std::vector<size_t>> m_blockVariables = {{}};

        std::vector<GotoToken *> m_jumps;

        std::vector<FunctionTokenSequence *> m_funcs;
        std::vector<std::unique_ptr<Struct::Structure>> m_structs;
        FunctionTokenSequence *m_currentFunction = nullptr;

        std::vector<Token *>::const_iterator m_it;

        /**
         * @brief Array for storing all tokens created during the conversion process, used only to be able to delete them once the process ends
         *
         */
        std::vector<Token *> m_compilerTokens;

        /**
         * @brief Stack of tokens that require a comma separated sequence
         *
         */
        std::vector<MultiArgToken *> m_multiArgSequences;

        bool m_isInConditionHead = false;
        size_t m_markCounter = 0;

        bool m_isVariableDeclaration = false;

        Parser const &m_parser;
    };
}