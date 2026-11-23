#include "Error.hpp"

const char *GobLang::Errors::GobError::what() const throw()
{
	return m_message.c_str();
}

GobLang::Errors::GobError::GobError(std::string const &msg) : m_message(msg)
{
}
