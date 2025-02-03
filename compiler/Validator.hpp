#pragma once
#include "Parser.hpp"
namespace GobLang::Compiler
{
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
        bool mathOperator(TokenIterator const &it);
        bool actionOperator(TokenIterator const &it, Operator op);
        bool separator(TokenIterator const &it, Separator sep);
        bool keyword(TokenIterator const &it, Keyword word);
        bool end(TokenIterator const &it);
        bool operand(TokenIterator const &it);

        bool expr(TokenIterator const &it, TokenIterator &endIt);
        bool mul(TokenIterator const &it, TokenIterator &endIt);
        /**
         * @brief Expression between '(' and ')'
         *
         * @param it
         * @param endIt
         * @return true
         * @return false
         */
        bool groupedExpr(TokenIterator const &it, TokenIterator &endIt);
        bool functionCall(TokenIterator const& it, TokenIterator &endIt);
        bool arrayAccess(TokenIterator const& it, TokenIterator &endIt);
        bool arrayIndex(TokenIterator const& it, TokenIterator &endIt);
        bool assignment(TokenIterator const &it, TokenIterator &endIt);
        bool arrayAssignment(TokenIterator const &it, TokenIterator &endIt);
        bool localVarCreation(TokenIterator const &it, TokenIterator &endIt);

        bool block(TokenIterator const &it, TokenIterator &endIt);
        

        TokenIterator getEnd() { return m_parser.getTokens().end(); }

    private:
        Parser const &m_parser;
    };
} // namespace GobLang::Compiler
