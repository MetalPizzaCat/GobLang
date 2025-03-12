#pragma once
#include <string>
#include <vector>
#include <cstdint>
#include <map>
#include "Memory.hpp"
#include "Value.hpp"
#include "Type.hpp"

namespace GobLang::Struct
{

    enum class StructureFieldType
    {
        Char,
        Bool,
        Float,
        Int,
        UnsignedInt,
        MemoryObj,
        Array,
        String
    };

    struct Field
    {
        std::string name;
        StructureFieldType type;
        /// @brief Reference to the custom type if type is MemoryObj
        size_t customStructureTypeId;
    };

    struct Structure
    {
        std::string name;
        std::vector<Field> fields;
    };

    class StructureObjectNode : public MemoryNode
    {
    public:
        explicit StructureObjectNode(Structure const *base);
        void setField(std::string const &field, MemoryValue const &value);
        MemoryValue getField(std::string const &field);

        virtual std::string toString(bool pretty = false, size_t depth = 0);

    private:
        Structure const* m_struct;
        std::vector<MemoryValue> m_fields;
        std::map<std::string, size_t> m_fieldNames;
    };
}