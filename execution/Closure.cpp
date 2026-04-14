#include "Closure.hpp"
#include <sstream>
#include "Error.hpp"
GobLang::Closure::Closure(GobFunction const *func, Object *owner) : m_owner(owner), m_func(func), m_nativeFunc(nullptr)
{
}
GobLang::Closure::Closure(FunctionValue const &f) : m_owner(nullptr), m_func(nullptr), m_nativeFunc(f)
{
}
std::string GobLang::Closure::toString() const
{
    const void *address = static_cast<const void *>(this);
    std::stringstream ss;
    ss << address;
    return std::string("function: ") + ss.str();
}

GobLang::GobFunction::GobFunction(std::vector<uint8_t> const &bytes,
                                  std::vector<std::string> const &strings,
                                  std::string const &name) : m_byteCode(bytes),
                                                             m_strings(strings),
                                                             m_name(name)
{
}

std::optional<std::string> GobLang::GobFunction::getConstantString(size_t id) const
{
    if (m_strings.size() <= id)
    {
        return {};
    }
    return m_strings.at(id);
}

std::string const &GobLang::GobFunction::getConstantStringByidOrError(size_t id, std::string const &errorMessage) const
{
    if (m_strings.size() <= id)
    {
        throw Errors::ExecutionError(errorMessage);
    }
    return m_strings.at(id);
}

void GobLang::GobFunction::setByteCode(std::vector<uint8_t> const &bytes)
{
    m_byteCode = bytes;
}

void GobLang::GobFunction::setStrings(std::vector<std::string> const &strings)
{
    m_strings = strings;
}

void GobLang::GobFunction::setArgumentCount(size_t count)
{
    m_argumentCount = count;
}
