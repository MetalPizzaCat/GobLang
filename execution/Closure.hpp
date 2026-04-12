#pragma once
#include "Memory.hpp"
#include "Value.hpp"
#include <optional>
namespace GobLang
{
    /**
     * @brief Information for the executable function within programming language
     *
     */
    class GobFunction : public Object
    {
    public:
        explicit GobFunction(std::vector<uint8_t> const &bytes, std::vector<std::string> const &strings, std::string const &name = "?");

        std::optional<std::string> getConstantString(size_t id) const;

        std::string const &getConstantStringByidOrError(size_t id, std::string const &errorMessage = "No string under given id") const;

        std::vector<uint8_t> const &getByteCode() const { return m_byteCode; }

    private:
        std::vector<uint8_t> m_byteCode;
        std::vector<std::string> m_strings;

        std::string m_name;
    };

    class Closure : public Object
    {
    public:
        explicit Closure(GobFunction const *func, Object *owner = nullptr);
        explicit Closure(FunctionValue const &f);
        bool hasOwner() const { return m_owner != nullptr; }
        bool isLocal() const { return m_func != nullptr; }
        Object *getOwner() { return m_owner; }

        FunctionValue const &getNativeFunction() const { return m_nativeFunc; }
        GobFunction const *getGobFunction() const { return m_func; }

        std::string toString() const override;

    private:
        size_t m_argumentCount;
        Object *m_owner = nullptr;
        GobFunction const *m_func;
        FunctionValue m_nativeFunc;

        // TODO: way to store captured variables
    };
}