#include "Structure.hpp"
#include "Value.hpp"
#include "Machine.hpp"

GobLang::Struct::StructureObjectNode::StructureObjectNode(Structure const *base) : m_struct(base)
{

    for (Field const &field : base->fields)
    {
        m_fieldNames[field.name] = m_fields.size();
        switch (field.type)
        {
        case StructureFieldType::Char:
            m_fields.push_back(MemoryValue{.type = Type::Char, .value = '\0'});
            break;
        case StructureFieldType::Bool:
            m_fields.push_back(MemoryValue{.type = Type::Char, .value = false});
            break;
        case StructureFieldType::Float:
            m_fields.push_back(MemoryValue{.type = Type::Float, .value = 0.f});
            break;
        case StructureFieldType::Int:
            m_fields.push_back(MemoryValue{.type = Type::Int, .value = 0});
            break;
        case StructureFieldType::UnsignedInt:
            m_fields.push_back(MemoryValue{.type = Type::Int, .value = 0u});
            break;
        default:
            m_fields.push_back(MemoryValue{.type = Type::Null, .value = 0});
            break;
        }
    }
}

std::string GobLang::Struct::StructureObjectNode::toString(bool pretty)
{
    std::string start = "{";
    for (std::vector<MemoryValue>::const_iterator it = m_fields.begin(); it != m_fields.end(); it++)
    {
        start += m_struct->fields[it - m_fields.begin()].name + "=" + valueToString(*it, pretty);

        start += (it + 1 == m_fields.end()) ? "}" : ",";
    }
    return start;
}
