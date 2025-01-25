#include "Token.hpp"

std::string SimpleLang::Compiler::KeywordToken::toString()
{
    return std::find_if(
               Keywords.begin(),
               Keywords.end(),
               [this](auto it)
               { return it.second == m_keyword; })
        ->first;
}

SimpleLang::Compiler::Token::Token(size_t row, size_t column) : m_row(row), m_column(m_column)
{
}

std::string SimpleLang::Compiler::OperatorToken::toString()
{
    return std::find_if(Operators.begin(), Operators.end(), [this](OperatorData const &op)
                 { return op.op == m_op; })->symbol;
}

std::string SimpleLang::Compiler::IdToken::toString()
{
    return "W" + std::to_string(m_id);
}

std::string SimpleLang::Compiler::IntToken::toString()
{
    return "NUM" + std::to_string(m_id);
}
