#include "Token.hpp"

std::string GobLang::Codegen::KeywordToken::toString()
{
    return std::find_if(
               Keywords.begin(),
               Keywords.end(),
               [this](auto it)
               { return it.second == m_keyword; })
        ->first;
}

int32_t GobLang::Codegen::Token::getPriority() const
{
    // by default it's lowest possible value
    return -1;
}

GobLang::Codegen::Token::Token(size_t row, size_t column) : m_row(row), m_column(column)
{
}


GobLang::Codegen::OperatorToken::OperatorToken(size_t row, size_t column, OperatorData const *data) : Token(row, column), m_data(data)
{
}

bool GobLang::Codegen::OperatorToken::isAssignment() const
{
    return m_data->op == Operator::Assign || m_data->isCombinedAssignment;
}

GobLang::Operation GobLang::Codegen::OperatorToken::getOperation() const
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

int32_t GobLang::Codegen::OperatorToken::getPriority() const
{
    return m_data->priority;
}

std::string GobLang::Codegen::OperatorToken::toString()
{
    return std::string(m_data->symbol) + (m_unary ? "u" : "");
}

std::string GobLang::Codegen::IdToken::toString()
{
    return "W" + std::to_string(m_id);
}

std::string GobLang::Codegen::IntToken::toString()
{
    return "NUM_" + std::to_string(m_value);
}

GobLang::Codegen::SeparatorToken::SeparatorToken(size_t row, size_t column, Separator sep) : Token(row, column)
{
    m_data = &(*std::find_if(Separators.begin(), Separators.end(), [sep](SeparatorData const &data)
                             { return data.separator == sep; }));
}

std::string GobLang::Codegen::SeparatorToken::toString()
{
    return std::string{m_data->symbol};
}

std::string GobLang::Codegen::StringToken::toString()
{
    return "STR" + std::to_string(m_id);
}

std::string GobLang::Codegen::GotoToken::toString()
{
    return "GOTO_M" + std::to_string(m_mark);
}

std::string GobLang::Codegen::IfToken::toString()
{
    return (m_elif ? "JMP_ELIF_FALSE_M" : "JMP_IF_FALSE_M") + std::to_string(getMark());
}

std::string GobLang::Codegen::JumpDestinationToken::toString()
{
    return "M" + std::to_string(m_id) + ":";
}

std::string GobLang::Codegen::WhileToken::toString()
{
    return "WHILE_M" + std::to_string(m_returnMark) + "_THEN_M" + std::to_string(getMark());
}

std::string GobLang::Codegen::FloatToken::toString()
{
    return "FLOAT_" + std::to_string(m_value);
}

std::string GobLang::Codegen::NullConstToken::toString()
{
    return "NULL";
}

std::string GobLang::Codegen::UnsignedIntToken::toString()
{
    return "UINT_" + std::to_string(m_value);
}
