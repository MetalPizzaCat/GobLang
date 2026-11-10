#include <vector>
#include <string>

namespace GobLang::Codegen
{
    class FunctionContext
    {
    public:
        explicit FunctionContext() = default;
        size_t getStringId(std::string const &str);

        std::vector<std::string> const &getStrings() const { return m_strings; }

    private:
        std::vector<std::string> m_strings;
    };
} // namespace GobLang::Codegen
