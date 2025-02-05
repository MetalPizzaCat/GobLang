#include "Exception.hpp"

const char *GobLang::RuntimeException::what() const throw()
{
    return m_msg.c_str();
}