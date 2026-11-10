#include "FunctionContext.hpp"
#include <algorithm>
size_t GobLang::Codegen::FunctionContext::getStringId(std::string const &str)
{

    std::vector<std::string>::const_iterator it = std::find(m_strings.begin(), m_strings.end(), str);
    if (it == m_strings.end())
    {
        m_strings.push_back(str);
        return m_strings.size() - 1;
    }
    return it - m_strings.begin();
}
