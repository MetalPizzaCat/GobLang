#include "Error.hpp"

GobLang::Errors::RuntimeError::RuntimeError(size_t row, size_t column, std::string const &msg) : m_row(row), m_column(column), GobError(msg)
{
}
