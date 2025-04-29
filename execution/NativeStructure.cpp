#include "NativeStructure.hpp"
#include "Machine.hpp"
#include "FunctionRef.hpp"

GobLang::Value GobLang::Struct::NativeStructureObjectNode::getField(std::string const &field)
{
    if (m_nativeStruct->methods.contains(field))
    {
        return Value(new FunctionRef(&m_nativeStruct->methods.at(field), this));
    }
    return MemoryNode::getField(field);
}

