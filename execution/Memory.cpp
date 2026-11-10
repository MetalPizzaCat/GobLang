#include "Memory.hpp"
#include <sstream>


void GobLang::Object::increaseRefCount()
{
    m_refCount++;
}

void GobLang::Object::decreaseRefCount()
{
    m_refCount--;
}


bool GobLang::Object::equalsTo(Object *other)
{
    return other == this;
}

std::string GobLang::Object::toString() const
{
    const void *address = static_cast<const void *>(this);
    std::stringstream ss;
    ss << address;
    return std::string("Object: ") + ss.str();
}
