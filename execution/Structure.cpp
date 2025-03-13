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

void GobLang::Struct::StructureObjectNode::setField(std::string const &field, MemoryValue const &value)
{
    if (m_fieldNames.count(field) == 0)
    {
        throw RuntimeException(std::string("Invalid field name ") + field);
    }
    size_t id = m_fieldNames[field];
    // check if object that we are setting is itself to avoid creating a ref cycle
    if (value.type == Type::MemoryObj && std::get<MemoryNode *>(value.value) != this)
    {
        std::get<MemoryNode *>(value.value)->increaseRefCount();
    }
    if (m_fields[id].type == Type::MemoryObj && std::get<MemoryNode *>(m_fields[id].value) != this)
    {
        std::get<MemoryNode *>(m_fields[id].value)->decreaseRefCount();
    }
    m_fields[id] = value;
}

GobLang::MemoryValue GobLang::Struct::StructureObjectNode::getField(std::string const &field)
{
    if (m_fieldNames.count(field) == 0)
    {
        throw RuntimeException(std::string("Invalid field name ") + field);
    }
    return m_fields[m_fieldNames[field]];
}

std::string GobLang::Struct::StructureObjectNode::toString(bool pretty, size_t depth)
{
    std::string start = "{";
    for (std::vector<MemoryValue>::const_iterator it = m_fields.begin(); it != m_fields.end(); it++)
    {
        start += m_struct->fields[it - m_fields.begin()].name + "=" + valueToString(*it, pretty, depth);

        start += (it + 1 == m_fields.end()) ? "}" : ",";
    }
    return start;
}

GobLang::Struct::StructureObjectNode::~StructureObjectNode()
{
    for (std::vector<MemoryValue>::iterator it = m_fields.begin(); it != m_fields.end(); it++)
    {
        if (it->type == Type::MemoryObj)
        {
            std::get<MemoryNode *>(it->value)->decreaseRefCount();
        }
    }
}
