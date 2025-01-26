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

int32_t SimpleLang::Compiler::Token::getPriority() const
{
    // by default it's lowest possible value
    return -1;
}

SimpleLang::Compiler::Token::Token(size_t row, size_t column) : m_row(row), m_column(m_column)
{
}

SimpleLang::Compiler::OperatorToken::OperatorToken(size_t row, size_t column, Operator oper) : Token(row, column)
{
    m_data = &(*std::find_if(Operators.begin(), Operators.end(), [oper](OperatorData const &op)
                             { return op.op == oper; }));
}

int32_t SimpleLang::Compiler::OperatorToken::getPriority() const
{
    return m_data->priority;
}

std::string SimpleLang::Compiler::OperatorToken::toString()
{
    return m_data->symbol;
}

std::string SimpleLang::Compiler::IdToken::toString()
{
    return "W" + std::to_string(m_id);
}

std::string SimpleLang::Compiler::IntToken::toString()
{
    return "NUM" + std::to_string(m_id);
}

SimpleLang::Compiler::SeparatorToken::SeparatorToken(size_t row, size_t column, Separator sep) : Token(row, column)
{
    m_data = &(*std::find_if(Separators.begin(), Separators.end(), [sep](SeparatorData const &data)
                             { return data.separator == sep; }));
}

std::string SimpleLang::Compiler::SeparatorToken::toString()
{
    return std::string{m_data->symbol};
}
