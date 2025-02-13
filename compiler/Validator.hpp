#pragma once
#include "Parser.hpp"
namespace GobLang::Compiler
{
    enum class BranchType
    {
        If,
        Elif,
        Else,
        While
    };
    /**
     * @brief Class that validates that code was written according to the language rules
     */
    class Validator
    {
    public:
        using TokenIterator = std::vector<Token *>::const_iterator;
        explicit Validator(Parser const &parser) : m_parser(parser) {}

        void validate();

        bool constant(TokenIterator const &it);
        bool id(TokenIterator const &it);
        bool unaryOperator(TokenIterator const &it);
        bool mathOperator(TokenIterator const &it);
        bool actionOperator(TokenIterator const &it, Operator op);
        bool separator(TokenIterator const &it, Separator sep);
        bool keyword(TokenIterator const &it, Keyword word);
        bool end(TokenIterator const &it);
        bool operand(TokenIterator const &it);

        /**
         * @brief Repeating rule for expr = mul {op mul}
         *
         * @param it
         * @param endIt
         * @return true
         * @return false
         */
        bool expr(TokenIterator const &it, TokenIterator &endIt);
        /**
         * @brief Expressions which only use one operand. "!a" and "-a"
         *
         * @param it
         * @param endIt
         * @return true
         * @return false
         */
        /**
         * @brief Check if keyword is `break` of `continue` ending with ';'
         */
        bool loopControlKeyWord(TokenIterator const &it, TokenIterator &endIt);
        bool unaryExpr(TokenIterator const &it, TokenIterator &endIt);
        /**
         * @brief Check that covers anything that follows this rule expr = arrayAccess | call | operand | (expr)
         *
         * @param it
         * @param endIt
         * @return true
         * @return false
         */
        bool mul(TokenIterator const &it, TokenIterator &endIt);
        bool functionCall(TokenIterator const &it, TokenIterator &endIt);
        bool arrayAccess(TokenIterator const &it, TokenIterator &endIt);
        bool call(TokenIterator const &it, TokenIterator &endIt);
        bool callOp(TokenIterator const &it, TokenIterator &endIt);
        bool arrayIndex(TokenIterator const &it, TokenIterator &endIt);
        bool assignment(TokenIterator const &it, TokenIterator &endIt);
        bool arrayAssignment(TokenIterator const &it, TokenIterator &endIt);
        bool localVarCreation(TokenIterator const &it, TokenIterator &endIt);
        bool returnOperation(TokenIterator const& it, TokenIterator &endIt);

        bool block(TokenIterator const &it, TokenIterator &endIt);
        bool code(TokenIterator const &it, TokenIterator &endIt);
        bool branch(BranchType branch, TokenIterator const &it, TokenIterator &endIt);
        bool ifElseChain(TokenIterator const &it, TokenIterator &endIt);
        
        /**
         * @brief "Function" id ({id,}) block
         * 
         * @param it 
         * @param endIt 
         * @return true 
         * @return false 
         */
        bool function(TokenIterator const& it, TokenIterator &endIt);

        TokenIterator getEnd() { return m_parser.getTokens().end(); }

        size_t getRowForToken(TokenIterator const &it);
        size_t getColumnForToken(TokenIterator const &it);

    private:
        Parser const &m_parser;
    };
} // namespace GobLang::Compiler
