#pragma once
#include "Memory.hpp"
#include <optional>
namespace GobLang
{
    class ArrayObject : public Object
    {
    public:
        explicit ArrayObject(size_t size);

        void setItem(size_t i, Value const &item);
        std::optional<Value> getItem(size_t i);

        std::string toString() const override;

        /// @brief Get array length
        /// @return 
        size_t getSize() const { return m_data.size(); }

        void append(Value const &item);

        virtual ~ArrayObject();

    private:
        std::vector<Value> m_data;
    };
} // namespace SimpleLang
