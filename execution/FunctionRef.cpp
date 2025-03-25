#include "FunctionRef.hpp"

GobLang::FunctionRef::FunctionRef(FunctionValue const * func, MemoryNode *owner) : m_func(func), m_owner(owner)
{

}

GobLang::FunctionRef::FunctionRef(size_t localFuncId) : m_localFuncId(localFuncId)
{
}
