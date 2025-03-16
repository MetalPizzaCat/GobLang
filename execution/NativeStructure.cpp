#include "NativeStructure.hpp"

bool GobLang::Struct::NativeStructureObjectNode::hasNativeMethod(std::string const &name)
{
    if (m_nativeStruct == nullptr)
    {
        return false;
    }
    return m_nativeStruct->methods.count(name);
}

GobLang::FunctionValue const *GobLang::Struct::NativeStructureObjectNode::getNativeMethod(std::string const &name) const
{
    if (m_nativeStruct == nullptr)
    {
        return nullptr;
    }
    return &m_nativeStruct->methods.at(name);
}
