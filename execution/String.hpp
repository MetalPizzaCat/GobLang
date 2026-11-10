#pragma once
#include <string>
#include "Memory.hpp"

namespace GobLang
{

    /// @brief Class that represents a string in GobLang. Strings in goblang are mutable and consist of 8 bit characters
    class StringObject : public Object
    {
    public:
        explicit StringObject(std::string const &str) : m_str(str) {}

        std::string const &getString() const { return m_str; }

        std::string toString() const override;

        char getCharAt(size_t ind);

        void setCharAt(char ch, size_t ind);

        /**
         * @brief Compare other memory node and return true if both contain same sequence of characters
         *
         * @param other
         * @return true
         * @return false
         */
        bool equalsTo(Object *other) override;

        size_t getSize() const { return m_str.size(); }

        virtual ~StringObject() = default;

    private:
        std::string m_str;
    };
} // namespace GobLang
