#include "NativeStructure.hpp"
#include "Machine.hpp"
#include "FunctionRef.hpp"

GobLang::MemoryValue GobLang::Struct::NativeStructureObjectNode::getField(std::string const &field)
{
    if (m_nativeStruct->methods.contains(field))
    {
        return MemoryValue{.type = Type::MemoryObj, .value = new FunctionRef(&m_nativeStruct->methods.at(field), this)};
    }
    return MemoryNode::getField(field);
}

