#pragma once
#include <exception>
#include <string>
#include "../common/Error.hpp"
namespace GobLang::Errors
{

	/// @brief Alternative error type that doesn't use any of the debug info
	class ExecutionError : public GobError
	{
	public:
		explicit ExecutionError(std::string const &msg) : GobError(msg) {}
	};

	class RuntimeError : public GobError
	{
	public:
		RuntimeError(size_t row, size_t column, std::string const &msg);

		size_t getLine() const { return m_row; }
		size_t getColumn() const { return m_column; }

	private:
		size_t m_row;
		size_t m_column;
	};
} // namespace GobLang
