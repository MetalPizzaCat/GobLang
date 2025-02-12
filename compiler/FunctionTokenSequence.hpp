#pragma once
#include "../execution/Function.hpp"
#include "Token.hpp"
#include <vector>

namespace GobLang::Compiler
{
    class FunctionTokenSequence
    {
    public:
        explicit FunctionTokenSequence(Function *func) : m_func(func) {}

        void addToken(Token *token) { m_tokens.push_back(token); }

        std::vector<Token *> const &getTokens() const { return m_tokens; }

        Function const *getInfo() const { return m_func; }

    private:
        std::vector<Token *> m_tokens;
        Function *m_func;
    };
} // namespace GobLang::Compiler
