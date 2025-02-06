#include "CompilerToken.hpp"

void GobLang::Compiler::FunctionCallToken::increaseArgCount()
{
    m_argCount++;
}

std::string GobLang::Compiler::LocalVarShrinkToken::toString()
{
    return "SHRINK_BY" + std::to_string(m_amount);
}
