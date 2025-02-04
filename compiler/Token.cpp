#include "Token.hpp"

std::string GobLang::Compiler::KeywordToken::toString()
{
    return std::find_if(
               Keywords.begin(),
               Keywords.end(),
               [this](auto it)
               { return it.second == m_keyword; })
        ->first;
}

int32_t GobLang::Compiler::Token::getPriority() const
{
    // by default it's lowest possible value
    return -1;
}

GobLang::Compiler::Token::Token(size_t row, size_t column) : m_row(row), m_column(column)
{
}

GobLang::Compiler::OperatorToken::OperatorToken(size_t row, size_t column, Operator oper) : Token(row, column)
{
    m_data = &(*std::find_if(Operators.begin(), Operators.end(), [oper](OperatorData const &op)
                             { return op.op == oper; }));
}

GobLang::Operation GobLang::Compiler::OperatorToken::getOperation() const
{
    if (!isUnary())
    {
        return m_data->operation;
    }
    switch (getOperator())
    {
    case Operator::Sub:
        return Operation::Negate;
    default:
        return m_data->operation;
    }
}

int32_t GobLang::Compiler::OperatorToken::getPriority() const
{
    return m_data->priority;
}

std::string GobLang::Compiler::OperatorToken::toString()
{
    return std::string(m_data->symbol) + (m_unary ? "u" : "");
}

std::string GobLang::Compiler::IdToken::toString()
{
    return "W" + std::to_string(m_id);
}

std::string GobLang::Compiler::IntToken::toString()
{
    return "NUM" + std::to_string(m_id);
}

GobLang::Compiler::SeparatorToken::SeparatorToken(size_t row, size_t column, Separator sep) : Token(row, column)
{
    m_data = &(*std::find_if(Separators.begin(), Separators.end(), [sep](SeparatorData const &data)
                             { return data.separator == sep; }));
}

std::string GobLang::Compiler::SeparatorToken::toString()
{
    return std::string{m_data->symbol};
}

std::string GobLang::Compiler::StringToken::toString()
{
    return "STR" + std::to_string(m_id);
}

std::string GobLang::Compiler::GotoToken::toString()
{
    return "GOTO_M" + std::to_string(m_mark);
}

std::string GobLang::Compiler::IfToken::toString()
{
    return (m_elif ? "JMP_ELIF_FALSE_M" : "JMP_IF_FALSE_M") + std::to_string(getMark());
}

std::string GobLang::Compiler::JumpDestinationToken::toString()
{
    return "M" + std::to_string(m_id) + ":";
}

std::string GobLang::Compiler::WhileToken::toString()
{
    return "WHILE_M" + std::to_string(m_returnMark) + "_THEN_M" + std::to_string(getMark());
}

std::string GobLang::Compiler::FloatToken::toString()
{
    return "FLOAT" + std::to_string(m_id);
}
