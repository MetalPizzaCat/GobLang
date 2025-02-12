#include "CompilerToken.hpp"

std::string GobLang::Compiler::FunctionCallToken::toString()
{
    return "CALL_" + std::to_string(m_argCount) + (m_usesLocalFunc ? (std::string("_LOCAL") + std::to_string(m_funcId)) : "");
}

void GobLang::Compiler::FunctionCallToken::increaseArgCount()
{
    m_argCount++;
}

std::string GobLang::Compiler::LocalVarShrinkToken::toString()
{
    return "SHRINK_BY" + std::to_string(m_amount);
}

std::string GobLang::Compiler::ReturnToken::toString()
{
    return std::string("RET") + (m_hasVal ? "_VAL" : "");
}
