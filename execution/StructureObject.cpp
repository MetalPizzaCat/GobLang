#include "StructureObject.hpp"

std::string GobLang::StructureObject::toString() const
{
    return m_class->getName();
}