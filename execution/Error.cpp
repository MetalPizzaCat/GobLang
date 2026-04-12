#include "Error.hpp"

const char *GobLang::Errors::RuntimeError::what() const throw()
{
    return m_message.c_str();
}

GobLang::Errors::RuntimeError::RuntimeError(size_t row, size_t column, std::string const &msg) : m_row(row), m_column(column), m_message(msg)
{
}


const char *GobLang::Errors::RuntimeMemoryError::what() const throw()
{
    return m_message.c_str();
}

GobLang::Errors::RuntimeMemoryError::RuntimeMemoryError(std::string const &msg) : m_message(msg)
{
}

const char *GobLang::Errors::ExecutionError::what() const throw()
{
    return m_message.c_str();
}

GobLang::Errors::ExecutionError::ExecutionError(std::string const &msg) : m_message(msg)
{
}
