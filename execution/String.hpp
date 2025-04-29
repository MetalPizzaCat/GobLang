#pragma once
#include <string>
#include "Memory.hpp"

namespace GobLang
{

    class StringNode : public MemoryNode
    {
    public:
        explicit StringNode(std::string const &str) : m_str(str) {}

        std::string const &getString() { return m_str; }

        std::string toString(bool pretty, size_t depth) override;

        char getCharAt(size_t ind);

        void setCharAt(char ch, size_t ind);

        /**
         * @brief Compare other memory node and return true if both contain same sequence of characters
         *
         * @param other
         * @return true
         * @return false
         */
        bool equalsTo(MemoryNode *other) override;

        size_t getSize() const { return m_str.size(); }

        virtual ~StringNode() = default;

    private:
        std::string m_str;
    };
} // namespace GobLang
