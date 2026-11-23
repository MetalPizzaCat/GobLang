#pragma once
#include <exception>
#include <string>
namespace GobLang::Errors
{
	class GobError : public std::exception
	{
	public:
		const char *what() const throw() override;
		GobError(std::string const &msg);

	private:
		std::string m_message;
	};
}