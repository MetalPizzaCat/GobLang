#pragma once
#include <exception>
#include <string>
namespace GobLang
{

    class RuntimeException : public std::exception
    {
    public:
        const char *what() const throw() override;
        explicit RuntimeException(std::string const &msg) : m_msg(msg) {}

    private:
        std::string m_msg;
    };
}