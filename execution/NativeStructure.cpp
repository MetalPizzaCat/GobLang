#include "NativeStructure.hpp"
#include "Machine.hpp"
#include "FunctionRef.hpp"

bool GobLang::Struct::NativeStructureObjectNode::hasNativeMethod(std::string const &name)
{
    if (m_nativeStruct == nullptr)
    {
        return false;
    }
    return m_nativeStruct->methods.count(name);
}

GobLang::MemoryValue GobLang::Struct::NativeStructureObjectNode::getField(std::string const &field)
{
    if (FunctionValue const *f = getNativeMethod(field); f != nullptr)
    {
        return MemoryValue{.type = Type::MemoryObj, .value = new FunctionRef(f, this)};
    }
    return StructureObjectNode::getField(field);
}

GobLang::FunctionValue const *GobLang::Struct::NativeStructureObjectNode::getNativeMethod(std::string const &name) const
{
    if (m_nativeStruct == nullptr)
    {
        return nullptr;
    }
    return &m_nativeStruct->methods.at(name);
}
