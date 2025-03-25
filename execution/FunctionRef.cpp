#include "FunctionRef.hpp"

GobLang::FunctionRef::FunctionRef(FunctionValue const * func, MemoryNode *owner) : m_func(func), m_owner(owner)
{

}